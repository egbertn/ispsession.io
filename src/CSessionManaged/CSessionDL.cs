using System;
using StackExchange.Redis;
using System.Threading;
using System.Web.SessionState;
using System.Web.Configuration;
using System.Runtime.InteropServices;
using System.IO;
using System.IO.Compression;
using ispsession.io.Interfaces;
using System.Linq;
using System.Collections.Generic;

namespace ispsession.io
{
    /// <summary>
    /// All Redis commands and connection management is here
    /// </summary>
    internal static class CSessionDL
    {
        internal static void ApplicationGet(IDatabase database, string appKey, IKeyManager pDictionary)
        {
            var appkey = appKey.ToUpperInvariant() ;
            var keymembers = database.SetMembers(appkey);
            var keyCount = keymembers.Length;
            if (keyCount > 0)
            {
                var values = database.StringGet(keymembers.Select(s => (RedisKey)(string)s).ToArray());                
                for (var x = 0; x < keyCount; x++)
                {
                    if (!values[x].IsNullOrEmpty)
                    {
                        pDictionary.DeserializeKey(values[x]);
                    }
                }
            }

        }

        internal static void ApplicationSave(IDatabase database,
                                                string appKey, IKeyManager pDictionary,                                        
                                                TimeSpan totalRequestTime)
        {
            var appkey = appKey.ToUpperInvariant();
            var appkeyPrefix = appkey + ":";
            var setKey = (RedisKey)appkey;

            pDictionary.KeyStates(out var changedKeys, out var newKeys, out var otherKeys, out var expireKeys, out var removedKeys);
            if (newKeys.Count > 0 || changedKeys.Count > 0 || removedKeys.Count > 0 || expireKeys.Count > 0)
            {
                var transaction = database.CreateTransaction();
                if (newKeys.Count > 0)
                {
                    database.SetAdd(setKey, newKeys.Select(s => (RedisValue)(appkeyPrefix + s.ToUpperInvariant())).ToArray(), CommandFlags.FireAndForget);
                }

                if (removedKeys.Count > 0)
                {
                    database.SetRemove(setKey, removedKeys.Select(s => (RedisValue)(appkeyPrefix + s.ToUpperInvariant())).ToArray(), CommandFlags.FireAndForget);
                }
                //KVP is a struct, therefore, values are copied by value, not by reference 
                //make sure to use the least amount of mem footprint
                var multipleSet = new KeyValuePair<RedisKey, RedisValue>[newKeys.Count + changedKeys.Count];

                int ct = 0;
                foreach (var key in newKeys.Concat(changedKeys))
                {
                    multipleSet[ct++] = new KeyValuePair<RedisKey, RedisValue>(appkeyPrefix + key.ToUpperInvariant(), pDictionary.SerializeKey(key));
                }

                if (multipleSet.Length > 0)
                {
                    database.StringSet(multipleSet, When.Always, CommandFlags.None);
                }
                if (expireKeys.Count > 0)
                {
                    foreach (var key in expireKeys)
                    {
                        database.KeyExpire(key.Item1, TimeSpan.FromMilliseconds(key.Item2), CommandFlags.FireAndForget);
                    }
                }
                transaction.Execute(CommandFlags.FireAndForget);
            }
        }

        private static readonly Lazy<ConfigurationOptions> configOptions = new Lazy<ConfigurationOptions>(() =>
            {
                var appSettings = WebConfigurationManager.AppSettings[SessionAppSettings.ispsession_io_pref + "DataSource"] ?? "localhost:6379";
                var pw = SessionAppSettings.GetDBFromConnString(appSettings, "password");
                var dbNo = SessionAppSettings.GetDBFromConnString(appSettings, "database");
                var db = (int?) -1;
                if (!string.IsNullOrEmpty(dbNo))
                {
                    db = int.Parse(dbNo);
                }
                var parts = appSettings.Split(',');
                var hostandPort = parts[0];//assumption, the first one
                var configOptions = new ConfigurationOptions()
                {
                    ClientName = "ISP Session Connection",
                    ConnectTimeout = 100000,
                    SyncTimeout = 100000,
                    AbortOnConnectFail = false,
                    Password = pw,
                    DefaultDatabase = db
                };
                configOptions.EndPoints.Add(hostandPort);
                return configOptions;
            });

