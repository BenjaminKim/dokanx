#include "OSVersion.h"

#include <tchar.h>
#include "Registry.h"

BOOL Is64BitWindows()
{
#if defined (_WIN64)
	// If the program is built by 64bit compiler, the program never can run on 32bit OS
	return TRUE;
#elif defined(_WIN32)
	HMODULE hModule;
	typedef BOOL (__stdcall *LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);

	hModule = GetModuleHandle(_T("kernel32"));
	if(!hModule)
	{
		return FALSE;
	}

	// Minimum supported client : Windows Vista, Windows XP with SP2
	LPFN_ISWOW64PROCESS fnIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress(hModule, "IsWow64Process");

	if(fnIsWow64Process)
	{
		BOOL bIsWow64 = FALSE;
		return (fnIsWow64Process(GetCurrentProcess(), &bIsWow64) && bIsWow64);
	}
	else
	{
		// If the function doesn't exist, we assume it is 32bit OS.
		return FALSE;
	}
#else
	// Win64 does not support Win16
	return FALSE;
#endif
}

BOOL IsWindows7()
{
	OSVERSIONINFO osvi;
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

	if (::GetVersionEx(&osvi) && 
		osvi.dwPlatformId == VER_PLATFORM_WIN32_NT && 
		(osvi.dwMajorVersion == 6) &&
		(osvi.dwMinorVersion == 1))
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL IsWindows7OrLater()
{
	OSVERSIONINFO osvi;
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

	if (::GetVersionEx(&osvi) && 
		osvi.dwPlatformId == VER_PLATFORM_WIN32_NT)
	{
		if(osvi.dwMajorVersion == 6 && osvi.dwMinorVersion >= 1)
		{
			return TRUE;
		}
		if(osvi.dwMajorVersion > 6)
		{
			return TRUE;
		}

		return FALSE;
	}
	else
	{
		return FALSE;
	}
}

BOOL IsVistaOrLater( void )
{
	OSVERSIONINFO osvi;

	osvi.dwOSVersionInfoSize = sizeof ( OSVERSIONINFO );

	// Major 5 is XP
	// Major 6 is VISTA
	if (::GetVersionEx( &osvi ) && 
		osvi.dwPlatformId == VER_PLATFORM_WIN32_NT && 
		(osvi.dwMajorVersion >= 6 ))
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL IsVistaOrWinServer2008Basic()
{
	// If OS is 2008 R2 version, the function returns FALSE.
	OSVERSIONINFO osvi;
	osvi.dwOSVersionInfoSize = sizeof ( OSVERSIONINFO );

	if (::GetVersionEx( &osvi ) && 
		osvi.dwPlatformId == VER_PLATFORM_WIN32_NT && 
		(osvi.dwMajorVersion == 6 ) &&
		(osvi.dwMinorVersion == 0 ))
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL IsServerOS()
{
	OSVERSIONINFOEXA osVer;
	osVer.dwOSVersionInfoSize = sizeof (OSVERSIONINFOEXA);
	GetVersionExA ((LPOSVERSIONINFOA) &osVer);

	return (osVer.wProductType == VER_NT_SERVER || osVer.wProductType == VER_NT_DOMAIN_CONTROLLER);
}

bool IsVistaProcessElevation()
{
	DWORD dwTokenElevation = 0;
	DWORD dwReturnLength = 0;
	HANDLE hToken = NULL;
	bool bElevation = false;

	if (::OpenProcessToken(::GetCurrentProcess(), TOKEN_QUERY, &hToken))
	{
		if (::GetTokenInformation(
			hToken,
			(TOKEN_INFORMATION_CLASS)20,
			&dwTokenElevation,
			sizeof(DWORD),
			&dwReturnLength
			))
		{
			if (0 != dwTokenElevation)
			{
				bElevation = true;
			}
		}
	}
	::CloseHandle(hToken);

	return bElevation;
}

bool IsUacSupported()
{
	if (!IsVistaOrLater())
	{
		return FALSE;
	}

	DWORD dwResult = Registry::RegGetSimpleDword(
		HKEY_LOCAL_MACHINE,
		L"Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\System",
		L"EnableLUA",
		0);

	return (dwResult != 0);
}
