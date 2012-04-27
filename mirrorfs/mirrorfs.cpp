// mirrorfs.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

/*

Copyright (c) 2007, 2008 Hiroki Asakawa info@dokan-dev.net

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

#include <windows.h>
#include <winbase.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include "../dokan.h"
#include "../dokanx/fileinfo.h"

BOOL g_UseStdErr;
BOOL g_DebugMode;

static void DbgPrint(LPCWSTR format, ...)
{
    if (g_DebugMode) {
        WCHAR buffer[512];
        va_list argp;
        va_start(argp, format);
        vswprintf_s(buffer, _countof(buffer), format, argp);
        va_end(argp);
        if (g_UseStdErr) {
            fwprintf(stderr, buffer);
        } else {
            OutputDebugStringW(buffer);
        }
    }
}

static WCHAR g_RootDirectory[MAX_PATH] = L"C:";
static WCHAR g_MountPoint[MAX_PATH] = L"M:";


std::wstring GetFilePath(
    __in const std::wstring& fileName
    )
{
    return std::wstring(g_RootDirectory) + fileName;
}

//
//    PWCHAR	filePath,
//    ULONG	numberOfElements,
//    LPCWSTR FileName)
//{
//    RtlZeroMemory(filePath, numberOfElements * sizeof(WCHAR));
//    wcsncpy_s(filePath, numberOfElements, g_RootDirectory, wcslen(g_RootDirectory));
//    wcsncat_s(filePath, numberOfElements, FileName, wcslen(FileName));
//}
//

static void
PrintUserName(PDOKAN_FILE_INFO	DokanFileInfo)
{
    HANDLE	handle;
    UCHAR buffer[1024];
    DWORD returnLength;
    WCHAR accountName[256];
    WCHAR domainName[256];
    DWORD accountLength = _countof(accountName);
    DWORD domainLength = _countof(domainName);
    PTOKEN_USER tokenUser;
    SID_NAME_USE snu;

    handle = DokanOpenRequestorToken(DokanFileInfo);
    if (handle == INVALID_HANDLE_VALUE) {
        DbgPrint(L"  DokanOpenRequestorToken failed\n");
        return;
    }

    if (!GetTokenInformation(handle, TokenUser, buffer, sizeof(buffer), &returnLength)) {
        DbgPrint(L"  GetTokenInformaiton failed: %d\n", GetLastError());
        CloseHandle(handle);
        return;
    }

    CloseHandle(handle);

    tokenUser = (PTOKEN_USER)buffer;
    if (!LookupAccountSid(NULL, tokenUser->User.Sid, accountName,
            &accountLength, domainName, &domainLength, &snu)) {
        DbgPrint(L"  LookupAccountSid failed: %d\n", GetLastError());
        return;
    }

    DbgPrint(L"  AccountName: %s, DomainName: %s\n", accountName, domainName);
}

#define MirrorCheckFlag(val, flag) if (val&flag) { DbgPrint(L"\t" L#flag L"\n"); }


void CheckFileAttributeFlags(DWORD FlagsAndAttributes)
{
    MirrorCheckFlag(FlagsAndAttributes, FILE_ATTRIBUTE_ARCHIVE);
    MirrorCheckFlag(FlagsAndAttributes, FILE_ATTRIBUTE_ENCRYPTED);
    MirrorCheckFlag(FlagsAndAttributes, FILE_ATTRIBUTE_HIDDEN);
    MirrorCheckFlag(FlagsAndAttributes, FILE_ATTRIBUTE_NORMAL);
    MirrorCheckFlag(FlagsAndAttributes, FILE_ATTRIBUTE_NOT_CONTENT_INDEXED);
    MirrorCheckFlag(FlagsAndAttributes, FILE_ATTRIBUTE_OFFLINE);
    MirrorCheckFlag(FlagsAndAttributes, FILE_ATTRIBUTE_READONLY);
    MirrorCheckFlag(FlagsAndAttributes, FILE_ATTRIBUTE_SYSTEM);
    MirrorCheckFlag(FlagsAndAttributes, FILE_ATTRIBUTE_TEMPORARY);
    MirrorCheckFlag(FlagsAndAttributes, FILE_FLAG_WRITE_THROUGH);
    MirrorCheckFlag(FlagsAndAttributes, FILE_FLAG_OVERLAPPED);
    MirrorCheckFlag(FlagsAndAttributes, FILE_FLAG_NO_BUFFERING);
    MirrorCheckFlag(FlagsAndAttributes, FILE_FLAG_RANDOM_ACCESS);
    MirrorCheckFlag(FlagsAndAttributes, FILE_FLAG_SEQUENTIAL_SCAN);
    MirrorCheckFlag(FlagsAndAttributes, FILE_FLAG_DELETE_ON_CLOSE);
    MirrorCheckFlag(FlagsAndAttributes, FILE_FLAG_BACKUP_SEMANTICS);
    MirrorCheckFlag(FlagsAndAttributes, FILE_FLAG_POSIX_SEMANTICS);
    MirrorCheckFlag(FlagsAndAttributes, FILE_FLAG_OPEN_REPARSE_POINT);
    MirrorCheckFlag(FlagsAndAttributes, FILE_FLAG_OPEN_NO_RECALL);
    MirrorCheckFlag(FlagsAndAttributes, SECURITY_ANONYMOUS);
    MirrorCheckFlag(FlagsAndAttributes, SECURITY_IDENTIFICATION);
    MirrorCheckFlag(FlagsAndAttributes, SECURITY_IMPERSONATION);
    MirrorCheckFlag(FlagsAndAttributes, SECURITY_DELEGATION);
    MirrorCheckFlag(FlagsAndAttributes, SECURITY_CONTEXT_TRACKING);
    MirrorCheckFlag(FlagsAndAttributes, SECURITY_EFFECTIVE_ONLY);
    MirrorCheckFlag(FlagsAndAttributes, SECURITY_SQOS_PRESENT);
}

void CheckDesiredAccessFlags(DWORD DesiredAccess)
{
    MirrorCheckFlag(DesiredAccess, GENERIC_READ);
    MirrorCheckFlag(DesiredAccess, GENERIC_WRITE);
    MirrorCheckFlag(DesiredAccess, GENERIC_EXECUTE);

    MirrorCheckFlag(DesiredAccess, DELETE);
    MirrorCheckFlag(DesiredAccess, FILE_READ_DATA);
    MirrorCheckFlag(DesiredAccess, FILE_READ_ATTRIBUTES);
    MirrorCheckFlag(DesiredAccess, FILE_READ_EA);
    MirrorCheckFlag(DesiredAccess, READ_CONTROL);
    MirrorCheckFlag(DesiredAccess, FILE_WRITE_DATA);
    MirrorCheckFlag(DesiredAccess, FILE_WRITE_ATTRIBUTES);
    MirrorCheckFlag(DesiredAccess, FILE_WRITE_EA);
    MirrorCheckFlag(DesiredAccess, FILE_APPEND_DATA);
    MirrorCheckFlag(DesiredAccess, WRITE_DAC);
    MirrorCheckFlag(DesiredAccess, WRITE_OWNER);
    MirrorCheckFlag(DesiredAccess, SYNCHRONIZE);
    MirrorCheckFlag(DesiredAccess, FILE_EXECUTE);
    MirrorCheckFlag(DesiredAccess, STANDARD_RIGHTS_READ);
    MirrorCheckFlag(DesiredAccess, STANDARD_RIGHTS_WRITE);
    MirrorCheckFlag(DesiredAccess, STANDARD_RIGHTS_EXECUTE);
}

void CheckShareModeFlags(DWORD ShareMode)
{
    MirrorCheckFlag(ShareMode, FILE_SHARE_READ);
    MirrorCheckFlag(ShareMode, FILE_SHARE_WRITE);
    MirrorCheckFlag(ShareMode, FILE_SHARE_DELETE);
}

NTSTATUS MirrorCreateFile(
    LPCWSTR					FileName,
    DWORD					DesiredAccess,
    DWORD					ShareMode,
    DWORD					CreationDisposition,
    DWORD					FlagsAndAttributes,
    PDOKAN_FILE_INFO		DokanFileInfo)
{
    std::wstring filePath;
    HANDLE handle;
    DWORD fileAttr;

    filePath = GetFilePath(FileName);

    DbgPrint(L"CreateFile : %s\n", filePath.c_str());

    PrintUserName(DokanFileInfo);

    if (CreationDisposition == CREATE_NEW)
        DbgPrint(L"\tCREATE_NEW\n");
    if (CreationDisposition == OPEN_ALWAYS)
        DbgPrint(L"\tOPEN_ALWAYS\n");
    if (CreationDisposition == CREATE_ALWAYS)
        DbgPrint(L"\tCREATE_ALWAYS\n");
    if (CreationDisposition == OPEN_EXISTING)
        DbgPrint(L"\tOPEN_EXISTING\n");
    if (CreationDisposition == TRUNCATE_EXISTING)
        DbgPrint(L"\tTRUNCATE_EXISTING\n");

    DbgPrint(L"\tShareMode = 0x%x\n", ShareMode);

    CheckShareModeFlags(ShareMode);

    DbgPrint(L"\tAccessMode = 0x%x\n", DesiredAccess);

    CheckDesiredAccessFlags(DesiredAccess);

    // When filePath is a directory, needs to change the flag so that the file can be opened.
    fileAttr = GetFileAttributes(filePath.c_str());
    if (fileAttr && fileAttr & FILE_ATTRIBUTE_DIRECTORY) {
        FlagsAndAttributes |= FILE_FLAG_BACKUP_SEMANTICS;
        //AccessMode = 0;
    }
    DbgPrint(L"\tFlagsAndAttributes = 0x%x\n", FlagsAndAttributes);

    CheckFileAttributeFlags(FlagsAndAttributes);

    handle = CreateFile(
        filePath.c_str(),
        DesiredAccess,//GENERIC_READ|GENERIC_WRITE|GENERIC_EXECUTE,
        ShareMode,
        NULL, // security attribute
        CreationDisposition,
        FlagsAndAttributes,// |FILE_FLAG_NO_BUFFERING,
        NULL); // template file handle

    if (handle == INVALID_HANDLE_VALUE) {
        DWORD error = GetLastError();
        DbgPrint(L"\terror code = %d\n\n", error);
        // fixlater;
        return STATUS_ACCESS_DENIED;
    }

    DbgPrint(L"\n");

    // save the file handle in Context
    DokanFileInfo->Context = (ULONG64)handle;
    return STATUS_SUCCESS;
}


NTSTATUS MirrorCreateDirectory(
    LPCWSTR					FileName,
    PDOKAN_FILE_INFO		DokanFileInfo)
{
    std::wstring filePath = GetFilePath(FileName);

    DbgPrint(L"CreateDirectory : %s\n", filePath.c_str());
    if (!CreateDirectory(filePath.c_str(), NULL)) {
        DWORD error = GetLastError();
        DbgPrint(L"\terror code = %d\n\n", error);
        // fixlater;
        return STATUS_ACCESS_DENIED;
        //return error * -1; // error codes are negated value of Windows System Error codes
    }
    return STATUS_SUCCESS;
}


NTSTATUS MirrorOpenDirectory(
    LPCWSTR					FileName,
    PDOKAN_FILE_INFO		DokanFileInfo)
{
    HANDLE handle;
    DWORD attr;
    std::wstring filePath = GetFilePath(FileName);

    DbgPrint(L"OpenDirectory : %s\n", filePath.c_str());

    attr = GetFileAttributes(filePath.c_str());
    if (attr == INVALID_FILE_ATTRIBUTES) {
        DWORD error = GetLastError();
        DbgPrint(L"\terror code = %d\n\n", error);
        // fixlater;
        return STATUS_ACCESS_DENIED;
        //return error * -1;
    }
    if (!(attr & FILE_ATTRIBUTE_DIRECTORY)) {
        return STATUS_NOT_A_DIRECTORY;
    }

    handle = CreateFile(
        filePath.c_str(),
        0,
        FILE_SHARE_READ|FILE_SHARE_WRITE,
        NULL,
        OPEN_EXISTING,
        FILE_FLAG_BACKUP_SEMANTICS,
        NULL);

    if (handle == INVALID_HANDLE_VALUE) {
        DWORD error = GetLastError();
        DbgPrint(L"\terror code = %d\n\n", error);
        // fixlater;
        return STATUS_ACCESS_DENIED;
        //return error * -1;
    }

    DbgPrint(L"\n");

    DokanFileInfo->Context = (ULONG64)handle;

    return STATUS_SUCCESS;
}


void MirrorCloseFile(
    LPCWSTR					FileName,
    PDOKAN_FILE_INFO		DokanFileInfo)
{
    std::wstring filePath = GetFilePath(FileName);

    if (DokanFileInfo->Context) {
        DbgPrint(L"CloseFile: %s\n", filePath.c_str());
        DbgPrint(L"\terror : not cleanuped file\n\n");
        CloseHandle((HANDLE)DokanFileInfo->Context);
        DokanFileInfo->Context = 0;
    } else {
        //DbgPrint(L"Close: %s\n\tinvalid handle\n\n", filePath.c_str());
        DbgPrint(L"Close: %s\n\n", filePath.c_str());
    }

    //DbgPrint(L"\n");
}

void MirrorCleanup(
    LPCWSTR					FileName,
    PDOKAN_FILE_INFO		DokanFileInfo)
{
    std::wstring filePath = GetFilePath(FileName);

    if (DokanFileInfo->Context) {
        DbgPrint(L"Cleanup: %s\n\n", filePath.c_str());
        CloseHandle((HANDLE)DokanFileInfo->Context);
        DokanFileInfo->Context = 0;

        if (DokanFileInfo->DeleteOnClose) {
            DbgPrint(L"\tDeleteOnClose\n");
            if (DokanFileInfo->IsDirectory) {
                DbgPrint(L"  DeleteDirectory ");
                if (!RemoveDirectory(filePath.c_str())) {
                    DbgPrint(L"error code = %d\n\n", GetLastError());
                } else {
                    DbgPrint(L"success\n\n");
                }
            } else {
                DbgPrint(L"  DeleteFile ");
                if (DeleteFile(filePath.c_str()) == 0) {
                    DbgPrint(L" error code = %d\n\n", GetLastError());
                } else {
                    DbgPrint(L"success\n\n");
                }
            }
        }

    } else {
        DbgPrint(L"Cleanup: %s\n\tinvalid handle\n\n", filePath.c_str());
    }
}


NTSTATUS MirrorReadFile(
    LPCWSTR				FileName,
    LPVOID				Buffer,
    DWORD				BufferLength,
    LPDWORD				ReadLength,
    LONGLONG			Offset,
    PDOKAN_FILE_INFO	DokanFileInfo)
{
    HANDLE	handle = (HANDLE)DokanFileInfo->Context;
    ULONG	offset = (ULONG)Offset;
    BOOL	opened = FALSE;
    std::wstring filePath = GetFilePath(FileName);

    DbgPrint(L"ReadFile : %s\n", filePath.c_str());

    if (!handle || handle == INVALID_HANDLE_VALUE) {
        DbgPrint(L"\tinvalid handle, cleanuped?\n");
        handle = CreateFile(
            filePath.c_str(),
            GENERIC_READ,
            FILE_SHARE_READ,
            NULL,
            OPEN_EXISTING,
            0,
            NULL);
        if (handle == INVALID_HANDLE_VALUE) {
            DbgPrint(L"\tCreateFile error : %d\n\n", GetLastError());
            // fixlater;
            return STATUS_ACCESS_DENIED;
        }
        opened = TRUE;
    }
    
    if (SetFilePointer(handle, offset, NULL, FILE_BEGIN) == 0xFFFFFFFF) {
        DbgPrint(L"\tseek error, offset = %d\n\n", offset);
        if (opened)
            CloseHandle(handle);
        // fixlater;
        return STATUS_ACCESS_DENIED;
    }
        
    if (!ReadFile(handle, Buffer, BufferLength, ReadLength,NULL)) {
        DbgPrint(L"\tread error = %u, buffer length = %d, read length = %d\n\n",
            GetLastError(), BufferLength, *ReadLength);
        if (opened)
            CloseHandle(handle);
        // fixlater;
        return STATUS_ACCESS_DENIED;

    } else {
        DbgPrint(L"\tread %d, offset %d\n\n", *ReadLength, offset);
    }

    if (opened)
        CloseHandle(handle);

    return STATUS_SUCCESS;
}


NTSTATUS MirrorWriteFile(
    LPCWSTR		FileName,
    LPCVOID		Buffer,
    DWORD		NumberOfBytesToWrite,
    LPDWORD		NumberOfBytesWritten,
    LONGLONG			Offset,
    PDOKAN_FILE_INFO	DokanFileInfo)
{
    HANDLE	handle = (HANDLE)DokanFileInfo->Context;
    ULONG	offset = (ULONG)Offset;
    BOOL	opened = FALSE;
    std::wstring filePath = GetFilePath(FileName);

    DbgPrint(L"WriteFile : %s, offset %I64d, length %d\n", filePath.c_str(), Offset, NumberOfBytesToWrite);

    // reopen the file
    if (!handle || handle == INVALID_HANDLE_VALUE) {
        DbgPrint(L"\tinvalid handle, cleanuped?\n");
        handle = CreateFile(
            filePath.c_str(),
            GENERIC_WRITE,
            FILE_SHARE_WRITE,
            NULL,
            OPEN_EXISTING,
            0,
            NULL);
        if (handle == INVALID_HANDLE_VALUE) {
            DbgPrint(L"\tCreateFile error : %d\n\n", GetLastError());
            // fixlater;
            return STATUS_ACCESS_DENIED;
        }
        opened = TRUE;
    }

    if (DokanFileInfo->WriteToEndOfFile) {
        if (SetFilePointer(handle, 0, NULL, FILE_END) == INVALID_SET_FILE_POINTER) {
            DbgPrint(L"\tseek error, offset = EOF, error = %d\n", GetLastError());
            // fixlater;
            return STATUS_ACCESS_DENIED;
        }
    } else if (SetFilePointer(handle, offset, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER) {
        DbgPrint(L"\tseek error, offset = %d, error = %d\n", offset, GetLastError());
        // fixlater;
        return STATUS_ACCESS_DENIED;
    }
        
    if (!WriteFile(handle, Buffer, NumberOfBytesToWrite, NumberOfBytesWritten, NULL)) {
        DbgPrint(L"\twrite error = %u, buffer length = %d, write length = %d\n",
            GetLastError(), NumberOfBytesToWrite, *NumberOfBytesWritten);
        // fixlater;
        return STATUS_ACCESS_DENIED;

    } else {
        DbgPrint(L"\twrite %d, offset %d\n\n", *NumberOfBytesWritten, offset);
    }

    // close the file when it is reopened
    if (opened)
        CloseHandle(handle);

    return STATUS_SUCCESS;
}

NTSTATUS MirrorFlushFileBuffers(
    LPCWSTR		FileName,
    PDOKAN_FILE_INFO	DokanFileInfo)
{
    HANDLE	handle = (HANDLE)DokanFileInfo->Context;
    std::wstring filePath = GetFilePath(FileName);

    DbgPrint(L"FlushFileBuffers : %s\n", filePath.c_str());

    if (!handle || handle == INVALID_HANDLE_VALUE) {
        DbgPrint(L"\tinvalid handle\n\n");
        // fixlater;
        // should return success? why?
        return STATUS_INVALID_HANDLE;
    }

    if (FlushFileBuffers(handle)) {
        return STATUS_SUCCESS;
    } else {
        DbgPrint(L"\tflush error code = %d\n", GetLastError());
        // fixlater;
        return STATUS_ACCESS_DENIED;
    }
}


NTSTATUS MirrorGetFileInformation(
    LPCWSTR							FileName,
    LPBY_HANDLE_FILE_INFORMATION	HandleFileInformation,
    PDOKAN_FILE_INFO				DokanFileInfo)
{
    HANDLE	handle = (HANDLE)DokanFileInfo->Context;
    BOOL	opened = FALSE;

    std::wstring filePath = GetFilePath(FileName);

    DbgPrint(L"GetFileInfo : %s\n", filePath.c_str());

    if (!handle || handle == INVALID_HANDLE_VALUE) {
        DbgPrint(L"\tinvalid handle\n\n");

        // If CreateDirectory returned FILE_ALREADY_EXISTS and 
        // it is called with FILE_OPEN_IF, that handle must be opened.
        handle = CreateFile(filePath.c_str(), 0, FILE_SHARE_READ, NULL, OPEN_EXISTING,
            FILE_FLAG_BACKUP_SEMANTICS, NULL);
        if (handle == INVALID_HANDLE_VALUE)
        {
            // fixlater;
            return STATUS_ACCESS_DENIED;
        }
        opened = TRUE;
    }

    if (!GetFileInformationByHandle(handle,HandleFileInformation)) {
        DbgPrint(L"\terror code = %d\n", GetLastError());

        // FileName is a root directory
        // in this case, FindFirstFile can't get directory information
        if (wcslen(FileName) == 1) {
            DbgPrint(L"  root dir\n");
            HandleFileInformation->dwFileAttributes = GetFileAttributes(filePath.c_str());

        } else {
            WIN32_FIND_DATAW find;
            ZeroMemory(&find, sizeof(WIN32_FIND_DATAW));
            handle = FindFirstFile(filePath.c_str(), &find);
            if (handle == INVALID_HANDLE_VALUE) {
                DbgPrint(L"\tFindFirstFile error code = %d\n\n", GetLastError());
                // fixlater;
                return STATUS_ACCESS_DENIED;
            }
            HandleFileInformation->dwFileAttributes = find.dwFileAttributes;
            HandleFileInformation->ftCreationTime = find.ftCreationTime;
            HandleFileInformation->ftLastAccessTime = find.ftLastAccessTime;
            HandleFileInformation->ftLastWriteTime = find.ftLastWriteTime;
            HandleFileInformation->nFileSizeHigh = find.nFileSizeHigh;
            HandleFileInformation->nFileSizeLow = find.nFileSizeLow;
            DbgPrint(L"\tFindFiles OK, file size = %d\n", find.nFileSizeLow);
            FindClose(handle);
        }
    } else {
        DbgPrint(L"\tGetFileInformationByHandle success, file size = %d\n",
            HandleFileInformation->nFileSizeLow);
    }

    DbgPrint(L"\n");

    if (opened) {
        CloseHandle(handle);
    }

    return STATUS_SUCCESS;
}

NTSTATUS MirrorFindFiles(
    LPCWSTR				FileName,
    PFillFindData		FillFindData, // function pointer
    PDOKAN_FILE_INFO	DokanFileInfo)
{
    HANDLE				hFind;
    WIN32_FIND_DATAW	findData;
    DWORD				error;
    PWCHAR				yenStar = L"\\*";
    int count = 0;

    std::wstring filePath = GetFilePath(FileName);
    filePath = filePath + yenStar;
    DbgPrint(L"FindFiles :%s\n", filePath.c_str());

    hFind = FindFirstFile(filePath.c_str(), &findData);

    if (hFind == INVALID_HANDLE_VALUE) {
        DbgPrint(L"\tinvalid file handle. Error is %u\n\n", GetLastError());
        // fixlater;
        return STATUS_ACCESS_DENIED;
    }

    FillFindData(&findData, DokanFileInfo);
    count++;

    while (FindNextFile(hFind, &findData) != 0) {
        FillFindData(&findData, DokanFileInfo);
        count++;
    }
    
    error = GetLastError();
    FindClose(hFind);

    if (error != ERROR_NO_MORE_FILES) {
        DbgPrint(L"\tFindNextFile error. Error is %u\n\n", error);
        // fixlater;
        return STATUS_ACCESS_DENIED;
    }

    DbgPrint(L"\tFindFiles return %d entries in %s\n\n", count, filePath.c_str());

    return STATUS_SUCCESS;
}


NTSTATUS MirrorDeleteFile(
    LPCWSTR				FileName,
    PDOKAN_FILE_INFO	DokanFileInfo)
{
    HANDLE	handle = (HANDLE)DokanFileInfo->Context;

    std::wstring filePath = GetFilePath(FileName);

    DbgPrint(L"DeleteFile %s\n", filePath.c_str());

    return STATUS_SUCCESS;
}

NTSTATUS MirrorDeleteDirectory(
    LPCWSTR				FileName,
    PDOKAN_FILE_INFO	DokanFileInfo)
{
    HANDLE	handle = (HANDLE)DokanFileInfo->Context;
    HANDLE	hFind;
    WIN32_FIND_DATAW	findData;
    ULONG	fileLen;

    std::wstring filePath = GetFilePath(FileName);

    DbgPrint(L"DeleteDirectory %s\n", filePath.c_str());

    fileLen = wcslen(filePath.c_str());
    if (filePath[fileLen-1] != L'\\') {
        filePath[fileLen++] = L'\\';
    }
    filePath[fileLen] = L'*';

    hFind = FindFirstFile(filePath.c_str(), &findData);
    while (hFind != INVALID_HANDLE_VALUE) {
        if (wcscmp(findData.cFileName, L"..") != 0 &&
            wcscmp(findData.cFileName, L".") != 0) {
            FindClose(hFind);
            DbgPrint(L"  Directory is not empty: %s\n", findData.cFileName);
            return -(int)ERROR_DIR_NOT_EMPTY;
        }
        if (!FindNextFile(hFind, &findData)) {
            break;
        }
    }
    FindClose(hFind);

    if (GetLastError() == ERROR_NO_MORE_FILES) {
        return STATUS_SUCCESS;
    } else {
        // fixlater;
        return STATUS_ACCESS_DENIED;
    }
}


NTSTATUS MirrorMoveFile(
    LPCWSTR				FileName, // existing file name
    LPCWSTR				NewFileName,
    BOOL				ReplaceIfExisting,
    PDOKAN_FILE_INFO	DokanFileInfo)
{
    BOOL			status;

    std::wstring filePath = GetFilePath(FileName);
    std::wstring newFilePath = GetFilePath(NewFileName);

    DbgPrint(L"MoveFile %s -> %s\n\n", filePath.c_str(), newFilePath.c_str());

    if (DokanFileInfo->Context) {
        // should close? or rename at closing?
        CloseHandle((HANDLE)DokanFileInfo->Context);
        DokanFileInfo->Context = 0;
    }

    if (ReplaceIfExisting)
        status = MoveFileEx(filePath.c_str(), newFilePath.c_str(), MOVEFILE_REPLACE_EXISTING);
    else
        status = MoveFile(filePath.c_str(), newFilePath.c_str());

    if (status == FALSE) {
        DWORD error = GetLastError();
        DbgPrint(L"\tMoveFile failed status = %d, code = %d\n", status, error);
        return -(int)error;
    } else {
        return STATUS_SUCCESS;
    }
}

NTSTATUS MirrorLockFile(
    LPCWSTR				FileName,
    LONGLONG			ByteOffset,
    LONGLONG			Length,
    PDOKAN_FILE_INFO	DokanFileInfo)
{
    HANDLE	handle;
    LARGE_INTEGER offset;
    LARGE_INTEGER length;

    std::wstring filePath = GetFilePath(FileName);

    DbgPrint(L"LockFile %s\n", filePath.c_str());

    handle = (HANDLE)DokanFileInfo->Context;
    if (!handle || handle == INVALID_HANDLE_VALUE) {
        DbgPrint(L"\tinvalid handle\n\n");
        // fixlater;
        return STATUS_ACCESS_DENIED;
    }

    length.QuadPart = Length;
    offset.QuadPart = ByteOffset;

    if (LockFile(handle, offset.HighPart, offset.LowPart, length.HighPart, length.LowPart)) {
        DbgPrint(L"\tsuccess\n\n");
        return STATUS_SUCCESS;
    } else {
        DbgPrint(L"\tfail\n\n");
        // fixlater;
        return STATUS_ACCESS_DENIED;
    }
}

NTSTATUS MirrorSetEndOfFile(
    LPCWSTR				FileName,
    LONGLONG			ByteOffset,
    PDOKAN_FILE_INFO	DokanFileInfo)
{
    HANDLE			handle;
    LARGE_INTEGER	offset;

    std::wstring filePath = GetFilePath(FileName);

    DbgPrint(L"SetEndOfFile %s, %I64d\n", filePath.c_str(), ByteOffset);

    handle = (HANDLE)DokanFileInfo->Context;
    if (!handle || handle == INVALID_HANDLE_VALUE) {
        DbgPrint(L"\tinvalid handle\n\n");
        // fixlater;
        return STATUS_ACCESS_DENIED;
    }

    offset.QuadPart = ByteOffset;
    if (!SetFilePointerEx(handle, offset, NULL, FILE_BEGIN)) {
        DbgPrint(L"\tSetFilePointer error: %d, offset = %I64d\n\n",
                GetLastError(), ByteOffset);
        // fixlater;
        return STATUS_ACCESS_DENIED;
    }

    if (!SetEndOfFile(handle)) {
        DWORD error = GetLastError();
        DbgPrint(L"\terror code = %d\n\n", error);
        // fixlater;
        return STATUS_ACCESS_DENIED;
    }

    return STATUS_SUCCESS;
}


NTSTATUS MirrorSetAllocationSize(
    LPCWSTR				FileName,
    LONGLONG			AllocSize,
    PDOKAN_FILE_INFO	DokanFileInfo)
{
    HANDLE			handle;
    LARGE_INTEGER	fileSize;

    std::wstring filePath = GetFilePath(FileName);

    DbgPrint(L"SetAllocationSize %s, %I64d\n", filePath.c_str(), AllocSize);

    handle = (HANDLE)DokanFileInfo->Context;
    if (!handle || handle == INVALID_HANDLE_VALUE) {
        DbgPrint(L"\tinvalid handle\n\n");
        // fixlater;
        return STATUS_ACCESS_DENIED;
    }

    if (GetFileSizeEx(handle, &fileSize)) {
        if (AllocSize < fileSize.QuadPart) {
            fileSize.QuadPart = AllocSize;
            if (!SetFilePointerEx(handle, fileSize, NULL, FILE_BEGIN)) {
                DbgPrint(L"\tSetAllocationSize: SetFilePointer eror: %d, "
                    L"offset = %I64d\n\n", GetLastError(), AllocSize);
                // fixlater;
                return STATUS_ACCESS_DENIED;
            }
            if (!SetEndOfFile(handle)) {
                DWORD error = GetLastError();
                DbgPrint(L"\terror code = %d\n\n", error);
                // fixlater;
                return STATUS_ACCESS_DENIED;
            }
        }
    } else {
        DWORD error = GetLastError();
        DbgPrint(L"\terror code = %d\n\n", error);
        // fixlater;
        return STATUS_ACCESS_DENIED;
    }
    return STATUS_SUCCESS;
}

NTSTATUS MirrorSetFileAttributes(
    LPCWSTR				FileName,
    DWORD				FileAttributes,
    PDOKAN_FILE_INFO	DokanFileInfo)
{
    std::wstring filePath = GetFilePath(FileName);

    DbgPrint(L"SetFileAttributes %s\n", filePath.c_str());

    if (!SetFileAttributes(filePath.c_str(), FileAttributes)) {
        DWORD error = GetLastError();
        DbgPrint(L"\terror code = %d\n\n", error);
        // fixlater;
        return STATUS_ACCESS_DENIED;
    }

    DbgPrint(L"\n");
    return STATUS_SUCCESS;
}

NTSTATUS MirrorSetFileTime(
    LPCWSTR				FileName,
    CONST FILETIME*		CreationTime,
    CONST FILETIME*		LastAccessTime,
    CONST FILETIME*		LastWriteTime,
    PDOKAN_FILE_INFO	DokanFileInfo)
{
    HANDLE	handle;

    std::wstring filePath = GetFilePath(FileName);

    DbgPrint(L"SetFileTime %s\n", filePath.c_str());

    handle = (HANDLE)DokanFileInfo->Context;

    if (!handle || handle == INVALID_HANDLE_VALUE) {
        DbgPrint(L"\tinvalid handle\n\n");
        // fixlater;
        return STATUS_ACCESS_DENIED;
    }

    if (!SetFileTime(handle, CreationTime, LastAccessTime, LastWriteTime)) {
        DWORD error = GetLastError();
        DbgPrint(L"\terror code = %d\n\n", error);
        // fixlater;
        return STATUS_ACCESS_DENIED;
    }

    DbgPrint(L"\n");
    return STATUS_SUCCESS;
}

NTSTATUS MirrorUnlockFile(
    LPCWSTR				FileName,
    LONGLONG			ByteOffset,
    LONGLONG			Length,
    PDOKAN_FILE_INFO	DokanFileInfo)
{
    HANDLE	handle;
    LARGE_INTEGER	length;
    LARGE_INTEGER	offset;

    std::wstring filePath = GetFilePath(FileName);

    DbgPrint(L"UnlockFile %s\n", filePath.c_str());

    handle = (HANDLE)DokanFileInfo->Context;
    if (!handle || handle == INVALID_HANDLE_VALUE) {
        DbgPrint(L"\tinvalid handle\n\n");
        // fixlater;
        return STATUS_ACCESS_DENIED;
    }

    length.QuadPart = Length;
    offset.QuadPart = ByteOffset;

    if (UnlockFile(handle, offset.HighPart, offset.LowPart, length.HighPart, length.LowPart)) {
        DbgPrint(L"\tsuccess\n\n");
        return STATUS_SUCCESS;
    } else {
        DbgPrint(L"\tfail\n\n");
        // fixlater;
        return STATUS_ACCESS_DENIED;
    }
}

NTSTATUS MirrorGetFileSecurity(
    LPCWSTR					FileName,
    PSECURITY_INFORMATION	SecurityInformation,
    PSECURITY_DESCRIPTOR	SecurityDescriptor,
    ULONG				BufferLength,
    PULONG				LengthNeeded,
    PDOKAN_FILE_INFO	DokanFileInfo)
{
    HANDLE	handle;
    std::wstring filePath = GetFilePath(FileName);

    DbgPrint(L"GetFileSecurity %s\n", filePath.c_str());

    handle = (HANDLE)DokanFileInfo->Context;
    if (!handle || handle == INVALID_HANDLE_VALUE) {
        DbgPrint(L"\tinvalid handle\n\n");
        // fixlater;
        return STATUS_ACCESS_DENIED;
    }

    if (!GetUserObjectSecurity(handle, SecurityInformation, SecurityDescriptor,
            BufferLength, LengthNeeded)) {
        int error = GetLastError();
        if (error == ERROR_INSUFFICIENT_BUFFER) {
            DbgPrint(L"  GetUserObjectSecurity failed: ERROR_INSUFFICIENT_BUFFER\n");
            // fixlater;
            return STATUS_BUFFER_OVERFLOW;
        } else {
            DbgPrint(L"  GetUserObjectSecurity failed: %d\n", error);
            // fixlater;
            return STATUS_ACCESS_DENIED;
        }
    }
    return STATUS_SUCCESS;
}

NTSTATUS MirrorSetFileSecurity(
    LPCWSTR					FileName,
    PSECURITY_INFORMATION	SecurityInformation,
    PSECURITY_DESCRIPTOR	SecurityDescriptor,
    ULONG				/*SecurityDescriptorLength*/,
    PDOKAN_FILE_INFO	DokanFileInfo)
{
    HANDLE	handle;
    std::wstring filePath = GetFilePath(FileName);

    DbgPrint(L"SetFileSecurity %s\n", filePath.c_str());

    handle = (HANDLE)DokanFileInfo->Context;
    if (!handle || handle == INVALID_HANDLE_VALUE) {
        DbgPrint(L"\tinvalid handle\n\n");
        // fixlater;
        return STATUS_ACCESS_DENIED;
    }

    if (!SetUserObjectSecurity(handle, SecurityInformation, SecurityDescriptor)) {
        int error = GetLastError();
        DbgPrint(L"  SetUserObjectSecurity failed: %d\n", error);
        // fixlater;
        return STATUS_ACCESS_DENIED;
    }
    return STATUS_SUCCESS;
}

