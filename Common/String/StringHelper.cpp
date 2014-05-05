#include "StringHelper.h"
#include <tchar.h>
#include <strsafe.h>
#include <math.h>
#include <sstream>
#include <stdarg.h>
#include <algorithm>
#include <utility>

using namespace std;

wstring AddComma(const wstring& source)
{
	PWCHAR pBuf = (PWCHAR)malloc(source.size() * 4);
	if (pBuf == NULL)
	{
		return L"";
	}

	size_t len, i, j;
	for (len = source.size(), i = 0, j = 0; i < len; i++, j++)
	{
		if (!((len - i) % 3) && i != 0)
		{
			if (j > 0 && pBuf[j - 1] != L'-')
			{
				pBuf[j++] = L',';
			}
		}

		pBuf[j] = source[i];
	}
	pBuf[j] = 0;

	wstring s(pBuf);
	free(pBuf);
	return s;
}

wstring AddComma(INT64 jNumber)
{
	return AddComma(FormatV(L"%I64d", jNumber));
}

size_t SplitStringW(
	__in LPCWSTR str,
	__in LPCWSTR delim,
	__out vector<wstring>& results
	)
{
	WCHAR* pstr = const_cast<WCHAR*>(str);
	WCHAR* r = NULL;
	r = wcsstr(pstr, delim);
	size_t dlen = wcslen(delim);
	while( r != NULL )
	{
		WCHAR* cp = new WCHAR[(r - pstr) + 1];
		memcpy(cp, pstr, (r - pstr) * sizeof(WCHAR));
		cp[(r - pstr)] = L'\0';
		if( wcslen(cp) > 0 )
		{
			wstring s(cp);
			results.push_back(s);
		}
		delete[] cp;
		pstr = r + dlen;
		r = wcsstr(pstr, delim);
	}
	if( wcslen(pstr) > 0 )
	{
		results.push_back(wstring(pstr));
	}
	return results.size();
}

string& ReplaceinA(string& s, const string& sub, const string& other)
{
	_ASSERT(!sub.empty());
	size_t b = 0;
	for ( ; ; )
	{
		b = s.find(sub, b);
		if (b == s.npos)
		{
			break;
		}
		s.replace(b, sub.size(), other);
		b += other.size();
	}
	return s;
}

wstring& ReplaceinW(wstring& s, const wstring& sub, const wstring& other)
{
	_ASSERT(!sub.empty());
	size_t b = 0;
	for ( ; ; )
	{
		b = s.find(sub, b);
		if (b == s.npos)
		{
			break;
		}
		s.replace(b, sub.size(), other);
		b += other.size();
	}
	return s;
}

void SelfGrow( string& s, size_t appendlength )
{
	size_t c = s.capacity();
	if( c <= ( s.length() + appendlength ) )
	{
		while( (c *= 2) <= ( s.length() + appendlength ) );
		s.reserve(c);
	}
}

wstring BasePath(const wstring& sBinaryPath)
{
	if(sBinaryPath.size() <= 3)
	{
		return sBinaryPath;
	}

	//_ASSERT(sBinaryPath[sBinaryPath.size() - 1] != L'\\');

	size_t i = sBinaryPath.find_last_of(L"/\\");
	if(i != sBinaryPath.npos)
	{
		return sBinaryPath.substr(0, i);
	}
	else
	{
		return L"";
	}
}

wstring FileName(const wstring& sBinaryPath)
{
	size_t i = sBinaryPath.find_last_of(L"/\\");
	if(i != sBinaryPath.npos)
	{
		return sBinaryPath.substr(i + 1);
	}
	else
	{
		return sBinaryPath;
	}
}

bool StartWith(const wstring& fullStr, const wstring& startStr, BOOL fIgnoreCase)
{
	if(fullStr.empty())
	{
		return false;
	}

	wstring s = fullStr.substr(0, startStr.size());

	if(fIgnoreCase)
	{
		return (_wcsicmp(s.c_str(), startStr.c_str()) == 0);
	}
	else
	{
		return (s.compare(startStr) == 0);
	}
}

