#include "FileSystem.h"

#include <Windows.h>

//BOOL FileHasReadOnlyAttribute(
//	__in const WCHAR* path
//	)
//{
//	DWORD attributes = GetFileAttributesW(path);
//	return attributes != INVALID_FILE_ATTRIBUTES && (attributes & FILE_ATTRIBUTE_READONLY) != 0;
//}
//
//BOOL IsFileOnReadOnlyFilesystem(
//	__in const WCHAR *path
//	)
//{
//	WCHAR root[MAX_PATH];
//	if (!GetVolumePathNameW(path, root, _countof(root)))
//	{
//		return FALSE;
//	}
//
//	DWORD flags, d;
//	if (!GetVolumeInformationW(root, NULL, 0,  NULL, &d, &flags, NULL, 0))
//	{
//		return FALSE;
//	}
//
//	return (flags & FILE_READ_ONLY_VOLUME) ? TRUE : FALSE;
//}

//DWORD MoveFileOverwriteIf(
//	__in const std::wstring& sSrcFile,
//	__in const std::wstring& sDestFile
//	)
//{
//	DWORD dwError;
//	DWORD srcAttributes = GetFileAttributes(sSrcFile.c_str());
//	if (srcAttributes & FILE_ATTRIBUTE_DIRECTORY)
//	{
//		DWORD destAttributes = GetFileAttributes(sDestFile.c_str());
//		if (destAttributes == INVALID_FILE_ATTRIBUTES)
//		{
//			if (!CreateDirectory(sDestFile.c_str(), NULL))
//			{
//				dwError = GetLastError();
//				return dwError;
//			}
//		}
//
//		BOOL fOk = RemoveDirectory(sSrcFile.c_str());
//		if (fOk)
//		{
//			if (!CreateDirectory(sDestFile.c_str(), NULL))
//			{
//				dwError = GetLastError();
//				if (dwError != ERROR_ALREADY_EXISTS && dwError != ERROR_SUCCESS)
//				{
//					return dwError;
//				}
//			}
//		}
//		else
//		{
//			dwError = GetLastError();
//			if (dwError == ERROR_DIR_NOT_EMPTY)
//			{
//				WIN32_FIND_DATA findData;
//				HANDLE hFindFile = FindFirstFile((sSrcFile + L"\\*").c_str(), &findData);
//				std::vector<WIN32_FIND_DATA> vFiles;
//				if (hFindFile != INVALID_HANDLE_VALUE)
//				{
//					if (_wcsicmp(findData.cFileName, L".") != 0 &&
//						_wcsicmp(findData.cFileName, L"..") != 0)
//					{
//						vFiles.push_back(findData);
//					}
//
//					while (FindNextFile(hFindFile, &findData))
//					{
//						if (_wcsicmp(findData.cFileName, L".") != 0 &&
//							_wcsicmp(findData.cFileName, L"..") != 0)
//						{
//							vFiles.push_back(findData);
//						}
//					}
//
//					FindClose(hFindFile);
//				}
//				else
//				{
//					dwError = GetLastError();
//					return dwError;
//				}
//
//				for (size_t i = 0; i < vFiles.size(); ++i)
//				{
//					dwError = MoveFileOverwriteIf(
//						sSrcFile + L"\\" + vFiles[i].cFileName,
//						sDestFile + L"\\" + vFiles[i].cFileName
//						);
//					if (dwError != ERROR_SUCCESS)
//					{
//						return dwError;
//					}
//				}
//			}
//
//			fOk = RemoveDirectory(sSrcFile.c_str());
//			if (fOk)
//			{
//				return ERROR_SUCCESS;
//			}
//			else
//			{
//				return GetLastError();
//			}
//		}
//	}
//	else
//	{
//		if (srcAttributes == INVALID_FILE_ATTRIBUTES)
//		{
//			dwError = GetLastError();
//			return dwError;
//		}
//
//		if (!MoveFileEx(sSrcFile.c_str(), sDestFile.c_str(), MOVEFILE_REPLACE_EXISTING))
//		{
//			dwError = GetLastError();
//			return dwError;
//		}
//	}
//
//	return ERROR_SUCCESS;
//}

BOOL IsFileAlreadyOpenedByAnotherProcess(
	__in const std::wstring& sPath
	)
{
	HANDLE hFile = CreateFile(sPath.c_str(), GENERIC_WRITE, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
	if(hFile != INVALID_HANDLE_VALUE)
	{
		CloseHandle(hFile);
		return FALSE;
	}
	else
	{
		DWORD dwErr = GetLastError();
		if(dwErr == ERROR_FILE_NOT_FOUND || dwErr == ERROR_PATH_NOT_FOUND)
		{
			return FALSE;
		}
	}

	return TRUE;
}

DWORD GetFileList(
	__in const std::wstring& canonicalPath,
	__out std::vector<WIN32_FIND_DATA>* pFileList
	)
{
	WIN32_FIND_DATA findData;
	HANDLE hFindFile = FindFirstFile((canonicalPath + L"\\*").c_str(), &findData);
	if (hFindFile != INVALID_HANDLE_VALUE)
	{
		if (_wcsicmp(findData.cFileName, L".") != 0 &&
			_wcsicmp(findData.cFileName, L"..") != 0)
		{
			pFileList->push_back(findData);
		}

		while (FindNextFile(hFindFile, &findData))
		{
			if (_wcsicmp(findData.cFileName, L".") != 0 &&
				_wcsicmp(findData.cFileName, L"..") != 0)
			{
				pFileList->push_back(findData);
			}
		}

		FindClose(hFindFile);
	}
	else
	{
		return GetLastError();
	}

	return ERROR_SUCCESS;
}