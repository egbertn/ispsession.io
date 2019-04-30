using System;
using StackExchange.Redis;
using System.Threading;
using System.Runtime.InteropServices;
using System.IO;
using System.IO.Compression;
using ispsession.io.core.Interfaces;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

namespace ispsession.io.core
{
    /// <summary>
    /// All Redis commands and connection management is here
    /// </summary>
    internal static class CSessionDL
    {
        private static readonly object locker = new object();
        private static string[] EndPoint;
        private static void setEndPoint(string endP)
        {
            lock(locker)
            {                
                EndPoint = endP.Split(',');
            }
        }
        private static readonly Lazy<ConfigurationOptions> configOptions = new Lazy<ConfigurationOptions>(() =>
        {
            var appSettings = EndPoint ?? (new[] { "localhost:6379" });//TODO

            var configOptions = new ConfigurationOptions()
            {
                ClientName = "ISP Session Connection",
                ConnectTimeout = 100000,
                SyncTimeout = 100000,
                AbortOnConnectFail = false
            };
            foreach(var e in appSettings)
                configOptions.EndPoints.Add(e);

            return configOptions;
        });

        private static readonly Lazy<ConnectionMultiplexer> conn = new Lazy<ConnectionMultiplexer>(
            () => ConnectionMultiplexer.Connect(configOptions.Value));

        private static ConnectionMultiplexer SafeConn
        {
            get
            {
                return conn.Value;
            }
        }
        internal static async Task ApplicationGet(CacheAppSettings settings, IKeySerializer pDictionary)
        {
            var appKey = settings.AppKey.ToUpperInvariant();
            var database = SafeConn.GetDatabase(settings.DataBase);
            var keymembers = await database.SetMembersAsync(appKey);
            var keyCount = keymembers.Length;
            if (keyCount > 0)
            {
                var values = await database.StringGetAsync(keymembers.Select(s => (RedisKey)(string)s).ToArray());
                for (var x = 0; x < keyCount; x++)
                {
                    if (!values[x].IsNullOrEmpty)
                    {
                        pDictionary.DeserializeKey(values[x]);
                    }
                }
            }

        }

        internal static async Task ApplicationSave(CacheAppSettings settings, IKeySerializer pDictionary,
                                                TimeSpan totalRequestTime)
        {
            var appkey = settings.AppKey.ToUpperInvariant();
            var database = SafeConn.GetDatabase(settings.DataBase);
            var appkeyPrefix = appkey + ":";
            var setKey = (RedisKey)appkey;

            pDictionary.KeyStates(out var changedKeys, out var newKeys, out var otherKeys, out var expireKeys, out var removedKeys);
            if (newKeys.Count > 0 || changedKeys.Count > 0 || removedKeys.Count > 0 || expireKeys.Count > 0)
            {
                var transaction = database.CreateTransaction();
                if (newKeys.Count > 0)
                {
                    await database.SetAddAsync(setKey, newKeys.Select(s => (RedisValue)(appkeyPrefix + s.ToUpperInvariant())).ToArray(), CommandFlags.FireAndForget);
                }

                if (removedKeys.Count > 0)
                {
                   await database.SetRemoveAsync(setKey, removedKeys.Select(s => (RedisValue)(appkeyPrefix + s.ToUpperInvariant())).ToArray(), CommandFlags.FireAndForget);
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
                    await database.StringSetAsync(multipleSet, When.Always, CommandFlags.FireAndForget);
                }
                if (expireKeys.Count > 0)
                {
                    foreach (var key in expireKeys)
                    {
                        await database.KeyExpireAsync(key.Item1, TimeSpan.FromMilliseconds(key.Item2), CommandFlags.FireAndForget);
                    }
                }
               await transaction.ExecuteAsync(CommandFlags.FireAndForget);
            }
        }
        internal static IDatabase GetDatabase(CacheAppSettings settings)
        {
            setEndPoint(settings.DatabaseConnection);
            return SafeConn.GetDatabase(settings.DataBase);
        }
        internal static IDatabase GetDatabase(SessionAppSettings settings)
        {
            setEndPoint(settings.DatabaseConnection);
            return SafeConn.GetDatabase(settings.DataBase);
        }
        internal static async Task<bool> RedundantExistsAsync(this IDatabase db, string cookie, SessionAppSettings settings)
        {
            for (var x = 0; x < 3; x++)
            {
                try
                {
                    return await db.KeyExistsAsync(settings.GetKey(cookie)); //async has problems
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
            return $"{id.AppKey}:{SessionId}";
        }
        internal static async Task< ISPSessionStateItemCollection> SessionGetAsync(SessionAppSettings settings, string SessionId)
        {
            var db = CSessionDL.GetDatabase(settings);
            var key = settings.GetKey(SessionId);
            var ms = default(Stream);
            if (string.IsNullOrEmpty(SessionId))
            {
                throw new ArgumentNullException(nameof(SessionId));
            }
            try
            {
                var data = await db.StringGetAsync(key);
                if (data.IsNull)
                {
                    return null;// session not found
                }
                var blob = (byte[])data;
                var blobLen = blob.Length;
                var meta = FromByteArray(blob);
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


                var retVal = new ISPSessionStateItemCollection()
                {
                    Meta = meta,
                    Items = PersistUtil.LocalLoad(ms),
                };
              
                return retVal;

            }
            catch (InvalidDataException)  // not zipped! "The CRC in GZip footer does not match the CRC calculated from the decompressed data."
            {
                //try again, but now without unzipping
                var cloned = settings.Clone();
                cloned.Compress = false;
                var retVal = await SessionGetAsync(cloned, SessionId);
                
                return retVal;
            }
            catch (Exception ex)
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
        internal static async Task SessionSave(SessionAppSettings settings, ISPSessionStateItemCollection2 state, PersistMetaData meta, string sessionId, string oldSessionId) //, 
        {

            var contents = PersistUtil.LocalContents(state, out int zLen, settings.Compress);
            var contLen = contents.Length;
            meta.ZLen = zLen;
            Array.Resize(ref contents, contLen + meta.SizeofMeta);
            Array.Copy(contents, 0, contents, meta.SizeofMeta, contLen);
            var tinybuf = meta.ToByteArray();
            Array.Copy(tinybuf, 0, contents, 0, meta.SizeofMeta);

            var ts = TimeSpan.FromMinutes(meta.Expires );

            var db = CSessionDL.GetDatabase(settings);
            var key = settings.GetKey(sessionId);
            //todo, if readonly, only set expiration
            if (!string.IsNullOrEmpty(oldSessionId) && !sessionId.Equals(oldSessionId))
            {
                await db.KeyRenameAsync(settings.GetKey(oldSessionId), key, When.Always, CommandFlags.FireAndForget);

            }
            try
            {
                if (state.IsReadOnly || !state.Dirty)
                {
                    if (!state.IsNew)
                    {
                       await db.KeyExpireAsync(key, ts, CommandFlags.FireAndForget);
                    }
                }
                else
                {
                    await db.StringSetAsync(key, contents, ts, When.Always, CommandFlags.FireAndForget);
                }
            }
            catch (Exception ex)
            {
                StreamManager.TraceError("Fatal SessionSave {0}", ex);
            }

        }

        internal static async Task SessionRemoveAsync(SessionAppSettings settings, string sessionId)
        {
            var db = CSessionDL.GetDatabase(settings);
            var key = settings.GetKey(sessionId);
            try
            {
                await db.KeyDeleteAsync(key, CommandFlags.FireAndForget);                
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
                var meta = Marshal.PtrToStructure<PersistMetaData>(ptr);

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
    }
}
