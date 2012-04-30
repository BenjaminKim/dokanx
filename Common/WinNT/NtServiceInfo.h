#pragma once

#include <Windows.h>

/**
*
*/
class NTServiceInfo
{
public:
	static BOOL IsServiceRunning(LPCWSTR lpServiceName);
	static BOOL IsServiceInstalled(LPCWSTR lpServiceName);
};