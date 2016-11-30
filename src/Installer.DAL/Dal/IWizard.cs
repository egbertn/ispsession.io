using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace ADCCure.Configurator.Wizard
{
    [Flags]
    public enum ButtonsEnabled
    {
        BackButtonEnabled = 1,
        NextButtonEnabled = 2,
        FinishButtonEnabled = 4,
        CancelButtonEnabled = 8

    }
    public enum EnumLoadResult
    {
        None = 0,
        Skip = 1,
        MovePrevious = 2
    }
    /// <summary>
    /// 
    /// </summary>
    public interface IWizard
    {
        EnumLoadResult OnLoad();
        
        void OnPrevious();
        void OnNext();
        /// <summary>
        /// PropertyBag for setup properties to be persisted
        /// </summary>
        Dictionary<string, object> Properties { get; set; }
   
        /// <summary>
        /// from this step a license is required
        /// </summary>
        bool RequiresLicense { get; }
        /// <summary>
        ///  will be set if the license was ok
        /// </summary>
        bool LicenseSet { get; set; }

        /// <summary>
        /// loads this control into wizard
        /// </summary>

        /// <summary>
        /// 
        /// </summary>
        ButtonsEnabled ButtonsEnabled { get; }
        /// <summary>
        /// validates input before we move on
        /// </summary>
        bool IsValid { get; }
    }
}
