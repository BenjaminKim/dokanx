#include "stdafx.h"
#include "resource.h"
#include "DokanSSHFSControl.h"

class CDokanSSHFSControlModule : public CAtlDllModuleT< CDokanSSHFSControlModule >
{
public :
	DECLARE_LIBID(LIBID_DokanSSHFSControlLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_DOKANSSHFSCONTROL, "{F99AD9AF-5924-4A99-8E39-C74AD9234ED7}")
};

CDokanSSHFSControlModule _AtlModule;


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