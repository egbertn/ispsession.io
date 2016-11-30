using System;
using System.Security.Cryptography.X509Certificates;
using System.Diagnostics;
using System.IO;

namespace ADCCure.Configurator.DAL.Certificates
{
    public sealed class CertificateHelper
    {
        private readonly X509Store store;
        public CertificateHelper(StoreName name, StoreLocation location)
        {
            store = new X509Store(name, location);

        }
        /// <summary>
        /// if true, a dialog will ask for replacement if it already exists
        /// </summary>
        public bool NoDuplicates { get; set; }
        /// <summary>
        /// ensures that a root CA and a client CA exists
        /// </summary>
        public void EnsureCertificates(string subjectName)
        {
            //NewSSLCertificate(subjectName);
            // NewServiceCertificate(subjectName, subjectName, (SecurityIdentifier)NtSecurityInterop.NetworkService.Translate(typeof(SecurityIdentifier)));
        }

        public X509Certificate2 GetCertificate(string subjectName)
        {
            store.Open(OpenFlags.ReadOnly);
            X509Certificate2Collection certificates = store.Certificates;
            int ctCount = certificates.Count;

            try
            {
                X509Certificate2 result = null;

                //
                // Every time we call store.Certificates property, a new collection will be returned.
                //
                subjectName = "CN=" + subjectName;
                for (int i = 0; i < ctCount; i++)
                {
                    X509Certificate2 cert = certificates[i];

                    if (string.Compare(cert.Subject, subjectName, true) == 0)
                    {
                        if (result != null)
                            throw new ApplicationException(string.Format("There is more than one certificate found for subject Name {0}", subjectName));

                        result = new X509Certificate2(cert);
                        break;
                    }
                }

                if (result == null)
                {
                    throw new ApplicationException(string.Format("No certificate was found for subject Name {0}", subjectName));
                }
                store.Close();
                return result;
            }
            finally
            {
                if (certificates != null)
                {
                    for (int i = 0; i < ctCount; i++)
                    {
                        certificates[i].Reset();

                    }
                }
            }

        }
        //public X509Certificate2 NewServiceCertificate(string rootSubject, string subjectName, SecurityIdentifier allowAccessFor)
        //{
        //    store.Close();
        //    store.Open(OpenFlags.MaxAllowed);
        //    X509Certificate2Collection certs = store.Certificates;
        //    int c = store.Certificates.Count;
        //    bool exitme = false;
        //    if (AskForReplace)
        //    {
        //        subjectName = "CN=" + subjectName;
        //        for (int x = 0; (x < c) || exitme; x++)
        //        {
        //            if (certs[x].Subject == subjectName)
        //            {
        //                exitme = true;
        //                return certs[x];
        //            }
        //            certs[x].Reset();

        //        }

        //    }

        //    //store.Close();
        //    var args = new[] { "-sk", subjectName, "–eku 1.3.6.1.5.5.7.3.1", "-iv", string.Format("rt{0}.pvk", rootSubject), "-n", "CN=" + subjectName, "-ic", string.Format("rt{0}.cer", rootSubject), string.Format("{0}.cer", subjectName), "-sky exchange", "-pe", "-sy 12" };
        //    //var args = new[] { "–eku 1.3.6.1.5.5.7.3.1", "-n", "CN=" + subjectName, string.Format("{0}.cer", subjectName), "-sky exchange", "-pe", "-sy 12", "-r" };
        //    ProcessStartInfo nfo = new ProcessStartInfo(Path.Combine(util.AppPath, "makecert.exe"), string.Join(" ", args));
        //    nfo.UseShellExecute = false;
        //    nfo.WindowStyle = ProcessWindowStyle.Hidden;
        //    nfo.WorkingDirectory = util.AppPath;
        //    Process p = Process.Start(nfo);

        //    p.WaitForExit();


        //    X509Certificate2 cert = new X509Certificate2(Path.Combine(util.AppPath, string.Format("{0}.cer", subjectName)));

        //    store.Add(cert);
        //    store.Close();
        //    string file = FindPrivateKey.GetKeyFileName(cert);
        //    string dir = FindPrivateKey.GetKeyFileDirectory (file);
        //    NtSecurityInterop.EnsureACLReadExecute(Path.Combine(dir, file), false, allowAccessFor);

        //    return null;
        //}

        public X509Certificate2 NewSSLCertificate(string rootName)
        {
            X509Store store = new X509Store(StoreName.My, StoreLocation.LocalMachine);
            store.Open(OpenFlags.MaxAllowed);

            //TODO: warning
            string rn = "CN=" + rootName;
            string fileRootcert = Path.Combine(util.AppPath, string.Format("rt{0}.cer", rootName));

            int ctC = store.Certificates.Count;
            if (NoDuplicates)
            {
                for (int x = 0; x < ctC; x++)
                {
                    var cert2 = store.Certificates[x];
                    if (cert2.Subject == rn)
                    {
                        //var replace = System.Windows.Forms.MessageBox.Show("Replace existing certificate?", "Replacement notification", System.Windows.Forms.MessageBoxButtons.YesNo, System.Windows.Forms.MessageBoxIcon.Question);
                        //if (replace == System.Windows.Forms.DialogResult.No)
                        //{
                        return cert2;
                        //}

                    }
                    cert2.Reset();
                }
            }
            if (File.Exists(fileRootcert))
            {
                File.Delete(fileRootcert);
            }
            //C:\Temp>makecert.exe -eku 1.3.6.1.5.5.7.3.1  -n CN=blaat2  blaat2.cer -pe -sy 12
            //-sky exchange -r -ss my -sr localmachine
            //makecert -sk myTestKey -n "CN=XXZZYY" -ss my

            //var args = new[] { "-n", "CN=" + rootName, "-r", "-sv", string.Format("rt{0}.pvk", rootName), string.Format("rt{0}.cer", rootName), "-sky exchange" };


            var args = new[] { "-r", "-pe", "-n", "CN=" + rootName, "-eku 1.3.6.1.5.5.7.3.1", "-ss my", "-sr localMachine", "-sky exchange", "-sp \"Microsoft RSA SChannel Cryptographic Provider\"", "-sy 12", string.Format("rt{0}.cer", rootName) };
            ProcessStartInfo nfo = new ProcessStartInfo(Path.Combine(util.AppPath, "makecert.exe"), string.Join(" ", args));
            nfo.UseShellExecute = false;
            nfo.WorkingDirectory = util.AppPath;
            Process p = Process.Start(nfo);

            p.WaitForExit();

            X509Certificate2 cert = new X509Certificate2(fileRootcert);

            for (int x = 0; x < ctC; x++)
            {
                var cert2 = store.Certificates[x];
                if (cert2.Subject == rn)
                {
                    store.Certificates.RemoveAt(x);
                    break;
                }
            }
            store.Add(cert);
            //copy the SSL cert to the root if it does not yet exist
            string thumbPrint = cert.Thumbprint;


            //cert.Subject = rootName;
            //cert.is
            store.Close();
            store = new X509Store(StoreName.Root, StoreLocation.LocalMachine);
            store.Open(OpenFlags.MaxAllowed);
            int c = store.Certificates.Count;

            for (int x = 0; x < c; x++)
            {
                if (store.Certificates[x].Thumbprint == thumbPrint)
                {
                    store.Remove(store.Certificates[x]);
                }
            }
            store.Add(cert);
            store.Close();


            return cert;
        }
    }
}