NTSTATUS MirrorGetVolumeInformation(
    LPWSTR		VolumeNameBuffer,
    DWORD		VolumeNameSize,
    LPDWORD		VolumeSerialNumber,
    LPDWORD		MaximumComponentLength,
    LPDWORD		FileSystemFlags,
    LPWSTR		FileSystemNameBuffer,
    DWORD		FileSystemNameSize,
    PDOKAN_FILE_INFO	/*DokanFileInfo*/)
{
    wcscpy_s(VolumeNameBuffer, VolumeNameSize / sizeof(WCHAR), L"DOKAN");
    *VolumeSerialNumber = 0x19831116;
    *MaximumComponentLength = 256;
    *FileSystemFlags = FILE_CASE_SENSITIVE_SEARCH | 
                        FILE_CASE_PRESERVED_NAMES | 
                        FILE_SUPPORTS_REMOTE_STORAGE |
                        FILE_UNICODE_ON_DISK |
                        FILE_PERSISTENT_ACLS;

    wcscpy_s(FileSystemNameBuffer, FileSystemNameSize / sizeof(WCHAR), L"Dokan");

    return STATUS_SUCCESS;
}

NTSTATUS MirrorUnmount(
    PDOKAN_FILE_INFO	DokanFileInfo)
{
    DbgPrint(L"Unmount\n");
    return STATUS_SUCCESS;
}

