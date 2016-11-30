using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;
using System.Collections;

namespace ADCCure.Configurator.DAL
{

    sealed public class CopyInfo
    {
        internal CopyInfo(string fullName, bool isFolder)
        {
            m_Path = fullName;
            m_IsFolder = isFolder;
        }
        internal CopyInfo(string fullName)
            : this(fullName, false)
        {
        }
        private bool m_IsFolder; // if false, it is a file
        private string m_Path;
        public bool IsFolder
        {
            get { return m_IsFolder; }
        }
        public string Path
        {
            get { return m_Path; }
        }
    }


    [ComImport(), InterfaceType(ComInterfaceType.InterfaceIsDual), Guid("001677d0-fd16-11ce-abc4-02608c9e7553")]
    public interface IADsContainer
    {
        int Count
        { get; }

        object _newEnum
        {
            [return: MarshalAs(UnmanagedType.IUnknown)]
            get;
        }
        object Filter
        {
            [return: MarshalAs(UnmanagedType.Struct)]
            get;
            [return: MarshalAs(UnmanagedType.Struct)]
            set;
        }
        object Hints
        {
            [return: MarshalAs(UnmanagedType.Struct)]
            get;
            [return: MarshalAs(UnmanagedType.Struct)]
            set;
        }
        [return: MarshalAs(UnmanagedType.IDispatch)]
        object GetObject(
            [MarshalAs(UnmanagedType.BStr)]
                string ClassName,
            [MarshalAs(UnmanagedType.BStr)]
                string RelativeName);

        [return: MarshalAs(UnmanagedType.IDispatch)]
        object Create(
            [MarshalAs(UnmanagedType.BStr)]
                string ClassName,
            [MarshalAs(UnmanagedType.BStr)]
                string RelativeName);

        void Delete(
           [MarshalAs(UnmanagedType.BStr)]
                string ClassName,
           [MarshalAs(UnmanagedType.BStr)]
                string RelativeName);

        [return: MarshalAs(UnmanagedType.IDispatch)]
        object CopyHere(
            [MarshalAs(UnmanagedType.BStr)]
                string SourceName,
            [MarshalAs(UnmanagedType.BStr)]
                string NewName);

        [return: MarshalAs(UnmanagedType.IDispatch)]
        object MoveHere(
            [MarshalAs(UnmanagedType.BStr)]
                string SourceName,
            [MarshalAs(UnmanagedType.BStr)]
                string NewName);
    }

    public enum AdsPropertyOperationEnum : int
    {
        ADS_PROPERTY_CLEAR = 1,
        ADS_PROPERTY_UPDATE = 2,
        ADS_PROPERTY_APPEND = 3,
        ADS_PROPERTY_DELETE = 4
    }
        //    [DispId(18)]
        //void Add(string bstrNewItem);
        //[DispId(17)]
        //bool IsMember(string bstrMember);
        //[DispId(16)]
        //IADsMembers Members();
        //[DispId(19)]
        //void Remove(string bstrItemToBeRemoved);

    [Guid("451A0030-72EC-11CF-B03B-00AA006E0975"), ComImport()
    ,InterfaceType(ComInterfaceType.InterfaceIsIDispatch)]
    public interface IADsMembers : IEnumerable
    {
        [DispId(2)]
        int Count { get; }
        [DispId(3)]
        object Filter { get; set; }

        //[DispId(-4)]
        //IEnumerator GetEnumerator();
    }
    //WARNING: do not set to InterfaceIsDual. There are unknown problems with that.
    [ComImport(), InterfaceType(ComInterfaceType.InterfaceIsIDispatch), //
    Guid("27636B00-410F-11CF-B1FF-02608C9E7553")]
    public interface IADsGroup : IADs
    {
        [DispId(0x0000000f)]
        string Description
        {
            [return: MarshalAs(UnmanagedType.BStr)]
            get;
            set;
        }
        [DispId(0x00000010)] //do not use        
        IADsMembers Members();

        [DispId(0x00000011)]
        [return: MarshalAs(UnmanagedType.VariantBool)]
        bool IsMember([MarshalAs(UnmanagedType.BStr)] string bstrMember);

        [DispId(0x00000012)]
        void Add([MarshalAs(UnmanagedType.BStr)] string bstrNewItem);
        [DispId(0x00000013)]
        void Remove([MarshalAs(UnmanagedType.BStr)] string bstrItemToBeRemoved);
    }
    [ComImport(), InterfaceType(ComInterfaceType.InterfaceIsIDispatch), Guid("3E37E320-17E2-11CF-ABC4-02608C9E7553")]
    public interface IADsUser : IADs
    {   // we don't need other properties & methods add them here, use DispId!
        [DispId(0x00000043)]
        void SetPassword([MarshalAs(UnmanagedType.BStr)] string NewPassword);
    }

    [ComImport(),
    InterfaceType(ComInterfaceType.InterfaceIsDual),
    Guid("fd8256d0-fd15-11ce-abc4-02608c9e7553")]
    public interface IADs
    {
        [DispId(2)]
        string Name
        {
            [return: MarshalAs(UnmanagedType.BStr)]
            get;
        }
        [DispId(3)]
        string Class
        {
            [return: MarshalAs(UnmanagedType.BStr)]
            get;
        }
        [DispId(4)]
        string Guid
        {
            [return: MarshalAs(UnmanagedType.BStr)]
            get;
        }
        [DispId(5)]
        string ADsPath
        {
            [return: MarshalAs(UnmanagedType.BStr)]
            get;
        }
        [DispId(6)]
        string Parent
        {
            [return: MarshalAs(UnmanagedType.BStr)]
            get;
        }
        [DispId(7)]
        string Schema
        {
            [return: MarshalAs(UnmanagedType.BStr)]
            get;
        }
        [DispId(8)]
        void GetInfo();
        [DispId(9)]
        void SetInfo();
        [DispId(0xa)]
        [return: MarshalAs(UnmanagedType.Struct)]
        object Get(string bstrName);
        [DispId(0xb)]
        void Set([MarshalAs(UnmanagedType.BStr)]
                    string bstrName,
                [MarshalAs(UnmanagedType.Struct)]
                object vProp);
        [DispId(0xc)]
        [return: MarshalAs(UnmanagedType.Struct)]
        object GetEx([MarshalAs(UnmanagedType.BStr)]
                    string bstrName);
        [DispId(0xd)]
        void PutEx([MarshalAs(UnmanagedType.U4)]
                AdsPropertyOperationEnum lngControlCode,
                        [MarshalAs(UnmanagedType.BStr)]
                         string bstrName,
                        [MarshalAs(UnmanagedType.Struct)]
                        object vProp);
        [DispId(0xe)]
        void GetInfoEx([MarshalAs(UnmanagedType.Struct)]
                object vProperties, int lnReserved);
    }

}