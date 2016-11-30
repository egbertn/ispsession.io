using System;
using System.Collections.Generic;
using System.Configuration;
using System.ServiceModel.Configuration;
using System.Xml;
using System.Web.Configuration;
using ADCCure.Configurator.DAL.Interop;

namespace ADCCure.Configurator.DAL
{
    public sealed class DbSelector
    {
        public string name { get; set; }
        /// <summary>
        /// only used for finding the correct mapping
        /// </summary>
        public string ConnectionString { get; set; }
        public int Ordinal { get; set; }
    }
    public sealed class OtherConfigException : Exception
    {
        public OtherConfigException()
        {
        }
        public OtherConfigException(string msg)
            : base(msg)
        {
        }
    }
    /// <summary>
    /// serves as a helper for applicationKey settings
    /// </summary>
    public sealed class AppKeys
    {
        public string Key { get; set; }
        public string Value { get; set; }
    }
    public class EndPointItem
    {
        public string ServiceName { get; set; }
        public Uri EndPointAddress { get; set; }
        public string Action { get { return "Modify"; } }
        public bool HasTLS { get; set; }
        public string BindingType { get; set; }
       // public string BindingConfiguration { get; set; }
        public string BehaviorConfiguration { get; set; }

    }
    public sealed class ClientItem: EndPointItem
    {
        public ChannelEndpointElement Endpoint {get;set;}

    }
    public sealed class ServiceItem : EndPointItem
    {
        public ServiceEndpointElement Endpoint { get; set; }
    }

    /// <summary>
    /// Opens the config file different to the current application
    /// </summary>
    public sealed class OtherConfig
    {
        private readonly Configuration m_StateConfig;
        private XmlDocument _mDoc;
        private readonly ClientSettingsSection m_Settings;
        private readonly SettingElementCollection m_SettingsCollection;

        private static IList<DbSelector> ConvertConnectionString(ConnectionStringSettingsCollection conn)
        {
            var retVal = new List<DbSelector>(conn.Count);
            foreach (ConnectionStringSettings v in conn)
            {
                
                //if it's null it's inherited?
                if (v.ElementInformation.Source != null)
                {
                    retVal.Add(new DbSelector() { name = v.Name, ConnectionString = v.ConnectionString });
                }
            }
            return retVal;
        }
        static IList<AppKeys> ConvertApplicationToList(KeyValueConfigurationCollection keyvalues)
        {
            var retval = new List<AppKeys>(keyvalues.Count);
            foreach (string keyEl in keyvalues.AllKeys)
            {
                if (keyvalues[keyEl].ElementInformation.Source != null)
                {
                    retval.Add(new AppKeys() { Key = keyEl, Value = keyvalues[keyEl].Value });
                }
            }
            return retval;
         }

