#include "Registry.h"

#include "../String/UnicodeHelper.h"

HKEY Registry::GetKeyName(
	__in const std::wstring& strData
	)
{
	HKEY hKey = NULL;
	if(_wcsicmp(strData.c_str(), L"HKEY_CLASSES_ROOT") == 0)
	{
		hKey = HKEY_CLASSES_ROOT;
	}
	else if(_wcsicmp(strData.c_str(), L"HKEY_CURRENT_USER") == 0)
	{
		hKey = HKEY_CURRENT_USER;
	}
	else if(_wcsicmp(strData.c_str(), L"HKEY_LOCAL_MACHINE") == 0)
	{
		hKey = HKEY_LOCAL_MACHINE;
	}
	else if(_wcsicmp(strData.c_str(), L"HKEY_USERS") == 0)
	{
		hKey = HKEY_USERS;
	}
	else if(_wcsicmp(strData.c_str(), L"HKEY_CURRENT_CONFIG") == 0)
	{
		hKey = HKEY_CURRENT_CONFIG;
	}

	return hKey;
}

std::wstring Registry::GetKeyNameString(
	__in HKEY hKey
	)
{
	std::wstring strReturn = L"";
	if(hKey == HKEY_CLASSES_ROOT)
	{
		strReturn = L"HKEY_CLASSES_ROOT";
	}
	else if(hKey == HKEY_CURRENT_USER)
	{
		strReturn = L"HKEY_CURRENT_USER";
	}
	else if(hKey == HKEY_LOCAL_MACHINE)
	{
		strReturn = L"HKEY_LOCAL_MACHINE";
	}
	else if(hKey == HKEY_USERS)
	{
		strReturn = L"HKEY_USERS";
	}
	else if(hKey == HKEY_CURRENT_CONFIG)
	{
		strReturn = L"HKEY_CURRENT_CONFIG";
	}
	return strReturn;
}

std::wstring Registry::RegGetSimpleString(
	__in HKEY hKey,
	__in const std::wstring& strOpenKey,
	__in const std::wstring& strValue,
	__in const std::wstring& strDefaultData,
	__in REGSAM samDesired
	)
{
	HKEY hSubKey;

	long r = ::RegOpenKeyEx(hKey, strOpenKey.c_str(), 0, samDesired, &hSubKey);

	std::wstring strRet = strDefaultData;

	if (r == ERROR_SUCCESS)
	{
		WCHAR sz[MAX_PATH];
		DWORD dwType;
		DWORD dwSize = sizeof(sz);

		r = RegQueryValueEx(hSubKey, strValue.c_str(), NULL, &dwType, (LPBYTE)sz, &dwSize);
		if (r == ERROR_SUCCESS)
		{
			strRet = sz;
		}
		else
		{
			strRet = strDefaultData;
		}
		RegCloseKey(hSubKey);
	}

	return strRet;
}

DWORD Registry::RegGetSimpleDword(
	__in HKEY hKey,
	__in const std::wstring& strOpenKey,
	__in const std::wstring& strValueName,
	__in DWORD dwDefaultData,
	__in REGSAM samDesired
	)
{
	HKEY hSubKey;

	long r = ::RegOpenKeyEx(hKey, strOpenKey.c_str(), 0, samDesired, &hSubKey);

	DWORD dwRet = dwDefaultData;
	if(r == ERROR_SUCCESS)
	{
		DWORD dwBuff;
		DWORD dwType;
		DWORD dwSize = 4;

		r = RegQueryValueEx(hSubKey, strValueName.c_str(), NULL, &dwType, (LPBYTE)&dwBuff, &dwSize);

		if(r != ERROR_SUCCESS)
		{
			dwRet = dwDefaultData;
		}
		else
		{
			dwRet = dwBuff;
		}

		RegCloseKey(hSubKey);
	}

	return dwRet;
}

BOOL Registry::RegSetSimpleStringA(
	__in HKEY hKey,
	__in const std::string& strOpenKey,
	__in const std::string& strValueName,
	__in const std::string& strData,
	__in BOOL fPermanently,
	__in REGSAM samDesired
	)
{
	std::wstring wOpenKey       = ToUniStr(strOpenKey.c_str());
	std::wstring wValueName     = ToUniStr(strValueName.c_str());
	std::wstring wData          = ToUniStr(strData.c_str());

	return RegSetSimpleStringW(hKey, wOpenKey, wValueName, wData, fPermanently, samDesired);
}

