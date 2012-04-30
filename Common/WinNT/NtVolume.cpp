#include "NtVolume.h"

#include <tchar.h>

int GetFirstAvailableDrive()
{
	DWORD dwUsedDrives = GetLogicalDrives();
	int i;

	for (i = 3; i < 26; i++)
	{
		if (!(dwUsedDrives & 1 << i))
		{
			return i;
		}
	}

	return -1;
}

int GetLastAvailableDrive()
{
	DWORD dwUsedDrives = GetLogicalDrives();
	int i;

	for (i = 25; i > 2; i--)
	{
		if (!(dwUsedDrives & 1 << i))
		{
			return i;
		}
	}

	return -1;
}

std::vector<WCHAR> GetAvailableVolumes()
{
	std::vector<WCHAR> av;
	DWORD dwUsedDrives = GetLogicalDrives();
	for (WCHAR i = 3; i < 26; i++)
	{
		if (!(dwUsedDrives & (1 << i)))
		{
			av.push_back(i + L'A');
		}
	}

	return av;
}

bool IsDriveAvailable(int driveNo)
{
	return (GetLogicalDrives() & (1 << driveNo)) == 0;
}

bool IsVolumeDesignatorAvailable(WCHAR chVolume)
{
	return IsDriveAvailable(towupper(chVolume) - L'A');
}

bool IsDeviceMounted(WCHAR* deviceName)
{
	BOOL bResult = FALSE;
	DWORD dwResult;
	HANDLE dev = INVALID_HANDLE_VALUE;

	if ((dev = CreateFileW(deviceName,
		GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,
		OPEN_EXISTING,
		0,
		NULL)) != INVALID_HANDLE_VALUE)
	{
		bResult = DeviceIoControl(dev, FSCTL_IS_VOLUME_MOUNTED, NULL, 0, NULL, 0, &dwResult, NULL);
		CloseHandle (dev);
	}

	return bResult ? true : false;
}

bool GetDriveLabel(int driveNo, wchar_t *label, int labelSize)
{
	DWORD fileSystemFlags;
	wchar_t root[] = { L'A' + (wchar_t) driveNo, L':', L'\\', 0 };

	return GetVolumeInformationW(root, label, labelSize / 2, NULL, NULL, &fileSystemFlags, NULL, 0) ? true : false;
}


/* Callback command types */
typedef enum
{
	PROGRESS = 0,
	DONEWITHSTRUCTURE,
	UNKNOWN2,
	UNKNOWN3,
	UNKNOWN4,
	UNKNOWN5,
	INSUFFICIENTRIGHTS,
	FSNOTSUPPORTED,
	VOLUMEINUSE,
	UNKNOWN9,
	UNKNOWNA,
	DONE,
	FMIFS_HARDDISK,
	UNKNOWND,
	OUTPUT,
	STRUCTUREPROGRESS,
	CLUSTERSIZETOOSMALL,
} CALLBACKCOMMAND;

// FMIFS
typedef BOOLEAN (__stdcall *PFMIFSCALLBACK)( int command, DWORD subCommand, PVOID parameter ); 
typedef VOID (__stdcall *PFORMATEX)(PWCHAR DriveRoot, DWORD MediaFlag, PWCHAR Format, PWCHAR Label,
									BOOL QuickFormat, DWORD ClusterSize, PFMIFSCALLBACK Callback );
volatile BOOLEAN FormatExResult;
PUSHORT g_pFormatProgressCount = 0;

BOOLEAN __stdcall FormatExCallback(int command, DWORD /*subCommand*/, PVOID parameter)
{
	switch(command)
	{
	   case STRUCTUREPROGRESS:
			if(g_pFormatProgressCount)
			{
				++(*g_pFormatProgressCount);
			}
			break;

	   case DONE:
		   FormatExResult = *(BOOLEAN *) parameter;
		   return TRUE;
	}

	return TRUE;
}

//
// Requires a symbolic link in GLOBAL namespace.
//
BOOL FormatNtfs(LPCWSTR pszVolume, int clusterSize, __out_opt PUSHORT pProgressCount)
{
	PFORMATEX FormatEx;
	HMODULE hDllInstance = LoadLibraryA("fmifs.dll");
	int i;
	WCHAR szVolume[MAX_PATH] = { 0 };
	_tcscpy_s(szVolume, pszVolume);

	if (hDllInstance == NULL)
	{
		return FALSE;
	}

	HMODULE hModule = GetModuleHandle(L"fmifs.dll");
	if (hModule == NULL)
	{
		FreeLibrary(hDllInstance);
		return FALSE;
	}

	if ((FormatEx = (PFORMATEX)GetProcAddress(hModule, "FormatEx")) == 0)
	{
		FreeLibrary(hDllInstance);
		return FALSE;
	}

	FormatExResult = FALSE;
	
	// Windows sometimes fails to format a volume (hosted on a removable medium) as NTFS.
	// It often helps to retry several times.
	for (i = 0; i < 50 && FormatExResult != TRUE; i++)
	{
		if(pProgressCount)
		{
			*pProgressCount = 0;
			g_pFormatProgressCount = pProgressCount;            
		}
		FormatEx(szVolume, FMIFS_HARDDISK, L"NTFS", L"", TRUE, clusterSize * 512, FormatExCallback);
	}

	// The device may be referenced for some time after FormatEx() returns
	Sleep (2000);

	FreeLibrary (hModule);
	return FormatExResult;
}