        public void Set(string settingName, string value)
        {
            var xmlEl = _mDoc.CreateElement("value");
            xmlEl.InnerText = value;
            //assumes that the setting exists
            SettingElement elem = m_SettingsCollection.Get(settingName);
            elem.Value.ValueXml = xmlEl;
        }
        public object Get(string settingName)
        {
            var elem = m_Settings.Settings.Get(settingName);
            if (_mDoc == null && elem != null)
            {
                _mDoc = elem.Value.ValueXml.OwnerDocument;
            }
            if (elem.SerializeAs == SettingsSerializeAs.String)
            {
                return elem.Value.ValueXml.InnerText;
            }
            return null;
        }
        public void Save()
        {
            //foreach (EndpointBehaviorElement ebe in  GetServiceModel.Behaviors.EndpointBehaviors)
            //{
            //    Trace.TraceInformation("{0} behavior {1}", ebe.Name, ebe);
            //    foreach (BehaviorExtensionElement bee in ebe)
            //    {
            //        if (bee is ClientCredentialsElement)
            //        {
            //            ClientCredentialsElement cce = (ClientCredentialsElement)bee;
            //            //Trace.TraceInformation("\t{0} name, {1}, certificatefindtype({2})", bee.ConfigurationElementName, bee.BehaviorType, );
            //        }
                    
            //    }
                
            //}
            //foreach (ServiceBehaviorElement sbe in GetServiceModel.Behaviors.ServiceBehaviors)
            //{
            //    Trace.TraceInformation("{0} behavior, {1}", sbe.Name, sbe);
            //    foreach (BehaviorExtensionElement bee in sbe)
            //    {
            //        Trace.TraceInformation("behaviorextension element {0}", bee.BehaviorType);
            //        if (bee is ServiceCredentialsElement)
            //        {
            //            ServiceCredentialsElement cce = (ServiceCredentialsElement)bee;
            //            Trace.TraceInformation("\t{0} name, {1}, certificatefindtype({2})", cce.ClientCertificate, bee.BehaviorType,  cce.ServiceCertificate);
            //        }

            //    }

            //}
            m_StateConfig.Save();
            
        }
        public IList<DbSelector> GetDBConnectionStrings
        {
            get
            {
                return ConvertConnectionString(this.GetConnectionSection.ConnectionStrings);
            }
        }
        public IList<ServiceItem> GetServices
        {
            get
            {
                return ConvertServiceSection(this.GetServiceModel.Services, this.GetServiceModel.Bindings);
            }
        }
        public IList<ClientItem> GetClients
        {
            get
            {
                return ConvertClientSection(this.GetServiceModel.Client, this.GetServiceModel.Bindings);
            }
        }
        private static IList<ClientItem> ConvertClientSection(ClientSection clientSection, BindingsSection bindings)
        {
            ConfigurationSection clientSection2 = clientSection;
            //string rawXml = clientSection.SectionInformation.GetRawXml();
            IList<ClientItem> retVal = new List<ClientItem>(clientSection.Endpoints.Count);
            foreach (ChannelEndpointElement cli in clientSection.Endpoints)
            {
                // we process it back at writing by testing
                // if there is a mexHttpBinding together with the wsHttpBinding endpoint.
                if (cli.Binding != "mexHttpBinding")
                {
                    retVal.Add(new ClientItem()
                    {
                        EndPointAddress = cli.Address,
                        ServiceName = cli.Name,
                        BindingType = cli.Binding,
                        Endpoint = cli,
                        //BindingConfiguration = cli.BindingConfiguration,
                        BehaviorConfiguration = cli.BehaviorConfiguration,
                        HasTLS = ServiceHasTLS(bindings, cli.BindingConfiguration)
                    });
                }
                
            }
            return retVal;
        }
        private static IList<ServiceItem> ConvertServiceSection(ServicesSection servicesSection, BindingsSection bindings)
        {
            var retVal = new List<ServiceItem>(servicesSection.Services.Count);
            // assume that there are 2 endpoints and 1 base address
            // first wsHttpBinding
            // second mex ending with /MEX
            foreach (ServiceElement sve in servicesSection.Services)
            {
                if (sve.Endpoints != null)
                {
                    Uri buildIt = null;
                    if (sve.Endpoints[0].Contract != "IMetadataExchange")
                    {
                        bool goOn = true;
                        bool hasAddress = !string.IsNullOrEmpty(sve.Endpoints[0].Address.OriginalString);
                        if (hasAddress)
                        {
                            buildIt = sve.Endpoints[0].Address;

                        }
                        else if (sve.Host != null && sve.Host.BaseAddresses.Count > 0)
                        {
                            buildIt = new Uri(sve.Host.BaseAddresses[0].BaseAddress);

                        }
                        else
                        {
                            goOn = false;
                        }

                        if (goOn)
                        {
                            string behaviorConfig = string.IsNullOrEmpty(sve.BehaviorConfiguration) ? sve.Endpoints[0].BehaviorConfiguration : sve.BehaviorConfiguration;
                            retVal.Add(new ServiceItem()
                                {
                                    EndPointAddress = buildIt,
                                    ServiceName = sve.Name,
                                    BindingType = sve.Endpoints[0].Binding,
                                    Endpoint = sve.Endpoints[0],
                                    //BindingConfiguration = sve.Endpoints[0].BindingConfiguration,
                                    BehaviorConfiguration = behaviorConfig,
                                    HasTLS = ServiceHasTLS(bindings, sve.Endpoints[0].BindingConfiguration)
                                });
                        }
                    }
                }
            }
            return retVal;
        }
        public ServiceModelSectionGroup GetServiceModel
        {
            get
            {
                return ServiceModelSectionGroup.GetSectionGroup(m_StateConfig);                
            }
        }
        public IList<AppKeys> GetAppKeys
        {
            get
            {
                return ConvertApplicationToList(this.GetAppsettings.Settings);
            }
        }
        public AppSettingsSection GetAppsettings
        {
            get
            {
                return m_StateConfig.AppSettings;

            }
        }
        public ConnectionStringsSection GetConnectionSection
        {
            get
            {                
                return m_StateConfig.ConnectionStrings;
            }
        }
        /// <summary>
        /// helper function
        /// </summary>
        /// <param name="virtualPath"></param>
        /// <param name="site"></param>
        /// <returns></returns>
        public static bool IsAtIIS(string virtualPath, string site)
        {
            try
            {
               //var test = WebConfigurationManager.OpenWebConfiguration('/' + virtualPath, site);
               int instance;
               bool exists = IIS.VDirExists(IIS.Getsites, virtualPath, out instance);
                return exists;
            }
            catch
            {
            }
            return false;
        }
        public OtherConfig(string virtualPath, string site, bool useIIS)
        {
            if (useIIS == false)
            {
                throw new Exception("useIIS must be true if you use this constructor");
            }
            else
            {
                m_StateConfig = WebConfigurationManager.OpenWebConfiguration('/' + virtualPath, site);
            }
        }
        public OtherConfig(string file)
        {
            //m_StateConfig = ConfigurationManager.OpenExeConfiguration(file); don't use
            
            var configMap = new ExeConfigurationFileMap {ExeConfigFilename = file};
            m_StateConfig = ConfigurationManager.OpenMappedExeConfiguration(configMap, ConfigurationUserLevel.None);
        }
        public OtherConfig(string file, string pPath)
        {
            var configMap = new ExeConfigurationFileMap();
            configMap.ExeConfigFilename = file;
            m_StateConfig = ConfigurationManager.OpenMappedExeConfiguration(configMap, ConfigurationUserLevel.None);
            var secttions = m_StateConfig.GetSection(pPath);

            m_Settings = (ClientSettingsSection)secttions;
            m_SettingsCollection = m_Settings.Settings;
        }
        public static bool ServiceHasTLS(BindingsSection sc, string bindingConfig)
        {
            var bindings = sc.WSHttpBinding.Bindings;
            int ct = bindings.Count;
            for (int x = 0; x < ct; x++)
            {
                WSHttpBindingElement bs = bindings[x];
                if (bs.Name != bindingConfig) continue;
                //TODO Transport & Security
                return bs.Security.Transport.ClientCredentialType == System.ServiceModel.HttpClientCredentialType.Certificate; 
            }
            return false;

        }
       

