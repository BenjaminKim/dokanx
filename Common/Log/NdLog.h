#pragma once

#include <Windows.h>
#include <tchar.h>
#include <strsafe.h>

#pragma warning(push)
#pragma warning(disable:4995)
#include <string>
#pragma warning(pop)

#include "../HelperMacros.h"

enum LOG_OUTPUT_FLAG
{
	LOF_NULL				= 0x00,
	LOF_DEBUG_VIEW			= 0x01,
	LOF_FILE				= 0x02,
	LOF_PROCESS_MONITOR		= 0x04,
};

extern enum LOG_OUTPUT_FLAG g_OutputLogFlag;


VOID NdLogVar(LOG_OUTPUT_FLAG outputLogFlag, __in __format_string LPCWSTR fmt, ...);
VOID NdFileLog(__in LPCWSTR lpszLogMessage);
VOID NdPerfLog(__in LPCWSTR logfile, __in __format_string LPCWSTR  fmt, ...);

inline VOID SetOutputLogFlag(LOG_OUTPUT_FLAG flag)
{
	g_OutputLogFlag = flag;
}

#if !defined(_MODULE_NAME)
#define _MODULE_NAME L"Unnamed"
#endif

#define logbasic(outputMode, fmt, ...) \
	do { \
	NdLogVar(outputMode, L"[%04d]" _MODULE_NAME L"!" __FUNCTIONW__ L": " fmt L"\n", GetCurrentThreadId(), __VA_ARGS__);\
	__pragma (warning(push)) \
	__pragma (warning(disable:4127)) \
	} while (0) \
	__pragma (warning(pop))

#if defined(_DISABLE_LOGGING)
#define logw(fmt, ...) \
	__noop(fmt, __VA_ARGS__); \
	UNREFERENCED_PARAMETERS(__VA_ARGS__)
#else
#define logw(fmt, ...) logbasic(g_OutputLogFlag, fmt, __VA_ARGS__);
#endif

#define logf(fmt, ...) logbasic(static_cast<LOG_OUTPUT_FLAG>(LOF_DEBUG_VIEW | LOF_FILE), fmt, __VA_ARGS__);

#define logd(fmt, ...) logbasic(static_cast<LOG_OUTPUT_FLAG>(LOF_DEBUG_VIEW), fmt, __VA_ARGS__);

std::wstring GetWin32FormatMessage(DWORD dwWin32Error);
VOID PrintFormatMessage(DWORD dwWin32Error);
VOID PrintFormatMessage_Dbg(DWORD dwWin32Error);
