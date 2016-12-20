
using ispsession.io;

namespace CookieWebCore.Models
{
    public class HomeModel
    {
        public int CountRefresh { get; set; }
        public string Refresh { get; set; }
        //I know, this is not like this should be done in MVC :) just 
        // to keep the sample more or less equal with classic ASP
        public IISPSession Session { get; set; }
    }
}
