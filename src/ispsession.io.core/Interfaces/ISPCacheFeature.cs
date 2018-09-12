using ispsession.io.core.Interfaces;


namespace ispsession.io
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