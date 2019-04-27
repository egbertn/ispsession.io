
using System.Collections;

namespace ispsession.io.core.Interfaces
{
    public interface IISPSession: Microsoft.AspNetCore.Http.ISession
    {
       
        object this[int index] { get; set; }
       
        object this[string name] { get; set; }
     
        int Count { get; }

        bool IsNewSession { get; }        
        bool IsReadOnly { get; }      
        int LCID { get; set; }
               
        string SessionID { get; }
       
        int Timeout { get; set; }

        //
        // Summary:
        //     Ends the current session.
        void Abandon();
      
        void Add(string name, object value);
      
        IEnumerator GetEnumerator();
      
        void RemoveAll();
     
        void RemoveAt(int index);
        /// <summary>
        /// sets or gets the liquid session cookie feature, 
        /// causes the SessionCookie to change at each request. 
        /// Use with care sessions could get lost.
        /// </summary>
        bool Liquid { get; set; }
    }
}
