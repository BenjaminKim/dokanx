#pragma once

#include "../../DokanStatus.h"

LPCWSTR GetNtStatusString(NTSTATUS status);
LPCWSTR FileInformationClassString(INT clazz);
LPCWSTR GetMajorFunctionString(UCHAR majorFunction);
LPCWSTR Win32CreateDispositionStr(DWORD dwDisposition);
LPCWSTR GetNtCreateDispositionStr(DWORD dwDisposition);