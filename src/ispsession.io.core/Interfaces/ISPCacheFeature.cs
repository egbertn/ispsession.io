using ispsession.io.core.Interfaces;


namespace ispsession.io.core.Interfaces
{
    public interface ICacheFeature
    {
      
        /// <summary>
        /// global application cache
        /// </summary>
        IApplicationCache Application { get; set; }
    }
    public class ISPCacheFeature: ICacheFeature
    {
       
        public IApplicationCache Application { get; set; }
     
    }
}