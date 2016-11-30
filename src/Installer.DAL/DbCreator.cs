using System;
using System.Collections.Generic;
using System.Data;
using System.Data.OleDb;
using System.Data.SqlClient;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Text;

namespace ADCCure.Configurator.DAL
{

    /// <summary>
    /// helper class to create or delete a db
    /// </summary>
    public sealed class DbCreator
    {
        private static string _gPasswordCache;
        private readonly IDbConnection _mDbConn;
        /// <summary>
        /// server
        /// </summary>
        private string _mDatasource;
        private string _mDatabase;
        private bool _mIntegratedSecurity;
        readonly Dictionary<string, string> m_dict = new Dictionary<string, string>(StringComparer.InvariantCultureIgnoreCase);

        private EnumDbTypes _mDbType = EnumDbTypes.Unknown;
        public Action<string, int> StateCallBack;
        public DbCreator(IDbConnection dbConn)
        {
            if (dbConn == null)
            {
                throw new ArgumentNullException("dbConn");
            }
            if (dbConn is SqlConnection)
            {
                _mDbType = EnumDbTypes.SQLServer;
            }
            _mDbConn = dbConn;

        }
        public static string ConnectionStringFromProps(Dictionary<string, string> dictionary, bool includeProvider)
        {

            return string.Join("", dictionary.Where(s => string.Compare(s.Key, "provider", StringComparison.OrdinalIgnoreCase) != 0 || includeProvider).
                Select(p => string.Format("{0}={1};", p.Key, p.Value))).TrimEnd(';');

        }
        public static Dictionary<string, string> ConnectionStringToProps(string connectionString)
        {
            var retVal = new Dictionary<string, string>(StringComparer.OrdinalIgnoreCase);
            var parts = connectionString.Split(';');
            foreach (string s in parts)
            {
                string[] parts2 = s.Split('=');
                if (parts2.Length == 2 && parts2[0].Trim().Length > 0)
                {
                    retVal[parts2[0].Trim()] = parts2[1].Trim();
                }
            }

            return retVal;
        }
        private void ParseConnectionString(string connectionString)
        {
            string[] parts = connectionString.Split(';');
            foreach (string s in parts)
            {
                string[] parts2 = s.Split('=');
                if (parts2.Length == 2 && parts2[0].Trim().Length > 0)
                {
                    m_dict[parts2[0].Trim()] = parts2[1].Trim();
                }
            }

            string temp = m_dict.ContainsKey("data source") ? m_dict["data source"] : string.Empty;
            if (temp.Length == 0)
            {
                temp = m_dict.ContainsKey("server") ? m_dict["server"] : string.Empty;
            }
            int colon = temp.IndexOf(':');
            if (colon > 0)
            {
                temp = temp.Substring(colon + 1);
            }
            _mDatasource = temp;
            _mIntegratedSecurity = false;
            if (m_dict.ContainsKey("Integrated Security"))
            {
                _mIntegratedSecurity = string.Compare(m_dict["Integrated Security"], "true", StringComparison.OrdinalIgnoreCase) == 0;

            }
            if (m_dict.ContainsKey("User ID"))
            {
                _mIntegratedSecurity = false;
            }
            if (m_dict.ContainsKey("Database"))
            {
                _mDatabase = m_dict["Database"];
            }
            else if (m_dict.ContainsKey("Initial Catalog"))
            {
                _mDatabase = m_dict["Initial Catalog"];
            }


        }
        /// <summary>
        /// ensures a connection string having 'master'  ignores Database/Initial Catalog
        /// </summary>
        /// <param name="connectionString"></param>
        /// <returns></returns>
        public static string ConnectionStringMaster(string connectionString)
        {
            var parts = connectionString.Split(';');

            var mDict = new Dictionary<string, string>(StringComparer.OrdinalIgnoreCase);
            foreach (string s in parts)
            {
                var parts2 = s.Split('=');
                if (parts2.Length == 2 && parts2[0].Trim().Length > 0)
                {
                    mDict[parts2[0].Trim()] = parts2[1].Trim();
                }
            }
            if (mDict.ContainsKey("Database"))
            {
                mDict["Database"] = "master";
            }
            else if (mDict.ContainsKey("Initial Catalog"))
            {
                mDict["Initial Catalog"] = "master";
            }
            return ConnectionStringFromProps(mDict, false);

        }
        /// <summary>
        /// creates a DB user, and adds that to the rolename
        /// If this already exists, it leaves it as is.
        /// </summary>
        /// <param name="userName">the user mapping to the db</param>
        /// <param name="roleName">optional rolename. If null or "" will be ignored</param>
        public void EnsureUserInDBRole(string userName, string roleName)
        {
            if (!string.IsNullOrEmpty(roleName))
            {
                if (_mDbType == EnumDbTypes.SQLServer)
                {
                    using (var cmd = _mDbConn.CreateCommand())
                    {

                        cmd.CommandType = CommandType.Text;
                        //CREATE USER [WIN-AOOMC7Y9P3S\XDSAdmin] FOR LOGIN [WIN-AOOMC7Y9P3S\XDSAdmin] WITH DEFAULT_SCHEMA=[dbo]

                        userName = userName.Replace("''", "''");
                        roleName = roleName.Replace("'", "''");
                        cmd.CommandText = string.Format("IF NOT EXISTS (SELECT * FROM sys.database_principals WHERE name=N'{0}') BEGIN CREATE USER [{0}] FOR LOGIN [{0}] WITH DEFAULT_SCHEMA=[dbo] END", userName);
                        cmd.ExecuteNonQuery();
                        cmd.CommandText = string.Format("IF NOT EXISTS (SELECT * FROM sys.database_principals WHERE name = N'{0}' AND type='R') BEGIN CREATE ROLE [{0}] AUTHORIZATION [dbo] END", roleName);
                        cmd.ExecuteNonQuery();

                        cmd.CommandText = string.Format("EXEC sp_addrolemember @rolename = '{0}', @membername = '{1}'", roleName, userName);
                        cmd.ExecuteNonQuery();
                    }
                }
            }
        }
        /// <summary>
        /// creates a random pass using some simple algorithm with capitals,numbers and symbols
        /// </summary>
        /// <param name="globalCache">If true, the password is cached in the process memory and generated just once</param>
        /// <returns></returns>
        public static string GenerateRandomPass(bool globalCache)
        {
            if (globalCache && _gPasswordCache != null)
            {
                return _gPasswordCache;
            }
            var hasHadCapital = false;

            const int passwordLength = 10;
            const string numstuff = "0123456789#@!*_-";
            const string alfabeth = "abcdefghijklmnopqrstuvwxyz";
            var maxValue = alfabeth.Length;
            var maxNumValue = numstuff.Length;
            var sb = new StringBuilder(passwordLength);
            var rnd = new Random(Environment.TickCount);
            var rndNum = new Random(Environment.TickCount);
            for (int x = 0; x < passwordLength; x++)
            {
                char ch;
                if (x % (rndNum.Next(5) + 1) == 0)
                {
                    int el = rnd.Next(maxNumValue);
                    ch = numstuff[el];
                }
                else
                {
                    int el = rnd.Next(maxValue);
                    ch = alfabeth[el];
                    if (char.IsLetter(ch)  && ((x % 2 == 0) || hasHadCapital == false) )
                    {
                        hasHadCapital = true;
                        ch = char.ToUpper(ch);
                    }
                }
                sb.Append(ch);
            }

            return globalCache ? _gPasswordCache = sb.ToString() : sb.ToString();
        }
        
