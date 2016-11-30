using System;
using System.Collections.Generic;
using System.Data.SqlClient;
using System.IO;
using System.Windows;
using ADCCure.Configurator.DAL;
using ADCCure.ISPSession.setup.Properties;
using ADCCure.ISPSession.setup.util;
using ADCCure.ISPSession.setup.ViewModels;

namespace ADCCure.ISPSession.setup.Forms
{

    /// <summary>
    /// Interaction logic for FormDbCreate.xaml
    /// </summary>
    public partial class FormDbCreate 
    {
       
        public FormDbCreate()
        {
            InitializeComponent();
            DatabaseName = "ISPSESSION";
            ServerName = "(local)";
        }
        private string _connectionString;
        private string ConnectionString
        {
            get { return string.IsNullOrEmpty(_connectionString) ? string.Format("Data Source={0};Integrated Security=true;Initial Catalog={1}", ServerName, DatabaseName) : _connectionString; }
            set { _connectionString = value; }
        }
        private SqlConnection GetConnection()
        {
            return new SqlConnection(ConnectionString);
        }

        /// <summary>
        /// funky
        /// </summary>
        public string DatabaseName { get { return TxtDbName.Text; } set { TxtDbName.Text = value; } }

        public string ServerName { get { return TxtServerName.Text; } set { TxtServerName.Text = value; } }

        public bool IsValidated
        {
            get
            {
                return !(string.IsNullOrEmpty(ServerName) && string.IsNullOrEmpty(DatabaseName));
            }
        }
        private void frmConnect_ConnectionSuccessEvent(string connection, string dbName, bool save)
        {
            if (save)
            {
                ConnectionString = connection;
                var cn = GetConnection();
                ServerName = cn.DataSource;
            }

        }
        private void ButtonCreate_Click(object sender, System.Windows.RoutedEventArgs e)
        {
            var creator = new DbCreator(GetConnection());
            creator.StateCallBack += (status, perc) =>
            {
                StatusText.Text = status;
                ProgressBar.Value = perc;
            };
            if (!creator.DatabaseExists(DatabaseName))
            {
                try
                {

                    StatusText.Text = string.Format("Creating database {0}", DatabaseName);

                    creator.CreateDatabase(DatabaseName);
                    StatusText.Text = string.Format("Creating user {0}", "aspsession");
                    creator.EnsureUserInDatabase("aspsession", DbCreator.GenerateRandomPass(true), false);
                    Clipboard.SetText(DbCreator.GenerateRandomPass(true));
                    MessageBox.Show(
                        string.Format(
                            "Your password for ISP Session is: ({0}) (without the parenthesises!). This password IS copied to your cliboard. Open your favorite TextEditor and press Ctrl+V to see it.",
                            DbCreator.GenerateRandomPass(true)), AppInfo.AssemblyTitle);
                    StatusText.Text = string.Format("Creating db role {0}", "visitor");
                    creator.EnsureUserInDBRole("aspsession", "visitor");
                    var findReplaces = new List<FindReplaceVar>();

                    using (
                        var file = File.OpenText(Path.Combine(AppInfo.CurrentPath, (ChkNoTempDb.IsChecked.Value ? "Resources\\CreateStateScript_NOTEMP.sql" : "Resources\\CreateStateScript.sql"))))
                    {
                        creator.RunDbCreationScript(file, findReplaces);
                    }
                }
                catch (Exception ex)
                {
                    MessageBox.Show(string.Format("During creating database {0} the following error occurred {1}",DatabaseName, ex.Message),  AppInfo.AssemblyTitle);
                }
            }
            
            ButtonCreate.Content = string.Format("Database {0} succesfully created. Click to close", DatabaseName);
            Settings.Default.DBCreated = DatabaseName;
            Settings.Default.Save();
            ButtonCreate.Click -= ButtonCreate_Click;
            ButtonCreate.Click += (o, args) => 
            {
                DialogResult = true;
                Close();
            };
        }

        public string StatusMessage { get; set; }

        private void ButtonSpecify_Click(object sender, RoutedEventArgs e)
        {
            var frmConnect = new DbConnect();
            frmConnect.ConnectionSuccessEvent += frmConnect_ConnectionSuccessEvent;

            frmConnect.ConnectionString = this.ConnectionString;
            frmConnect.Database = "master";
            frmConnect.ShowDialog();
        }
      
    }
}
