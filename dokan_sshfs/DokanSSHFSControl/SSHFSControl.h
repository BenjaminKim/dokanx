#pragma once
#include "resource.h"

#include <shlobj.h>
#include <comdef.h>
#include <string>
#include <list>

#include "DokanSSHFSControl.h"

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

// CSSHFSControl

class ATL_NO_VTABLE CSSHFSControl :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CSSHFSControl, &CLSID_SSHFSControl>,
	public IShellExtInit,
	public IContextMenu
{
public:
	CSSHFSControl()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_SSHFSCONTROL)

DECLARE_NOT_AGGREGATABLE(CSSHFSControl)

BEGIN_COM_MAP(CSSHFSControl)
	COM_INTERFACE_ENTRY(IShellExtInit)
	COM_INTERFACE_ENTRY(IContextMenu)
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

	// IContextMenu
	STDMETHODIMP GetCommandString(UINT, UINT, UINT*, LPSTR, UINT);
	STDMETHODIMP InvokeCommand(LPCMINVOKECOMMANDINFO);
	STDMETHODIMP QueryContextMenu(HMENU, UINT, UINT, UINT, UINT);

protected:
	string_list m_lsFiles;

};

OBJECT_ENTRY_AUTO(__uuidof(SSHFSControl), CSSHFSControl)