string TrimLeft(const string& str)
{
	size_t n = str.find_first_not_of(" \t\v");
	return (n == string::npos) ? str : str.substr(n, str.length());
}

string TrimRight(const string& str)
{
	size_t n = str.find_last_not_of(" \t\v");
	return (n == string::npos) ? str : str.substr(0, n + 1);
} 

string Trim(const string& str)
{
	return TrimLeft(TrimRight(str));
}

wstring TrimLeftW(const wstring& str)
{
	size_t n = str.find_first_not_of(L" \t\v");
	return (n == wstring::npos) ? str : str.substr(n, str.length());
}

wstring TrimRightW(const wstring& str)
{
	size_t n = str.find_last_not_of(L" \t\v");
	return (n == wstring::npos) ? str : str.substr(0, n + 1);
} 

wstring TrimW(const wstring& str)
{
	return TrimLeftW(TrimRightW(str));
}

bool Compare(const string& origin, const string& dest)
{
	return (_stricmp(Trim(origin).c_str(), Trim(dest).c_str()) == 0);
}

//Function to convert a Hex string of length 2 to an unsigned char
bool Hex2Char(CONST CHAR* szHex, UCHAR& rch)
{
	if(*szHex >= '0' && *szHex <= '9')
	{
		rch = *szHex - '0';
	}
	else if(*szHex >= 'A' && *szHex <= 'F')
	{
		rch = *szHex - 55; //-'A' + 10
	}
	else
	{
		//Is not really a Hex string
		return false; 
	}

	++szHex;

	if(*szHex >= '0' && *szHex <= '9')
	{
		(rch <<= 4) += *szHex - '0';
	}
	else if(*szHex >= 'A' && *szHex <= 'F')
	{
		(rch <<= 4) += *szHex - 55; //-'A' + 10;
	}
	else
	{
		//Is not really a Hex string
		return false;
	}
 
	return true;
}

wstring TrimLastRightPathSeperator(
	__in const wstring& str,
	__in_opt WCHAR pathSeperator
	)
{
	// if only '/' characer is passed to str, dont' trim it.
	if (str.size() > 1)
	{
		wstring s = str.substr(1);
		size_t n = s.find_last_not_of(pathSeperator);
		if (n != string::npos)
		{
			return str[0] + s.substr(0, n + 1);
		}
		else
		{
			s = pathSeperator;
			return s;
		}
	}

	return str;
}

string & TrimBothSide(string &str)
{
	size_t start = str.find_first_not_of( "\n\r\t " );
	size_t end = str.find_last_not_of( "\n\r\t " );

	str = str.substr( start, end - start + 1 );
	return str;
}

void SHTokenizeA( 
	const string& target, 
	vector<string>& tokens,
	const string &delim 
	)
{
	string::size_type lastPos = target.find_first_not_of( delim );
	string::size_type pos = target.find_first_of( delim, lastPos );

	while( string::npos != pos || string::npos != lastPos )
	{
		tokens.push_back( target.substr(lastPos, pos - lastPos) );
		lastPos = target.find_first_not_of( delim, pos );
		pos = target.find_first_of( delim, lastPos );
	}
}

void SHTokenizeW( 
	const wstring& target, 
	vector<wstring>& tokens, 
	const wstring &delim 
	)
{
	wstring::size_type lastPos = target.find_first_not_of( delim );
	wstring::size_type pos = target.find_first_of( delim, lastPos );

	while( wstring::npos != pos || wstring::npos != lastPos )
	{
		tokens.push_back( target.substr(lastPos, pos - lastPos) );
		lastPos = target.find_first_not_of( delim, pos );
		pos = target.find_first_of( delim, lastPos );
	}
}

int StringAToInt( const string &str )
{
	int retVal = 0;
	stringstream sstm(str);
	sstm >> retVal;
	return retVal;
}

int StringWToInt( const wstring &str )
{
	int retVal = 0;
	wstringstream sstm(str);
	sstm >> retVal;
	return retVal;
}

wstring IntegerToStr(LONGLONG num)
{
	wstringstream sstream;
	sstream << num;
	return sstream.str();
}

