/*//////////////////////////////////////////////////////////////////////////////
// Process Monitor Debug Output Header File
//
// History:
// - April 1, 2010 - Version 1.0 - John Robbins/Wintellect
//      - Initial release
// - March 1, 2014 - Version 1.1 - John Robbins/Wintellect
//      - Fixed an issue in DLL main where the handle could get close too
//        soon.
//      - Moved the project to VS 2013.
//
//////////////////////////////////////////////////////////////////////////////*/

#pragma once

#ifdef __cplusplus
extern "C" {
#endif  

    /*//////////////////////////////////////////////////////////////////////////////
    // The defines that set up how the functions or classes are exported or
    // imported.
    //////////////////////////////////////////////////////////////////////////////*/
#ifndef PROCMONDEBUGOUTPUT_DLLINTERFACE
#ifdef PROCMONDEBUGOUTPUT_EXPORTS
#define PROCMONDEBUGOUTPUT_DLLINTERFACE __declspec ( dllexport )
#else
#define PROCMONDEBUGOUTPUT_DLLINTERFACE __declspec ( dllimport )
#endif  
#endif  


    /*//////////////////////////////////////////////////////////////////////////////
    // ProcMonDebugOutput
    //  Sends a string to Process Monitor for display.
    //
    // Parameters:
    //  pszOutputString
    //      The null-terminated wide character string to be displayed.
    //
    // Return Values:
    //  TRUE  - The string was sent to Process Monitor.
    //  FALSE - There was a problem sending the string to Process Monitor. To get
    //          extended error information, call GetLastError to determine the
    //          exact failure.
    //
    // Last Error Codes:
    //  ERROR_INVALID_PARAMETER - The pszOutputString parameter is NULL.
    //  ERROR_WRITE_FAULT       - The Process Monitor driver is loaded but the
    //                            Process Monitor user mode portion is not running.
    //  ERROR_BAD_DRIVER        - The Process Monitor driver is not loaded.
    //////////////////////////////////////////////////////////////////////////////*/
    PROCMONDEBUGOUTPUT_DLLINTERFACE
        _Success_(return == TRUE)
        BOOL __stdcall ProcMonDebugOutput(_In_z_ LPCWSTR pszOutputString);

#ifdef __cplusplus
}
#endif  
