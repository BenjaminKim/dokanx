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
// 잡다한 함수들을 모아놓는 곳이다. 아직 관련한 함수들의 덩어리들이 너무 작기 때문에 새로운 모듈을 추가하는 것이 부담스러울 경우가 있다.
// 그런 함수들은 이 곳에 잠시 넣어뒀다가 나중에 어느 정도 적당한 모임이 생기게 되면 새로운 모듈로 분리해내야 한다.
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