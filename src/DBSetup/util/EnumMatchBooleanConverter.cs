using System;
using System.Globalization;
using System.Windows.Data;

namespace ispsession.io.setup
{
    public class EnumMatchToBooleanConverter : IValueConverter
    {
        public object Convert(object value, Type targetType,
                              object parameter, CultureInfo culture)
        {
            if (value == null || parameter == null)
                return false;

            return string.Compare(value.ToString(), parameter.ToString(), StringComparison.InvariantCultureIgnoreCase) == 0;
        }

        public object ConvertBack(object value, Type targetType,
                                  object parameter, CultureInfo culture)
        {
            if (value == null || parameter == null)
                return null;

            bool useValue = (bool)value;
            string targetValue = parameter.ToString();
            return useValue ? (bool?) bool.Parse(targetValue) : null;
        }
    }
}
