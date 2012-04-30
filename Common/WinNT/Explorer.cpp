#include "explorer.h"
#include <ShlObj.h>
#include <Shlwapi.h>
#include <tchar.h>
#include <strsafe.h>
#pragma warning(push)
#pragma warning(disable:4995)
#include <atlbase.h>
#pragma warning(push)

#include <Psapi.h>
#pragma comment(lib, "Psapi.lib")

#include "../Log/NdLog.h"
#include "Registry.h"
#include "OsVersion.h"

// 아래 함수는 윈도우에서 전처리 되어 잇는 W/A 방식을 따라가서는 안된다. 이러한 구분은 Vista이상에서만 지원되기
// 때문에 XP에서 제대로 동작하지 않는다.
#undef Shell_GetCachedImageIndex

bool OpenWindowsExplorer(
	__in const std::wstring& filePath,
	__in bool withSelectingFileItem
	)
{
	if (filePath.empty())
	{
		return false;
	}

	std::wstring sCmdParameters;
	if (withSelectingFileItem)
	{
		sCmdParameters = L"/select,";
	}
	else
	{
		sCmdParameters = L"/e,";
	}

	if (filePath[0] != L'"')
	{
		sCmdParameters += std::wstring(L"\"") + filePath + L"\"";
	}
	else
	{
		sCmdParameters += filePath;
	}

	SHELLEXECUTEINFO sei = { 0 };
	sei.cbSize = sizeof(sei);
	sei.fMask = SEE_MASK_NOCLOSEPROCESS;
	sei.nShow = SW_SHOWNORMAL;
	sei.lpVerb = L"open";
	sei.lpParameters = sCmdParameters.c_str();
	sei.lpFile = L"explorer";

	return !!ShellExecuteExW(&sei);
}

bool UpdateFileFolderIcon(
	__in const std::wstring& filePath
	)
{
	// Update the icon cache
	SHFILEINFO sfi;
	if (SHGetFileInfo(filePath.c_str(), 0, &sfi, sizeof(sfi), SHGFI_ICONLOCATION))
	{
		int iIconIndex = Shell_GetCachedImageIndex(sfi.szDisplayName, sfi.iIcon, 0);
		SHUpdateImage(PathFindFileName(sfi.szDisplayName), sfi.iIcon, 0, iIconIndex);
		return true;
	}

	return false;
}

std::wstring GetSpecialFolderPath(
	__in int csidl
	)
{
	WCHAR szPath[MAX_PATH];
	BOOL fOk = SHGetSpecialFolderPath(0, szPath, csidl, FALSE);
	if (!fOk)
	{
		return L"";
	}

	return std::wstring(szPath);
}

bool ProcessNameMatched(
	__in const std::wstring& imageFileName,
	__in DWORD pid
	)
{
	std::wstring sProcessImageName = ProcessImageName(pid);
	logw(L"ProcessImageName(%d)<%s>", pid, sProcessImageName.c_str());
	if (sProcessImageName.size() < imageFileName.size())
	{
		return false;
	}

	sProcessImageName = sProcessImageName.substr(sProcessImageName.size() - imageFileName.size());
	return (_wcsicmp(sProcessImageName.c_str(), imageFileName.c_str()) == 0);
}

std::wstring ProcessImageName(
	__in DWORD pid
	)
{
	TCHAR szFilePath[MAX_PATH] = { 0 };
	std::wstring sImageName = L"";
	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
	if (hProcess)
	{
		if (GetProcessImageFileName(hProcess, szFilePath, _countof(szFilePath)))
		{
			sImageName = szFilePath;
		}

		CloseHandle(hProcess);
	}

	return sImageName;
}

bool IsExplorerProcess(
	__in DWORD pid
	)
{
	return ProcessNameMatched(L"explorer.exe", pid);
}

bool ShellDeleteFile(
	__in const std::wstring& sFilePath
	)
{
	//SHFileOperation 으로 폴더 삭제에 bug(?) 있음. 메모리 초기화한 곳에 경로명을 copy하여 shfo.pFrom 에 대입한다.
	TCHAR szRemovePath[MAX_PATH];
	StringCchPrintf(szRemovePath, _countof(szRemovePath), L"%s", sFilePath.c_str());

	bool bRet = true;

	SHFILEOPSTRUCT shfo;
	ZeroMemory(&shfo, sizeof(SHFILEOPSTRUCT));
	shfo.wFunc = FO_DELETE; 
	shfo.pFrom = szRemovePath;
	shfo.fFlags = FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_ALLOWUNDO | FOF_SILENT;
	int nDeleteRes = SHFileOperation(&shfo);

	// return value 2 means that the file was not found.
	if (0 != nDeleteRes && 2 != nDeleteRes)
	{
		bRet = false;
	}

	return bRet;
}

bool SetVolumeIcon(
	__in wchar_t volumeDesignator,
	__in const std::wstring& canonicalIconPath
	)
{
	logw(L"Start<%s>(%c:)", canonicalIconPath.c_str(), volumeDesignator);
	WCHAR defaultIconBase[MAX_PATH];
	StringCchPrintf(
		defaultIconBase,
		_countof(defaultIconBase),
		L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\DriveIcons\\%c\\DefaultIcon",
		volumeDesignator
		);

	if (canonicalIconPath.empty())
	{
		logw(L"Delete the key<%s>", defaultIconBase);
		return (Registry::RegDeleteKeyNT(HKEY_LOCAL_MACHINE, defaultIconBase) == ERROR_SUCCESS);
	}
	else
	{
		return Registry::RegSetSimpleStringW(
			HKEY_LOCAL_MACHINE,
			defaultIconBase,
			L"",
			canonicalIconPath.c_str(),
			false
			) ? true : false;
	}
}

