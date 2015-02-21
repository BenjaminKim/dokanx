#pragma once
#include "resource.h"
#include <string>
#include <list>

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

// CSSHProperty

class ATL_NO_VTABLE CSSHProperty :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CSSHProperty, &CLSID_SSHProperty>,
	public IShellExtInit,
	public IShellPropSheetExt
{
public:
	CSSHProperty()
	{
	}


DECLARE_REGISTRY_RESOURCEID(IDR_SSHPROPERTY)

DECLARE_NOT_AGGREGATABLE(CSSHProperty)

BEGIN_COM_MAP(CSSHProperty)
	COM_INTERFACE_ENTRY(IShellExtInit)
	COM_INTERFACE_ENTRY(IShellPropSheetExt)
END_COM_MAP()



	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalRelease()
	{
	}

	typedef std::list< std::wstring > string_list;
 
public:


    // IShellExtInit
    STDMETHODIMP Initialize(LPCITEMIDLIST, LPDATAOBJECT, HKEY);

	// IShellPropSheetExt
    STDMETHODIMP AddPages(LPFNADDPROPSHEETPAGE, LPARAM);
    STDMETHODIMP ReplacePage(UINT, LPFNADDPROPSHEETPAGE, LPARAM) { return E_NOTIMPL; }

protected:
	string_list m_lsFiles;

};

OBJECT_ENTRY_AUTO(__uuidof(SSHProperty), CSSHProperty)