int _tmain(int argc, _TCHAR* argv[])
{
    int status;
    int command;
    PDOKAN_OPERATIONS dokanOperations = (PDOKAN_OPERATIONS)malloc(sizeof(DOKAN_OPERATIONS));
    if (dokanOperations == nullptr)
    {
        return EXIT_FAILURE;
    }

    PDOKAN_OPTIONS dokanOptions = (PDOKAN_OPTIONS)malloc(sizeof(DOKAN_OPTIONS));
    if (dokanOperations == nullptr)
    {
        free(dokanOperations);
        return EXIT_FAILURE;
    }

    if (argc < 5) {
        fprintf(stderr, "mirror.exe\n"
            "  /r RootDirectory (ex. /r c:\\test)\n"
            "  /l DriveLetter (ex. /l m)\n"
            "  /t ThreadCount (ex. /t 5)\n"
            "  /d (enable debug output)\n"
            "  /s (use stderr for output)\n"
            "  /n (use network drive)\n"
            "  /m (use removable drive)\n");
        return EXIT_FAILURE;
    }

    g_DebugMode = FALSE;
    g_UseStdErr = FALSE;

    ZeroMemory(dokanOptions, sizeof(DOKAN_OPTIONS));
    dokanOptions->Version = DOKAN_VERSION;
    dokanOptions->ThreadCount = 0; // use default

    for (command = 1; command < argc; command++) {
        switch (towlower(argv[command][1])) {
        case L'r':
            command++;
            wcscpy_s(g_RootDirectory, _countof(g_RootDirectory), argv[command]);
            DbgPrint(L"RootDirectory: %ls\n", g_RootDirectory);
            break;
        case L'l':
            command++;
            wcscpy_s(g_MountPoint, _countof(g_MountPoint), argv[command]);
            dokanOptions->MountPoint = g_MountPoint;
            break;
        case L't':
            command++;
            dokanOptions->ThreadCount = (USHORT)_wtoi(argv[command]);
            break;
        case L'd':
            g_DebugMode = TRUE;
            break;
        case L's':
            g_UseStdErr = TRUE;
            break;
        case L'n':
            dokanOptions->Options |= DOKAN_OPTION_NETWORK;
            break;
        case L'm':
            dokanOptions->Options |= DOKAN_OPTION_REMOVABLE;
            break;
        default:
            fwprintf(stderr, L"unknown command: %s\n", argv[command]);
            free(dokanOperations);
            free(dokanOptions);
            return EXIT_FAILURE;
        }
    }

    if (g_DebugMode) {
        dokanOptions->Options |= DOKAN_OPTION_DEBUG;
    }
    if (g_UseStdErr) {
        dokanOptions->Options |= DOKAN_OPTION_STDERR;
    }

    dokanOptions->Options |= DOKAN_OPTION_KEEP_ALIVE;

    ZeroMemory(dokanOperations, sizeof(DOKAN_OPERATIONS));
    dokanOperations->CreateFile = MirrorCreateFile;
    dokanOperations->OpenDirectory = MirrorOpenDirectory;
    dokanOperations->CreateDirectory = MirrorCreateDirectory;
    dokanOperations->Cleanup = MirrorCleanup;
    dokanOperations->CloseFile = MirrorCloseFile;
    dokanOperations->ReadFile = MirrorReadFile;
    dokanOperations->WriteFile = MirrorWriteFile;
    dokanOperations->FlushFileBuffers = MirrorFlushFileBuffers;
    dokanOperations->GetFileInformation = MirrorGetFileInformation;
    dokanOperations->FindFiles = MirrorFindFiles;
    dokanOperations->FindFilesWithPattern = nullptr;
    dokanOperations->SetFileAttributes = MirrorSetFileAttributes;
    dokanOperations->SetFileTime = MirrorSetFileTime;
    dokanOperations->DeleteFile = MirrorDeleteFile;
    dokanOperations->DeleteDirectory = MirrorDeleteDirectory;
    dokanOperations->MoveFile = MirrorMoveFile;
    dokanOperations->SetEndOfFile = MirrorSetEndOfFile;
    dokanOperations->SetAllocationSize = MirrorSetAllocationSize;
    dokanOperations->LockFile = MirrorLockFile;
    dokanOperations->UnlockFile = MirrorUnlockFile;
    dokanOperations->GetFileSecurity = MirrorGetFileSecurity;
    dokanOperations->SetFileSecurity = MirrorSetFileSecurity;
    dokanOperations->GetDiskFreeSpace = nullptr;
    dokanOperations->GetVolumeInformation = MirrorGetVolumeInformation;
    dokanOperations->Unmount = MirrorUnmount;

    status = DokanMain(dokanOptions, dokanOperations);
    switch (status) {
    case DOKAN_SUCCESS:
        fprintf(stderr, "Success\n");
        break;
    case DOKAN_ERROR:
        fprintf(stderr, "Error\n");
        break;
    case DOKAN_DRIVE_LETTER_ERROR:
        fprintf(stderr, "Bad Drive letter\n");
        break;
    case DOKAN_DRIVER_INSTALL_ERROR:
        fprintf(stderr, "Can't install driver\n");
        break;
    case DOKAN_START_ERROR:
        fprintf(stderr, "Driver something wrong\n");
        break;
    case DOKAN_MOUNT_ERROR:
        fprintf(stderr, "Can't assign a drive letter\n");
        break;
    case DOKAN_MOUNT_POINT_ERROR:
        fprintf(stderr, "Mount point error\n");
        break;
    default:
        fprintf(stderr, "Unknown error: %d\n", status);
        break;
    }

    free(dokanOptions);
    free(dokanOperations);

    return 0;
}