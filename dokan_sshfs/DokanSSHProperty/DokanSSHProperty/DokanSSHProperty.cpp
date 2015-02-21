#include "stdafx.h"
#include "resource.h"
#include "DokanSSHProperty.h"
#include "DokanSSHPropertyModule.h"

CDokanSSHPropertyModule _AtlModule;


#ifdef _MANAGED
#pragma managed(push, off)
#endif

extern "C" BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	hInstance;
    return _AtlModule.DllMain(dwReason, lpReserved); 
}

#ifdef _MANAGED
#pragma managed(pop)
#endif

STDAPI DllCanUnloadNow(void)
{
    return _AtlModule.DllCanUnloadNow();
}

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
    return _AtlModule.DllGetClassObject(rclsid, riid, ppv);
}

STDAPI DllRegisterServer(void)
{
    HRESULT hr = _AtlModule.DllRegisterServer();
	return hr;
}

STDAPI DllUnregisterServer(void)
{
	HRESULT hr = _AtlModule.DllUnregisterServer();
	return hr;
}

