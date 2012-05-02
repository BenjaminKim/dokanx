#include "NdLog.h" // NS

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

//VOID CheckFileLogOption()
//{
//	DWORD logEnabled = Registry::RegGetSimpleDword(HKEY_CURRENT_USER, NDRIVE_ROOT_REG_KEY, L"logopt", 0);
//
//	if (logEnabled == 0x1234 || logEnabled == 1)
//	{
//		g_OutputLogFlag = static_cast<LOG_OUTPUT_FLAG>(g_OutputLogFlag | LOF_FILE);
//	}
//	else if (logEnabled == 2)
//	{
//		g_OutputLogFlag = static_cast<LOG_OUTPUT_FLAG>(g_OutputLogFlag | LOF_DEBUG_VIEW);
//	}
//	else if (logEnabled == 3)
//	{
//		g_OutputLogFlag = static_cast<LOG_OUTPUT_FLAG>(g_OutputLogFlag | (LOF_DEBUG_VIEW | LOF_FILE));
//	}
//}

//std::wstring GetLogFilePath()
//{
//	SYSTEMTIME st;
//	WCHAR szTempFolder[MAX_PATH];
//	WCHAR szLogFilePath[MAX_PATH];
//
//	GetSystemTime(&st);	
//
//	NdGetTempFolder(szTempFolder, _countof(szTempFolder));
//	StringCchPrintf(szLogFilePath, _countof(szLogFilePath),
//		L"%s\\%04d%02d%02d.log", szTempFolder, st.wYear, st.wMonth, st.wDay);
//
//	return szLogFilePath;
//}

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
//
//VOID NdFileLog(__in LPCWSTR lpszLogMessage)
//{
//	if (s_hLogMutex == INVALID_HANDLE_VALUE)
//	{
//		s_hLogMutex = CreateMutexW(NULL, FALSE, L"NDLogLock");
//		if (s_hLogMutex == nullptr)
//		{
//			return;
//		}
//	}
//
//	WCHAR msg[4096];
//
//	LONG tick = GetTickCount();
//	StringCchPrintf(msg, _countof(msg), L"%u %s", tick, lpszLogMessage);
//
//#ifdef _DEBUG
//	DWORD dwRet = WaitForSingleObject(s_hLogMutex, 10000);
//#else
//	DWORD dwRet = WaitForSingleObject(s_hLogMutex, INFINITE);
//#endif
//	if (dwRet == WAIT_OBJECT_0)
//	{
//		HANDLE hFile = CreateFile(
//			sLogPath.c_str(),
//			GENERIC_WRITE,
//			FILE_SHARE_READ | FILE_SHARE_WRITE,
//			NULL,
//			OPEN_ALWAYS,
//			FILE_ATTRIBUTE_ARCHIVE,
//			NULL
//			);
//		if (hFile == INVALID_HANDLE_VALUE)
//		{
//			_stprintf_s(msg, L"Cannot create a log file. [errcode: %d][%s]\n", GetLastError(), sLogPath.c_str());
//			OutputDebugString(msg);
//		}
//		else
//		{
//			(VOID)SetFilePointer(hFile, 0, 0, FILE_END);
//			std::string s = ToAnsiStr(msg);
//			DWORD dwWritten;
//			(VOID)WriteFile(hFile, s.c_str(), (DWORD)s.size(), &dwWritten, 0);
//			CloseHandle(hFile);
//		}
//		ReleaseMutex(s_hLogMutex);
//	}
//	else
//	{
//		_ASSERT(FALSE);
//	}
//}

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

	//MSG(동기화 추가);
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
