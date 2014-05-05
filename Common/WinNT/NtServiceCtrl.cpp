#include "NtServiceCtrl.h"

DWORD InstallService(
	__in        LPCWSTR     lpServiceName,
	__in_opt    LPCWSTR     lpDisplayName,
	__in        DWORD       dwDesiredAccess,
	__in        DWORD       dwServiceType,
	__in        DWORD       dwStartType,
	__in        DWORD       dwErrorControl,
	__in_opt    LPCWSTR     lpBinaryPathName,
	__in_opt    LPCWSTR     lpLoadOrderGroup,
	__out_opt   LPDWORD     lpdwTagId,
	__in_opt    LPCWSTR     lpDependencies,
	__in_opt    LPCWSTR     lpServiceStartName,
	__in_opt    LPCWSTR     lpPassword
	)
{
	SC_HANDLE sch;
	SC_HANDLE service;
	DWORD dwError = ERROR_SUCCESS;

	sch = OpenSCManager(NULL, NULL, GENERIC_WRITE);
	if(sch)
	{
		service = CreateServiceW(
			sch,
			lpServiceName,
			lpDisplayName,
			dwDesiredAccess,
			dwServiceType,
			dwStartType,
			dwErrorControl,
			lpBinaryPathName,
			lpLoadOrderGroup,
			lpdwTagId,
			lpDependencies,
			lpServiceStartName,
			lpPassword);

		if(service)
		{
			CloseServiceHandle(service);
		}
		else
		{
			dwError = GetLastError();
		}
		
		CloseServiceHandle(sch);
	}
	else
	{
		dwError = GetLastError();
	}

	return dwError;
}

BOOL LoadDriver(
	__in LPCWSTR lpServiceName
	)
{
	DWORD dwError = LoadDriverEx(lpServiceName, 0, 0);

	if(dwError != ERROR_SUCCESS)
	{
		SetLastError(dwError);
	}

	return (dwError == ERROR_SUCCESS || dwError == ERROR_SERVICE_ALREADY_RUNNING);
}

DWORD LoadDriverEx(
	__in LPCWSTR lpServiceName,
	__in DWORD dwNumServiceArgs, 
	__in_ecount_opt(dwNumServiceArgs) LPCWSTR *lpServiceArgVectors
	)
{
	DWORD dwError = ERROR_SUCCESS;
	SC_HANDLE sch = OpenSCManager(NULL, NULL, GENERIC_EXECUTE);    

	if(sch)
	{
		SC_HANDLE service = OpenServiceW(sch, lpServiceName, GENERIC_EXECUTE);
		if(service)
		{
			BOOL result = StartServiceW(service, dwNumServiceArgs, lpServiceArgVectors);

			if(!result)
			{
				dwError = GetLastError();
			}

			CloseServiceHandle(service);
		}
		else
		{
			dwError = GetLastError();
		}
		CloseServiceHandle(sch);
	}
	else
	{
		dwError = GetLastError();
	}

	return dwError;
}

BOOL StopService(
	__in LPCWSTR lpServiceName
	)
{
	SC_HANDLE sch;
	SC_HANDLE service;
	BOOL fStopped = FALSE;
	DWORD dwError = ERROR_SUCCESS;

	sch = OpenSCManager(NULL, NULL, GENERIC_EXECUTE);
	if(sch)
	{
		service = OpenService(sch, lpServiceName, GENERIC_EXECUTE);
		if(service)
		{
			SERVICE_STATUS ss;

			BOOL result = ControlService(service, SERVICE_CONTROL_STOP, &ss);
			dwError = GetLastError();

			if(result)
			{
				fStopped = TRUE;
			}
			else if(dwError == ERROR_SERVICE_NOT_ACTIVE)
			{
				fStopped = TRUE;
			}
			CloseServiceHandle(service);
		}
		else
		{
			dwError = GetLastError();
		}
		CloseServiceHandle(sch);
	}
	else
	{
		dwError = GetLastError();
	}

	SetLastError(dwError);
	return fStopped;
}

BOOL UninstallService(
	__in LPCWSTR lpServiceName
	)
{
	SC_HANDLE sch;
	SC_HANDLE service;
	BOOL success = TRUE;
	DWORD dwError = ERROR_SUCCESS;

	sch = OpenSCManager(NULL, NULL, GENERIC_WRITE);
	if(sch)
	{
		service = OpenService(sch, lpServiceName, DELETE);
		if(service)
		{
			if(IsServiceRunning(lpServiceName))
			{
				(VOID)StopService(lpServiceName);
			}

			BOOL result = DeleteService(service);

			if(result)
			{
				success = TRUE;
			}
			else
			{
				dwError = GetLastError();
				if(dwError == ERROR_SERVICE_MARKED_FOR_DELETE)
				{
					success = TRUE;
				}
				else
				{
					success = FALSE;
				}
			}

			CloseServiceHandle(service);
		}
		else
		{
			dwError = GetLastError();
		}
		CloseServiceHandle(sch);
	}
	else
	{
		dwError = GetLastError();
		success = FALSE;
	}

	SetLastError(dwError);
	return success;
}

BOOL IsServiceRunning(
	__in LPCWSTR lpServiceName
	)
{
	SC_HANDLE hService;
	SERVICE_STATUS ss;
	BOOL fOk;
	SC_HANDLE sch = OpenSCManager(NULL, NULL, GENERIC_READ);
	if(sch == 0)
	{
		return FALSE;
	}

	hService = OpenService(sch, lpServiceName, GENERIC_READ);

	if(!hService)
	{
		CloseServiceHandle(sch);
		return FALSE;
	}
	
	fOk = QueryServiceStatus(hService, &ss);

	CloseServiceHandle(hService);
	CloseServiceHandle(sch);

	if(!fOk)
	{        
		return FALSE;
	}

	return (ss.dwCurrentState == SERVICE_RUNNING);
}

BOOL IsServiceInstalled(
	__in LPCWSTR lpServiceName
	)
{
	BOOL fOk = FALSE;
	SC_HANDLE hService;
	SC_HANDLE sch = OpenSCManager(NULL, NULL, GENERIC_READ);
	if(sch == 0)
	{
		return FALSE;
	}

	hService = OpenService(sch, lpServiceName, GENERIC_READ);

	if(hService)
	{
		fOk = TRUE;
		CloseServiceHandle(hService);
	}

	CloseServiceHandle(sch);

	return fOk;
}