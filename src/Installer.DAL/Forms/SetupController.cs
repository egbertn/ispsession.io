using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using ADCCure.Configurator.DAL;
using System.Drawing;

namespace ADCCure.Configurator.Wizard
{
    public partial class SetupController: Form
    {
        public SetupController()
        {
             
            InitializeComponent();
        
        }
        private int m_maxSteps;
        private IWizard m_ifCurrentStep;
        private int m_CurrentStep;
        private bool blnFinishClicked;
        private string m_AppTitle;
        private string m_hrefLogo;
        List<ADCCure.Configurator.DAL.Control> m_WizardSteps;
        Dictionary<string, object> m_PropertyBag;
        public SetupController(ADCCure.Configurator.DAL.Wizard setupData) :
            this()
        {
            m_AppTitle = setupData.Title;
            m_WizardSteps = setupData.Control;
            m_hrefLogo = setupData.Logo.href;
        }
        protected override void OnLoad(EventArgs e)
        {
            base.OnLoad(e);
            m_maxSteps = m_WizardSteps.Count;
            m_PropertyBag = new Dictionary<string, object>();
            m_PropertyBag["AskVariables"] = new List<AskVariables>();
            m_PropertyBag["ProgramTitle"] = m_AppTitle;
            
            if (!string.IsNullOrEmpty(m_hrefLogo))
            {
                pictLogo.Image = Image.FromFile(m_hrefLogo);
            }
            foreach (var step in m_WizardSteps)
            {

                Type t = Type.GetType(step.Type);
                UserControl ctr = null;
                //try
                {
                    ctr = (UserControl)Activator.CreateInstance(t);
                }
                //catch (Exception ex)
                {
                    //  MessageBox.Show(ex.Message, "error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                    //Trace.Write(ex.Message);
                }
                if (!(ctr is IWizard))
                {
                    throw new Exception(step.Name + " must support interface IWizard");
                }
                var tmpIface = (IWizard)ctr;
                ctr.Visible = false;
                //lstWizSteps.Items.Add(new ListViewItem(tmpIface.Caption));
                panel1.Controls.Add(ctr);
                ControlController();   
            }   
        }
        private EnumLoadResult ControlController()
        {

            UserControl ctrlSelected = null;
            var cols = panel1.Controls;
            //var colList = lstWizSteps.Items;
            for (int cx = 0; cx < cols.Count; cx++)
            {
                cols[cx].Visible = false;

               // colList[cx].Selected = cx != m_CurrentStep;
                if (cx == m_CurrentStep)
                {
                    ctrlSelected = (UserControl)cols[cx];
                }
            }

            ctrlSelected.Dock = DockStyle.Fill;

            m_ifCurrentStep = (IWizard)cols[m_CurrentStep];
            btnPrevious.Enabled = (m_ifCurrentStep.ButtonsEnabled & ButtonsEnabled.BackButtonEnabled)
                == ButtonsEnabled.BackButtonEnabled;

            btnFinish.Enabled = (m_ifCurrentStep.ButtonsEnabled & ButtonsEnabled.FinishButtonEnabled)
                == ButtonsEnabled.FinishButtonEnabled;

            btnNext.Enabled = (m_ifCurrentStep.ButtonsEnabled & ButtonsEnabled.NextButtonEnabled)
                == ButtonsEnabled.NextButtonEnabled;

            //btnCancel.Enabled = (m_ifCurrentStep.ButtonsEnabled & ButtonsEnabled.CancelButtonEnabled) 
            //    == ButtonsEnabled.CancelButtonEnabled;

            ctrlSelected.Anchor = AnchorStyles.Left | AnchorStyles.Top;
            m_ifCurrentStep = (IWizard)ctrlSelected;
            txtExplanation.Text = m_WizardSteps[m_CurrentStep].Text.Value;
            m_ifCurrentStep.Properties = m_PropertyBag;

            EnumLoadResult result = m_ifCurrentStep.OnLoad();
          
            ctrlSelected.Visible = true;
            this.Text = string.Format("{0} - {1}", m_AppTitle, m_WizardSteps[m_CurrentStep].Name);
            if (blnFinishClicked)
            {
                this.DialogResult = DialogResult.OK;
                //SaveInput();
                Close();
            }
            return result;
        }

        private void btnPrevious_Click(object sender, EventArgs e)
        {

            if (this.m_ifCurrentStep != null && m_CurrentStep > 0)
            {
                    var ctrl = (UserControl)panel1.Controls[m_CurrentStep];
                    bool result = ctrl.Validate();
                    if (result)
                    {
                        ((IWizard)ctrl).OnPrevious();
                        // lstWizSteps.Items[m_CurrentStep].Focused = true;
                        //lstWizSteps.Items[m_currentStep] = true;
                        do{
                            m_CurrentStep--;
                            
                        } while (ControlController() == EnumLoadResult.Skip);
                    }
                
            }
        }

        private void btnFinish_Click(object sender, EventArgs e)
        {

            var ctrl = (UserControl)panel1.Controls[m_CurrentStep];
            IInstallTasks install = ctrl as IInstallTasks;
            if (install == null)
            {
                throw new Exception("interface IInstallTasks must be implemented on " + ctrl.GetType().Name);
            }
            this.ControlBox = 
            btnFinish.Enabled = 
            btnPrevious.Enabled = false;
            install.DoIt();
            btnFinish.Text ="Close";
            this.ControlBox = 
            btnFinish.Enabled = true;
            btnFinish.Click -= btnFinish_Click;
            btnFinish.Click += delegate(object sndr, EventArgs e2) { this.Close(); };
            

        }

        private void btnNext_Click(object sender, EventArgs e)
        {
            if (btnFinish == sender) blnFinishClicked = true;
            if (this.m_ifCurrentStep != null)
            {
                if (m_ifCurrentStep.RequiresLicense)
                {

                    return;
                }
                else
                {
                    var ctrl = (UserControl)panel1.Controls[m_CurrentStep];
                    IInstallTasks install = ctrl as IInstallTasks;
                    if (install != null)
                    {
                        btnNext.Enabled =
                        btnPrevious.Enabled = false;
                        install.DoIt();
                        btnNext.Enabled =
                       btnPrevious.Enabled = true;
                    }
                    bool result = (ctrl as IWizard).IsValid;
                    int maxStep = panel1.Controls.Count;
                    if (result)
                    {
                        ((IWizard)ctrl).OnNext();
                        // lstWizSteps.Items[m_CurrentStep].Focused = true;
                        //lstWizSteps.Items[m_currentStep] = true;
                        bool skipAction = false;
                        do
                        {
                            m_CurrentStep++;
                            // no work to do. 
                            skipAction = ControlController() == EnumLoadResult.Skip;
                            //avoid a widow screen, if there is not any database action
                            if (m_CurrentStep+1 >= maxStep && skipAction)
                            {
                                this.DialogResult = DialogResult.OK;
                                this.Close();
                            }
                        } while (skipAction);
                    }
                }
            }
        }

        private void WizardController_FormClosing(object sender, FormClosingEventArgs e)
        {
            
            this.DialogResult = DialogResult.Cancel;
        }

   
    }
}