BOOL Registry::RegSetSimpleStringW(
	__in HKEY hKey,
	__in const std::wstring& strOpenKey,
	__in const std::wstring& strValueName,
	__in const std::wstring& strData,
	__in BOOL fPermanently,
	__in REGSAM samDesired
	)
{
	BOOL bResult = FALSE;
	if (hKey == NULL || strOpenKey == L"")
	{
		return bResult;
	}

	HKEY hSubKey;
	DWORD dwState;
	LSTATUS lStatus = RegCreateKeyEx(hKey, strOpenKey.c_str(), NULL, NULL,
		fPermanently ? REG_OPTION_NON_VOLATILE : REG_OPTION_VOLATILE, samDesired, NULL, &hSubKey, &dwState);
	if (lStatus != ERROR_SUCCESS)
	{
		return bResult;
	}

	if (strData.empty())
	{
		lStatus = RegDeleteValue(hSubKey, strValueName.c_str());
	}
	else
	{
		//
		// The size of the information pointed to by the lpData parameter, in bytes.
		// If the data is of type REG_SZ, REG_EXPAND_SZ, or REG_MULTI_SZ,
		// cbData must include the size of the terminating null character or characters.
		//
		DWORD size = (DWORD)((strData.size() * sizeof WCHAR) + sizeof(WCHAR));

		lStatus = RegSetValueEx(hSubKey, strValueName.empty() ? NULL : strValueName.c_str(), 
			NULL, REG_SZ, (BYTE*)strData.c_str(), size);
	}

	if (lStatus == ERROR_SUCCESS)
	{
		bResult = TRUE;
	}

	RegCloseKey(hSubKey);
	return bResult;
}

BOOL Registry::RegSetSimpleStringVolatileW(
	__in HKEY hKey,
	__in const std::wstring& strOpenKey,
	__in const std::wstring& strValueName,
	__in const std::wstring& strData
	)
{
	return RegSetSimpleStringW(hKey, strOpenKey, strValueName, strData, FALSE);
}

BOOL Registry::RegSetSimpleDword(
	__in HKEY hKey,
	__in const std::wstring& strOpenKey,
	__in const std::wstring& strValueName,
	__in DWORD dwData,
	__in BOOL fPermanently,
	__in BOOL bDelete,
	__in REGSAM samDesired
	)
{
	BOOL bResult = FALSE;
	if (hKey == NULL || strOpenKey == L"" || strValueName == L"")
	{
		return bResult;
	}
	HKEY hSubKey;
	DWORD dwState;
	long ret;

	ret = RegCreateKeyEx(hKey, strOpenKey.c_str(), NULL, NULL,
		fPermanently ? REG_OPTION_NON_VOLATILE : REG_OPTION_VOLATILE, samDesired, NULL, &hSubKey, &dwState);
	if (ret != ERROR_SUCCESS)
	{
		return bResult;
	}

	if (bDelete)
	{
		ret = RegDeleteValue(hSubKey, strValueName.c_str());
	}
	else
	{
		ret = RegSetValueEx(hSubKey, strValueName.c_str(),
			NULL, REG_DWORD, (unsigned char*)&dwData, sizeof(dwData));
		if (ret == ERROR_SUCCESS)
		{
			bResult = TRUE;
		}
	}

	RegCloseKey(hSubKey);
	return bResult;
}

BOOL Registry::RegSetSimpleBinary(
	__in HKEY hKey,
	__in const std::wstring& strOpenKey,
	__in const std::wstring& strValueName,
	__in const void* pData,
	__in int nDataSize,
	__in REGSAM samDesired
	)
{
	BOOL bResult = FALSE;
	if(hKey == NULL || strOpenKey == L"" || strValueName == L"")
	{
		return bResult;
	}
	HKEY hSubKey;
	DWORD dwState;
	long ret;

	ret = RegCreateKeyEx(hKey, strOpenKey.c_str(),
		NULL, NULL, REG_OPTION_NON_VOLATILE, samDesired, NULL, &hSubKey, &dwState);
	if (ret != ERROR_SUCCESS)
	{
		return bResult;
	}

	if(nDataSize <= 0)
	{
		ret = RegDeleteValue(hSubKey, strValueName.c_str());
	}
	else
	{
		ret = RegSetValueEx(hSubKey, strValueName.c_str(),
			NULL, REG_BINARY, (unsigned char*)pData, nDataSize);
		if (ret == ERROR_SUCCESS)
		{
			bResult = TRUE;
		}
	}

	RegCloseKey(hSubKey);	
	return bResult;
}

