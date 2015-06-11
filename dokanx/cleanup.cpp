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
DispatchCleanup(
	HANDLE				Handle,
	PEVENT_CONTEXT		EventContext,
	PDOKAN_INSTANCE		DokanInstance)
{
	PEVENT_INFORMATION		eventInfo;
	DOKAN_FILE_INFO			fileInfo;	
	PDOKAN_OPEN_INFO		openInfo;
	ULONG					sizeOfEventInfo = sizeof(EVENT_INFORMATION);

	logw(L"Start");

	CheckFileName(EventContext->Cleanup.FileName);

	eventInfo = DispatchCommon(
		EventContext, sizeOfEventInfo, DokanInstance, &fileInfo, &openInfo);
	
	eventInfo->Status = STATUS_SUCCESS; // return success at any case

	logw(L"###Cleanup %04d", openInfo != NULL ? openInfo->EventId : -1);

	if (DokanInstance->DokanOperations->Cleanup) {
		// ignore return value
		DokanInstance->DokanOperations->Cleanup(
			EventContext->Cleanup.FileName,
			&fileInfo);
	}

	//windows 8 doesn't use filedispositioninformation for delete
	if (EventContext->FileFlags & DOKAN_DELETE_ON_CLOSE){

		logw(L"DOKAN_DELETE_ON_CLOSE is there so execute Delete");
		
		if (&fileInfo.IsDirectory == reinterpret_cast<UCHAR*>("1")) {
			if (DokanInstance->DokanOperations->DeleteDirectory){
				logw(L"Call DokanInstance->DokanOperations->DeleteDirectory");
				DokanInstance->DokanOperations->DeleteDirectory(EventContext->Cleanup.FileName, &fileInfo);
			}
		}
		else {
			if (DokanInstance->DokanOperations->DeleteFile){
				logw(L"Call DokanInstance->DokanOperations->DeleteFile");
				DokanInstance->DokanOperations->DeleteFile(EventContext->Cleanup.FileName, &fileInfo);
			}
		}
	}

	openInfo->UserContext = fileInfo.Context;

	SendEventInformation(Handle, eventInfo, sizeOfEventInfo, DokanInstance);

	free(eventInfo);
	return;
}

