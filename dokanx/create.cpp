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
#include "../Common/Log/NtLogString.h"
#include "../Common/Log/NdLog.h"

VOID OnFileOpenSucceed(
    __inout PEVENT_INFORMATION pEventInfo,
    __in DWORD dwNtDisposition,
    __in LONG status,
    __in BOOL isDirectory
    )
{
    pEventInfo->Status = STATUS_SUCCESS;
    pEventInfo->Create.Information = FILE_OPENED;

    if (dwNtDisposition == FILE_CREATE ||
        dwNtDisposition == FILE_OPEN_IF ||
        dwNtDisposition == FILE_OVERWRITE_IF)
    {
        if (status == ERROR_ALREADY_EXISTS || status == ERROR_FILE_EXISTS)
        {
            if (dwNtDisposition == FILE_OPEN_IF)
            {
                pEventInfo->Create.Information = FILE_OPENED;
            }
            else if (dwNtDisposition == FILE_OVERWRITE_IF)
            {
                pEventInfo->Create.Information = FILE_OVERWRITTEN;
            }
        }
        else
        {
            pEventInfo->Create.Information = FILE_CREATED;
        }
    }

    if ((dwNtDisposition == FILE_OVERWRITE_IF || dwNtDisposition == FILE_OVERWRITE) &&
        pEventInfo->Create.Information != FILE_CREATED)
    {
        pEventInfo->Create.Information = FILE_OVERWRITTEN;
    }

    if (isDirectory)
    {
        pEventInfo->Create.Flags |= FILE_DIRECTORY_FILE;
    }
}

