#include "misc.h"
#include <sstream>
#include <iomanip>
#include <strsafe.h>
#pragma warning(push)
#pragma warning(disable:4995)
#include <regex>
#pragma warning(pop)

VOID TrimMilliSeconds(LONGLONG* pTime)
{
	*pTime = *pTime / 10000000 * 10000000;
}

VOID TrimFileTime(__inout FILETIME* ft)
{
	LARGE_INTEGER li;
	li.HighPart        = ft->dwHighDateTime;
	li.LowPart         = ft->dwLowDateTime;
	TrimMilliSeconds(&li.QuadPart);
	ft->dwHighDateTime = li.HighPart;
	ft->dwLowDateTime  = li.LowPart;
}

FILETIME TrimFileTime(__in const FILETIME& ft)
{
	LARGE_INTEGER li;
	li.HighPart = ft.dwHighDateTime;
	li.LowPart = ft.dwLowDateTime;
	TrimMilliSeconds(&li.QuadPart);
	FILETIME trimmedTime;
	trimmedTime.dwHighDateTime = li.HighPart;
	trimmedTime.dwLowDateTime = li.LowPart;
	return trimmedTime;
}

std::wstring GetCurrentTimeStr()
{
	SYSTEMTIME cTime;
	::GetLocalTime(&cTime);

	std::wstringstream stm;
	stm << std::setfill(L'0') << 
		cTime.wYear << L"/" <<
		std::setw(2) << cTime.wMonth << L"/" <<
		std::setw(2) << cTime.wDay << L" " <<
		std::setw(2) << cTime.wHour << L":" <<
		std::setw(2) << cTime.wMinute << L":" <<
		std::setw(2) << cTime.wSecond << L"." <<
		std::setw(3) << cTime.wMilliseconds;
	return stm.str();
}

FILETIME CurrentTime()
{
	FILETIME ft;
	SYSTEMTIME st;

	GetSystemTime(&st);              // Gets the current system time
	SystemTimeToFileTime(&st, &ft);  // Converts the current system time to file time format

	FILETIME localFileTime;
	FileTimeToLocalFileTime(&ft, &localFileTime);

	return localFileTime;
}

time_t CurrentTime_t()
{
	time_t current;
	time(&current);
	return current;
}

std::wstring FileTimeToHumanReadableTime(
	__in FILETIME fileTime
	)
{
	SYSTEMTIME localTime;
	if (FileTimeToSystemTime(&fileTime, &localTime))
	{
		WCHAR sz[48];
		HRESULT hr = StringCchPrintf(
			sz,
			_countof(sz),
			L"%04d-%02d-%02dT%02d:%02d:%02d",
			localTime.wYear, localTime.wMonth, localTime.wDay, localTime.wHour, localTime.wMinute, localTime.wSecond
			);

		if (SUCCEEDED(hr))
		{
			return sz;
		}
	}

	return L"";
}

VOID BuildSecurityAttributes(PSECURITY_ATTRIBUTES SecurityAttributes)
{
	LPTSTR sd = L"D:P(A;;GA;;;SY)(A;;GRGWGX;;;BA)(A;;GRGW;;;WD)(A;;GR;;;RC)";

	ZeroMemory(SecurityAttributes, sizeof(SECURITY_ATTRIBUTES));

	ConvertStringSecurityDescriptorToSecurityDescriptor(
		sd,
		SDDL_REVISION_1,
		&SecurityAttributes->lpSecurityDescriptor,
		NULL);

	SecurityAttributes->nLength = sizeof(SECURITY_ATTRIBUTES);
	SecurityAttributes->bInheritHandle = TRUE;
}

HANDLE OpenNamedPipeHandle(
	__in const std::wstring& pipeName,
	__in DWORD dwTimeout
	)
{
	HANDLE hPipe = INVALID_HANDLE_VALUE;

	for ( ; ; )
	{
		hPipe = CreateFile(pipeName.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
		if (hPipe != INVALID_HANDLE_VALUE)
		{
			break;
		}

		if (!WaitNamedPipe(pipeName.c_str(), dwTimeout))
		{
			break;
		}
	}

	return hPipe;
}

bool MsOfficeTmpFileForJournalling(const std::wstring& sFileName)
{
	std::wregex r(L"^[a-zA-Z0-9]+\\.tmp", std::tr1::regex_constants::icase);
	return std::regex_match(sFileName.begin(), sFileName.end(), r);
}

bool MsOfficeTmpFileForSharing(const std::wstring& sFileName)
{
	std::wregex r(L"^~\\$.+\\.(doc|xls|ppt)x?", std::tr1::regex_constants::icase);
	return std::regex_match(sFileName.begin(), sFileName.end(), r);
}

bool IsTemporaryFileNameMsOfficeMakes(const std::wstring& sFileName)
{
	return (MsOfficeTmpFileForJournalling(sFileName) || MsOfficeTmpFileForSharing(sFileName));
}