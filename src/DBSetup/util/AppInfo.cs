using System;
using System.Collections.ObjectModel;
using System.Globalization;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Windows;
using System.Windows.Data;

namespace ispsession.io.setup.util
{
    public static class AppInfo
    {
        private static string _appPath;
        /// <summary>
        /// returns the path of the currently running Assembly
        /// </summary>
        internal static string CurrentPath
        {
            get 
            { 
                return _appPath ?? (_appPath = Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location)); 
            }
        }

        private static string _assemblyTitle;
        /// <summary>
        /// Gets the assembly title.
        /// </summary>
        /// <value>The assembly title.</value>
        public static string AssemblyTitle
        {
            get
            {
                if (_assemblyTitle != null) return _assemblyTitle;
                var thisAssembly = Assembly.GetExecutingAssembly();
                // Get all Title attributes on this assembly
                var attribute = thisAssembly.GetCustomAttributes(typeof(AssemblyTitleAttribute), false).FirstOrDefault();
                // If there is at least one Title attribute
                if (attribute != null)
                {
                    // Select the first one
                    var titleAttribute = (AssemblyTitleAttribute)attribute;
                    // If it is not an empty string, return it
                    if (!string.IsNullOrEmpty(titleAttribute.Title))
                        return _assemblyTitle = titleAttribute.Title;

                }
                // If there was no Title attribute, or if the Title attribute was the empty string, return the .exe name
                return Path.GetFileNameWithoutExtension(thisAssembly.CodeBase);
            }

        }
        
        public static bool IsInDesignMode
        {
            get
            {
                return System.ComponentModel.DesignerProperties.GetIsInDesignMode(
                    new DependencyObject()); ;
            }

        }

        
    }
    [ValueConversion(typeof(string), typeof(int))]
    public sealed class TextToIntConverter : IValueConverter 
    {
        object IValueConverter.Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            if (value != null)
            {
                if (targetType == typeof(string))
                {
                    return value.ToString();

                }
                if (targetType == typeof(System.ServiceProcess.ServiceStartMode))
                {
                    return (int)value;
                }
            }
            return null;
        }
    

        object IValueConverter.ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            if (targetType == typeof(int))
            {
                if (value != null && value != Type.Missing)
                {
                    return int.Parse((string)value);
                }
            }
            else if (targetType == typeof(System.ServiceProcess.ServiceStartMode))
            {
                return (System.ServiceProcess.ServiceStartMode)value;
            }
            return DependencyProperty.UnsetValue;
        }
    }
    public sealed class StartupType<T> where T : struct
    {
        public string Display { get; private set; }
        public int Value { get; private set; }
        public static Collection<StartupType<T>> StartupTypes 
        {
            get
            {
                var values = Enum.GetValues(typeof(T));
                var x = values.Length;
                var retval = new StartupType<T>[x];
                x = 0;
                foreach (var value in values)
                {
                    retval[x++] = new StartupType<T>()
                    {
                        Display = Enum.GetName(typeof(T), value),
                        Value = (int)value
                    };
                }
                return new Collection<StartupType<T>>(retval);
            }
        }
    }
}