        public void EnsureUserInDatabase(string userName, string passWord, bool integratedSecurity)
        {
            //CREATE LOGIN [COMPUTER OR DOMAIN\Egbert Nierop] FROM WINDOWS 
            string saveDatabase = _mDbConn.Database;

            if (_mDbType == EnumDbTypes.SQLServer)
            {
                _mDbConn.ChangeDatabase("master");
                string userCreate = string.Format("IF NOT EXISTS (SELECT * FROM sys.server_principals WHERE name=N'{0}') BEGIN CREATE LOGIN [{0}]", userName);
                if (integratedSecurity)
                {
                    userCreate += string.Format(" FROM WINDOWS");
                }
                else
                {
                    userCreate += string.Format(" WITH PASSWORD=N'{0}', CHECK_EXPIRATION=OFF, CHECK_POLICY=OFF", passWord.Replace("'", "''"));
                }
                userCreate += " END ";
                using (var cmd = _mDbConn.CreateCommand())
                {
                    cmd.CommandText = userCreate;
                    cmd.CommandType = CommandType.Text;
                    cmd.ExecuteNonQuery();
                }
                _mDbConn.ChangeDatabase(saveDatabase);
            }
        }

        /// <summary>
        /// creates specified datbase and makes it the current one
        /// </summary>
        /// <param name="dbName"></param>
        public void CreateDatabase(string dbName)
        {
            if (EnsureOpenDb() == false) return;

            if (_mDbType == EnumDbTypes.SQLServer)
            {
                //todo insert a table with version
                using (var cmdCheck = _mDbConn.CreateCommand())
                {
                    cmdCheck.CommandType = CommandType.Text;
                    _mDbType = EnumDbTypes.SQLServer;
                    var backupdb = _mDbConn.Database;
                    _mDbConn.ChangeDatabase("master");
                    cmdCheck.CommandText = string.Format("CREATE DATABASE {0}", dbName);
                    cmdCheck.CommandType = CommandType.Text;
                    cmdCheck.ExecuteNonQuery();
                    //change context back
                    if (backupdb != dbName)
                        _mDbConn.ChangeDatabase(dbName);
                }
            }

        }
        private bool EnsureOpenDb()
        {
            if (_mDbConn.State != ConnectionState.Open)
            {

                try
                {
                    if (DatabaseExists(_mDbConn.Database))
                        _mDbConn.Open();
                    else
                    {
                        //connect to the DB using the master catalog
                        // otherwise setting the initial catalog would throw exceptions
                        ParseConnectionString(_mDbConn.ConnectionString);
                        //database=initial catalog (oledb vs .NET syntax)

                        m_dict.Remove("Database");
                        m_dict.Remove("Initial Catalog");
                        _mDbConn.ConnectionString = ConnectionStringFromProps(m_dict, false);
                        _mDbConn.Open();
                        m_dict["Initial Catalog"] = _mDatabase;
                    }
                    return true;
                }
                catch
                {
                    return false;
                }

            }
            return true;
        }
        public bool DatabaseExists(string dbName)
        {
            if (_mDbConn.State != ConnectionState.Open)
            {
                try
                {
                    using (var sqlConn = new SqlConnection(ConnectionStringMaster(_mDbConn.ConnectionString)))
                    {
                        sqlConn.Open();
                        using (var cmd = sqlConn.CreateCommand())
                        {
                            cmd.CommandText = string.Format("SELECT name FROM sys.SYSDATABASES WHERE name=N'{0}'", dbName);
                            cmd.CommandType = CommandType.Text;
                            object testdb = cmd.ExecuteScalar();
                            return testdb != null;
                        }

                    }
                }
                catch (SqlException ex)
                {
                    if (ex.Message.Contains("login failed"))
                    {
                        return true;
                    }
                    return false;
                }
            }

            if (_mDbType == EnumDbTypes.SQLServer)
            {
                using (var cmdCheck = _mDbConn.CreateCommand())
                {
                    cmdCheck.CommandType = System.Data.CommandType.Text;

                    cmdCheck.CommandText = "SELECT name FROM master..sysdatabases WHERE name=@name";
                    var par = cmdCheck.CreateParameter();
                    par.DbType = System.Data.DbType.String;
                    par.Size = 256; //sysname
                    par.Value = dbName;
                    par.ParameterName = "name";
                    cmdCheck.Parameters.Add(par);
                    object dbDatabaseName = cmdCheck.ExecuteScalar();
                    return dbDatabaseName != DBNull.Value;
                }
            }
            return false;
        }


