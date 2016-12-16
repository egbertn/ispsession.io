using ispsession.io;
using ispsession.io.Store;

using System;
//unfortunate, but necessary for shortcutting and being similar to the MS Syntax
namespace Microsoft.Extensions.DependencyInjection
{
    public static class ISPSessionServiceCollectionExtensions
    {
        public static IServiceCollection AddISPSession(this IServiceCollection services)
        {
            if (services == null)
            {
                throw new ArgumentNullException(nameof(services));
            }
            ServiceCollectionServiceExtensions.AddTransient<IISPSessionStore, ISPSessionStore>(services);
            //DataProtectionServiceCollectionExtensions.AddDataProtection(services);
            return services;
        }

        public static IServiceCollection AddISPSession(this IServiceCollection services, Action<SessionAppSettings> configure)
        {
            if (services == null)
            {
                throw new ArgumentNullException(nameof(services));
            }
            if (configure == null)
            {
                throw new ArgumentNullException(nameof(configure));
            }
            OptionsServiceCollectionExtensions.Configure(services, configure);
            services.AddISPSession();
            return services;
        }

    }
}