        /// <summary>
        /// Configures the specified service having TLS
        /// </summary>
        /// <param name="serviceConfig">the service item wich must be set/unset with TLS</param>
        /// <param name="subjectName">specifies the certificate to be used</param>
        /// <param name="set">true set TLS, false = unset</param>
        public void SetServiceTLS(ServiceItem serviceConfig, string thumbPrint, bool set)
        {
            string bindingConfig = serviceConfig.Endpoint.BindingConfiguration;
            string serviceBehavior = serviceConfig.BehaviorConfiguration;
            //find a binding wich has specified certificate
            // we do not yet do this for other type of bindings
            if (serviceConfig.BindingType == "wsHttpBinding")
            {
                var bindings = this.GetServiceModel.Bindings.WSHttpBinding.Bindings;
                int ct = bindings.Count;
                for(int x = 0; x < ct; x++)
                {
                    WSHttpBindingElement bs = bindings[x];
                    if (bs.Name != bindingConfig) continue;
                    
                    bs.Security.Mode = set ? System.ServiceModel.SecurityMode.Transport : System.ServiceModel.SecurityMode.None;
                    bs.Security.Transport.ClientCredentialType = set ? System.ServiceModel.HttpClientCredentialType.Certificate : System.ServiceModel.HttpClientCredentialType.None;
                    bs.Security.Transport.ProxyCredentialType = System.ServiceModel.HttpProxyCredentialType.None;
                    bs.Security.Message.ClientCredentialType = System.ServiceModel.MessageCredentialType.None;
                    bs.Security.Message.EstablishSecurityContext = false;
                    bs.Security.Message.NegotiateServiceCredential = false;
                    break;
                }
                var services = this.GetServiceModel.Services.Services;
                ct = services.Count;
                for (int x = 0; x < ct; x++)
                {
                    if (services[x].Name == serviceConfig.ServiceName)
                    {
                        var endp = services[x].Endpoints;
                        int y = endp.Count;
                        for (int z = 0; z < y; z++)
                        {
                            if (endp[z].Address != null)
                            {
                                string newaddrss = endp[z].Address.OriginalString;
                                if (set)
                                {
                                    newaddrss = newaddrss.Replace("http:", "https:");
                                }
                                else
                                {
                                    newaddrss = newaddrss.Replace("https:", "http:");
                                }
                                endp[z].Address = new Uri(newaddrss);
                            }
                            //TODO: update BASE 
                            else
                            {
                            }
                        }
                        if (services[x].Host != null && services[x].Host.BaseAddresses != null)
                        {
                            string htp = services[x].Host.BaseAddresses[0].BaseAddress;
                            if (set)
                            {
                                htp = htp.Replace("http:", "https:");
                            }
                            else
                            {
                                htp = htp.Replace("https:", "http:");
                            }
                            services[x].Host.BaseAddresses[0].BaseAddress = htp;
                        }
                        
                    }
                }
                var behaviors = this.GetServiceModel.Behaviors.ServiceBehaviors;
                ct = behaviors.Count;

                if (!string.IsNullOrEmpty(serviceBehavior))
                {
                    for (int x = 0; x < ct; x++)
                    {
                        ServiceBehaviorElement sbe = behaviors[x];
                        if (sbe.Name != serviceBehavior) continue;
                        bool found = false;
                        ServiceCredentialsElement sce = null;
                        foreach (BehaviorExtensionElement bee in sbe)
                        {
                            if (bee is ServiceMetadataPublishingElement)
                            {
                                ServiceMetadataPublishingElement smpe = (ServiceMetadataPublishingElement)bee;
                                smpe.HttpGetEnabled = !set;
                                smpe.HttpsGetEnabled = set;

                            }
                            else if (bee is ServiceCredentialsElement)
                            {
                                sce = (ServiceCredentialsElement)bee;
                                found = true;
                                break;
                            }
                        }
                        if (!found)
                        {
                           
                            sce = new ServiceCredentialsElement();
                            sbe.Add(sce);
                        }
                        sce.ServiceCertificate.FindValue = thumbPrint;
                        sce.ServiceCertificate.StoreLocation = System.Security.Cryptography.X509Certificates.StoreLocation.LocalMachine;
                        sce.ServiceCertificate.X509FindType = System.Security.Cryptography.X509Certificates.X509FindType.FindByThumbprint;
                        sce.ServiceCertificate.StoreName = System.Security.Cryptography.X509Certificates.StoreName.My;
                    }
                }
                else 
                {
                    throw new NotSupportedException("The service TLS element has no behaviorConfiguration");
                    
                }
            }
        }

