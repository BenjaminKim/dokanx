/*
  Dokan : user-mode file system library for Windows

  Copyright (C) 2008 Hiroki Asakawa info@dokan-dev.net

  http://dokan-dev.net/en

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free
Software Foundation; either version 3 of the License, or (at your option) any
later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along
with this program. If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef _DOKANI_H_
#define _DOKANI_H_

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

#include "../public.h"
#include "dokanc.h"
#include "list.h"
#include "dokan.h"

#ifdef __cplusplus
extern "C" {
#endif



typedef struct _DOKAN_INSTANCE {
	// to ensure that unmount dispatch is called at once
	CRITICAL_SECTION	CriticalSection;

	// store CurrentDeviceName
	// (when there are many mounts, each mount use
	// other DeviceName)
	WCHAR	DeviceName[64];
	WCHAR	MountPoint[MAX_PATH];

	ULONG	DeviceNumber;
	ULONG	MountId;

	PDOKAN_OPTIONS		DokanOptions;
	PDOKAN_OPERATIONS	DokanOperations;

	LIST_ENTRY	ListEntry;
} DOKAN_INSTANCE, *PDOKAN_INSTANCE;

//
// OPEN_INFO는 하나의 열려진 파일에 대한 정보를 담고 있다.
// 유저모드 파일 시스템이 Create Irp를 받을 때 메모리를 할당하며 Close Irp 에서 해제한다.
// 드라이버의 CCB와 대응된다.(확실하게 확인 필요)
//
typedef struct _DOKAN_OPEN_INFO {
	BOOL			IsDirectory;
	// 뭔지 확인할 것. 알아야 한다.
	// It is set to 2 when he creates.
	// Everytime we send a I/O result information, it will be increased.
	// Because IRP_MJ_CLOSE decrease it, it should be 2 first.
	ULONG			OpenCount;
	PEVENT_CONTEXT	EventContext;
	PDOKAN_INSTANCE	DokanInstance;
	//
	// 사용자 모드 파일의 실제 핸들이 여기에 담긴다.
	//
	ULONG64			UserContext;
	//
	// 이벤트 아이디는 OPEN_INFO 구조체에 대한 시퀀스이다. 즉 드라이버가 구동된 후, 파일이 몇번이나 열렸었는지(IRP가 몇개나 들어왔었는지가 아니라) 알 수 있다.
	//
	ULONG			EventId;
	// 뭔지 확인할 것. 알아야 한다.
	PLIST_ENTRY		DirListHead;
} DOKAN_OPEN_INFO, *PDOKAN_OPEN_INFO;


BOOL
DokanStart(
	PDOKAN_INSTANCE	Instance
	);

BOOL
SendToDevice(
	LPCWSTR	DeviceName,
	DWORD	IoControlCode,
	PVOID	InputBuffer,
	ULONG	InputLength,
	PVOID	OutputBuffer,
	ULONG	OutputLength,
	PULONG	ReturnedLength);

LPCWSTR
GetRawDeviceName(LPCWSTR	DeviceName);

UINT __stdcall
DokanLoop(
	PVOID Param);


BOOL
DokanMount(
	LPCWSTR	MountPoint,
	LPCWSTR	DeviceName);

VOID
SendEventInformation(
	HANDLE				Handle,
	PEVENT_INFORMATION	EventInfo,
	ULONG				EventLength,
	PDOKAN_INSTANCE		DokanInstance);


PEVENT_INFORMATION
DispatchCommon(
	PEVENT_CONTEXT		EventContext,
	ULONG				SizeOfEventInfo,
	PDOKAN_INSTANCE		DokanInstance,
	PDOKAN_FILE_INFO	DokanFileInfo,
	PDOKAN_OPEN_INFO*	DokanOpenInfo);


VOID
DispatchDirectoryInformation(
	HANDLE				Handle,
	PEVENT_CONTEXT		EventContext,
	PDOKAN_INSTANCE		DokanInstance);


VOID
DispatchQueryInformation(
	HANDLE				Handle,
	PEVENT_CONTEXT		EventContext,
	PDOKAN_INSTANCE		DokanInstance);


VOID
DispatchQueryVolumeInformation(
	HANDLE				Handle,
	PEVENT_CONTEXT		EventContext,
	PDOKAN_INSTANCE		DokanInstance);


VOID
DispatchSetInformation(
	HANDLE				Handle,
	PEVENT_CONTEXT		EventContext,
	PDOKAN_INSTANCE		DokanInstance);


VOID
DispatchRead(
	HANDLE				Handle,
	PEVENT_CONTEXT		EventContext,
	PDOKAN_INSTANCE		DokanInstance);


VOID
DispatchWrite(
	HANDLE				Handle,
	PEVENT_CONTEXT		EventContext,
	PDOKAN_INSTANCE		DokanInstance);


VOID
DispatchCreate(
	HANDLE				Handle,
	PEVENT_CONTEXT		EventContext,
	PDOKAN_INSTANCE		DokanInstance);


VOID
DispatchClose(
	HANDLE				Handle,
	PEVENT_CONTEXT		EventContext,
	PDOKAN_INSTANCE		DokanInstance);


VOID
DispatchCleanup(
	HANDLE				Handle,
	PEVENT_CONTEXT		EventContext,
	PDOKAN_INSTANCE		DokanInstance);


VOID
DispatchFlush(
	HANDLE				Handle,
	PEVENT_CONTEXT		EventContext,
	PDOKAN_INSTANCE		DokanInstance);


VOID
DispatchUnmount(
	HANDLE				Handle,
	PEVENT_CONTEXT		EventContext,
	PDOKAN_INSTANCE		DokanInstance);


VOID
DispatchLock(
	HANDLE				Handle,
	PEVENT_CONTEXT		EventContext,
	PDOKAN_INSTANCE		DokanInstance);


VOID
DispatchQuerySecurity(
	HANDLE				Handle,
	PEVENT_CONTEXT		EventContext,
	PDOKAN_INSTANCE		DokanInstance);


VOID
DispatchSetSecurity(
	HANDLE			Handle,
	PEVENT_CONTEXT	EventContext,
	PDOKAN_INSTANCE	DokanInstance);


BOOLEAN
InstallDriver(
	SC_HANDLE  SchSCManager,
	LPCWSTR    DriverName,
	LPCWSTR    ServiceExe);


BOOLEAN
RemoveDriver(
	SC_HANDLE  SchSCManager,
	LPCWSTR    DriverName);


BOOLEAN
StartDriver(
	SC_HANDLE  SchSCManager,
	LPCWSTR    DriverName);


BOOLEAN
StopDriver(
	SC_HANDLE  SchSCManager,
	LPCWSTR    DriverName);


BOOLEAN
ManageDriver(
	LPCWSTR  DriverName,
	LPCWSTR  ServiceName,
	USHORT   Function);


BOOL
SendReleaseIRP(
	LPCWSTR DeviceName);

VOID
CheckFileName(
	LPWSTR	FileName);

VOID
ClearFindData(
  PLIST_ENTRY	ListHead);

UINT WINAPI
	DokanKeepAlive(
	PVOID pDokanInstance);


NTSTATUS
GetNTStatus(DWORD ErrorCode);

PDOKAN_OPEN_INFO
GetDokanOpenInfo(
	PEVENT_CONTEXT		EventInfomation,
	PDOKAN_INSTANCE		DokanInstance);

VOID
ReleaseDokanOpenInfo(
	PEVENT_INFORMATION	EventInfomation,
	PDOKAN_INSTANCE		DokanInstance);


#ifdef __cplusplus
}
#endif


#endif
