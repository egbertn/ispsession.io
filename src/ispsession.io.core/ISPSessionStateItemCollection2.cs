using System;
using System.Collections;
using System.Collections.Generic;

namespace ispsession.io.core
{
    public class ISPSessionStateItemCollection2
    {

        private readonly SortedSet<string> _entriesTable;
        private readonly Dictionary<int, object> _entries;

        public ISPSessionStateItemCollection2()
        {
            _entries = new Dictionary<int, object>();            
            _entriesTable = new SortedSet<string>();
            
        }
        public int Count
        {
            get { return _entriesTable.Count; }
        }
        /// <summary>
        /// Gets or sets a value in the collection by name.
        /// </summary>
        /// <param name="name">The key name of the value in the collection.</param>
        /// <returns>
        /// The value in the collection with the specified name. If the specified key is
        /// not found, attempting to get it returns null, and attempting to set it creates
        /// a new element using the specified key.
        /// </returns>        
        public object this[string name]
        {
            get
            {
                if (string.IsNullOrEmpty(name))
                {
                    throw new ArgumentNullException(nameof(name));
                }
                
                if (!_entriesTable.Contains(name))
                {
                    return null;
                }
                int idx = name.GetHashCode();
                return _entries[idx] ;
            }
            set
            {
                if (string.IsNullOrEmpty(name))
                {
                    throw new ArgumentNullException(nameof(name));
                }
                int idx = name.GetHashCode();
                if (!_entriesTable.Contains(name))
                {
                    _entriesTable.Add(name);
                }
                Dirty = true;
                _entries[idx] = value;
            }
        }
      
        public object this[int index]
        {
            get
            {
               if (index<0 || index > _entriesTable.Count)
                {
                    throw new ArgumentOutOfRangeException("index < 0 or > the number of items in the collection");
                }
                int ct = 0;
               foreach(var i in _entriesTable)
                {
                    if (ct++==index)
                    {
                        var idx = i.GetHashCode();
                        return _entries[idx];
                    }
                }
                return null;
            }
            set
            {
                if (index < 0 || index > _entriesTable.Count)
                {
                    throw new ArgumentOutOfRangeException("index < 0 or > the number of items in the collection");
                }
                int ct = 0;
                foreach (var i in _entriesTable)
                {
                    if (ct++ == index)
                    {
                        var idx = i.GetHashCode();
                        _entries[idx] = value;
                        Dirty = true;
                        break;
                    }
                }
            }
        }

        private bool _dirty;
        public bool Dirty
        {
            get
            {
                return _dirty;
            }
            private set
            {
                if (_dirty == false && value)
                {
                    StreamManager.TraceInformation("SEtting Session ID {0} dirty", this.SessionID);
                }
                _dirty = value;
                
            }
        }

            
     
        public IEnumerable<string> Keys
        {
            get
            {
                return _entriesTable;
            }
        }


        
        /// <summary>
        /// Removes all values and keys from the session-state collection.
        /// </summary>
        public void Clear()
        {
            var doSetDirty = _entriesTable.Count > 0;
            _entriesTable.Clear();
            _entries.Clear();
            Dirty |= doSetDirty;
        }
        
        /// <summary>
        /// Returns an enumerator that can be used to read all the key names in the collection.
        /// </summary>
        /// <returns>
        /// An System.Collections.IEnumerator that can iterate through the variable names
        /// in the session-state collection.
        /// </returns>
        public IEnumerator GetEnumerator()
        {
            return (IEnumerator)_entriesTable;
        }        
        /// <summary>
        ///  Deletes an item from the collection.
        /// </summary>
        /// <param name="name">The name of the item to delete from the collection.</param>
        public void Remove(string name)
        {
            if (_entriesTable.Contains(name))
            {
                bool isRemoved = _entriesTable.Remove(name);
                if (isRemoved)
                {
                    _entries.Remove(name.GetHashCode());
                    Dirty |= isRemoved;
                }                
            }
        }
        //
        // Summary:
        //     Deletes an item at a specified index from the collection.
        //
        // Parameters:
        //   index:
        //     The index of the item to remove from the collection.
        //
        // Exceptions:
        //   T:System.ArgumentOutOfRangeException:
        //     index is less than zero.- or -index is equal to or greater than System.Collections.ICollection.Count.
        public void RemoveAt(int index)
        {
            if (index < 0 || index > _entriesTable.Count)
            {
                throw new ArgumentOutOfRangeException("index < 0 or > the number of items in the collection");
            }
            int ct = 0;
            foreach (var i in _entriesTable)
            {
                if (ct++ == index)
                {
                    var idx = i.GetHashCode();
                    _entriesTable.Remove(i);
                    _entries.Remove(idx);
                    Dirty = true;
                    break;
                }
            }

        }
        public bool IsReadOnly { get; set; }
        public bool IsNew { get; set; }
        public int Timeout { get; set; }
        public string SessionID { get; set; }
        public string OldSessionID { get; set; }
        
    }
}