        private static readonly Lazy<ConnectionMultiplexer> conn = new Lazy<ConnectionMultiplexer>(
            () => ConnectionMultiplexer.Connect(configOptions.Value));

        internal static ConnectionMultiplexer SafeConn
        {
            get
            {
                return conn.Value;
            }
        }
        internal static bool RedundantExists(this IDatabase db, string cookie, SessionAppSettings settings)
        {
            for (var x = 0; x < 3; x++)
            {
                try
                {
                    return db.KeyExists(settings.GetKey(cookie)); 
                }
                catch (TimeoutException) //shit happens sometimes
                {
                    Thread.Sleep(400);
                    continue;
                }
                catch { throw; }
            }
            return false; //ok
        }
        internal static string GetKey(this SessionAppSettings id, string SessionId)
        {
            return string.Format("{0}:{1}", id.AppKey, SessionId);
        }
        /// <summary>
        /// 
        /// </summary>
        /// <param name="id"></param>
        /// <param name="SessionId">assumption SessionId is valid</param>
        internal static ISPSessionStateItemCollection SessionGet(SessionAppSettings settings, string SessionId)
        {
            var db = CSessionDL.SafeConn.GetDatabase(settings.DataBase);
            var key = settings.GetKey(SessionId);
            var ms = default(Stream);
            try
            {
                var data = db.StringGet(key);;
                if (data.IsNull)
                {
                    return null;// session not found
                }
                var blob = (byte[])data;
                var blobLen = blob.Length;
                var meta = FromByteArray(blob);
                if (blobLen == meta.SizeofMeta)
                {
                    return null;// obviously, only meta can be overwritten
                }
                if (settings.Compress)
                {   //deflateStream is faster, but leaves no CRC numbers behind, it seems.                    
                    ms = new MemoryStream(meta.ZLen);
                    using (var zippedData = new MemoryStream(blob, meta.SizeofMeta, blobLen - meta.SizeofMeta))
                    using (var unzip = new DeflateStream(zippedData, CompressionMode.Decompress))
                    {
                        unzip.CopyTo(ms);                        
                    }
                    ms.Position = 0;
                }
                else
                {
                    ms = new MemoryStream(blob, meta.SizeofMeta, blobLen - meta.SizeofMeta);
                }
                
                    
                var retVal = new ISPSessionStateItemCollection(){
                    Meta = meta ,
                    Items = PersistUtil.LocalLoad(ms)                 
                };
                return retVal;              
                
            }
            catch (InvalidDataException)  // not zipped! "The CRC in GZip footer does not match the CRC calculated from the decompressed data."
            {
                //try again, but now without unzipping
                var cloned = settings.Clone();
                cloned.Compress = false;
                var retVal = SessionGet(cloned, SessionId);
                return retVal;
            }
            catch(Exception ex)
            {
                StreamManager.TraceError("Fatal SessionGet {0}", ex);
                throw ex;
            }
            finally
            {
                if (ms != null)
                {
                    ms.Dispose();
                }
            }
        }
        /// <summary>
        /// saves session and settings. If readonly, will only set expiration
        /// </summary>        
        internal static void SessionSave(SessionAppSettings settings, IHttpSessionState state, ref PersistMetaData meta) //, 
        {
            int zLen = 0;

            var contents = state.IsReadOnly ? new byte[] {0} : PersistUtil.LocalContents(state, out zLen, settings.Compress);
            var contLen = contents.Length;
            meta.ZLen = zLen;
            if (zLen == 0 && !state.IsReadOnly)
            {
                StreamManager.TraceError("SessionSave session should not be zero in length");
            }         
            Array.Resize(ref contents, contLen + meta.SizeofMeta);
            Array.Copy(contents, 0, contents, meta.SizeofMeta, contLen);
            var tinybuf = meta.ToByteArray();
            Array.Copy(tinybuf, 0, contents, 0, meta.SizeofMeta);

            var ts = TimeSpan.FromMinutes(state.Timeout == 0 ? settings.SessionTimeout : state.Timeout);

            var db = CSessionDL.SafeConn.GetDatabase(settings.DataBase);
            var key = settings.GetKey(state.SessionID);
            //todo, if readonly, only set expiration
            try
            {
                if (state.IsReadOnly)
                {
                    db.KeyExpire(key, ts, CommandFlags.FireAndForget);
                }
                else
                {
                    db.StringSet(key, contents, ts, When.Always, CommandFlags.FireAndForget);
                }
            }
            catch (Exception ex)
            {
                StreamManager.TraceError("Fatal SessionSave {0}", ex);
            }

        }

