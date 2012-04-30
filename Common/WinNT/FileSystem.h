#pragma once

#include <Windows.h>
#pragma warning(push)
#pragma warning(disable:4995)
#include <string>
#include <vector>
#pragma warning(pop)

BOOL FileHasReadOnlyAttribute(
	__in const WCHAR* path
	);
BOOL IsFileOnReadOnlyFilesystem(
	__in const WCHAR *path
	);
BOOL IsFileAlreadyOpenedByAnotherProcess(
	__in const std::wstring& sPath
	);
DWORD MoveFileOverwriteIf(
	__in const std::wstring& sSrcFile,
	__in const std::wstring& sDestFile
	);
DWORD GetFileList(
	__in const std::wstring& canonicalPath,
	__out std::vector<WIN32_FIND_DATA>* pFileList
	);