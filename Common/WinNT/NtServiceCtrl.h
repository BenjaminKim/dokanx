#pragma once

#include <Windows.h>

DWORD InstallService(
	__in        LPCWSTR lpServiceName,
	__in_opt    LPCWSTR lpDisplayName,
	__in        DWORD dwDesiredAccess,
	__in        DWORD dwServiceType,
	__in        DWORD dwStartType,
	__in        DWORD dwErrorControl,
	__in_opt    LPCWSTR lpBinaryPathName,
	__in_opt    LPCWSTR lpLoadOrderGroup,
	__out_opt   LPDWORD lpdwTagId,
	__in_opt    LPCWSTR lpDependencies,
	__in_opt    LPCWSTR lpServiceStartName,
	__in_opt    LPCWSTR lpPassword
	);

BOOL LoadDriver(
	__in LPCWSTR lpServiceName
	);

DWORD LoadDriverEx(
	__in LPCWSTR lpServiceName,
	__in DWORD dwNumServiceArgs, 
	__in_ecount_opt(dwNumServiceArgs) LPCWSTR *lpServiceArgVectors
	);

BOOL StopService(
	__in LPCWSTR lpServiceName
	);

BOOL UninstallService(
	__in LPCWSTR lpServiceName
	);

BOOL IsServiceRunning(
	__in LPCWSTR lpServiceName
	);

BOOL IsServiceInstalled(
	__in LPCWSTR lpServiceName
	);