        internal static void SessionRemove(SessionAppSettings settings, string sessionId)
        {
            var db = CSessionDL.SafeConn.GetDatabase(settings.DataBase);
            var key = settings.GetKey(sessionId);
            try
            {
                if (db.KeyExists(key))
                {
                    db.KeyDelete(key, CommandFlags.FireAndForget);                    
                }
            }
            catch (Exception ex)
            {
                StreamManager.TraceError("Fatal SessionRemove {0}", ex);
            }
        }

        public static byte[] ToByteArray(this PersistMetaData meta)
        {

            IntPtr ptr = IntPtr.Zero;
            try
            {
                int size = Marshal.SizeOf(meta);
                var arr = new byte[size];
                ptr = Marshal.AllocHGlobal(size);
                Marshal.StructureToPtr(meta, ptr, true);
                Marshal.Copy(ptr, arr, 0, size);
                return arr;
            }
            catch (Exception e)
            {
                throw new InvalidOperationException("Cannot serialize metadata", e);
            }
            finally
            {
                if (ptr != IntPtr.Zero)
                    Marshal.FreeHGlobal(ptr);
            }
        }
        /// <summary>
        /// deserializes a (32-byte) byte array to PersistMetaData
        /// </summary>
        /// <param name="arr">byte array must be equal to length of PersistMetaData</param>
        /// <returns>an instance of PersistMetaData</returns>
        /// <exception cref="ArgumentOutOfRangeException"/>
        private static PersistMetaData FromByteArray(byte[] arr)
        {
            if (arr == null)
            {
                throw new ArgumentNullException("arr");
            }

            IntPtr ptr = IntPtr.Zero;
            try
            {
                int size = Marshal.SizeOf<PersistMetaData>();
                var arrLen = arr.Length;
                if (arrLen < size)
                {
                    throw new ArgumentOutOfRangeException("arr", "length of PersistMetaData is > arr.Length");
                }
                ptr = Marshal.AllocHGlobal(size);
                Marshal.Copy(arr, 0, ptr, size); //array length may be smaller
                var meta = Marshal.PtrToStructure <PersistMetaData>(ptr);

                return meta;
            }
            catch
            {
                throw;
            }
            finally
            {
                if (ptr != IntPtr.Zero)
                    Marshal.FreeHGlobal(ptr);
            }
        }
      
        internal static void SessionInsert(SessionAppSettings settings, string sessionID, PersistMetaData meta)
        {
            IDatabase db = CSessionDL.SafeConn.GetDatabase(settings.DataBase);
            var key = settings.GetKey(sessionID);
            var contents = meta.ToByteArray();
            var ts = TimeSpan.FromMinutes(meta.Expires);
            try
            {
                db.StringSet(key, contents, ts, When.Always, CommandFlags.FireAndForget);
            }
            catch(Exception ex)
            {
                StreamManager.TraceError("Fatal SessionInsert {0}", ex);
            }
        }
    }
}
