#pragma once
#include <Windows.h>
#include <WinBase.h>
#pragma warning(push)
#pragma warning(disable:4995)
#include <string>
#pragma warning(pop)
#include <Sddl.h>
#include <tchar.h>

#if !defined(tstring)
#define tstring	std::basic_string<TCHAR>
#endif

//
// Miscellaneous functions.
//

VOID TrimFileTime(__inout FILETIME* ft);
FILETIME TrimFileTime(__in const FILETIME& ft);
std::wstring GetCurrentTimeStr();
FILETIME CurrentTime();
time_t CurrentTime_t();
std::wstring FileTimeToHumanReadableTime(
	__in FILETIME fileTime
	);
VOID BuildSecurityAttributes(PSECURITY_ATTRIBUTES SecurityAttributes);
HANDLE OpenNamedPipeHandle(
	__in const std::wstring& pipeName,
	__in DWORD dwTimeout
	);
bool MsOfficeTmpFileForJournalling(const std::wstring& sFileName);
bool MsOfficeTmpFileForSharing(const std::wstring& sFileName);
bool IsTemporaryFileNameMsOfficeMakes(const std::wstring& sFileName);
