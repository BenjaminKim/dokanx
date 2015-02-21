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
    using System;
    using System.Diagnostics;
    using System.Globalization;

    /// <summary>
    /// Implements a <see cref="TraceListener"/> that redirects output to 
    /// Sysinternal's Process Monitor program.
    /// </summary>
    public class ProcessMonitorTraceListener : TraceListener
    {
        /// <summary>
        /// Writes a trace message to Process Monitor.
        /// </summary>
        /// <param name="message">
        /// A message to write.
        /// </param>
        public override void Write(string message)
        {
            NativeMethods.ProcMonDebugOutput(message);
        }

        /// <summary>
        /// Writes a trace message to Process Monitor.
        /// </summary>
        /// <param name="message">
        /// A message to write.
        /// </param>
        public override void WriteLine(string message)
        {
            NativeMethods.ProcMonDebugOutput(message);
        }
    }
}
