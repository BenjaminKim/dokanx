#include "UnicodeHelper.h"


#include <StrSafe.h>
#pragma warning(push)
#pragma warning(disable:4995)
#include <vector>
#pragma warning(pop)

int GetAnsiByteSize(__in LPCWSTR uniStr)
{
	return ::WideCharToMultiByte(CP_ACP, 0, uniStr, -1, NULL, 0, NULL, NULL);
}

int GetUniByteSize(__in LPCSTR ansiStr)
{
	int needLength = ::MultiByteToWideChar(CP_ACP, 0, ansiStr, -1, NULL, 0);
	return (needLength * sizeof(WCHAR));
}

std::string ToAnsiStr(__in LPCWSTR uniStr)
{
	int needLength = GetAnsiByteSize(uniStr);
	std::vector<CHAR> ansiBuf;
	ansiBuf.resize(needLength);
	CHAR* pAnsiStr = &ansiBuf[0];
	ZeroMemory(pAnsiStr, needLength);
	::WideCharToMultiByte(CP_ACP, 0, uniStr, EOF, pAnsiStr, needLength, "?", NULL);

	std::string ansiStrBuf = pAnsiStr;
	return ansiStrBuf;
}

std::wstring ToUniStr(__in LPCSTR ansiStr)
{
	int needLength = GetUniByteSize(ansiStr);
	std::vector<WCHAR> uniBuf;
	uniBuf.resize(needLength);
	WCHAR* pWideChar = &uniBuf[0];
	ZeroMemory(pWideChar, needLength);
	::MultiByteToWideChar(CP_ACP, 0, ansiStr, EOF, pWideChar, static_cast<INT>(strlen(ansiStr)) + 1);

	std::wstring uniStrBuf = pWideChar;
	return uniStrBuf;
}

std::wstring ToWideCharString(
	__in LPCSTR pszAnsiStr
	)
{
	if(pszAnsiStr == NULL)
	{
		return L"";
	}

	std::string s = pszAnsiStr;
	if(s.empty())
	{
		return L"";
	}

	return ToUniStr(s.c_str());
}

std::wstring UTF8ToUTF16(
	__in LPCSTR pszTextUTF8
	)
{
	if(pszTextUTF8 == NULL)
	{
		return L"";
	}

	std::string TextUTF8(pszTextUTF8);
	if(TextUTF8.empty())
	{
		return L"";
	}

	//
	// Consider CHAR's count corresponding to total input string length,
	// including end-of-string (\0) character
	//
	const size_t cchUTF8Max = INT_MAX - 1;
	size_t cchUTF8;
	HRESULT hr = ::StringCchLengthA(TextUTF8.c_str(), cchUTF8Max, &cchUTF8);
	if ( FAILED( hr ) )
	{
		return L"";
	}

	// Consider also terminating \0
	++cchUTF8;

	// Convert to 'int' for use with MultiByteToWideChar API
	int cbUTF8 = static_cast<int>( cchUTF8 );


	//
	// Get size of destination UTF-16 buffer, in WCHAR's
	//
	int cchUTF16 = ::MultiByteToWideChar(
		CP_UTF8,                // convert from UTF-8
		MB_ERR_INVALID_CHARS,   // error on invalid chars
		TextUTF8.c_str(),            // source UTF-8 string
		cbUTF8,                 // total length of source UTF-8 string,
		// in CHAR's (= bytes), including end-of-string \0
		NULL,                   // unused - no conversion done in this step
		0                       // request size of destination buffer, in WCHAR's
		);

	if ( cchUTF16 == 0 )
	{
		return L"";
	}


	//
	// Allocate destination buffer to store UTF-16 string
	//
	PWCHAR pszUTF16 = new WCHAR[cchUTF16];
	if(pszUTF16 == NULL)
	{
		return L"";
	}

	//
	// Do the conversion from UTF-8 to UTF-16
	//
	int result = ::MultiByteToWideChar(
		CP_UTF8,                // convert from UTF-8
		MB_ERR_INVALID_CHARS,   // error on invalid chars
		TextUTF8.c_str(),            // source UTF-8 string
		cbUTF8,                 // total length of source UTF-8 string,
		// in CHAR's (= bytes), including end-of-string \0
		pszUTF16,               // destination buffer
		cchUTF16                // size of destination buffer, in WCHAR's
		);
	
	if ( result == 0 )
	{
		delete[] pszUTF16;
		return L"";
	}

	std::wstring strUTF16 = pszUTF16;

	// Release internal CString buffer
	delete[] pszUTF16;

	// Return resulting UTF16 string
	return strUTF16;
}