        public void RunDbCreationScript(StreamReader file, IEnumerable<FindReplaceVar> findReplaceVars)
        {
            string script = file.ReadToEnd();
            RunDbCreationScript(script, findReplaceVars);

        }
        public void RunDbCreationScript(string script, IEnumerable<FindReplaceVar> findReplaceVars)
        {
            if (EnsureOpenDb() == false) return;


            using (var db = _mDbConn.CreateCommand())
            {
                //make sure all prespacing is removed so we can detect 'GO' or 'go'
             
                var sb = new StringBuilder(script);
                if (findReplaceVars != null)
                {
                    foreach (var var in findReplaceVars.Where(var => script.IndexOf(var.Find, StringComparison.OrdinalIgnoreCase) >= 0))
                    {
                        sb.Replace(var.Find, var.ToString());
                    }
                }
                
                var lines = sb.ToString().Split(new [] { "GO" }, StringSplitOptions.RemoveEmptyEntries);

                //  string[] batches = Array.FindAll(lines, f => !f.StartsWith("GO", StringComparison.OrdinalIgnoreCase) && !string.IsNullOrEmpty(f)); // lines.Split(new string[] { "GO" }, StringSplitOptions.RemoveEmptyEntries);
                int total = lines.Length;
                int current = 0;

                foreach (var batch in lines)
                {
                    current++;
                    if (StateCallBack != null)
                    {
                        StateCallBack(string.Format("line {0} of {1}, script {2}", current, total, batch),  (int)(((current/(float)total)*100F)));
                    }
                    
                    Trace.TraceInformation("Executing sql: {0}", batch);
                    db.CommandText = batch;
                    db.CommandType = CommandType.Text;
                    try
                    {
                        db.ExecuteNonQuery();
                    }
                    catch (SqlException ex)
                    {
                        if (ex.Message.Contains("does not exist"))
                        {
                            this.CreateDatabase(_mDatabase);
                            db.ExecuteNonQuery();
                        }
                        else
                        {
                            Trace.TraceError("Failure during execution sql: {0}", batch);
                            throw;
                        }
                    }
                }
            }
        }
        /// <summary>
        /// tests using connection string if the connection can work
        /// </summary>
        /// <param name="p"></param>
        /// <param name="DbType"></param>
        /// <param name="testServerConnectOnly">if true, will connect using 'master'</param>
        /// <returns></returns>
        public static bool CanConnect(DbInstallType bbType, bool testServerConnectOnly)
        {
            var connectionString = bbType.ConnectionString;
            var providerName = bbType.ProviderName;
            if (bbType.IncludeProviderPrefix == false)
            {
                connectionString = ConnectionStringFromProps(ConnectionStringToProps(connectionString),
                    false);
            }
            //todo: providername can differ if IncludeProviderPrefix == true
            string p = connectionString;
            if (testServerConnectOnly)
            {
                p = ConnectionStringMaster(connectionString);
            }
            p += ";connection timeout =2";
            if (bbType.DbType == EnumDbTypes.OleDb)
            {
                //MessageBox.Show(p);
                Trace.TraceInformation(p);
                using (var conn = new OleDbConnection(p))
                {

                    conn.Open();
                    return true;
                }
            }
            else if (bbType.DbType == EnumDbTypes.SQLServer)
            {
                try
                {
                
                    //MessageBox.Show(p);
                    Trace.TraceInformation(p);
                    using (var conn = new SqlConnection(p))
                    {

                        conn.Open();
                        return true;
                    }
                }
                catch(Exception ex)
                {
                    Trace.TraceError("within DbCreator {0}", ex.Message);
                    return false;
                }

            }
            return false;
        }

