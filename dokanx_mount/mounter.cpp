/*

Copyright (c) 2007, 2008 Hiroki Asakawa asakaw@gmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/
#include "stdafx.h"
#include <windows.h>
#include <WinIoCtl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sddl.h>
#include <WtsApi32.h> 
#include "mount.h"
#include "../public.h"
#include "../Common/WinNT/Explorer.h"

#pragma comment(lib, "WtsApi32.lib")

static HANDLE                g_EventControl = NULL;
static SERVICE_STATUS        g_ServiceStatus;
static SERVICE_STATUS_HANDLE g_StatusHandle = NULL;

static HANDLE	g_EventLog = NULL;

static CRITICAL_SECTION	g_CriticalSection;
static LIST_ENTRY		g_MountList;

BOOL g_DebugMode = TRUE;
BOOL g_UseStdErr = FALSE;

PMOUNT_ENTRY
InsertMountEntry(PDOKAN_CONTROL DokanControl)
{
    logw(L"Start");
    PMOUNT_ENTRY mountEntry = (PMOUNT_ENTRY)malloc(sizeof(MOUNT_ENTRY));
    if (mountEntry == nullptr) {
        logw(L"InsertMountEntry malloc failed");
        return NULL;
    }
    ZeroMemory(mountEntry, sizeof(MOUNT_ENTRY));
    CopyMemory(&mountEntry->MountControl, DokanControl, sizeof(DOKAN_CONTROL));
    InitializeListHead(&mountEntry->ListEntry);

    EnterCriticalSection(&g_CriticalSection);
    InsertTailList(&g_MountList, &mountEntry->ListEntry);
    LeaveCriticalSection(&g_CriticalSection);

    return mountEntry;
}

VOID
RemoveMountEntry(PMOUNT_ENTRY MountEntry)
{
    logw(L"Start");
    EnterCriticalSection(&g_CriticalSection);
    RemoveEntryList(&MountEntry->ListEntry);
    LeaveCriticalSection(&g_CriticalSection);

    free(MountEntry);
}

PMOUNT_ENTRY
FindMountEntry(PDOKAN_CONTROL	DokanControl)
{
    logw(L"Start");
    PLIST_ENTRY		listEntry;
    PMOUNT_ENTRY	mountEntry = nullptr;
    BOOL			useMountPoint = wcslen(DokanControl->MountPoint) > 0;
    BOOL			found = FALSE;

    if (!useMountPoint && wcslen(DokanControl->DeviceName) == 0) {
        return NULL;
    }

    EnterCriticalSection(&g_CriticalSection);

    for (listEntry = g_MountList.Flink; listEntry != &g_MountList; listEntry = listEntry->Flink) {
        mountEntry = CONTAINING_RECORD(listEntry, MOUNT_ENTRY, ListEntry);
        if (useMountPoint) {
            if (wcscmp(DokanControl->MountPoint, mountEntry->MountControl.MountPoint) == 0) {
                found = TRUE;
                break;
            }
        } else {
            if (wcscmp(DokanControl->DeviceName, mountEntry->MountControl.DeviceName) == 0) {
                found = TRUE;
                break;
            }
        }
    }

    LeaveCriticalSection(&g_CriticalSection);

    if (found) {
        logw(L"FindMountEntry %s -> %s\n",
            mountEntry->MountControl.MountPoint, mountEntry->MountControl.DeviceName);
        return mountEntry;
    } else {
        return NULL;
    }
}

VOID
DokanControlFind(PDOKAN_CONTROL Control)
{
    logw(L"Start");
    //PLIST_ENTRY		listEntry;
    PMOUNT_ENTRY	mountEntry;

    mountEntry = FindMountEntry(Control);
    if (mountEntry == NULL) {
        Control->Status = DOKAN_CONTROL_FAIL;
    } else {
        wcscpy_s(Control->DeviceName, sizeof(Control->DeviceName) / sizeof(WCHAR),
                mountEntry->MountControl.DeviceName);
        wcscpy_s(Control->MountPoint, sizeof(Control->MountPoint) / sizeof(WCHAR),
                mountEntry->MountControl.MountPoint);
        Control->Status = DOKAN_CONTROL_SUCCESS;
    }
}

VOID
DokanControlList(PDOKAN_CONTROL Control)
{
    logw(L"Start");
    PLIST_ENTRY		listEntry;
    PMOUNT_ENTRY	mountEntry;
    ULONG			index = 0;

    EnterCriticalSection(&g_CriticalSection);
    Control->Status = DOKAN_CONTROL_FAIL;

    for (listEntry = g_MountList.Flink;
        listEntry != &g_MountList;
        listEntry = listEntry->Flink) {
        mountEntry = CONTAINING_RECORD(listEntry, MOUNT_ENTRY, ListEntry);
        if (Control->Option == index++) {
            wcscpy_s(Control->DeviceName, sizeof(Control->DeviceName) / sizeof(WCHAR),
                    mountEntry->MountControl.DeviceName);
            wcscpy_s(Control->MountPoint, sizeof(Control->MountPoint) / sizeof(WCHAR),
                    mountEntry->MountControl.MountPoint);
            Control->Status = DOKAN_CONTROL_SUCCESS;
            break;
        }
    }
    LeaveCriticalSection(&g_CriticalSection);
}

static BOOL ImpersonateUserBySession(DWORD dwSessionId, PHANDLE phToken){
	
	logw(L"Start");

	logw(L"WTSQueryUserToken:%d", dwSessionId);

	if (!WTSQueryUserToken(dwSessionId, phToken)){

		logw(L"WTSQueryUserToken for session %d failed:%d", dwSessionId, GetLastError());

		return FALSE;
	}

	if (!ImpersonateLoggedOnUser(*phToken)){

		logw(L"ImpersonateLoggedOnUser for token %d failed:%d", *phToken, GetLastError());
		CloseHandle(*phToken);
		return FALSE;
	}

	logw(L"Impersonation successfully done");

	return TRUE;

}

static VOID DokanControl(PDOKAN_CONTROL Control)
{
    logw(L"Start");
    PMOUNT_ENTRY	mountEntry;
    ULONG	index = 0;
    DWORD written = 0;
	BOOL impersonate;
	HANDLE token;

    Control->Status = DOKAN_CONTROL_FAIL;

    switch (Control->Type)
    {
    case DOKAN_CONTROL_MOUNT:

        logw(L"DokanControl Mount");

		impersonate = Control->Option & DOKAN_CONTROL_OPTION_LOCAL_CONTEXT;

		if (impersonate){
			logw(L"Impersonate is enabled");
			if (!ImpersonateUserBySession(Control->SessionId, &token))
				break;
		}

        if (DokanControlMount(Control->MountPoint, Control->DeviceName)) {
            Control->Status = DOKAN_CONTROL_SUCCESS;
            InsertMountEntry(Control);
        } else {
            Control->Status = DOKAN_CONTROL_FAIL;
        }

		if (impersonate){
			RevertToSelf();
			CloseHandle(token);
		}

        break;

    case DOKAN_CONTROL_UNMOUNT:

        logw(L"DokanControl Unmount");

        mountEntry = FindMountEntry(Control);
        if (mountEntry == NULL) {
            if (Control->Option == DOKAN_CONTROL_OPTION_FORCE_UNMOUNT &&
                DokanControlUnmount(Control->MountPoint)) {
                Control->Status = DOKAN_CONTROL_SUCCESS;
                break;
            }
            Control->Status = DOKAN_CONTROL_FAIL;
            break;	
        }

		impersonate = mountEntry->MountControl.Option & DOKAN_CONTROL_OPTION_LOCAL_CONTEXT;

		if (impersonate){
			if (!ImpersonateUserBySession(Control->SessionId, &token))
				break;
		}

        if (DokanControlUnmount(mountEntry->MountControl.MountPoint)) {
            Control->Status = DOKAN_CONTROL_SUCCESS;
            if (wcslen(Control->DeviceName) == 0) {
                wcscpy_s(Control->DeviceName, sizeof(Control->DeviceName) / sizeof(WCHAR),
                        mountEntry->MountControl.DeviceName);
            }
            RemoveMountEntry(mountEntry);
        } else {
            mountEntry->MountControl.Status = DOKAN_CONTROL_FAIL;
            Control->Status = DOKAN_CONTROL_FAIL;
        }

		if (impersonate){
			RevertToSelf();
			CloseHandle(token);
		}

        break;

    case DOKAN_CONTROL_CHECK:
        {
            logw(L"DokanControl Check");
            Control->Status = 0;
        }
        break;

    case DOKAN_CONTROL_FIND:
        {
            logw(L"DokanControl Find");
            DokanControlFind(Control);
        }
        break;

    case DOKAN_CONTROL_LIST:
        {
            logw(L"DokanControl List");
            DokanControlList(Control);
        }
        break;

    default:
        logw(L"DokanControl UnknownType %u\n", Control->Type);
    }

    return;
}



static DWORD WINAPI HandlerEx(
    DWORD dwControl,
    DWORD dwEventType,
    LPVOID lpEventData,
    LPVOID lpContext
    )
{
    logw(L"Start");
    switch (dwControl) {
    case SERVICE_CONTROL_STOP:

        g_ServiceStatus.dwWaitHint     = 50000;
        g_ServiceStatus.dwCheckPoint   = 0;
        g_ServiceStatus.dwCurrentState = SERVICE_STOP_PENDING;
        SetServiceStatus(g_StatusHandle, &g_ServiceStatus);

        SetEvent(g_EventControl);

        break;
    
    case SERVICE_CONTROL_INTERROGATE:
        SetServiceStatus(g_StatusHandle, &g_ServiceStatus);
        break;

    default:
        break;
    }

    return NO_ERROR;
}


static VOID BuildSecurityAttributes(PSECURITY_ATTRIBUTES SecurityAttributes)
{
    static const std::wstring sd = L"D:P(A;;GA;;;SY)(A;;GRGWGX;;;BA)(A;;GRGW;;;WD)(A;;GR;;;RC)";

    ZeroMemory(SecurityAttributes, sizeof(SECURITY_ATTRIBUTES));
    
    ConvertStringSecurityDescriptorToSecurityDescriptor(
        sd.c_str(),
        SDDL_REVISION_1,
        &SecurityAttributes->lpSecurityDescriptor,
        NULL);

    SecurityAttributes->nLength = sizeof(SECURITY_ATTRIBUTES);
    SecurityAttributes->bInheritHandle = TRUE;
}


static VOID WINAPI ServiceMain(DWORD dwArgc, LPTSTR *lpszArgv)
{
    logw(L"Start");
    DWORD			eventNo;
    HANDLE			pipe, device;
    HANDLE			eventConnect, eventUnmount;
    HANDLE			eventArray[3];
    DOKAN_CONTROL	control, unmount;
    OVERLAPPED		ov, driver;
    ULONG			returnedBytes;
    EVENT_CONTEXT	eventContext;
    SECURITY_ATTRIBUTES sa;

    InitializeCriticalSectionAndSpinCount(&g_CriticalSection, 4000);
    InitializeListHead(&g_MountList);

    g_StatusHandle = RegisterServiceCtrlHandlerEx(DOKAN_MOUNTER_SERVICE, HandlerEx, NULL);

    // extend completion time
    g_ServiceStatus.dwServiceType				= SERVICE_WIN32_OWN_PROCESS;
    g_ServiceStatus.dwWin32ExitCode				= NO_ERROR;
    g_ServiceStatus.dwControlsAccepted			= SERVICE_ACCEPT_STOP;
    g_ServiceStatus.dwServiceSpecificExitCode	= 0;
    g_ServiceStatus.dwWaitHint					= 30000;
    g_ServiceStatus.dwCheckPoint				= 1;
    g_ServiceStatus.dwCurrentState				= SERVICE_START_PENDING;
    SetServiceStatus(g_StatusHandle, &g_ServiceStatus);

    BuildSecurityAttributes(&sa);

    pipe = CreateNamedPipe(DOKAN_CONTROL_PIPE,
        PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED, 
        PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
        1, sizeof(control), sizeof(control), 1000, &sa);

    if (pipe == INVALID_HANDLE_VALUE) {
        // TODO: should do something
        logw(L"DokanMounter: failed to create named pipe: %d\n", GetLastError());
    }

    device = CreateFile(
                DOKAN_GLOBAL_DEVICE_NAME,			// lpFileName
                GENERIC_READ | GENERIC_WRITE,       // dwDesiredAccess
                FILE_SHARE_READ | FILE_SHARE_WRITE, // dwShareMode
                NULL,                               // lpSecurityAttributes
                OPEN_EXISTING,                      // dwCreationDistribution
                FILE_FLAG_OVERLAPPED,               // dwFlagsAndAttributes
                NULL                                // hTemplateFile
            );
    
    if (device == INVALID_HANDLE_VALUE) {
        // TODO: should do something
        logw(L"DokanMounter: failed to open device: %d\n", GetLastError());
    }

    eventConnect = CreateEvent(NULL, FALSE, FALSE, NULL);
    eventUnmount = CreateEvent(NULL, FALSE, FALSE, NULL);
    g_EventControl = CreateEvent(NULL, TRUE, FALSE, NULL);

    g_ServiceStatus.dwWaitHint     = 0;
    g_ServiceStatus.dwCheckPoint   = 0;
    g_ServiceStatus.dwCurrentState = SERVICE_RUNNING;
    SetServiceStatus(g_StatusHandle, &g_ServiceStatus);

    for (;;) {
        ZeroMemory(&ov, sizeof(OVERLAPPED));
        ZeroMemory(&driver, sizeof(OVERLAPPED));
        ZeroMemory(&eventContext, sizeof(EVENT_CONTEXT));

        ov.hEvent = eventConnect;
        driver.hEvent = eventUnmount;

        ConnectNamedPipe(pipe, &ov);
        if (!DeviceIoControl(device, IOCTL_SERVICE_WAIT, NULL, 0,
            &eventContext, sizeof(EVENT_CONTEXT), NULL, &driver)) {
            DWORD error = GetLastError();
            if (error != 997) {
                logw(L"DokanMounter: DeviceIoControl error: %d\n", error);
            }
        }

        eventArray[0] = eventConnect;
        eventArray[1] = eventUnmount;
        eventArray[2] = g_EventControl;

        eventNo = WaitForMultipleObjects(3, eventArray, FALSE, INFINITE) - WAIT_OBJECT_0;

        logw(L"DokanMouner: get an event");

        if (eventNo == 0) {

            DWORD result = 0;

            ZeroMemory(&control, sizeof(control));
            if (ReadFile(pipe, &control, sizeof(control), &result, NULL)) {
                DokanControl(&control);
                WriteFile(pipe, &control, sizeof(control), &result, NULL);
            }
            FlushFileBuffers(pipe);
            DisconnectNamedPipe(pipe);
        
        } else if (eventNo == 1) {

            if (GetOverlappedResult(device, &driver, &returnedBytes, FALSE)) {
                if (returnedBytes == sizeof(EVENT_CONTEXT)) {
                    logw(L"DokanMounter: Unmount");

                    ZeroMemory(&unmount, sizeof(DOKAN_CONTROL));
                    unmount.Type = DOKAN_CONTROL_UNMOUNT;
                    wcscpy_s(unmount.DeviceName, _countof(unmount.DeviceName),
                            eventContext.Unmount.DeviceName);
                    DokanControl(&unmount);
                } else {
                    logw(L"DokanMounter: Unmount error\n", control.Type);
                }
            }

			FlushFileBuffers(pipe);
			DisconnectNamedPipe(pipe);

        } else if (eventNo == 2) {
            logw(L"DokanMounter: stop mounter service");
			
			FlushFileBuffers(pipe);
			DisconnectNamedPipe(pipe);

            g_ServiceStatus.dwWaitHint     = 0;
            g_ServiceStatus.dwCheckPoint   = 0;
            g_ServiceStatus.dwCurrentState = SERVICE_STOPPED;
            SetServiceStatus(g_StatusHandle, &g_ServiceStatus);

            break;
        }
        else
            break;
    }


    CloseHandle(pipe);
    CloseHandle(eventConnect);
    CloseHandle(g_EventControl);
    CloseHandle(device);
    CloseHandle(eventUnmount);

    DeleteCriticalSection(&g_CriticalSection);

    return;
}

int WINAPI WinMain(
    HINSTANCE hinst,
    HINSTANCE hinstPrev,
    LPSTR lpszCmdLine,
    int nCmdShow
    )
{
    logw(L"Start");
    SERVICE_TABLE_ENTRY serviceTable[] = {
        {DOKAN_MOUNTER_SERVICE, ServiceMain}, {NULL, NULL}
    };

    StartServiceCtrlDispatcher(serviceTable);

    logw(L"End");
    return 0;
}