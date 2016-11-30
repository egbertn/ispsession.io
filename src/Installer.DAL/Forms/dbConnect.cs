using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Data;
using System.Data.OleDb;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Data.Sql;
using System.Data.SqlClient;
using System.Windows.Threading;
using ADCCure.Configurator.DAL.Interop;


namespace ADCCure.Configurator.DAL
{

  
    public partial class DbConnect : Form
    {
        private const string FormCaption = Version.AssemblyCompany + " DB Connector";
        public DbConnect()
        {
            InitializeComponent();
            ServerEnum.OnNewServer += EnumServers;
            cmbServers.DisplayMember = "Name";
            Load += (sender, args) => Dispatcher.CurrentDispatcher.BeginInvoke(new Action<NetServerTypeEnum>(ServerEnum.EnumServers),
                NetServerTypeEnum.SV_TYPE_SQLSERVER);
            
            FillOledbProviders();

            _dict = new Dictionary<string, string>(StringComparer.InvariantCultureIgnoreCase);
        }
        private readonly Dictionary<string, string> _dict;

        private void FillOledbProviders()
        {
            var oledbEnum = OleDbEnumerator.GetRootEnumerator();
            int ord = oledbEnum.GetOrdinal("SOURCES_NAME");
            int val = oledbEnum.GetOrdinal("SOURCES_DESCRIPTION");
            int type = oledbEnum.GetOrdinal("SOURCES_TYPE");
            cmbProviders.DisplayMember = "Item2";
            cmbProviders.ValueMember = "Item1";
            var lst = new List<Tuple<string, string>>();
            while (oledbEnum.Read())
            {
                var vtype = oledbEnum.GetInt32(type);
                if (vtype != 1) continue;
                var tp = new Tuple<string, string>(oledbEnum.GetString(ord), oledbEnum.GetString(val));
                if (lst.Find(f => f.Item1 == tp.Item1) == null)
                    lst.Add(tp);
            } oledbEnum.Close();
            cmbProviders.DataSource = lst;
        }
        public string ConnectionString
        {
            private get
            {
                return string.Join(";", _dict.Select(p => string.Format("{0}={1}", p.Key, p.Value))).TrimEnd(';');
            }
            set
            {
                _dict.Clear();
                if (string.IsNullOrEmpty(value))
                {
                    return;
                }
                var parts = value.Split(';');
                foreach (string s in parts)
                {
                    var parts2 = s.Split('=');
                    if (parts2.Length == 2 && parts2[0].Trim().Length > 0)
                    {
                        _dict[parts2[0].Trim()] = parts2[1].Trim();
                    }
                }
                if (_dict.ContainsKey("provider"))
                {
                    var val = _dict["provider"];
                    grpProviders.Visible = true;
                    val = Path.GetFileNameWithoutExtension(val); //remove the .1 version from progid
                    cmbProviders.SelectedValue = val;
                }
                else
                {
                    grpProviders.Visible = false;
                }
                var temp = _dict.ContainsKey("data source") ? _dict["data source"] : string.Empty;
                var colon = temp.IndexOf(':');
                if (colon > 0)
                {
                    temp = temp.Substring(colon + 1);
                }
                cmbServers.Text = temp;
                if (_dict.ContainsKey("Integrated Security") )
                {
                    btnSQLAuth.Checked = string.Compare(_dict["Integrated Security"], "true", StringComparison.InvariantCultureIgnoreCase) != 0;
                    EnableDisableUidpw(btnSQLAuth.Checked);
                }
                else
                {
                    btnSQLAuth.Checked = _dict.ContainsKey("User ID");
                    EnableDisableUidpw(btnSQLAuth.Checked);
                    //btnWindowsAuth.Checked = true;
                }
                if (btnSQLAuth.Checked)
                {
                    _dict.Remove("Integrated Security");
                }
                btnWindowsAuth.Checked = !btnSQLAuth.Checked;
                txtPassword.Text = _dict.ContainsKey("Password") ? _dict["Password"] : string.Empty;
                txtUserId.Text = _dict.ContainsKey("User ID") ? _dict["User ID"] : string.Empty;
                Database = cmbDatabases.Text = _dict.ContainsKey("Initial Catalog") ? _dict["Initial Catalog"] : string.Empty;
                
            }
        }
        [DefaultValue("master")]
        public string Database
        {
            private get 
            {
                return string.IsNullOrEmpty(cmbDatabases.Text) ? "master" : cmbDatabases.Text; 
            }
            set 
            { 

                cmbDatabases.Text =  value;
                lblSelectOrEnter.Visible =
                    cmbDatabases.Visible= (StringComparer.CurrentCultureIgnoreCase.Compare(value, "master") != 0);
            }
        }
        public delegate void ConnectionSuccess(string connection, string dbName, bool save);
        public event ConnectionSuccess ConnectionSuccessEvent;
        private void btnRefresh_Click(object sender, EventArgs e)
        {
            Dispatcher.CurrentDispatcher.BeginInvoke(new System.Action<NetServerTypeEnum>(ServerEnum.EnumServers),
                 NetServerTypeEnum.SV_TYPE_SQLSERVER);
        }

        private void EnumServers(ServerArgs args2)
        {
            // this method is a lot slower
            //var table = SqlDataSourceEnumerator.Instance.GetDataSources();
            //cmbServers.DisplayMember = "srv";
            //table.Columns[0].ColumnName = "Name";
            //cmbServers.DataSource = table;
            cmbServers.DisplayMember = null;
            cmbServers.Items.Clear();
            cmbServers.DisplayMember = "Name";
            cmbServers.Items.AddRange(args2.Servers.Cast<object>().ToArray());
        }

