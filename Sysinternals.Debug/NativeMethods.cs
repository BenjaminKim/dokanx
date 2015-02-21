/*//////////////////////////////////////////////////////////////////////////////
// ProcessMonitorTraceListener
// 
// History:
// - April 1, 2010 - Version 1.0 - John Robbins/Wintellect
//      - Initial release.
// - March 1, 2014 - Version 1.1 - John Robbins/Wintellect
//      - Moved to VS 2013 and .NET 4.5.1
//////////////////////////////////////////////////////////////////////////////*/

namespace Sysinternals.Debug
{
    using Microsoft.Win32.SafeHandles;
    using System;
    using System.Diagnostics;
    using System.Diagnostics.CodeAnalysis;
    using System.Globalization;
    using System.Runtime.InteropServices;
    using System.Security;
    using System.Text;

    /// <summary>
    /// A class to wrap all the native code needed by this assembly.
    /// </summary>
    [SuppressMessage("Microsoft.Portability",
                     "CA1903:UseOnlyApiFromTargetedFramework",
                     MessageId = "System.Security.SecuritySafeCriticalAttribute",
                     Justification = "Everyone is running .NET 2.0 SP2 so they have SecuritySafeCritical")]
    [SecuritySafeCritical]
    internal static class NativeMethods
    {
        // Constants to represent C preprocessor macros for PInvokes
        private const uint GENERIC_WRITE = 0x40000000;
        private const uint OPEN_EXISTING = 3;
        private const uint FILE_WRITE_ACCESS = 0x0002;
        private const uint FILE_SHARE_WRITE = 0x00000002;
        private const uint FILE_ATTRIBUTE_NORMAL = 0x00000080;
        private const uint METHOD_BUFFERED = 0;

        // Process Monitor Constants 
        private const uint FILE_DEVICE_PROCMON_LOG = 0x00009535;
        private const string PROCMON_DEBUGGER_HANDLER = "\\\\.\\Global\\ProcmonDebugLogger";

        /// <summary>
        /// The handle to the Process Monitor log device.
        /// </summary>
        private static SafeFileHandle hProcMon;

        /// <summary>
        /// Gets the IO Control code for the ProcMon log.
        /// </summary>
        private static uint IOCTL_EXTERNAL_LOG_DEBUGOUT { get { return CTL_CODE(); } }

        /// <summary>
        /// Builds the control code for the Process Monitor driver access.
        /// </summary>
        /// <seealso href="http://msdn.microsoft.com/en-us/library/windows/hardware/ff543023(v=vs.85).aspx"/>
        private static uint CTL_CODE(uint DeviceType = FILE_DEVICE_PROCMON_LOG,
                                      uint Function = 0x81,
                                      uint Method = METHOD_BUFFERED,
                                      uint Access = FILE_WRITE_ACCESS)
        {
            return ((DeviceType << 16) | (Access << 14) | (Function << 2) | Method);
        }

        /// <summary>
        /// Handles calling CreateFile.
        /// </summary>
        /// <remarks>
        /// This is only used for opening the Process Monitor log handle, hence the default parameters.
        /// </remarks>
        /// <seealso href="http://msdn.microsoft.com/en-us/library/windows/desktop/aa363858(v=vs.85).aspx"/>
        [SuppressMessage("Microsoft.Security",
                         "CA5122:PInvokesShouldNotBeSafeCriticalFxCopRule",
                         Justification = "This is a bug in Code Analysis on pre-4.0 assemblies: http://connect.microsoft.com/VisualStudio/feedback/details/729254/bogus-ca5122-warning-about-p-invoke-declarations-should-not-be-safe-critical")]
        [DllImport("kernel32.dll",
                   SetLastError = true,
                   CharSet = CharSet.Unicode)]
        private static extern SafeFileHandle CreateFile(string lpFileName = PROCMON_DEBUGGER_HANDLER,
                                                        uint dwDesiredAccess = GENERIC_WRITE,
                                                        uint dwShareMode = FILE_SHARE_WRITE,
                                                        IntPtr lpSecurityAttributes = default(IntPtr),
                                                        uint dwCreationDisposition = OPEN_EXISTING,
                                                        uint dwFlagsAndAttributes = FILE_ATTRIBUTE_NORMAL,
                                                        IntPtr hTemplateFile = default(IntPtr));

        [SuppressMessage("Microsoft.Security",
                         "CA5122:PInvokesShouldNotBeSafeCriticalFxCopRule",
                         Justification = "This is a bug in Code Analysis on pre-4.0 assemblies: http://connect.microsoft.com/VisualStudio/feedback/details/729254/bogus-ca5122-warning-about-p-invoke-declarations-should-not-be-safe-critical")]
        [DllImport("kernel32.dll",
                    ExactSpelling = true,
                    SetLastError = true,
                    CharSet = CharSet.Unicode)]
        [return: MarshalAs(UnmanagedType.Bool)]
        private static extern bool DeviceIoControl(SafeFileHandle hDevice,
                                                   uint dwIoControlCode,
                                                   StringBuilder lpInBuffer,
                                                   uint nInBufferSize,
                                                   IntPtr lpOutBuffer,
                                                   uint nOutBufferSize,
                                                   out uint lpBytesReturned,
                                                   IntPtr lpOverlapped);

        [SuppressMessage("Microsoft.Performance",
                         "CA1810:InitializeReferenceTypeStaticFieldsInline",
                         Justification = "How else are you going to set up a static event? (http://social.msdn.microsoft.com/Forums/en-US/d11fe313-278c-4cae-bfcc-b119204866c7/ca1810-incorrect?forum=vstscode)")]
        static NativeMethods()
        {
            AppDomain.CurrentDomain.ProcessExit += (sender, args) =>
            {
                if (!hProcMon.IsInvalid)
                {
                    hProcMon.Close();
                }
            };
        }

        /// <summary>
        /// Does the actual tracing to Process Monitor.
        /// </summary>
        /// <param name="message">
        /// The message to display.
        /// </param>
        /// <param name="args">
        /// The formatting arguments for the message
        /// </param>
        /// <returns>
        /// True if the trace succeeded, false otherwise.
        /// </returns>
        public static bool ProcMonDebugOutput(string message, params object[] args)
        {
            bool returnValue = false;
            StringBuilder renderedMessage = new StringBuilder();
            renderedMessage.AppendFormat(CultureInfo.CurrentCulture, message, args);
            uint outLen;

            if (hProcMon == null || hProcMon.IsInvalid)
            {
                hProcMon = CreateFile();
            }

            returnValue = DeviceIoControl(hProcMon, 
                                          IOCTL_EXTERNAL_LOG_DEBUGOUT,
                                          renderedMessage, 
                                          (uint)(message.Length * sizeof(System.Char)),
                                          IntPtr.Zero, 
                                          0, 
                                          out outLen, 
                                          IntPtr.Zero);
            return returnValue;
        }
    }
}
