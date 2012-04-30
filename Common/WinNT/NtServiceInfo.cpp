#include "NTServiceInfo.h"

#include <Windows.h>
#include <WinSvc.h>

#pragma warning(push)
#pragma warning(disable:4995)
#include <string>
#pragma warning(pop)

#include "../Log/NdLog.h"

BOOL NTServiceInfo::IsServiceRunning(LPCWSTR lpServiceName)
{
	SC_HANDLE sch = OpenSCManager(NULL, NULL, GENERIC_READ);
	if(sch == 0)
	{
		return FALSE;
	}

	SC_HANDLE hService = OpenService(sch, lpServiceName, GENERIC_READ);

	if(!hService)
	{
		CloseServiceHandle(sch);
		return FALSE;
	}

	SERVICE_STATUS ss;
	BOOL fOk = QueryServiceStatus(hService, &ss);
	if(!fOk)
	{
		CloseServiceHandle(hService);
		CloseServiceHandle(sch);
		return FALSE;
	}

	CloseServiceHandle(hService);
	CloseServiceHandle(sch);

	if (ss.dwCurrentState == SERVICE_RUNNING)
	{
		logw(L"%s", (std::wstring(lpServiceName) + L", The service is running\n").c_str());
		return TRUE;
	}
	else
	{
		logw(L"%s", (std::wstring(lpServiceName) + L", The service is not running\n").c_str());
		return FALSE;
	}
}

BOOL NTServiceInfo::IsServiceInstalled(LPCWSTR lpServiceName)
{
	SC_HANDLE sch = OpenSCManager(NULL, NULL, GENERIC_READ);
	if(sch == 0)
	{
		return FALSE;
	}

	SC_HANDLE hService = OpenService(sch, lpServiceName, GENERIC_READ);

	BOOL fOk = hService ? TRUE : FALSE;

	if(hService)
	{
		CloseServiceHandle(hService);
	}

	if(sch)
	{
		CloseServiceHandle(sch);
	}

	return fOk;
}
