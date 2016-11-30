using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;
using System.Runtime.ConstrainedExecution;

namespace ADCCure.Configurator.DAL
{
    public sealed class Win32Handle : SafeHandle
    {
        public Win32Handle(IntPtr hnd, bool own)
            : base(hnd, own)
        {
        }
        Win32Handle()
            : base(IntPtr.Zero, true)
        {
        }
        [DllImport(util.KERNEL32, SetLastError = false)]
        [ReliabilityContract(Consistency.WillNotCorruptState, Cer.Success)]
        private static extern int CloseHandle(IntPtr handle);

        public override bool IsInvalid
        {
            get { return IsClosed || handle == IntPtr.Zero; }
        }

        protected override bool ReleaseHandle()
        {
            return CloseHandle(this.handle) == 0;
        }
    }
}