        private void PropertyLogic()
        {
            

            if (btnSQLAuth.Checked)
            {
                _dict["User ID"] = txtUserId.Text;
                _dict["Password"] = txtPassword.Text;
                _dict.Remove("Integrated Security");
            }
            if (btnWindowsAuth.Checked)
            {
                _dict.Remove("User ID");
                _dict.Remove("Password");
                _dict["Integrated Security"] = "True"; //not necessary
            }
            string server = cmbServers.Text;
            //tcp: and np: were removed
            if (cmbProviders.Visible )
                _dict["Provider"] = (string)cmbProviders.SelectedValue;

            _dict["Data Source"] = (btnSQLAuth.Checked ? "" : (server.StartsWith("(local") ? "": "")) + server;
            _dict["Persist Security Info"] = "True";
            _dict["Initial Catalog"] = string.IsNullOrEmpty(Database) ? "master" : Database;
            //MessageBox.Show(cn);
           
        }
        private void btnConnection_Click(object sender, EventArgs e)
        {
            PropertyLogic();
            if (DbCreator.CanConnect(new DbInstallType() { 
                ConnectionString = ConnectionString,
                DbType = _dict.ContainsKey("provider") ? EnumDbTypes.OleDb : EnumDbTypes.SQLServer,
                IncludeProviderPrefix = _dict.ContainsKey("provider")
                }, 
                !this.ShowDatabaseConnectGroup))
            {

                MessageBox.Show("Test connection succeeded.", FormCaption, MessageBoxButtons.OK, MessageBoxIcon.Information);
                if (ConnectionSuccessEvent != null)
                {
                    _dict["Initial Catalog"] = Database;
                    ConnectionSuccessEvent(ConnectionString, Database, false);
                }
            }
            else
            {
                MessageBox.Show("Cannot connect. Test failed", FormCaption, MessageBoxButtons.OK, MessageBoxIcon.Information);
            }
       
        }

        private void btnOK_Click(object sender, EventArgs e)
        {
            if (ConnectionSuccessEvent != null)
            {
                PropertyLogic();
                
                ConnectionSuccessEvent(ConnectionString, Database, true);
            }
            this.DialogResult = DialogResult.OK;
            this.Close();
        }
        public bool ShowDatabaseConnectGroup
        {
            get { return grpConnectDb.Visible; }
            set { grpConnectDb.Visible = value; }
        }
        private void btnCancel_Click(object sender, EventArgs e)
        {            
            DialogResult = DialogResult.Cancel;
            Close();
        }

        private void btnSQLAuth_CheckedChanged(object sender, EventArgs e)
        {
            EnableDisableUidpw(btnSQLAuth.Checked); 
        }

        private void btnWindowsAuth_CheckedChanged(object sender, EventArgs e)
        {
            EnableDisableUidpw(btnSQLAuth.Checked);
        }
        private void EnableDisableUidpw(bool enable)
        {
            txtPassword.Enabled = txtUserId.Enabled = enable;
            if (btnSQLAuth.Checked)
                _dict["Integrated Security"] =  btnSQLAuth.Checked ? "false" : "true";

        }

        private void cmbDatabases_SelectedIndexChanged(object sender, EventArgs e)
        {
           this._dict["Initial Catalog"] =  this.Database = cmbDatabases.Text;
        }

        private void cmbProviders_SelectedIndexChanged(object sender, EventArgs e)
        {
            
            if (_dict != null)
                this._dict["Provider"] = (string)cmbProviders.SelectedValue;
        }
        private void cmbDatabases_DropDown(object sender, EventArgs e)
        {
            using (var conn = new SqlConnection())
            {
                try
                {
                    PropertyLogic();
                    conn.ConnectionString = DbCreator.ConnectionStringFromProps( DbCreator.ConnectionStringToProps( ConnectionString), false);
                    conn.Open();
                }
                catch (Exception ex)
                {
                    MessageBox.Show(ex.Message, FormCaption);
                    return;
                }
                var dt = conn.GetSchema("Databases");
                cmbDatabases.Items.Clear();
                foreach (DataRow dr in dt.Rows)
                {
                    cmbDatabases.Items.Add(dr[0]);
                }
            }
        }

        private void cmbServers_TextChanged(object sender, EventArgs e)
        {
            if (_dict.ContainsKey("server"))
            {
                _dict["server"] = cmbServers.Text;
            }
            if (_dict.ContainsKey("data source"))
            {
                _dict["data source"] = cmbServers.Text;
            }
        }

        private void txtPassword_TextChanged(object sender, EventArgs e)
        {
            _dict["password"] = txtPassword.Text;

        }

        private void txtUserId_TextChanged(object sender, EventArgs e)
        {
            _dict["user id"] = txtUserId.Text;
        }

        private void cmbServers_SelectedIndexChanged(object sender, EventArgs e)
        {

        }

        private void cmbServers_MouseClick(object sender, MouseEventArgs e)
        {

            ServerEnum.OnNewServer += args =>
            {
                cmbServers.DataSource = null;
                cmbServers.Items.Clear();
                cmbServers.Items.AddRange(args.Servers.Cast<object>().ToArray());
            };
            ServerEnum.EnumServers(NetServerTypeEnum.SV_TYPE_SQLSERVER);
            
        }
    }
}