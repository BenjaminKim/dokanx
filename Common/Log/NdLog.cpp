#include "NdLog.h"

#pragma warning(push)
#pragma warning(disable:4995)
#include <atlbase.h>
#pragma warning(pop)

#include "../misc.h"
#include "../HelperMacros.h"
#include "../String/UnicodeHelper.h"
#include "../WinNT/Registry.h"

#ifdef _DEBUG
enum LOG_OUTPUT_FLAG g_OutputLogFlag = LOF_DEBUG_VIEW;
#else
enum LOG_OUTPUT_FLAG g_OutputLogFlag = LOF_NULL;
#endif

/*
	static variables
*/
static HANDLE s_hLogMutex = INVALID_HANDLE_VALUE;

VOID NdLogVar(LOG_OUTPUT_FLAG outputLogFlag, __in __format_string LPCWSTR fmt, ...)
{
	va_list argList;
	va_start(argList, fmt);
	WCHAR szMsg[4096];
	StringCchVPrintfW(szMsg, _countof(szMsg), fmt, argList);
	va_end(argList);

	if (outputLogFlag & LOF_DEBUG_VIEW)
	{
		OutputDebugStringW(szMsg);
	}

	if (outputLogFlag & LOF_PROCESS_MONITOR)
	{
		// ProcMonDebugOutput(szMsg);
	}

	if (outputLogFlag & LOF_FILE)
	{
		//NdFileLog(szMsg);
	}
}

#define _ENABLE_PERF_LOG
#include <time.h>

std::wstring GetWin32FormatMessage(DWORD dwWin32Error)
{
	HLOCAL hlocal = NULL;
	BOOL fOk = FormatMessage(
		FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER,
		NULL,
		dwWin32Error,
		MAKELANGID(LANG_KOREAN, SUBLANG_ENGLISH_US),
		(LPTSTR)&hlocal,
		0,
		NULL
		);

	std::wstring sFormatMessage;
	if (fOk)
	{
		sFormatMessage = (PCTSTR)LocalLock(hlocal);
	}
	return sFormatMessage;
}

VOID PrintFormatMessage(DWORD dwWin32Error)
{
	OutputDebugString(GetWin32FormatMessage(dwWin32Error).c_str());
}

VOID PrintFormatMessage_Dbg(DWORD dwWin32Error)
{
#ifdef _DEBUG
	PrintFormatMessage(dwWin32Error);
#else
	UNREFERENCED_PARAMETER(dwWin32Error);
#endif //_DEBUG
}

VOID NdPerfLog(__in LPCWSTR logfile, __in __format_string LPCWSTR  fmt, ...)
{
	va_list argList;
	va_start(argList, fmt);	
	WCHAR szMsg[4096];
	StringCchVPrintfW(szMsg, _countof(szMsg), fmt, argList);
	va_end(argList);

	MSG(Add synchronization);
	HANDLE hFile = CreateFile(
		logfile,
		GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,
		OPEN_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL
		);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		_stprintf_s(szMsg, L"Cannot create a log file. [errcode: %d][%s]\n", GetLastError(), logfile);
		OutputDebugString(szMsg);
	}
	else
	{
		(VOID)SetFilePointer(hFile, 0, 0, FILE_END);
		std::string s;
		s.append(ToAnsiStr(GetCurrentTimeStr().c_str()));
		s.append(" ");
		s.append(ToAnsiStr(szMsg));
		s.append("");
		DWORD dwWritten;
		(VOID)WriteFile(hFile, s.c_str(), (DWORD)s.size(), &dwWritten, 0);
		CloseHandle(hFile);
	}
}
