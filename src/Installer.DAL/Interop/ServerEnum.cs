using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;

namespace ADCCure.Configurator.DAL.Interop
{
    public sealed class Server
    {
        public Server(string name, string comment, System.Version version)
        {
            Name = name;
            Comment = comment;
            Version = version;
        }
        public Server(string name)
        {
            Name = name;

            Version = new System.Version();
        }
        public string Name { get; private set; }
        public System.Version Version { get; private set; }
        public string Comment { get; private set; }
    }

    public sealed class ServerArgs : EventArgs
    {
        internal ServerArgs(IEnumerable<Server> servers)
        {
            Servers = new List<Server>(servers);
        }

        public IList<Server> Servers { get; private set; }
    }

    public static class ServerEnum
    {
        public delegate void NewServerEvent(ServerArgs e);

        public static NewServerEvent OnNewServer;

        public static void EnumServers(NetServerTypeEnum serverType)
        {
            
            int NET_API_STATUS = Interop.NERR_Success;
            do
            {
                IntPtr buf;
                var entriesRead = 0;
                var entriesTotal = 0;
                var copyStrSize = Marshal.SizeOf(typeof(ServerInfo100));
                var resumeHandle = IntPtr.Zero;
                NET_API_STATUS = Interop.NetServerEnum(null, 100, out buf,
                    1000,
                    out entriesRead,
                    out entriesTotal,
                    serverType, null, ref resumeHandle);
                // no network
                IList<Server> srvs = null;
                if (entriesRead == 0)
                {
                    srvs = new[]
                    {
                        new Server(Environment.MachineName, "your workstation", new System.Version())
                    };
                    if (OnNewServer != null)
                    {
                        OnNewServer(new ServerArgs(srvs));
                    }
                }
                    //for (int x = 0; x < entriesRead * copyStrSize; x += copyStrSize)
                    //{
                else
                {
                    if (NET_API_STATUS == Interop.NERR_Success || NET_API_STATUS == Interop.ERROR_MORE_DATA)
                    {
                        srvs = new List<Server>(entriesRead);
                        for (int x = 0; x < entriesRead; x++)
                        {
                            var nfo = Marshal.PtrToStructure< ServerInfo100>(IntPtr.Add(buf, x * copyStrSize));
                            srvs.Add(new Server(nfo.sv100_name));
                          // causes crash on X64
                            //  Marshal.DestroyStructure(IntPtr.Add(buf, x * copyStrSize), typeof(SERVER_INFO_101));
                        }
                    }
                    //}
                    if (OnNewServer != null)
                    {
                        OnNewServer(new ServerArgs(srvs));
                    }

                    Interop.NetApiBufferFree(buf);
                }
            } while (NET_API_STATUS == Interop.ERROR_MORE_DATA);
        }
    }
}
