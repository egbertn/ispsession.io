using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace ADCCure.Configurator.DAL.Forms
{
    public enum PasswordChangeMode
    {
        Existing,
        NewPassword,
        None
    }
    public partial class AskPassword : Form
    {
        public PasswordChangeMode PasswordChangeMode { private set; get; }
        public string NewPassword
        {
            get { return txtNewPassword1.Text; }
        }
        public string ExistingPassword
        {
            get { return txtPassword.Text; }
        }
        public AskPassword()
        {
            InitializeComponent();
        }

        private void btnApply_Click(object sender, EventArgs e)
        {
            if (string.IsNullOrEmpty(txtNewPassword1.Text) && string.IsNullOrEmpty(txtPassword.Text))
            {
                PasswordChangeMode = PasswordChangeMode.None;
            }
            else if (string.IsNullOrEmpty(txtPassword.Text) && !string.IsNullOrEmpty(txtNewPassword1.Text))
            {
                if (txtNewPassword1.Text != txtNewPassword2.Text)
                {
                    MessageBox.Show("Passwords differ");
                    return;
                }
                else
                {
                    PasswordChangeMode = PasswordChangeMode.NewPassword;
                }
            }
            else
            {

            }
            this.DialogResult = DialogResult.OK;
            this.Close();
        }

        private void btnCancel_Click(object sender, EventArgs e)
        {
            this.DialogResult = DialogResult.Cancel;
            this.Close();
        }

       
    }
}
