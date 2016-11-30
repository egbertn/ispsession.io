using ispsession.io;
using ispsession.io.Store;

using System;
//unfortunate, but necessary for shortcutting and being similar to the MS Syntax
namespace Microsoft.Extensions.DependencyInjection
{
    /// <summary>
    /// Extension methods for adding session services to the DI container.
    /// </summary>
    public static class ISPSessionServiceCollectionExtensions
    {
        /// <summary>
        /// Adds services required for application session state.
        /// </summary>
        /// <param name="services">The <see cref="T:Microsoft.Extensions.DependencyInjection.IServiceCollection" /> to add the services to.</param>
        /// <returns>The <see cref="T:Microsoft.Extensions.DependencyInjection.IServiceCollection" /> so that additional calls can be chained.</returns>
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

        /// <summary>
        /// Adds services required for application session state.
        /// </summary>
        /// <param name="services">The <see cref="T:Microsoft.Extensions.DependencyInjection.IServiceCollection" /> to add the services to.</param>
        /// <param name="configure">The session options to configure the middleware with.</param>
        /// <returns>The <see cref="T:Microsoft.Extensions.DependencyInjection.IServiceCollection" /> so that additional calls can be chained.</returns>
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
