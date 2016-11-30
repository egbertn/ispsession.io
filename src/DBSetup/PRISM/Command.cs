using System;
using System.Windows.Input;

namespace ispsession.io.setup.Stuff
{
    public sealed class RelayCommand<T>: ICommand
    {
        readonly Action<T> _execute;
        readonly Func<bool> _canExecute;

        public RelayCommand(Action<T> execute)
            :       this(execute, null)
         {
         }
        public RelayCommand(Action<T> execute, Func<bool> canExecute)
        {
            if (execute == null)
            {
                throw new ArgumentNullException("execute");
            }
            _execute = execute;
            _canExecute = canExecute;
        }

        public bool CanExecute(object parameter)
        {
            return _canExecute == null || _canExecute();
        }

        public event EventHandler CanExecuteChanged;
        public void RaiseCanExecuteChanged()
        {
            if (CanExecuteChanged != null)
            {
                CanExecuteChanged(this, new EventArgs());
            }
        }
        public void Execute(object parameter)
        {
            _execute((T)parameter);
        }

       
    }
}