        /// <summary>
        /// returns true if database config was changed and needs to be saved
        /// </summary>
        /// <param name="db"></param>
        /// <param name="dbt"></param>
        /// <returns></returns>
        public static void CreateDB(Database db, EnumDbTypes dbt)
        {
            var dbType = db.DbTypes.Find(d => d.DbType == dbt);
            var connString = dbType.ConnectionString;
            var sqlScript = dbType.DbScript;
            var props = ConnectionStringToProps(connString);
            string userCheck;
            if (db.PreferIntegratedSecurity)
            {
                userCheck = NtSecurityInterop.UserAccountFormat(db.EnsuredDBUserAccount);
                bool blah;
                if (!NtSecurityInterop.UserExists(userCheck, out blah))
                {
                    NtSecurityInterop.AddUser(userCheck, DbCreator.GenerateRandomPass(true), string.Format("account for {0}", db.Name), blah);
                }
            }
            else
            {
                userCheck = db.EnsuredDBUserAccount;
            }
                //ADMIN???
          
            var dbc = new DbCreator(ConnectionFactory.DbConnection(dbt, connString));
            if (!dbc.DatabaseExists(db.Name))
            {
                dbc.CreateDatabase(db.Name);
                dbc._mDbConn.ChangeDatabase(db.Name);
                string password = "";
                if (!string.IsNullOrEmpty(db.EnsuredDBUserAccount))
                {
                    if (db.GenerateRandomPass)
                    {
                        db.Password =
                        password = GenerateRandomPass(true);
                        if (!db.PreferIntegratedSecurity)
                            props["password"] = password;

                    }
                    dbc.EnsureUserInDatabase(userCheck, password, db.PreferIntegratedSecurity);
                }

                dbc.EnsureUserInDBRole(userCheck, db.EnsuredDBRole);
                dbc.RunDbCreationScript(sqlScript, db.ReplaceVariables);

                props["Initial Catalog"] = db.Name;
                if (!db.PreferIntegratedSecurity)
                {
                    props["Integrated Security"] = "False";
                    props["User ID"] = db.EnsuredDBUserAccount;
                }
                else
                {
                    props["Integrated Security"] = "True"; //SSPI
                }
                dbType.ConnectionString = ConnectionStringFromProps(props, false);
            }

        }

        public static void SetRandomPassInCache(string p)
        {
            _gPasswordCache = p;
        }
    }
}