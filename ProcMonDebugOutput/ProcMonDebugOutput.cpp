
#include "stdafx.h"
#include "ProcMonDebugOutput.h"

#define FILE_DEVICE_PROCMON_LOG     0x00009535
#define IOCTL_EXTERNAL_LOG_DEBUGOUT	(ULONG) CTL_CODE(FILE_DEVICE_PROCMON_LOG ,\
                                                     0x81                    ,\
                                                     METHOD_BUFFERED         ,\
                                                     FILE_WRITE_ACCESS        )

// The global file handle to the Process Monitor device.
static HANDLE g_hDevice = INVALID_HANDLE_VALUE;

// Anonymous namespace for private helpers
namespace {

    HANDLE OpenProcessMonitorLogger()
    {
        if (INVALID_HANDLE_VALUE == g_hDevice)
        {
            // I'm attempting the open every time because the user could start 
            // Process Monitor after their process.
            g_hDevice = ::CreateFile(L"\\\\.\\Global\\ProcmonDebugLogger",
                                     GENERIC_WRITE,
                                     FILE_SHARE_WRITE,
                                     nullptr,
                                     OPEN_EXISTING,
                                     FILE_ATTRIBUTE_NORMAL,
                                     nullptr);
        }
        return g_hDevice;
    }

    void CloseProcessMonitorLogger()
    {
        if (INVALID_HANDLE_VALUE != g_hDevice)
        {
            ::CloseHandle(g_hDevice);
            g_hDevice = INVALID_HANDLE_VALUE;
        }
    }

    // Used to pass strings to legacy C APIs expecting a raw void* pointer.
    inline void* StringToPVoid(PCWSTR psz)
    {
        return reinterpret_cast<void *>(const_cast<wchar_t*>(psz));
    }

} // anonymous namespace


PROCMONDEBUGOUTPUT_DLLINTERFACE _Success_(return == TRUE)
BOOL __stdcall ProcMonDebugOutput(_In_z_ LPCWSTR pszOutputString)
{
    BOOL bRet = FALSE;

    if (nullptr == pszOutputString)
    {
        ::SetLastError(ERROR_INVALID_PARAMETER);
        bRet = FALSE;
    }
    else
    {
        HANDLE hProcMon = OpenProcessMonitorLogger();
        if (INVALID_HANDLE_VALUE != hProcMon)
        {
            DWORD iLen = static_cast<DWORD>(wcslen(pszOutputString) * sizeof (WCHAR));
            DWORD iOutLen = 0;
            bRet = ::DeviceIoControl(hProcMon,
                                     IOCTL_EXTERNAL_LOG_DEBUGOUT,
                                     StringToPVoid(pszOutputString),
                                     iLen,
                                     nullptr,
                                     0,
                                     &iOutLen,
                                     nullptr);
            if (FALSE == bRet)
            {
                DWORD dwLastError = ::GetLastError();
                if (ERROR_INVALID_PARAMETER == dwLastError)
                {
                    // The driver is loaded but the user mode Process Monitor
                    // program is not running so turn the last error into a 
                    // write failure.
                    ::SetLastError(ERROR_WRITE_FAULT);
                }
            }
        }
        else
        {
            // Process Monitor isn't loaded.
            ::SetLastError(ERROR_BAD_DRIVER);
            bRet = FALSE;
        }
    }
    return bRet;
}

BOOL APIENTRY DllMain(HMODULE /*hModule*/,
                      DWORD   ul_reason_for_call,
                      LPVOID  /*lpReserved*/)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
        break;
    case DLL_PROCESS_DETACH:
        // Close the handle to the driver.
        CloseProcessMonitorLogger();
        break;
    }
    return TRUE;
}