wstring AbbreviateFileName(__in const wstring& fileName, __in size_t cchMaxFileName)
{
	if (fileName.size() < cchMaxFileName)
	{
		return fileName;
	}

	size_t n = fileName.rfind('.');

	wstring ext;
	wstring abbreviatedFileNameOnly;
	if (n != fileName.npos)
	{
		// a case the filaName includes extension.
		ext = fileName.substr(n + 1);
		abbreviatedFileNameOnly = fileName.substr(0, cchMaxFileName - ext.size());
	}
	else
	{
		abbreviatedFileNameOnly = fileName.substr(0, cchMaxFileName);
	}
	return abbreviatedFileNameOnly + L"..." + ext;
}

wstring AbbreviateString(__in const wstring& sString, __in size_t cchMaxLen)
{
	wstring sAbbreyStr = sString;

	if( sAbbreyStr.length() > cchMaxLen )
	{
		sAbbreyStr = sAbbreyStr.substr( 0, cchMaxLen + 1 );
		sAbbreyStr.append( L"..." );
	}

	return sAbbreyStr;
}

wstring AppendPathSeperatorIfNotExist(
	__in const std::wstring& path,
	__in WCHAR pathSeperator /*= '/'*/
	)
{
	if (path.empty() || path[path.size() - 1] != pathSeperator)
	{
		return path + pathSeperator;
	}
	return path;
}

wstring AddFrontSlashIfNotExist(__in const wstring& path)
{
	if (path.size() == 0 || path[0] != L'/')
	{
		wstring sPath;
		sPath = L'/';
		return sPath + path;
	}
	return path;
}

wstring AddFrontAndRearSlashIfNotExist(__in const wstring& path)
{
	return AddFrontSlashIfNotExist(AppendPathSeperatorIfNotExist(path, L'/'));
}

string Hexify(const vector<unsigned char>& v)
{
	string s(2 * v.size(), 'x');
	string::iterator k = s.begin();
	for (auto i = v.begin(); i != v.end(); ++i)
	{
		*k++ = "0123456789ABCDEF"[*i >> 4];
		*k++ = "0123456789ABCDEF"[*i & 0x0F];
	}

	return s;
}

wstring ToLower(__in const wstring &ori)
{
	wstring dst;
	dst.resize(ori.size());
	transform(ori.cbegin(), ori.cend(), dst.begin(), towlower);
	return move(dst);
}

wstring ToLower(__in LPCWSTR ori)
{
	wstring dst;
	dst.resize(wcslen(ori));
	transform(ori, ori + dst.size(), dst.begin(), towlower);
	return move(dst);
}

bool CiCharLess(wchar_t c1, wchar_t c2)
{
	return towlower(c1) < towlower(c2);
}

bool CiStringCompare(const std::wstring& s1, const std::wstring& s2)
{
	return std::lexicographical_compare(
		s1.begin(), s1.end(),
		s2.begin(), s2.end(),
		CiCharLess);
}

bool StringEqualNoCase(const std::wstring& s1, const std::wstring& s2)
{
	return (_wcsicmp(s1.c_str(), s2.c_str()) == 0);
}

std::wstring ConvertNtPathToUnixPath(const std::wstring& sNtPath)
{
	std::wstring sNdPath = sNtPath;
	sNdPath = sNdPath.substr(sNdPath.find_first_of(L'\\'));
	return Replacein(sNdPath, L"\\", L"/");
}

std::wstring ConvertUnixPathToNtPath(const std::wstring & sNdPath, WCHAR chVolume)
{
	std::wstring sNtPath;
	sNtPath.append(1, chVolume);
	sNtPath.append(1, L':');
	sNtPath.append(sNdPath.substr(sNdPath.find_first_of(L'/')));
	return Replacein(sNtPath, L"/", L"\\");
}

std::wstring __cdecl FormatV(PCWSTR pszFormat, ...)
{
	WCHAR sz[4096];
	va_list argList;
	va_start(argList, pszFormat);
	if (FAILED(StringCchVPrintf(sz, _countof(sz), pszFormat, argList)))
	{
		va_end(argList);
		return L"";
	}
	va_end(argList);
	return sz;
}