        /// <summary>
        /// Configures the specified service having TLS
        /// </summary>
        /// <param name="serviceConfig">the service item wich must be set/unset with TLS</param>
        /// <param name="subjectName">specifies the certificate to be used</param>
        /// <param name="set">true set TLS, false = unset</param>
        public void SetEndpointTLS(ClientItem serviceConfig, string thumbPrint, bool set)
        {
            string bindingConfig = serviceConfig.Endpoint.BindingConfiguration;
            string serviceBehavior = serviceConfig.BehaviorConfiguration;
            //find a binding wich has specified certificate
            // we do not yet do this for other type of bindings
            if (serviceConfig.BindingType == "wsHttpBinding")
            {
                var wshbindings = this.GetServiceModel.Bindings.WSHttpBinding.Bindings;
                int ct = wshbindings.Count;
                for (int x = 0; x < ct; x++)
                {
                    WSHttpBindingElement bs = wshbindings[x];
                    if (bs.Name != bindingConfig) continue;

                    bs.Security.Mode = set ? System.ServiceModel.SecurityMode.Transport : System.ServiceModel.SecurityMode.None;
                    bs.Security.Transport.ClientCredentialType = set ? System.ServiceModel.HttpClientCredentialType.Certificate : System.ServiceModel.HttpClientCredentialType.None;
                    bs.Security.Transport.ProxyCredentialType = System.ServiceModel.HttpProxyCredentialType.None;
                    bs.Security.Message.ClientCredentialType = System.ServiceModel.MessageCredentialType.None;
                    bs.Security.Message.EstablishSecurityContext = false;
                    bs.Security.Message.NegotiateServiceCredential = false;
                }

                var endpoints = this.GetServiceModel.Client.Endpoints;
                ct = endpoints.Count;
                for (int x = 0; x < ct; x++)
                {
                    if (endpoints[x].Name == serviceConfig.ServiceName)
                    {
                        if (endpoints[x].Address != null)
                        {
                            string newaddrss = endpoints[x].Address.OriginalString;
                            if (set)
                            {
                                newaddrss = newaddrss.Replace("http:", "https:");
                            }
                            else
                            {
                                newaddrss = newaddrss.Replace("https:", "http:");
                            }
                            endpoints[x].Address = new Uri(newaddrss);
                        }
                    }
                }
                
                ct = this.GetServiceModel.Behaviors.EndpointBehaviors.Count;
             
                EndpointBehaviorElement sbe = null;
                if (string.IsNullOrEmpty(serviceBehavior))
                {
                    serviceBehavior = "generated" + new Random().Next().ToString();
                    sbe = new EndpointBehaviorElement(serviceBehavior);
                    serviceConfig.BehaviorConfiguration = serviceBehavior;
                    sbe.Name = serviceBehavior;
                    this.GetServiceModel.Behaviors.EndpointBehaviors.Add(sbe);
                    var ep = this.GetServiceModel.Client.Endpoints;
                    int z = ep.Count;
                    for (int x = 0; x < z; x++)
                    {
                        if (ep[x].Name == serviceConfig.ServiceName)
                        {
                            ep[x].BehaviorConfiguration = serviceBehavior;
                            break;
                        }
                    }
                }
                else
                {
                    var epb = this.GetServiceModel.Behaviors.EndpointBehaviors;
                    ct = epb.Count;
                    for (int x = 0; x < ct; x++)
                    {
                        sbe = epb[x];
                        if (sbe.Name != serviceBehavior) continue;
                    }
                }
                
                bool found = false;
                ClientCredentialsElement sce = null;
                foreach (BehaviorExtensionElement bee in sbe)
                {
                    if (bee is ClientCredentialsElement)
                    {
                        sce = (ClientCredentialsElement)bee;
                        found = true;
                        break;
                    }
                }
                if (!found)
                {
                    sce = new ClientCredentialsElement();
                    sbe.Add(sce);
                }
                sce.ClientCertificate.FindValue = thumbPrint;
                sce.ClientCertificate.StoreLocation = System.Security.Cryptography.X509Certificates.StoreLocation.LocalMachine;
                sce.ClientCertificate.X509FindType = System.Security.Cryptography.X509Certificates.X509FindType.FindByThumbprint;
                sce.ClientCertificate.StoreName = System.Security.Cryptography.X509Certificates.StoreName.My;
            }
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="clientConfig">old value</param>
        /// <param name="newUri">new url</param>
        /// <param name="forceUpdate">if true, will not throw OtherConfigException when the MEX address is not aligned with the baseAddress, it will overwrite it anyway</param>
        public void UpdateServerEndpoint(ServiceItem serviceConfig, Uri newUri, bool forceUpdate)
        {
            string testurl = serviceConfig.EndPointAddress.ToString();
            //bool isWsHttp = clientConfig.BindingType == "wsHttpBinding";
            foreach (ServiceElement svc in this.GetServiceModel.Services.Services)
            {
                bool found = false;
                foreach (ServiceEndpointElement element in svc.Endpoints)
                {
                    string currentEndpointToTest = element.Address.ToString();
                    if (element.Contract == "IMetadataExchange" && found)
                    {
                        // is this the MEX url?
                        if (currentEndpointToTest.StartsWith(testurl) && currentEndpointToTest.EndsWith("MEX", StringComparison.OrdinalIgnoreCase) || forceUpdate )
                        {
                            element.Address = new Uri(newUri, "MEX");
                        }
                        else
                        {
                            //var goOn = System.Windows.Forms.MessageBox.Show(string.Format("we expected {0} but we found {1}, continue to set this address named {2}?", testurl, element.Address, element.Name), Program.g_Apptitle, MessageBoxButtons.YesNo, MessageBoxIcon.Question);
                            throw new OtherConfigException(string.Format("We expected {0} but we found {1}, continue to set this address named {2}?", testurl, element.Address, element.Name));
                            //if (goOn == DialogResult.Yes)
                            //{
                            //}
                        }
                    }
                    else if (element.Address == serviceConfig.EndPointAddress)
                    {
                        element.Address = newUri;
                        found = true;
                    }
                    else if (string.IsNullOrEmpty(element.Address.OriginalString) && svc.Host != null && svc.Host.BaseAddresses != null &&  svc.Host.BaseAddresses.Count > 0 && svc.Host.BaseAddresses[0].BaseAddress == serviceConfig.EndPointAddress.OriginalString)
                    {
                       // svc.Host.BaseAddresses[0].BaseAddress = newUri.OriginalString;
                        found = true;
                    }

                }
                if (found && svc.Host != null && svc.Host.BaseAddresses.Count > 0)
                {
                    svc.Host.BaseAddresses[0].BaseAddress = newUri.ToString(); ;
                }
                found = false;
            }
            
            // string currentEndpointToTest = element.Address.ToString();
            //if (element.Contract == "IMetadataExchange")
            //{
            //    // is this the MEX url?
            //    if (currentEndpointToTest.StartsWith(testurl) && currentEndpointToTest.EndsWith("MEX", StringComparison.OrdinalIgnoreCase))
            //    {
            //        element.Address = new Uri(newUri, "MEX");
            //    }
            //}
            //else if (element.Address == serviceConfig.EndPointAddress)
            //{
            //    element.Address = newUri;
            //}
        }
        /// <summary>
        /// 
        /// </summary>
        /// <param name="clientConfig">old value</param>
        /// <param name="newUri">new url</param>
        public void UpdateClientEndpoint(ClientItem clientConfig, Uri newUri)
        {
            string testurl = clientConfig.EndPointAddress.ToString();
            //bool isWsHttp = clientConfig.BindingType == "wsHttpBinding";
            bool found = false;
            foreach (ChannelEndpointElement element in this.GetServiceModel.Client.Endpoints)
            {
                string currentEndpointToTest = element.Address.ToString();
                
                if (element.Contract == "IMetadataExchange" && found)
                {
                    found = false;
                    // is this the MEX url?
                    if (currentEndpointToTest.StartsWith(testurl) && currentEndpointToTest.EndsWith("MEX", StringComparison.OrdinalIgnoreCase))
                    {
                        element.Address = new Uri(newUri, "MEX");
                    }
                }
                else if (element.Address == clientConfig.EndPointAddress)
                {
                    element.Address = newUri;
                    found = true;
                }

                
            }
          
        }
    }
}