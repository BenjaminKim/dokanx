#pragma once
#include <Windows.h>

BOOL Is64BitWindows();
BOOL IsWindows7();
BOOL IsWindows7OrLater();
BOOL IsVistaOrLater();
BOOL IsVistaOrWinServer2008Basic();
BOOL IsServerOS();
bool IsVistaProcessElevation();
bool IsUacSupported();