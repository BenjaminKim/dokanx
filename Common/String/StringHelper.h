#pragma once
#include <Windows.h>

#pragma warning(push)
#pragma warning(disable:4995)
#include <string>
#include <vector>
#include <sstream>
#pragma warning(pop)

std::wstring AddComma(const std::wstring& source);
std::wstring AddComma(INT64 jNumber);
size_t SplitStringW(
	__in LPCWSTR str,
	__in LPCWSTR delim,
	__out std::vector<std::wstring>& results
	);
std::string& ReplaceinA(std::string &s, const std::string &sub, const std::string &other);
std::wstring& ReplaceinW(std::wstring &s, const std::wstring &sub, const std::wstring &other);
#if defined(UNICODE) || defined(_UNICODE)
#define Replacein	ReplaceinW
#else
#define Replacein	ReplaceinA
#endif

void SelfGrow( std::string& s, size_t appendlength );
std::wstring BasePath(const std::wstring& sBinaryPath);
std::wstring FileName(const std::wstring& sBinaryPath);
bool StartWith(const std::wstring& fullStr, const std::wstring& startStr, BOOL fIgnoreCase);
std::string TrimLeft(const std::string& str);
std::string TrimRight(const std::string& str);
std::string Trim(const std::string& str);
std::wstring TrimLeftW(const std::wstring& str);
std::wstring TrimRightW(const std::wstring& str);
std::wstring TrimW(const std::wstring& str);
bool Compare(const std::string& origin, const std::string& dest);
//Function to convert a Hex string of length 2 to an unsigned char
bool Hex2Char(CHAR const* szHex, UCHAR& rch);
std::wstring TrimLastRightPathSeperator(
	__in const std::wstring& str,
	__in_opt WCHAR pathSeperator = L'/'
	);
std::string & TrimBothSide(std::string &str);

void SHTokenizeA( const std::string& target, std::vector<std::string>& tokens, const std::string &delim );
void SHTokenizeW( const std::wstring& target, std::vector<std::wstring>& tokens, const std::wstring &delim );
#if defined(UNICODE) || defined(_UNICODE)
#define SHTokenize	SHTokenizeW
#else
#define SHTokenize	SHTokenizeA
#endif

int StringAToInt( const std::string &str );
int StringWToInt( const std::wstring &str );
#if defined(UNICODE) || defined(_UNICODE)
#define StringToInt	StringWToInt
#else
#define StringToInt	StringAToInt
#endif

std::wstring IntegerToStr(LONGLONG num);
std::wstring AbbreviateFileName(__in const std::wstring& ntFileName, __in size_t cchMaxFileName);
std::wstring AbbreviateString(__in const std::wstring& sString, __in size_t cchMaxLen);
std::wstring AppendPathSeperatorIfNotExist(
	__in const std::wstring& path,
	__in WCHAR pathSeperator = L'/'
	);
std::wstring AddTrailBackSlashIfNotExist(__in const std::wstring& path);
std::wstring AddFrontSlashIfNotExist(__in const std::wstring& path);
std::wstring AddFrontAndRearSlashIfNotExist(__in const std::wstring& path);
std::string Hexify(__in const std::vector<unsigned char>& v);
std::wstring ToLower(__in const std::wstring &ori);
std::wstring ToLower(__in LPCWSTR ori);
bool CiStringCompare(const std::wstring& s1, const std::wstring& s2);
bool StringEqualNoCase(const std::wstring& s1, const std::wstring& s2);
std::wstring ConvertNtPathToUnixPath(const std::wstring& sNtPath);
std::wstring ConvertUnixPathToNtPath(const std::wstring& sNdPath, WCHAR chVolume);
std::wstring __cdecl FormatV(PCWSTR pszFormat, ...);