VOID
DispatchCreate(
    HANDLE hDevice, // This handle is not for a file. It is for Dokan Device Driver(which is doing EVENT_WAIT).
    PEVENT_CONTEXT pEventContext,
    PDOKAN_INSTANCE	pDokanInstance)
{
    logw(L"Start");
    static int eventId = 0;
    ULONG cbEventInformation = sizeof(EVENT_INFORMATION);
    PEVENT_INFORMATION pEventInfo = (PEVENT_INFORMATION)malloc(cbEventInformation);
    int	status = STATUS_INSUFFICIENT_RESOURCES;
    DOKAN_FILE_INFO	fileInfo;
    DWORD dwNtDisposition;
    PDOKAN_OPEN_INFO pOpenInfo;
    BOOL directoryRequested = FALSE;
    DWORD options;

    CheckFileName(pEventContext->Create.FileName);

    RtlZeroMemory(pEventInfo, cbEventInformation);
    RtlZeroMemory(&fileInfo, sizeof(DOKAN_FILE_INFO));

    pEventInfo->BufferLength = 0;
    pEventInfo->SerialNumber = pEventContext->SerialNumber;

    fileInfo.ProcessId = pEventContext->ProcessId;
    fileInfo.DokanOptions = pDokanInstance->DokanOptions;

    // DOKAN_OPEN_INFO is structure for a opened file
    // this will be freed by Close
    pOpenInfo = (PDOKAN_OPEN_INFO)malloc(sizeof(DOKAN_OPEN_INFO));
    if (pOpenInfo == nullptr)
    {
        pEventInfo->Status = (ULONG)STATUS_INSUFFICIENT_RESOURCES;
        SendEventInformation(hDevice, pEventInfo, cbEventInformation, nullptr);
        return;
    }
    ZeroMemory(pOpenInfo, sizeof(DOKAN_OPEN_INFO));
    pOpenInfo->OpenCount = 2;
    pOpenInfo->EventContext = pEventContext;
    pOpenInfo->DokanInstance = pDokanInstance;
    fileInfo.DokanContext = (ULONG64)pOpenInfo;

    // pass it to driver and when the same handle is used get it back
    pEventInfo->Context = (ULONG64)pOpenInfo;

    // The high 8 bits of this parameter correspond to the Disposition parameter
    dwNtDisposition = (pEventContext->Create.CreateOptions >> 24) & 0x000000ff;

    // The low 24 bits of this member correspond to the CreateOptions parameter
    options = pEventContext->Create.CreateOptions & FILE_VALID_OPTION_FLAGS;
    //DbgPrint("Create.CreateOptions 0x%x\n", options);

    // to open directory
    // even if this flag is not specifed, 
    // there is a case to open a directory
    if (options & FILE_DIRECTORY_FILE)
    {
        directoryRequested = TRUE;
    }

    // to open no directory file
    // event if this flag is not specified,
    // there is a case to open non directory file
    if (options & FILE_NON_DIRECTORY_FILE)
    {
        logw(L"FILE_NON_DIRECTORY_FILE");
    }

    if (options & FILE_DELETE_ON_CLOSE)
    {
        pEventContext->Create.FileAttributes |= FILE_FLAG_DELETE_ON_CLOSE;
    }

    logw(L"###Create %04d", eventId);
    pOpenInfo->EventId = eventId++;

    logw(L"Path<%s>, NtDisposition<%s>, FileAttributes<>",
        pEventContext->Create.FileName,
        GetNtCreateDispositionStr(dwNtDisposition)
        );

    // make a directory or open
    if (directoryRequested)
    {
        fileInfo.IsDirectory = TRUE;

        if (dwNtDisposition == FILE_CREATE || dwNtDisposition == FILE_OPEN_IF)
        {
            if (pDokanInstance->DokanOperations->CreateDirectory)
            {
                status = pDokanInstance->DokanOperations->CreateDirectory(pEventContext->Create.FileName, &fileInfo);
            }
        }
        else if (dwNtDisposition == FILE_OPEN)
        {
            if (pDokanInstance->DokanOperations->OpenDirectory)
            {
                status = pDokanInstance->DokanOperations->OpenDirectory(pEventContext->Create.FileName, &fileInfo);
            }
        }
        else
        {
            logw(L"### Create other disposition : %d", dwNtDisposition);
        }
    }
    else
    {
        // open a file
        DWORD creationDisposition = OPEN_EXISTING;
        fileInfo.IsDirectory = FALSE;
        logw(L"   CreateDisposition %0x08X", dwNtDisposition);
        switch (dwNtDisposition)
        {
            case FILE_CREATE:
                creationDisposition = CREATE_NEW;
                break;
            case FILE_OPEN:
                creationDisposition = OPEN_EXISTING;
                break;
            case FILE_OPEN_IF:
                creationDisposition = OPEN_ALWAYS;
                break;
            case FILE_OVERWRITE:
                creationDisposition = TRUNCATE_EXISTING;
                break;
            case FILE_OVERWRITE_IF:
                creationDisposition = CREATE_ALWAYS;
                break;
            default:
                // TODO: should support FILE_SUPERSEDE ?
                logw(L"### Create other disposition : %d", dwNtDisposition);
                break;
        }
        
        if (pDokanInstance->DokanOperations->CreateFile)
        {
            status = pDokanInstance->DokanOperations->CreateFile(
                pEventContext->Create.FileName,
                pEventContext->Create.DesiredAccess,
                pEventContext->Create.ShareAccess,
                creationDisposition,
                pEventContext->Create.FileAttributes,
                &fileInfo);
        }
    }

    // save the information about this access in DOKAN_OPEN_INFO
    pOpenInfo->IsDirectory = fileInfo.IsDirectory;
    pOpenInfo->UserContext = fileInfo.Context;

    // FILE_CREATED
    // FILE_DOES_NOT_EXIST
    // FILE_EXISTS
    // FILE_OPENED
    // FILE_OVERWRITTEN
    // FILE_SUPERSEDED

    logw(L"CreateFile status = 0x%08X", status);
    if (status != STATUS_SUCCESS)
    {
        if (pEventContext->Flags & SL_OPEN_TARGET_DIRECTORY)
        {
            logw(L"SL_OPEN_TARGET_DIRECTORY spcefied");
        }
        pEventInfo->Create.Information = FILE_DOES_NOT_EXIST;
        pEventInfo->Status = status;

        if (status == STATUS_OBJECT_NAME_NOT_FOUND && pEventContext->Flags & SL_OPEN_TARGET_DIRECTORY)
        {
            logw(L"This case should be returned as SUCCESS");
            pEventInfo->Status = STATUS_SUCCESS;
        }

        if (status == STATUS_OBJECT_NAME_COLLISION)
        {
            pEventInfo->Create.Information = FILE_EXISTS;
        }

        if (pEventInfo->Status != STATUS_SUCCESS)
        {
            // Needs to free openInfo because Close is never called.
            free(pOpenInfo);
            pEventInfo->Context = 0;
        }
    }
    else
    {
        //
        // This is the case for opening file successfully.
        // Depending on the disposition value which came from IRP,
        // we should set the proper value to the Information field of Irp and return it to driver. 
        //
        OnFileOpenSucceed(pEventInfo, dwNtDisposition, status, fileInfo.IsDirectory);
    }
    
    SendEventInformation(hDevice, pEventInfo, cbEventInformation, pDokanInstance);
    free(pEventInfo);
}