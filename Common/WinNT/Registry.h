#pragma once

#include <Windows.h>

#pragma warning(push)
#pragma warning(disable:4995)
#include <vector>
#include <string>
#pragma warning(pop)

/**
*
*/
class Registry
{
public:
	static HKEY GetKeyName(
		__in const std::wstring& strData
		);
	static std::wstring GetKeyNameString(
		__in HKEY hKey
		);
	static std::wstring RegGetSimpleString(
		__in HKEY hKey,
		__in const std::wstring& strOpenKey,
		__in const std::wstring& strValue,
		__in const std::wstring& strDefaultData,
		__in REGSAM samDesired = KEY_READ
		);
	static DWORD RegGetSimpleDword(
		__in HKEY hKey,
		__in const std::wstring& strOpenKey,
		__in const std::wstring& strValueName,
		__in DWORD dwDefaultData,
		__in REGSAM samDesired = KEY_READ
		);
	static BOOL RegSetSimpleStringA(
		__in HKEY hKey,
		__in const std::string& strOpenKey,
		__in const std::string& strValueName,
		__in const std::string& strData,
		__in BOOL fPermanently,
		__in REGSAM samDesired = KEY_ALL_ACCESS
		); // NS
	//
	// Delete if strData is ""
	//
	static BOOL RegSetSimpleStringW(
		__in HKEY hKey,
		__in const std::wstring& strOpenKey,
		__in const std::wstring& strValueName,
		__in const std::wstring& strData,
		__in BOOL fPermanently,
		__in REGSAM samDesired = KEY_ALL_ACCESS
		);
	static BOOL RegSetSimpleStringVolatileW(
		__in HKEY hKey,
		__in const std::wstring& strOpenKey,
		__in const std::wstring& strValueName,
		__in const std::wstring& strData
		);
	static BOOL RegSetSimpleDword(
		__in HKEY hKey,
		__in const std::wstring& strOpenKey,
		__in const std::wstring& strValueName,
		__in DWORD dwData,
		__in BOOL fPermanently,
		__in BOOL bDelete,
		__in REGSAM samDesired = KEY_ALL_ACCESS
		);
	static BOOL RegSetSimpleBinary(
		__in HKEY hKey,
		__in const std::wstring& strOpenKey,
		__in const std::wstring& strValueName,
		__in const void* pData,
		__in int nDataSize,
		__in REGSAM samDesired = KEY_ALL_ACCESS
		);
	static BOOL RegSetExpandString(
		__in HKEY hKey,
		__in const std::wstring& sOpenKey,
		__in const std::wstring& sValueName,
		__in const std::wstring& sData,
		__in REGSAM samDesired = KEY_ALL_ACCESS
		);
	static std::wstring RegGetExpandString(
		__in HKEY hKey,
		__in const std::wstring& strOpenKey,
		__in const std::wstring& strValue,
		__in const std::wstring& strDefaultData,
		__in REGSAM samDesired
		);
	static BOOL HasRegisterdValue(
		__in HKEY hKey,
		__in const std::wstring& strOpenKey,
		__in const std::wstring& strValueName,
		__in REGSAM samDesired = KEY_READ
		);
	static BOOL HasRegisterdKey(
		__in HKEY hKey,
		__in const std::wstring& strOpenKey,
		__in REGSAM samDesired = KEY_READ
		);	
	static DWORD RegDeleteKeyNT(
		__in HKEY hStartKey,
		__in LPCWSTR pKeyName,
		__in REGSAM samDesired = KEY_ENUMERATE_SUB_KEYS | DELETE		
		);	
	static void GetValueNames(
		__in HKEY hRootKey,
		__in LPTSTR lpKey,
		__out std::vector<std::wstring>& vFoundValueNames,
		__in REGSAM samDesired = KEY_READ
		);
};
