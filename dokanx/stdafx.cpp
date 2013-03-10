// stdafx.cpp : source file that includes just the standard includes
// dokanx_mount.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

// TODO: reference any additional headers you need in STDAFX.H
// and not in this file


// Create a string with last error message
std::wstring GetLastErrorStdStr()
{
  DWORD error = GetLastError();
  if (error)
  {
    LPVOID lpMsgBuf;
    DWORD bufLen = FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | 
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        error,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPWSTR) &lpMsgBuf,
        0, NULL );
    if (bufLen)
    {
      LPCWSTR lpMsgStr = (LPCWSTR)lpMsgBuf;
      std::wstring result(lpMsgStr, bufLen);
      
      LocalFree(lpMsgBuf);

      return result;
    }
  }
  return std::wstring();
}