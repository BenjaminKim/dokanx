#pragma once

#pragma warning(push)
#pragma warning(disable:4995)
#include <Windows.h>
#include <string>
#pragma warning(pop)

std::string ToAnsiStr(__in LPCWSTR uniStr);
std::wstring ToUniStr(__in LPCSTR ansiStr);
std::wstring ToWideCharString(__in LPCSTR pszAnsiStr);
std::wstring UTF8ToUTF16(__in LPCSTR pszTextUTF8);
std::string UTF16ToUTF8(__in LPCWSTR pszTextUTF16);
std::string AnsiToUTF8(__in const std::string& strAnsi);
std::string UTF8ToAnsi(__in const std::string& sUtf8);
int Utf8CharCount(const char* pStr);