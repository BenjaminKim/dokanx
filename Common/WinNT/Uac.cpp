#include "UAC.h"

#include "OSVersion.h"

BOOL IsUacSupported()
{
	HKEY hkey;
	DWORD value = 1, size = sizeof (DWORD);

	if (!IsVistaOrLater())
	{
		return FALSE;
	}

	if (RegOpenKeyExW(HKEY_LOCAL_MACHINE,
		L"Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\System", 0, KEY_READ, &hkey) == ERROR_SUCCESS)
	{
		if (RegQueryValueExW(hkey, L"EnableLUA", 0, 0, (LPBYTE) &value, &size) != ERROR_SUCCESS)
		{
			value = 1;
		}

		RegCloseKey (hkey);
	}

	return value != 0;
}
