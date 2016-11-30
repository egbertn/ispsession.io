namespace ADCCure.Configurator.DAL
{
    partial class DbConnect
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.cmbServers = new System.Windows.Forms.ComboBox();
            this.label1 = new System.Windows.Forms.Label();
            this.btnRefresh = new System.Windows.Forms.Button();
            this.btnConnection = new System.Windows.Forms.Button();
            this.btnCancel = new System.Windows.Forms.Button();
            this.btnOK = new System.Windows.Forms.Button();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.btnSQLAuth = new System.Windows.Forms.RadioButton();
            this.btnWindowsAuth = new System.Windows.Forms.RadioButton();
            this.label4 = new System.Windows.Forms.Label();
            this.lblUserName = new System.Windows.Forms.Label();
            this.txtPassword = new System.Windows.Forms.TextBox();
            this.txtUserId = new System.Windows.Forms.TextBox();
            this.grpConnectDb = new System.Windows.Forms.GroupBox();
            this.lblSelectOrEnter = new System.Windows.Forms.Label();
            this.cmbDatabases = new System.Windows.Forms.ComboBox();
            this.grpProviders = new System.Windows.Forms.GroupBox();
            this.cmbProviders = new System.Windows.Forms.ComboBox();
            this.groupBox1.SuspendLayout();
            this.grpConnectDb.SuspendLayout();
            this.grpProviders.SuspendLayout();
            this.SuspendLayout();
            // 
            // cmbServers
            // 
            this.cmbServers.DisplayMember = "Name";
            this.cmbServers.FormattingEnabled = true;
            this.cmbServers.Location = new System.Drawing.Point(12, 27);
            this.cmbServers.Name = "cmbServers";
            this.cmbServers.Size = new System.Drawing.Size(242, 21);
            this.cmbServers.TabIndex = 0;
            this.cmbServers.SelectedIndexChanged += new System.EventHandler(this.cmbServers_SelectedIndexChanged);
            this.cmbServers.TextChanged += new System.EventHandler(this.cmbServers_TextChanged);
            this.cmbServers.MouseClick += new System.Windows.Forms.MouseEventHandler(this.cmbServers_MouseClick);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(12, 8);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(69, 13);
            this.label1.TabIndex = 2;
            this.label1.Text = "S&erver Name";
            // 
            // btnRefresh
            // 
            this.btnRefresh.Location = new System.Drawing.Point(261, 27);
            this.btnRefresh.Name = "btnRefresh";
            this.btnRefresh.Size = new System.Drawing.Size(75, 23);
            this.btnRefresh.TabIndex = 1;
            this.btnRefresh.Text = "&Refresh";
            this.btnRefresh.UseVisualStyleBackColor = true;
            this.btnRefresh.Click += new System.EventHandler(this.btnRefresh_Click);
            // 
            // btnConnection
            // 
            this.btnConnection.Location = new System.Drawing.Point(13, 372);
            this.btnConnection.Name = "btnConnection";
            this.btnConnection.Size = new System.Drawing.Size(109, 23);
            this.btnConnection.TabIndex = 5;
            this.btnConnection.Text = "&Test Connection";
            this.btnConnection.UseVisualStyleBackColor = true;
            this.btnConnection.Click += new System.EventHandler(this.btnConnection_Click);
            // 
            // btnCancel
            // 
            this.btnCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.btnCancel.Location = new System.Drawing.Point(253, 372);
            this.btnCancel.Name = "btnCancel";
            this.btnCancel.Size = new System.Drawing.Size(75, 23);
            this.btnCancel.TabIndex = 7;
            this.btnCancel.Text = "Cancel";
            this.btnCancel.UseVisualStyleBackColor = true;
            this.btnCancel.Click += new System.EventHandler(this.btnCancel_Click);
            // 
            // btnOK
            // 
            this.btnOK.Location = new System.Drawing.Point(172, 372);
            this.btnOK.Name = "btnOK";
            this.btnOK.Size = new System.Drawing.Size(75, 23);
            this.btnOK.TabIndex = 6;
            this.btnOK.Text = "OK";
            this.btnOK.UseVisualStyleBackColor = true;
            this.btnOK.Click += new System.EventHandler(this.btnOK_Click);
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.btnSQLAuth);
            this.groupBox1.Controls.Add(this.btnWindowsAuth);
            this.groupBox1.Controls.Add(this.label4);
            this.groupBox1.Controls.Add(this.lblUserName);
            this.groupBox1.Controls.Add(this.txtPassword);
            this.groupBox1.Controls.Add(this.txtUserId);
            this.groupBox1.Location = new System.Drawing.Point(13, 56);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(323, 142);
            this.groupBox1.TabIndex = 2;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "Logon to the server:";
            // 
            // btnSQLAuth
            // 
            this.btnSQLAuth.AutoSize = true;
            this.btnSQLAuth.Location = new System.Drawing.Point(6, 42);
            this.btnSQLAuth.Name = "btnSQLAuth";
            this.btnSQLAuth.Size = new System.Drawing.Size(173, 17);
            this.btnSQLAuth.TabIndex = 1;
            this.btnSQLAuth.TabStop = true;
            this.btnSQLAuth.Text = "Use S&QL Server Authentication";
            this.btnSQLAuth.UseVisualStyleBackColor = true;
            this.btnSQLAuth.CheckedChanged += new System.EventHandler(this.btnSQLAuth_CheckedChanged);
            // 
            // btnWindowsAuth
            // 
            this.btnWindowsAuth.AutoSize = true;
            this.btnWindowsAuth.Location = new System.Drawing.Point(6, 19);
            this.btnWindowsAuth.Name = "btnWindowsAuth";
            this.btnWindowsAuth.Size = new System.Drawing.Size(162, 17);
            this.btnWindowsAuth.TabIndex = 0;
            this.btnWindowsAuth.TabStop = true;
            this.btnWindowsAuth.Text = "Use &Windows Authentication";
            this.btnWindowsAuth.UseVisualStyleBackColor = true;
            this.btnWindowsAuth.CheckedChanged += new System.EventHandler(this.btnWindowsAuth_CheckedChanged);
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(26, 95);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(56, 13);
            this.label4.TabIndex = 8;
            this.label4.Text = "&Password:";
            // 
            // lblUserName
            // 
            this.lblUserName.AutoSize = true;
            this.lblUserName.Location = new System.Drawing.Point(21, 68);
            this.lblUserName.Name = "lblUserName";
            this.lblUserName.Size = new System.Drawing.Size(61, 13);
            this.lblUserName.TabIndex = 7;
            this.lblUserName.Text = "&User name:";
            // 
            // txtPassword
            // 
            this.txtPassword.Location = new System.Drawing.Point(85, 92);
            this.txtPassword.Name = "txtPassword";
            this.txtPassword.PasswordChar = '*';
            this.txtPassword.Size = new System.Drawing.Size(226, 20);
            this.txtPassword.TabIndex = 3;
            this.txtPassword.UseSystemPasswordChar = true;
            this.txtPassword.TextChanged += new System.EventHandler(this.txtPassword_TextChanged);
            // 
            // txtUserId
            // 
            this.txtUserId.Location = new System.Drawing.Point(85, 65);
            this.txtUserId.Name = "txtUserId";
            this.txtUserId.Size = new System.Drawing.Size(226, 20);
            this.txtUserId.TabIndex = 2;
            this.txtUserId.TextChanged += new System.EventHandler(this.txtUserId_TextChanged);
            // 
            // grpConnectDb
            // 
            this.grpConnectDb.Controls.Add(this.lblSelectOrEnter);
            this.grpConnectDb.Controls.Add(this.cmbDatabases);
            this.grpConnectDb.Location = new System.Drawing.Point(12, 204);
            this.grpConnectDb.Name = "grpConnectDb";
            this.grpConnectDb.Size = new System.Drawing.Size(324, 93);
            this.grpConnectDb.TabIndex = 3;
            this.grpConnectDb.TabStop = false;
            this.grpConnectDb.Text = "Connect to a Database:";
            // 
            // lblSelectOrEnter
            // 
            this.lblSelectOrEnter.AutoSize = true;
            this.lblSelectOrEnter.Location = new System.Drawing.Point(7, 20);
            this.lblSelectOrEnter.Name = "lblSelectOrEnter";
            this.lblSelectOrEnter.Size = new System.Drawing.Size(135, 13);
            this.lblSelectOrEnter.TabIndex = 1;
            this.lblSelectOrEnter.Text = "Select or enter a database:";
            // 
            // cmbDatabases
            // 
            this.cmbDatabases.FormattingEnabled = true;
            this.cmbDatabases.Location = new System.Drawing.Point(7, 39);
            this.cmbDatabases.Name = "cmbDatabases";
            this.cmbDatabases.Size = new System.Drawing.Size(305, 21);
            this.cmbDatabases.TabIndex = 0;
            this.cmbDatabases.DropDown += new System.EventHandler(this.cmbDatabases_DropDown);
            this.cmbDatabases.SelectedIndexChanged += new System.EventHandler(this.cmbDatabases_SelectedIndexChanged);
            // 
            // grpProviders
            // 
            this.grpProviders.Controls.Add(this.cmbProviders);
            this.grpProviders.Location = new System.Drawing.Point(12, 304);
            this.grpProviders.Name = "grpProviders";
            this.grpProviders.Size = new System.Drawing.Size(312, 62);
            this.grpProviders.TabIndex = 4;
            this.grpProviders.TabStop = false;
            this.grpProviders.Text = "Providers";
            // 
            // cmbProviders
            // 
            this.cmbProviders.FormattingEnabled = true;
            this.cmbProviders.Location = new System.Drawing.Point(10, 19);
            this.cmbProviders.Name = "cmbProviders";
            this.cmbProviders.Size = new System.Drawing.Size(296, 21);
            this.cmbProviders.TabIndex = 0;
            this.cmbProviders.SelectedIndexChanged += new System.EventHandler(this.cmbProviders_SelectedIndexChanged);
            // 
            // DbConnect
            // 
            this.AcceptButton = this.btnOK;
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.CancelButton = this.btnCancel;
            this.ClientSize = new System.Drawing.Size(341, 407);
            this.Controls.Add(this.grpProviders);
            this.Controls.Add(this.grpConnectDb);
            this.Controls.Add(this.groupBox1);
            this.Controls.Add(this.btnOK);
            this.Controls.Add(this.btnCancel);
            this.Controls.Add(this.btnConnection);
            this.Controls.Add(this.btnRefresh);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.cmbServers);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedToolWindow;
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "DbConnect";
            this.ShowIcon = false;
            this.ShowInTaskbar = false;
            this.Text = "Connect to SQL Server";
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            this.grpConnectDb.ResumeLayout(false);
            this.grpConnectDb.PerformLayout();
            this.grpProviders.ResumeLayout(false);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.ComboBox cmbServers;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Button btnRefresh;
        private System.Windows.Forms.Button btnConnection;
        private System.Windows.Forms.Button btnCancel;
        private System.Windows.Forms.Button btnOK;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.Label lblUserName;
        private System.Windows.Forms.TextBox txtPassword;
        private System.Windows.Forms.TextBox txtUserId;
        private System.Windows.Forms.RadioButton btnSQLAuth;
        private System.Windows.Forms.RadioButton btnWindowsAuth;
        private System.Windows.Forms.GroupBox grpConnectDb;
        private System.Windows.Forms.Label lblSelectOrEnter;
        private System.Windows.Forms.ComboBox cmbDatabases;
        private System.Windows.Forms.GroupBox grpProviders;
        private System.Windows.Forms.ComboBox cmbProviders;
    }
}