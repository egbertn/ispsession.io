using System;
using System.ComponentModel;
using System.Linq.Expressions;

namespace ispsession.io.setup.PRISM
{
    public abstract class NotificationObject: INotifyPropertyChanged
    {
        protected abstract void AfterPropertyUpdate(string name);
        protected void RaisePropertyChanged<T>(Expression<Func<T>> propertyExpression)
        {
            if (propertyExpression != null)
            {
                var body = propertyExpression.Body as MemberExpression;
                if (body != null)
                {
                    if (PropertyChanged != null)
                    {
                        var name = body.Member.Name;
                        PropertyChanged(this, new PropertyChangedEventArgs(name));
                        AfterPropertyUpdate(name);
                    }
                }
            }
        }
        #region INotifyPropertyChanged
        public event PropertyChangedEventHandler PropertyChanged;
        #endregion
    }
}