BOOL Registry::RegSetExpandString(
	__in HKEY hKey,
	__in const std::wstring& sOpenKey,
	__in const std::wstring& sValueName,
	__in const std::wstring& sData,
	__in REGSAM samDesired /*= KEY_ALL_ACCESS*/
	)
{
	BOOL bResult = FALSE;
	if (hKey == NULL || sOpenKey.empty())
	{
		return bResult;
	}

	HKEY hSubKey;
	DWORD dwState;
	long ret = RegCreateKeyEx(
		hKey, sOpenKey.c_str(), NULL, NULL, REG_OPTION_NON_VOLATILE, samDesired, NULL, &hSubKey, &dwState
		);

	if (ret != ERROR_SUCCESS)
	{
		return bResult;
	}

	if (sData.empty())
	{
		ret = RegDeleteValue(hSubKey, sValueName.c_str());
	}
	else
	{
		ret = RegSetValueEx(hSubKey, sValueName.empty() ? NULL : sValueName.c_str(),
			NULL, REG_EXPAND_SZ, (BYTE*)sData.c_str(), ((DWORD)sData.size() * sizeof(WCHAR)) + sizeof(WCHAR));

		if (ret == ERROR_SUCCESS)
		{
			bResult = TRUE;
		}
	}

	RegCloseKey(hSubKey);
	return bResult;
}

BOOL Registry::HasRegisterdValue(
	__in HKEY hKey,
	__in const std::wstring& strOpenKey,
	__in const std::wstring& strValueName,
	__in REGSAM samDesired
	)
{
	BOOL bResult = FALSE;
	HKEY hSubKey;
	WCHAR buff[MAX_PATH];
	DWORD dwType;
	DWORD dwSize = sizeof(buff);
	long r = ::RegOpenKeyEx(hKey, strOpenKey.c_str(), 0, samDesired, &hSubKey);

	if(r == ERROR_SUCCESS)
	{
		r = RegQueryValueEx(hSubKey, strValueName.c_str(), NULL, &dwType, (LPBYTE)buff, &dwSize);
		if(r == ERROR_SUCCESS)
		{
			bResult = TRUE;
		}
		RegCloseKey(hSubKey);
	}
	return bResult;
}

BOOL Registry::HasRegisterdKey(
	__in HKEY hKey,
	__in const std::wstring& strOpenKey,
	__in REGSAM samDesired 
	)
{
	HKEY hSubKey;
	long r = ::RegOpenKeyEx(hKey, strOpenKey.c_str(), 0, samDesired, &hSubKey);

	RegCloseKey(hSubKey);

	return (r == ERROR_SUCCESS);
}

DWORD Registry::RegDeleteKeyNT(
	__in HKEY hStartKey,
	__in LPCWSTR pKeyName,
	__in REGSAM samDesired
	)
{
	DWORD   dwRtn, dwSubKeyLength;
	TCHAR   szSubKey[MAX_PATH]; // (256) this should be dynamic.
	HKEY    hKey;

	// Do not allow NULL or empty key name
	if (pKeyName && lstrlen(pKeyName))
	{
		if ((dwRtn = RegOpenKeyEx(hStartKey, pKeyName, 0, samDesired, &hKey)) == ERROR_SUCCESS)
		{
			while (dwRtn == ERROR_SUCCESS)
			{
				dwSubKeyLength = MAX_PATH;
				dwRtn = RegEnumKeyEx(
					hKey,
					0,       // always index zero
					szSubKey,
					&dwSubKeyLength,
					NULL,
					NULL,
					NULL,
					NULL
					);

				if (dwRtn == ERROR_NO_MORE_ITEMS)
				{
					dwRtn = RegDeleteKey(hStartKey, pKeyName);
					break;
				}
				else if (dwRtn == ERROR_SUCCESS)
				{
					dwRtn = RegDeleteKeyNT(hKey, szSubKey);
				}
			}
			RegCloseKey(hKey);
			// Do not save return code because error
			// has already occurred
		}
	}
	else
	{
		dwRtn = ERROR_BADKEY;
	}

	return dwRtn;
}

void Registry::GetValueNames(
	__in HKEY hRootKey,
	__in LPTSTR lpKey,
	__out std::vector<std::wstring>& vFoundValueNames,
	__in REGSAM samDesired
	)
{
	HKEY hKey;
	DWORD dwRtn;
	TCHAR achValue[512];
	DWORD cchValue = _countof(achValue);

	vFoundValueNames.clear();

	if((dwRtn = RegOpenKeyEx(hRootKey, lpKey, 0, samDesired, &hKey )) == ERROR_SUCCESS)
	{
		int i = 0;

		while(dwRtn == ERROR_SUCCESS)
		{
			cchValue = MAX_PATH;
			dwRtn = RegEnumValue(
				hKey,
				i++,
				achValue,
				&cchValue,
				0,
				0,
				0,
				0
				);

			if(dwRtn == ERROR_NO_MORE_ITEMS)
			{
				break;
			}

			vFoundValueNames.push_back(achValue);
		}
		RegCloseKey(hKey);
	}
}