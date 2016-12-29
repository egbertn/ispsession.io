using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Net;
using System.Net.Http;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace PerfTest
{
    static class Program
    {
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
        static void Main(string[] args)
        {

            DOAll(new Args() {
                server = "localhost",
                commands = testScripts,
                ignoreBody = true
                
            });
            Console.CancelKeyPress += (e,a) =>  token.Cancel();
            Console.ReadKey();
            token.Cancel();
        }
        static void DOAll(Args args)
        {
            Task[] tasks = new Task[threads];

            for (int x = 0; x < threads; x++)
            {
                tasks[x] = Task.Run(()=>doit(args), token.Token);
                
               
            }
            Task.WaitAll(tasks, token.Token);
        }
        static void doit(Args args)
        {
            try
            {
                Client(args).Wait();
            }
            catch (AggregateException ex)
            {
                throw ex.InnerException;
            }
         
        }
        async static Task<int> Client(Args args)
        {

            using (var handler = new HttpClientHandler())
            using (var cli = new HttpClient(handler))
            {
                
                CookieContainer cookies = new CookieContainer();
                handler.CookieContainer = cookies;
                
                cli.BaseAddress = new Uri((args.useSSL ? Uri.UriSchemeHttps : Uri.UriSchemeHttp) + $"://{args.server}/");
                for (int x = 0; x < 2000 || token.IsCancellationRequested; x++)
                {
                    foreach (var url in args.commands)
                    {
                        if (token.IsCancellationRequested)
                        {
                            break;
                        }
                        try
                        {
                            var uri = new Uri(cli.BaseAddress, url.url);
                            var msg = new HttpRequestMessage(url.method, uri);

                            var response = await cli.SendAsync(msg, args.ignoreBody ? HttpCompletionOption.ResponseHeadersRead : HttpCompletionOption.ResponseContentRead, token.Token);
                            if (response.StatusCode != url.expect)
                            {
                                Debug.Fail(string.Format("Unexpected answer at {0} with {1}\r\n", cli.BaseAddress, response.StatusCode));
                            }
                          //  var responseCookies = cookies.GetCookies(cli.BaseAddress).Cast<Cookie>();
                            //foreach (Cookie cookie in responseCookies)
                            //{
                                
                            ////    Debug.WriteLine("{0} expired {1}, {2}", cookie.Value, cookie.Expired, cookie.Expires);

                            //}

                            //var cookie = cli.DefaultRequestHeaders.
                        }
                        catch (HttpRequestException ex)
                        {
                            Debug.WriteLine("exception at {0} with {1}", url.url, ex);
                        }
                    }
                }
                return 0;
            }
        }
    }
}
