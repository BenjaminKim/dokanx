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

#include "stdafx.h"
#include "../dokani.h"
#include "fileinfo.h"


VOID
DispatchLock(
	HANDLE				Handle,
	PEVENT_CONTEXT		EventContext,
	PDOKAN_INSTANCE		DokanInstance)
{
	DOKAN_FILE_INFO		fileInfo;
	PEVENT_INFORMATION	eventInfo;
	ULONG				sizeOfEventInfo = sizeof(EVENT_INFORMATION);
	PDOKAN_OPEN_INFO	openInfo;
	int status;

	logw(L"Start");

	CheckFileName(EventContext->Lock.FileName);

	eventInfo = DispatchCommon(
		EventContext, sizeOfEventInfo, DokanInstance, &fileInfo, &openInfo);

	logw(L"###Lock %04d", openInfo != NULL ? openInfo->EventId : -1);

	eventInfo->Status = STATUS_NOT_IMPLEMENTED;

	switch (EventContext->MinorFunction) {
	case IRP_MN_LOCK:
		if (DokanInstance->DokanOperations->LockFile) {

			status = DokanInstance->DokanOperations->LockFile(
						EventContext->Lock.FileName,
						EventContext->Lock.ByteOffset.QuadPart,
						EventContext->Lock.Length.QuadPart,
						//EventContext->Lock.Key,
						&fileInfo);

			eventInfo->Status = status < 0 ?
				STATUS_LOCK_NOT_GRANTED : STATUS_SUCCESS;
		}
		break;
	case IRP_MN_UNLOCK_ALL:
		break;
	case IRP_MN_UNLOCK_ALL_BY_KEY:
		break;
	case IRP_MN_UNLOCK_SINGLE:
		if (DokanInstance->DokanOperations->UnlockFile) {
		
			status = DokanInstance->DokanOperations->UnlockFile(
						EventContext->Lock.FileName,
						EventContext->Lock.ByteOffset.QuadPart,
						EventContext->Lock.Length.QuadPart,
						//EventContext->Lock.Key,
						&fileInfo);

			eventInfo->Status = STATUS_SUCCESS; // at any time return success ?
		}
		break;
	default:
		logw(L"unkown lock function %d\n", EventContext->MinorFunction);
	}

	openInfo->UserContext = fileInfo.Context;

	SendEventInformation(Handle, eventInfo, sizeOfEventInfo, DokanInstance);

	free(eventInfo);
	return;
}
