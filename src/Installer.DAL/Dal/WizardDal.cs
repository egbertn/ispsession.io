using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Reflection;
using System.Xml.Serialization;
using System.IO;
using System.ComponentModel;
using System.Data;
namespace ADCCure.Configurator.DAL
{
   
    /// <summary>
    /// your class must implement this iface
    /// </summary>
    public interface IInstallTasks
    {
        void DoIt();
    }
    [XmlType("logo")]
    public sealed class Logo
    {
        [XmlAttribute()]
        public string href { get; set; }
    }
    [XmlType(TypeName="text")]
    public sealed class Text
    {
        [XmlAttribute("langcode")]
        public int LangCode { get; set; }
        [XmlText()]
        public string Value { get; set; }
    }
    [XmlType(TypeName = "control")]
    public sealed class Control
    {
        public Control()
        {
            Text = new Text();
        }
        [XmlAttribute("name")]
        public string Name { get; set; }
        [XmlElement("text")]
        public Text Text { get; set; }
      
        [XmlAttribute("type")]
        public string Type { get; set; }

    }
    [XmlType(TypeName = "wizard")]
    public sealed class Wizard
    {
        public Wizard()
        {
            Control = new List<Control>();
            Logo = new Logo();
        }
        /// <summary>
        /// unique wizard id
        /// </summary>
        [XmlAttribute()]
        public string id { get; set; }
        [XmlElement("control")]
        public List<Control> Control { get; set; }
        /// <summary>
        /// contains the relative path + logo which the installer will display
        /// </summary>
        [XmlElement("logo")]
        public Logo Logo { get; set; }

        [XmlAttribute("title")]
        public string Title { get; set; }
    }
    [XmlType("licenseFile")]
    public sealed class LicenseFile
    {
        [XmlAttribute("file")]
        public string File { get; set; }
    }
    [XmlRoot("setup", Namespace = "http://addccure.nl/setup")]
    public sealed class SetupInstallables
    {
        List<Wizard> m_Wizards;

        public SetupInstallables()
        {
            m_Wizards = new List<Wizard>();
            LicenseFile = new LicenseFile();

        }
        /// <summary>
        /// the XML will be 'crc-d' to avoid changes after the compilation
        /// </summary>
        [XmlAttribute("crc")]
        public int Crc { get; set; }
        [XmlArray("wizards")]
        public List<Wizard> Wizards
        {
            get { return m_Wizards; }
            set { m_Wizards = value; }
        }
        [XmlElement("licenseFile")]
        public LicenseFile LicenseFile {get; set;}
        /// <summary>
        /// loads and creates instance of Setup
        /// </summary>
        /// <param name="path"></param>
        /// <returns></returns>
        public static SetupInstallables Load(string path)
        {
            XmlSerializer xl = new XmlSerializer(typeof(SetupInstallables));
            using (var str = File.OpenRead(path))
            {
                return (SetupInstallables)xl.Deserialize(str);
            }
        }
    }
}
