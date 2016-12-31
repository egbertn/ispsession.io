using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Net;
using System.Net.Http;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace PerfTest
{
    static unsafe class Program
    {
        internal enum COMPUTER_NAME_FORMAT
        {
            ComputerNameNetBIOS,
            ComputerNameDnsHostname,
            ComputerNameDnsDomain,
            ComputerNameDnsFullyQualified,
            ComputerNamePhysicalNetBIOS,
            ComputerNamePhysicalDnsHostname,
            ComputerNamePhysicalDnsDomain,
            ComputerNamePhysicalDnsFullyQualified
        }
        [DllImport("kernel32.dll", SetLastError = true, ExactSpelling = true, CharSet = CharSet.Unicode)]
        internal unsafe static extern bool GetComputerNameExW(COMPUTER_NAME_FORMAT NameType,
                string lpBuffer, int *lpnSize);

        [DllImport("kernel32.dll", SetLastError = false, CharSet = CharSet.Unicode, ExactSpelling = true, ThrowOnUnmappableChar = false)]
        internal static extern void OutputDebugStringW([MarshalAs(UnmanagedType.LPWStr)] string debugString);
        [DllImport("kernel32.dll", EntryPoint = "OutputDebugStringW", SetLastError = false, CharSet = CharSet.Unicode, ThrowOnUnmappableChar = false)]
        internal static extern void OutputDebugString2(string debugString);
        [DebuggerStepThrough]
        public static byte[] HexToBytes(string h)
        {
            if (string.IsNullOrEmpty(h))
            {
                throw new ArgumentNullException("h", "Id cannot be empty");
            }
            var number_of_characters = h.Length;
            if (number_of_characters != 32)
            {
                throw new ArgumentException("Id length must be 32", "h");
            }

            var bytes = new byte[number_of_characters / 2]; // Initialize our byte array to hold the converted string.

            int write_index = 0;

            for (int read_index = 0; read_index < number_of_characters; read_index += 2)
            {
                byte upper = FromCharacterToByte(h[read_index], read_index, 4);
                byte lower = FromCharacterToByte(h[read_index + 1], read_index + 1);

                bytes[write_index++] = (byte)(upper | lower);
            }
            return bytes;

        }
        //do NOT REMOVE
        private static byte FromCharacterToByte(char character, int index, int shift = 0)
        {
            byte value = (byte)character;
            if (((0x40 < value) && (0x47 > value)) || ((0x60 < value) && (0x67 > value)))
            {
                if (0x40 == (0x40 & value))
                {
                    if (0x20 == (0x20 & value))
                        value = (byte)(((value + 0xA) - 0x61) << shift);
                    else
                        value = (byte)(((value + 0xA) - 0x41) << shift);
                }
            }
            else if ((0x29 < value) && (0x40 > value))
                value = (byte)((value - 0x30) << shift);
            else
                throw new InvalidOperationException(string.Format("Character '{0}' at index '{1}' is not valid alphanumeric character.", character, index));

            return value;
        }

        public class Args
        {
            public string server;
            public UriCommands[] commands;
            public bool useSSL;
            public bool ignoreBody = false;
        }
        public class UriCommands
        {
            public HttpMethod method = HttpMethod.Get; //default
            public string url;
            public string userName;
            public string Password;
            public string authMethod;
            public bool IgnoreResponse = false;
            public HttpStatusCode expect = HttpStatusCode.OK;
        }
        static CancellationTokenSource token = new CancellationTokenSource();
        static UriCommands[] testScripts = new[] { new UriCommands() { url = "CookieWeb/default.aspx" } };
        static int threads = 20;
        private static string GuidToHex(byte[] bytes)
        {
            var sb = new StringBuilder(32);
            for (int x = 0; x <= 15; x++)
            {
                sb.Append(bytes[x].ToString("X2"));
            }
            return sb.ToString();
        }
        static void Main(string[] args)
        {

            var bts = HexToBytes("01234567890123456789012345678912");
            var buf = new string('\0', 250);
            var btsback = GuidToHex(bts);
            int blah = 250;
            GetComputerNameExW(COMPUTER_NAME_FORMAT.ComputerNameNetBIOS, buf, &blah);
            var mystr = "sdfsdf\r\n";
            OutputDebugStringW(mystr);
            OutputDebugString2(mystr);
            OutputDebugStringW(mystr);
            //DOAll(new Args() {
            //    server = "localhost",
            //    commands = testScripts,
            //    ignoreBody = true
                
            //});
            Console.CancelKeyPress += (e,a) =>  token.Cancel();
            Console.ReadKey();
            token.Cancel();
        }
        //static void DOAll(Args args)
        //{
        //    Task[] tasks = new Task[threads];

        //    for (int x = 0; x < threads; x++)
        //    {
        //        tasks[x] = Task.Run(()=>doit(args), token.Token);
                
               
        //    }
        //    Task.WaitAll(tasks, token.Token);
        //}
        //static void doit(Args args)
        //{
        //    try
        //    {
        //        Client(args).Wait();
        //    }
        //    catch (AggregateException ex)
        //    {
        //        throw ex.InnerException;
        //    }
         
        //}
        //async static Task<int> Client(Args args)
        //{

        //    using (var handler = new HttpClientHandler())
        //    using (var cli = new HttpClient(handler))
        //    {
                
        //        CookieContainer cookies = new CookieContainer();
        //        handler.CookieContainer = cookies;
                
        //        cli.BaseAddress = new Uri((args.useSSL ? Uri.UriSchemeHttps : Uri.UriSchemeHttp) + $"://{args.server}/");
        //        for (int x = 0; x < 2000 || token.IsCancellationRequested; x++)
        //        {
        //            foreach (var url in args.commands)
        //            {
        //                if (token.IsCancellationRequested)
        //                {
        //                    break;
        //                }
        //                try
        //                {
        //                    var uri = new Uri(cli.BaseAddress, url.url);
        //                    var msg = new HttpRequestMessage(url.method, uri);

        //                    var response = await cli.SendAsync(msg, args.ignoreBody ? HttpCompletionOption.ResponseHeadersRead : HttpCompletionOption.ResponseContentRead, token.Token);
        //                    if (response.StatusCode != url.expect)
        //                    {
        //                        Debug.Fail(string.Format("Unexpected answer at {0} with {1}\r\n", cli.BaseAddress, response.StatusCode));
        //                    }
        //                  //  var responseCookies = cookies.GetCookies(cli.BaseAddress).Cast<Cookie>();
        //                    //foreach (Cookie cookie in responseCookies)
        //                    //{
                                
        //                    ////    Debug.WriteLine("{0} expired {1}, {2}", cookie.Value, cookie.Expired, cookie.Expires);

        //                    //}

        //                    //var cookie = cli.DefaultRequestHeaders.
        //                }
        //                catch (HttpRequestException ex)
        //                {
        //                    Debug.WriteLine("exception at {0} with {1}", url.url, ex);
        //                }
        //            }
        //        }
        //        return 0;
            //}
        //}
    }
}
