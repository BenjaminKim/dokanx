#pragma once

#include <Windows.h>

#pragma warning(push)
#pragma warning(disable:4995)
#include <string>
#include <vector>
#include <atlstr.h>
#pragma warning(pop)

enum FileSizeFlags
{
	FILESIZE_BYTE =		0x0001,
	FILESIZE_KBYTE =	0x0002,
	FILESIZE_MBYTE =	0x0003,
	FILESIZE_GBYTE =	0x0004,
	FILESIZE_TBYTE =	0x0005,		
};

CString ReplaceBackslashToSlash(__in CONST CString& strPath);
CString ReplaceSlashToBackslash(__in CONST CString& strPath);
CString GetResString(UINT uStringID, HINSTANCE hResource);
CString ConvertToHumanReadableSize(INT64 jSize, UINT nDefault = FILESIZE_BYTE);
CString ConvertToHumanReadableSize(CONST CString& strSize, UINT nDefault = FILESIZE_BYTE);