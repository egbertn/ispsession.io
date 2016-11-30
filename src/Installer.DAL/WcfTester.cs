using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Net;
using System.ServiceModel;
using System.ServiceModel.Channels;

namespace ADCCure.Configurator.DAL
{
    public delegate void WcfCallBack(object sender, bool result);
    
    public sealed class WcfTester
    {
        
        Uri m_uri;
        bool m_result;
        bool m_JustOneRequest;
       // MessageVersion _messageVersion;
        public event WcfCallBack OnChecked;
        private string _bindingType;
        public WcfTester(Uri uri, string bindingType)
        {
            m_uri = uri;
            _bindingType = bindingType;
            //_messageVersion = null;
            m_JustOneRequest = false;
        }
        public bool IsWorking
        {
            get { return m_JustOneRequest; }  
        }
        /// <summary>
        /// tries a simple GET on the specified URL.
        /// </summary>
        /// <returns></returns>
        public void BeginRead()
        {
            if (m_JustOneRequest) throw new Exception("The previous request still runs");
            if (OnChecked == null)
            {
                throw new Exception("You must set the OnChecked delegate");
            }
            Binding binding;
            
            switch (_bindingType)
            {
                    //TODO security?
                case "wsHttpBinding":
                    binding = new WSHttpBinding(SecurityMode.None);
                    break;
                case "netTcpBinding":
                    binding = new NetTcpBinding(SecurityMode.None);
                    break;
                case "basicHttpBinding":
                    binding = new BasicHttpBinding(BasicHttpSecurityMode.None);
                    break;
                default:
                    throw new NotSupportedException(string.Format("This binding type {0} is not yet supported.", _bindingType));
            }
            m_JustOneRequest = true;
            //fct.BeginOpen( HttpCallback, fct);
            
            //MessageVersion[] trythese =  { MessageVersion.Soap11, MessageVersion.None, MessageVersion.Soap11WSAddressing10, MessageVersion.Soap12, MessageVersion.Soap12WSAddressing10, MessageVersion.Soap12WSAddressingAugust2004, MessageVersion.Soap11WSAddressingAugust2004 };
            //int trytheseLength = trythese.Length;
            //if (_messageVersion != null)
            //{
            //    Array.Resize<MessageVersion>(ref trythese, trytheseLength+1);
            //    Array.Reverse(trythese);
            //    trythese[0] = _messageVersion;
            //}
        

            ////detect the message version automatically
            //for (int x = 0; x < trythese.Length; x++)
            //{
            //    try
            //    {
            //System.ServiceModel.Description.ServiceEndpoint se = new System.ServiceModel.Description.ServiceEndpoint();
            
                    //ChannelFactory<IRequestChannel> fct;
            
                    Message msg = Message.CreateMessage(binding.MessageVersion, "hello");
                    if (_bindingType == "netTcpBinding")
                    {
                        
                        IChannelFactory<IDuplexSessionChannel> factory  = binding.BuildChannelFactory<IDuplexSessionChannel>();
                        factory.Open();
                        IDuplexSessionChannel req = factory.CreateChannel(new EndpointAddress(m_uri));
                        try
                        {
                            req.Open();
                            req.BeginSend(msg, HttpCallback, req);
                        }
                        catch
                        {
                            factory.Close();
                            m_result = false;
                        }
                    }
                    else
                    {                        
                        IChannelFactory<IRequestChannel> fct  = binding.BuildChannelFactory<IRequestChannel>();
                        fct.Open();
                        try
                        {
                            IRequestChannel req = fct.CreateChannel(new EndpointAddress(m_uri));
                            req.Open();
                            req.BeginRequest(msg, HttpCallback, req);
                        }
                        catch
                        {
                            m_result = false;
                        }
                    }
                    
            
              
//                    _messageVersion = trythese[x];
  //                  break;
                //}
            //    catch (ProtocolException)
            //    {
            //        if (channel.State == CommunicationState.Opened) 
            //            channel.Close();
            //    }
            
            
        }
        private void HttpCallback(IAsyncResult result)
        {
            //HttpWebRequest respo = (HttpWebRequest)result.AsyncState;
            if (result.AsyncState is IDuplexSessionChannel)
            {
                IDuplexSessionChannel fct = (IDuplexSessionChannel)result.AsyncState;
                try
                {
                    //respo.EndGetResponse(result);
                    fct.EndSend(result) ;
                    m_result = true;
                }
                // we are NOT interested in encoding issues, the URL is there and reponds, get out
                catch (ProtocolException)
                {
                    m_result = true;
                }
                catch
                {
                    m_result = false;
                }
                OnChecked(this, m_result);
                m_JustOneRequest = false;
            }
            else if (result.AsyncState is IRequestChannel)
            {
                IRequestChannel fct = (IRequestChannel)result.AsyncState;
                try
                {
                    //respo.EndGetResponse(result);
                    fct.EndRequest(result);
                    m_result = true;
                }
                // we are NOT interested in encoding issues, the URL is there and reponds, get out
                catch (ProtocolException)
                {
                    m_result = true;
                }
                catch
                {
                    m_result = false;
                }
                OnChecked(this, m_result);
                m_JustOneRequest = false;
            }

        }


       
    }
}