std::string UTF16ToUTF8(
	__in LPCWSTR pszTextUTF16
	)
{
	if(pszTextUTF16 == NULL)
	{
		return "";
	}

	std::wstring TextUTF16(pszTextUTF16);
	
	//
	// Special case of NULL or empty input string
	//
	if(TextUTF16.empty())
	{
		// Return empty string
		return "";
	}


	//
	// Consider WCHAR's count corresponding to total input string length,
	// including end-of-string (L'\0') character.
	//
	const size_t cchUTF16Max = INT_MAX - 1;
	size_t cchUTF16;
	HRESULT hr = ::StringCchLengthW(TextUTF16.c_str(), cchUTF16Max, &cchUTF16);
	if ( FAILED( hr ) )
	{
		return "";
	}

	// Consider also terminating \0
	++cchUTF16;

	DWORD dwConversionFlags = 0;

	//
	// Get size of destination UTF-8 buffer, in CHAR's (= bytes)
	//
	int cbUTF8 = ::WideCharToMultiByte(
		CP_UTF8,                // convert to UTF-8
		dwConversionFlags,      // specify conversion behavior
		TextUTF16.c_str(),           // source UTF-16 string
		static_cast<int>( cchUTF16 ),   // total source string length, in WCHAR's,
		// including end-of-string \0
		NULL,                   // unused - no conversion required in this step
		0,                      // request buffer size
		NULL, NULL              // unused
		);

	if ( cbUTF8 == 0 )
	{
		return "";
	}


	//
	// Allocate destination buffer for UTF-8 string
	//

	int cchUTF8 = cbUTF8; // sizeof(CHAR) = 1 byte
	PCHAR pszUTF8 = new CHAR[cchUTF8];
	if(pszUTF8 == NULL)
	{
		return "";
	}

	//
	// Do the conversion from UTF-16 to UTF-8
	//
	int result = ::WideCharToMultiByte(
		CP_UTF8,                // convert to UTF-8
		dwConversionFlags,      // specify conversion behavior
		TextUTF16.c_str(),           // source UTF-16 string
		static_cast<int>( cchUTF16 ),   // total source string length, in WCHAR's,
		// including end-of-string \0
		pszUTF8,                // destination buffer
		cbUTF8,                 // destination buffer size, in bytes
		NULL, NULL              // unused
		); 

	if ( result == 0 )
	{
		delete[] pszUTF8;
		return "";
	}

	std::string strUTF8 = pszUTF8;
	delete[] pszUTF8;

	// Return resulting UTF-8 string
	return strUTF8;
}

std::string AnsiToUTF8(__in const std::string& strAnsi)
{
	std::string strReturn;
	char* strUTF8 = NULL;

	BSTR strUnicode;
	int nLen = MultiByteToWideChar(CP_ACP, 0, strAnsi.c_str(), (int)strAnsi.size(), NULL, NULL);
	strUnicode = SysAllocStringLen(NULL, nLen);
	if(strUnicode == NULL)
	{
		return "";
	}

	MultiByteToWideChar(CP_ACP, 0, strAnsi.c_str(), (int)strAnsi.size(), strUnicode, nLen);
	nLen = WideCharToMultiByte(CP_UTF8, 0, strUnicode, -1, strUTF8, 0, NULL, NULL);
	strUTF8 = (char*)malloc(nLen + 1);
	if(strUTF8 == NULL)
	{
		SysFreeString(strUnicode);
		return "";
	}

	WideCharToMultiByte(CP_UTF8, 0, strUnicode, -1, strUTF8, nLen, NULL, NULL);
	strReturn = strUTF8;
	free(strUTF8);
	SysFreeString(strUnicode);
	return strReturn;
}

std::string UTF8ToAnsi(__in const std::string& sUtf8)
{
	std::string strReturn;
	char* pszUTF8 = NULL;

	BSTR strUnicode;
	int nLen = MultiByteToWideChar(CP_UTF8, 0, sUtf8.c_str(), (int)sUtf8.size(), NULL, NULL);
	strUnicode = SysAllocStringLen(NULL, nLen);
	if(strUnicode == NULL)
	{
		return "";
	}

	MultiByteToWideChar(CP_UTF8, 0, sUtf8.c_str(), (int)sUtf8.size(), strUnicode, nLen);
	nLen = WideCharToMultiByte(CP_ACP, 0, strUnicode, -1, 0, 0, NULL, NULL);  
	pszUTF8 = (char*)malloc(nLen + 1);
	if(pszUTF8 == NULL)
	{
		SysFreeString(strUnicode);
		return "";
	}

	WideCharToMultiByte(CP_ACP, 0, strUnicode, -1, pszUTF8, nLen, NULL, NULL);
	strReturn = pszUTF8;
	free(pszUTF8);
	SysFreeString(strUnicode);
	return strReturn;
}

int BytesCount(char utf8FirstByte)
{
	int nCount = 0;
	for (int i = 7; i >= 0; i--)
	{
		if (((utf8FirstByte >> i) & 1))
		{
			nCount++;
		}
		else
		{
			break;
		}
	}

	return (nCount == 0) ? 1 : nCount;
}

int Utf8CharCount(const char* pStr)
{
	int n = (int)strlen(pStr);
	int i = 0;
	int nCount = 0;
	while (i < n)
	{
		i += BytesCount(pStr[i]);
		nCount++;
	}
	
	return nCount;
}