HRESULT	GetFolderDescriptionId(
	__in LPCWSTR pszPath,
	__out SHDESCRIPTIONID *pdid
	)
{
	HRESULT hr;
	LPITEMIDLIST pidl;
	if (SUCCEEDED(hr = SHParseDisplayName(pszPath, NULL, &pidl, 0, NULL)))
	{
		IShellFolder *psf;
		LPCITEMIDLIST pidlChild;
		if (SUCCEEDED(hr = SHBindToParent(pidl, IID_IShellFolder, (void**)&psf, &pidlChild)))
		{
			hr = SHGetDataFromIDList(psf, pidlChild, SHGDFIL_DESCRIPTIONID, pdid, sizeof(*pdid));
			psf->Release();
		}
		CoTaskMemFree(pidl);
	}
	return hr;
}

bool IsRecycleBin(const std::wstring& path)
{
	SHDESCRIPTIONID did;
	return (SUCCEEDED(GetFolderDescriptionId(path.c_str(), &did)) && did.clsid == CLSID_RecycleBin);
}

void MarkShortcutRunAs(const std::wstring& sShortcut)
{
	CoInitialize(NULL);

	do
	{
		CComPtr<IPersistFile> sppf;
		if (FAILED(sppf.CoCreateInstance(CLSID_ShellLink)))
		{
			break;
		}
		if (FAILED(sppf->Load(sShortcut.c_str(), STGM_READWRITE)))
		{
			break;
		}
		CComQIPtr<IShellLinkDataList> spdl(sppf);
		if (!spdl)
		{
			break;
		}
		DWORD dwFlags;
		if (FAILED(spdl->GetFlags(&dwFlags)))
		{
			break;
		}
		dwFlags |= SLDF_RUNAS_USER;
		if (FAILED(spdl->SetFlags(dwFlags)))
		{
			break;
		}
		if (FAILED(sppf->Save(NULL, TRUE)))
		{
			break;
		}
#pragma warning(push)
#pragma warning(disable:4127)
	} while (0);
#pragma warning(pop)
	
	CoUninitialize();
}

void BroadcastDeviceChange(WPARAM message, int nDosDriveNo, DWORD driveMap)
{
	DEV_BROADCAST_VOLUME dbv;
	DWORD_PTR dwResult;
	LONG eventId = 0;

	if (message == DBT_DEVICEARRIVAL)
	{
		eventId = SHCNE_DRIVEADD;
	}
	else if (message == DBT_DEVICEREMOVECOMPLETE)
	{
		eventId = SHCNE_DRIVEREMOVED;
	}
	else if (IsWindows7() && message == DBT_DEVICEREMOVEPENDING)
	{
		// Explorer on Windows 7 holds open handles of all drives when 'Computer' is expanded in navigation pane.
		// SHCNE_DRIVEREMOVED must be used as DBT_DEVICEREMOVEPENDING is ignored.
		eventId = SHCNE_DRIVEREMOVED;
	}

	if (driveMap == 0)
	{
		driveMap = (1 << nDosDriveNo);
	}

	if (eventId != 0)
	{
		int i;
		for (i = 0; i < 26; i++)
		{
			if (driveMap & (1 << i))
			{
				char root[] = { (char) i + 'A', ':', '\\', 0 };
				SHChangeNotify(eventId, SHCNF_PATH, root, NULL);

				// We don't need to support Windows 2000

				//if (nCurrentOS == WIN_2000 && RemoteSession)
				//{
				//    char target[32];
				//    //wsprintf (target, "%ls%c", TC_MOUNT_PREFIX, i + 'A');
				//    root[2] = 0;

				//    if (message == DBT_DEVICEARRIVAL)
				//        DefineDosDevice (DDD_RAW_TARGET_PATH, root, target);
				//    else if (message == DBT_DEVICEREMOVECOMPLETE)
				//        DefineDosDevice (DDD_RAW_TARGET_PATH| DDD_REMOVE_DEFINITION
				//        | DDD_EXACT_MATCH_ON_REMOVE, root, target);
				//}
			}
		}
	}

	dbv.dbcv_size = sizeof (dbv);
	dbv.dbcv_devicetype = DBT_DEVTYP_VOLUME;
	dbv.dbcv_reserved = 0;
	dbv.dbcv_unitmask = driveMap;
	dbv.dbcv_flags = 0;

	UINT timeOut = 1000;

	// SHChangeNotify() works on Vista, so the Explorer does not require WM_DEVICECHANGE
	if (IsVistaOrLater())
	{
		timeOut = 100;
	}

	SendMessageTimeout(HWND_BROADCAST, WM_DEVICECHANGE, message, (LPARAM)(&dbv), SMTO_ABORTIFHUNG, timeOut, &dwResult);

	// Explorer prior Vista sometimes fails to register a new drive
	if (!IsVistaOrLater() && message == DBT_DEVICEARRIVAL)
	{
		SendMessageTimeout(HWND_BROADCAST, WM_DEVICECHANGE, message, (LPARAM)(&dbv), SMTO_ABORTIFHUNG, 200, &dwResult);
	}
}
