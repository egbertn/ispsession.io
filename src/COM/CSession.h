

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 8.01.0626 */
/* at Tue Jan 19 04:14:07 2038
 */
/* Compiler settings for CSession.idl:
    Oicf, W1, Zp8, env=Win64 (32b run), target_arch=AMD64 8.01.0626 
    protocol : all , ms_ext, app_config, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */



/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 500
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif /* __RPCNDR_H_VERSION__ */

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __CSession_h__
#define __CSession_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#ifndef DECLSPEC_XFGVIRT
#if _CONTROL_FLOW_GUARD_XFG
#define DECLSPEC_XFGVIRT(base, func) __declspec(xfg_virtual(base, func))
#else
#define DECLSPEC_XFGVIRT(base, func)
#endif
#endif

/* Forward Declarations */ 

#ifndef __INWCVariantDictionary_FWD_DEFINED__
#define __INWCVariantDictionary_FWD_DEFINED__
typedef interface INWCVariantDictionary INWCVariantDictionary;

#endif 	/* __INWCVariantDictionary_FWD_DEFINED__ */


#ifndef __IVariantDictionary2_FWD_DEFINED__
#define __IVariantDictionary2_FWD_DEFINED__
typedef interface IVariantDictionary2 IVariantDictionary2;

#endif 	/* __IVariantDictionary2_FWD_DEFINED__ */


#ifndef __IApplicationCache_FWD_DEFINED__
#define __IApplicationCache_FWD_DEFINED__
typedef interface IApplicationCache IApplicationCache;

#endif 	/* __IApplicationCache_FWD_DEFINED__ */


#ifndef __INWCSession_FWD_DEFINED__
#define __INWCSession_FWD_DEFINED__
typedef interface INWCSession INWCSession;

#endif 	/* __INWCSession_FWD_DEFINED__ */


#ifndef __CVariantDictionary_FWD_DEFINED__
#define __CVariantDictionary_FWD_DEFINED__

#ifdef __cplusplus
typedef class CVariantDictionary CVariantDictionary;
#else
typedef struct CVariantDictionary CVariantDictionary;
#endif /* __cplusplus */

#endif 	/* __CVariantDictionary_FWD_DEFINED__ */


#ifndef __NWCSession_FWD_DEFINED__
#define __NWCSession_FWD_DEFINED__

#ifdef __cplusplus
typedef class NWCSession NWCSession;
#else
typedef struct NWCSession NWCSession;
#endif /* __cplusplus */

#endif 	/* __NWCSession_FWD_DEFINED__ */


#ifndef __NWCApplication_FWD_DEFINED__
#define __NWCApplication_FWD_DEFINED__

#ifdef __cplusplus
typedef class NWCApplication NWCApplication;
#else
typedef struct NWCApplication NWCApplication;
#endif /* __cplusplus */

#endif 	/* __NWCApplication_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 


#ifndef __INWCVariantDictionary_INTERFACE_DEFINED__
#define __INWCVariantDictionary_INTERFACE_DEFINED__

/* interface INWCVariantDictionary */
/* [unique][oleautomation][dual][uuid][object] */ 


EXTERN_C const IID IID_INWCVariantDictionary;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("4EB7D29B-2455-4B1F-84F5-0EFE5C5FEC07")
    INWCVariantDictionary : public IDispatch
    {
    public:
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_Item( 
            /* [in] */ VARIANT Key,
            /* [retval][out] */ VARIANT *pVal) = 0;
        
        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_Item( 
            /* [in] */ VARIANT Key,
            /* [in] */ VARIANT newVal) = 0;
        
        virtual /* [defaultcollelem][id][propputref] */ HRESULT STDMETHODCALLTYPE putref_Item( 
            /* [in] */ VARIANT Key,
            /* [in] */ VARIANT newVal) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Key( 
            /* [in] */ VARIANT KeyIndex,
            /* [retval][out] */ VARIANT *pVal) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Count( 
            /* [retval][out] */ INT *pVal) = 0;
        
        virtual /* [restricted][id] */ HRESULT STDMETHODCALLTYPE _NewEnum( 
            /* [retval][out] */ IUnknown **pVal) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Remove( 
            /* [in] */ VARIANT varKey) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE RemoveAll( void) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Exists( 
            /* [in] */ VARIANT vKey,
            /* [retval][out] */ VARIANT_BOOL *pVal) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_VarType( 
            /* [in] */ VARIANT vKey,
            /* [retval][out] */ SHORT *pVal) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct INWCVariantDictionaryVtbl
    {
        BEGIN_INTERFACE
        
        DECLSPEC_XFGVIRT(IUnknown, QueryInterface)
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            INWCVariantDictionary * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        DECLSPEC_XFGVIRT(IUnknown, AddRef)
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            INWCVariantDictionary * This);
        
        DECLSPEC_XFGVIRT(IUnknown, Release)
        ULONG ( STDMETHODCALLTYPE *Release )( 
            INWCVariantDictionary * This);
        
        DECLSPEC_XFGVIRT(IDispatch, GetTypeInfoCount)
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            INWCVariantDictionary * This,
            /* [out] */ UINT *pctinfo);
        
        DECLSPEC_XFGVIRT(IDispatch, GetTypeInfo)
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            INWCVariantDictionary * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        DECLSPEC_XFGVIRT(IDispatch, GetIDsOfNames)
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            INWCVariantDictionary * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        DECLSPEC_XFGVIRT(IDispatch, Invoke)
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            INWCVariantDictionary * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        DECLSPEC_XFGVIRT(INWCVariantDictionary, get_Item)
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Item )( 
            INWCVariantDictionary * This,
            /* [in] */ VARIANT Key,
            /* [retval][out] */ VARIANT *pVal);
        
        DECLSPEC_XFGVIRT(INWCVariantDictionary, put_Item)
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Item )( 
            INWCVariantDictionary * This,
            /* [in] */ VARIANT Key,
            /* [in] */ VARIANT newVal);
        
        DECLSPEC_XFGVIRT(INWCVariantDictionary, putref_Item)
        /* [defaultcollelem][id][propputref] */ HRESULT ( STDMETHODCALLTYPE *putref_Item )( 
            INWCVariantDictionary * This,
            /* [in] */ VARIANT Key,
            /* [in] */ VARIANT newVal);
        
        DECLSPEC_XFGVIRT(INWCVariantDictionary, get_Key)
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_Key )( 
            INWCVariantDictionary * This,
            /* [in] */ VARIANT KeyIndex,
            /* [retval][out] */ VARIANT *pVal);
        
        DECLSPEC_XFGVIRT(INWCVariantDictionary, get_Count)
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_Count )( 
            INWCVariantDictionary * This,
            /* [retval][out] */ INT *pVal);
        
        DECLSPEC_XFGVIRT(INWCVariantDictionary, _NewEnum)
        /* [restricted][id] */ HRESULT ( STDMETHODCALLTYPE *_NewEnum )( 
            INWCVariantDictionary * This,
            /* [retval][out] */ IUnknown **pVal);
        
        DECLSPEC_XFGVIRT(INWCVariantDictionary, Remove)
        HRESULT ( STDMETHODCALLTYPE *Remove )( 
            INWCVariantDictionary * This,
            /* [in] */ VARIANT varKey);
        
        DECLSPEC_XFGVIRT(INWCVariantDictionary, RemoveAll)
        HRESULT ( STDMETHODCALLTYPE *RemoveAll )( 
            INWCVariantDictionary * This);
        
        DECLSPEC_XFGVIRT(INWCVariantDictionary, get_Exists)
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_Exists )( 
            INWCVariantDictionary * This,
            /* [in] */ VARIANT vKey,
            /* [retval][out] */ VARIANT_BOOL *pVal);
        
        DECLSPEC_XFGVIRT(INWCVariantDictionary, get_VarType)
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_VarType )( 
            INWCVariantDictionary * This,
            /* [in] */ VARIANT vKey,
            /* [retval][out] */ SHORT *pVal);
        
        END_INTERFACE
    } INWCVariantDictionaryVtbl;

    interface INWCVariantDictionary
    {
        CONST_VTBL struct INWCVariantDictionaryVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define INWCVariantDictionary_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define INWCVariantDictionary_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define INWCVariantDictionary_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define INWCVariantDictionary_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define INWCVariantDictionary_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define INWCVariantDictionary_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define INWCVariantDictionary_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define INWCVariantDictionary_get_Item(This,Key,pVal)	\
    ( (This)->lpVtbl -> get_Item(This,Key,pVal) ) 

#define INWCVariantDictionary_put_Item(This,Key,newVal)	\
    ( (This)->lpVtbl -> put_Item(This,Key,newVal) ) 

#define INWCVariantDictionary_putref_Item(This,Key,newVal)	\
    ( (This)->lpVtbl -> putref_Item(This,Key,newVal) ) 

#define INWCVariantDictionary_get_Key(This,KeyIndex,pVal)	\
    ( (This)->lpVtbl -> get_Key(This,KeyIndex,pVal) ) 

#define INWCVariantDictionary_get_Count(This,pVal)	\
    ( (This)->lpVtbl -> get_Count(This,pVal) ) 

#define INWCVariantDictionary__NewEnum(This,pVal)	\
    ( (This)->lpVtbl -> _NewEnum(This,pVal) ) 

#define INWCVariantDictionary_Remove(This,varKey)	\
    ( (This)->lpVtbl -> Remove(This,varKey) ) 

#define INWCVariantDictionary_RemoveAll(This)	\
    ( (This)->lpVtbl -> RemoveAll(This) ) 

#define INWCVariantDictionary_get_Exists(This,vKey,pVal)	\
    ( (This)->lpVtbl -> get_Exists(This,vKey,pVal) ) 

#define INWCVariantDictionary_get_VarType(This,vKey,pVal)	\
    ( (This)->lpVtbl -> get_VarType(This,vKey,pVal) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __INWCVariantDictionary_INTERFACE_DEFINED__ */


#ifndef __IVariantDictionary2_INTERFACE_DEFINED__
#define __IVariantDictionary2_INTERFACE_DEFINED__

/* interface IVariantDictionary2 */
/* [unique][oleautomation][dual][uuid][object] */ 


EXTERN_C const IID IID_IVariantDictionary2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("4a7deb90-b069-11d0-b373-00a0c90c2bd8")
    IVariantDictionary2 : public IDispatch
    {
    public:
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_Item( 
            /* [in] */ VARIANT Key,
            /* [retval][out] */ VARIANT *pVal) = 0;
        
        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_Item( 
            /* [in] */ VARIANT Key,
            /* [in] */ VARIANT newVal) = 0;
        
        virtual /* [defaultcollelem][id][propputref] */ HRESULT STDMETHODCALLTYPE putref_Item( 
            /* [in] */ VARIANT Key,
            /* [in] */ VARIANT newVal) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Key( 
            /* [in] */ VARIANT KeyIndex,
            /* [retval][out] */ VARIANT *pVal) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Count( 
            /* [retval][out] */ INT *pVal) = 0;
        
        virtual /* [restricted][id] */ HRESULT STDMETHODCALLTYPE _NewEnum( 
            /* [retval][out] */ IUnknown **pVal) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Remove( 
            /* [in] */ VARIANT varKey) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE RemoveAll( void) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IVariantDictionary2Vtbl
    {
        BEGIN_INTERFACE
        
        DECLSPEC_XFGVIRT(IUnknown, QueryInterface)
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IVariantDictionary2 * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        DECLSPEC_XFGVIRT(IUnknown, AddRef)
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IVariantDictionary2 * This);
        
        DECLSPEC_XFGVIRT(IUnknown, Release)
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IVariantDictionary2 * This);
        
        DECLSPEC_XFGVIRT(IDispatch, GetTypeInfoCount)
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IVariantDictionary2 * This,
            /* [out] */ UINT *pctinfo);
        
        DECLSPEC_XFGVIRT(IDispatch, GetTypeInfo)
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IVariantDictionary2 * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        DECLSPEC_XFGVIRT(IDispatch, GetIDsOfNames)
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IVariantDictionary2 * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        DECLSPEC_XFGVIRT(IDispatch, Invoke)
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IVariantDictionary2 * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        DECLSPEC_XFGVIRT(IVariantDictionary2, get_Item)
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Item )( 
            IVariantDictionary2 * This,
            /* [in] */ VARIANT Key,
            /* [retval][out] */ VARIANT *pVal);
        
        DECLSPEC_XFGVIRT(IVariantDictionary2, put_Item)
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Item )( 
            IVariantDictionary2 * This,
            /* [in] */ VARIANT Key,
            /* [in] */ VARIANT newVal);
        
        DECLSPEC_XFGVIRT(IVariantDictionary2, putref_Item)
        /* [defaultcollelem][id][propputref] */ HRESULT ( STDMETHODCALLTYPE *putref_Item )( 
            IVariantDictionary2 * This,
            /* [in] */ VARIANT Key,
            /* [in] */ VARIANT newVal);
        
        DECLSPEC_XFGVIRT(IVariantDictionary2, get_Key)
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_Key )( 
            IVariantDictionary2 * This,
            /* [in] */ VARIANT KeyIndex,
            /* [retval][out] */ VARIANT *pVal);
        
        DECLSPEC_XFGVIRT(IVariantDictionary2, get_Count)
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_Count )( 
            IVariantDictionary2 * This,
            /* [retval][out] */ INT *pVal);
        
        DECLSPEC_XFGVIRT(IVariantDictionary2, _NewEnum)
        /* [restricted][id] */ HRESULT ( STDMETHODCALLTYPE *_NewEnum )( 
            IVariantDictionary2 * This,
            /* [retval][out] */ IUnknown **pVal);
        
        DECLSPEC_XFGVIRT(IVariantDictionary2, Remove)
        HRESULT ( STDMETHODCALLTYPE *Remove )( 
            IVariantDictionary2 * This,
            /* [in] */ VARIANT varKey);
        
        DECLSPEC_XFGVIRT(IVariantDictionary2, RemoveAll)
        HRESULT ( STDMETHODCALLTYPE *RemoveAll )( 
            IVariantDictionary2 * This);
        
        END_INTERFACE
    } IVariantDictionary2Vtbl;

    interface IVariantDictionary2
    {
        CONST_VTBL struct IVariantDictionary2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IVariantDictionary2_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IVariantDictionary2_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IVariantDictionary2_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IVariantDictionary2_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IVariantDictionary2_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IVariantDictionary2_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IVariantDictionary2_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IVariantDictionary2_get_Item(This,Key,pVal)	\
    ( (This)->lpVtbl -> get_Item(This,Key,pVal) ) 

#define IVariantDictionary2_put_Item(This,Key,newVal)	\
    ( (This)->lpVtbl -> put_Item(This,Key,newVal) ) 

#define IVariantDictionary2_putref_Item(This,Key,newVal)	\
    ( (This)->lpVtbl -> putref_Item(This,Key,newVal) ) 

#define IVariantDictionary2_get_Key(This,KeyIndex,pVal)	\
    ( (This)->lpVtbl -> get_Key(This,KeyIndex,pVal) ) 

#define IVariantDictionary2_get_Count(This,pVal)	\
    ( (This)->lpVtbl -> get_Count(This,pVal) ) 

#define IVariantDictionary2__NewEnum(This,pVal)	\
    ( (This)->lpVtbl -> _NewEnum(This,pVal) ) 

#define IVariantDictionary2_Remove(This,varKey)	\
    ( (This)->lpVtbl -> Remove(This,varKey) ) 

#define IVariantDictionary2_RemoveAll(This)	\
    ( (This)->lpVtbl -> RemoveAll(This) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IVariantDictionary2_INTERFACE_DEFINED__ */


#ifndef __IApplicationCache_INTERFACE_DEFINED__
#define __IApplicationCache_INTERFACE_DEFINED__

/* interface IApplicationCache */
/* [unique][oleautomation][dual][uuid][object] */ 


EXTERN_C const IID IID_IApplicationCache;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("837D618E-FBC5-49B5-AF23-21545FFCF97D")
    IApplicationCache : public IDispatch
    {
    public:
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_Value( 
            /* [in] */ BSTR Key,
            /* [retval][out] */ VARIANT *pvar) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_Value( 
            /* [in] */ BSTR Key,
            /* [in] */ VARIANT pvar) = 0;
        
        virtual /* [propputref][id] */ HRESULT STDMETHODCALLTYPE putref_Value( 
            /* [in] */ BSTR Key,
            /* [in] */ VARIANT pvar) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Key( 
            /* [in] */ INT KeyIndex,
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Count( 
            /* [retval][out] */ int *pVal) = 0;
        
        virtual /* [restricted][id] */ HRESULT STDMETHODCALLTYPE _NewEnum( 
            /* [retval][out] */ IUnknown **pVal) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Remove( 
            /* [in] */ BSTR Key) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE RemoveAll( void) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE LockKey( 
            /* [in] */ BSTR Key) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE UnlockKey( 
            /* [in] */ BSTR Key) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE ExpireKeyAt( 
            /* [in] */ BSTR vKey,
            /* [in] */ INT at) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_Exists( 
            /* [in] */ BSTR Key,
            /* [retval][out] */ VARIANT_BOOL *pVal) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_KeyType( 
            /* [in] */ BSTR Key,
            /* [retval][out] */ SHORT *pVal) = 0;
        
        virtual /* [hidden] */ HRESULT STDMETHODCALLTYPE OnStartPage( 
            /* [in] */ IUnknown *p) = 0;
        
        virtual /* [hidden] */ HRESULT STDMETHODCALLTYPE OnEndPage( void) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IApplicationCacheVtbl
    {
        BEGIN_INTERFACE
        
        DECLSPEC_XFGVIRT(IUnknown, QueryInterface)
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IApplicationCache * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        DECLSPEC_XFGVIRT(IUnknown, AddRef)
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IApplicationCache * This);
        
        DECLSPEC_XFGVIRT(IUnknown, Release)
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IApplicationCache * This);
        
        DECLSPEC_XFGVIRT(IDispatch, GetTypeInfoCount)
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IApplicationCache * This,
            /* [out] */ UINT *pctinfo);
        
        DECLSPEC_XFGVIRT(IDispatch, GetTypeInfo)
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IApplicationCache * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        DECLSPEC_XFGVIRT(IDispatch, GetIDsOfNames)
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IApplicationCache * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        DECLSPEC_XFGVIRT(IDispatch, Invoke)
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IApplicationCache * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        DECLSPEC_XFGVIRT(IApplicationCache, get_Value)
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Value )( 
            IApplicationCache * This,
            /* [in] */ BSTR Key,
            /* [retval][out] */ VARIANT *pvar);
        
        DECLSPEC_XFGVIRT(IApplicationCache, put_Value)
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_Value )( 
            IApplicationCache * This,
            /* [in] */ BSTR Key,
            /* [in] */ VARIANT pvar);
        
        DECLSPEC_XFGVIRT(IApplicationCache, putref_Value)
        /* [propputref][id] */ HRESULT ( STDMETHODCALLTYPE *putref_Value )( 
            IApplicationCache * This,
            /* [in] */ BSTR Key,
            /* [in] */ VARIANT pvar);
        
        DECLSPEC_XFGVIRT(IApplicationCache, get_Key)
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_Key )( 
            IApplicationCache * This,
            /* [in] */ INT KeyIndex,
            /* [retval][out] */ BSTR *pVal);
        
        DECLSPEC_XFGVIRT(IApplicationCache, get_Count)
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_Count )( 
            IApplicationCache * This,
            /* [retval][out] */ int *pVal);
        
        DECLSPEC_XFGVIRT(IApplicationCache, _NewEnum)
        /* [restricted][id] */ HRESULT ( STDMETHODCALLTYPE *_NewEnum )( 
            IApplicationCache * This,
            /* [retval][out] */ IUnknown **pVal);
        
        DECLSPEC_XFGVIRT(IApplicationCache, Remove)
        HRESULT ( STDMETHODCALLTYPE *Remove )( 
            IApplicationCache * This,
            /* [in] */ BSTR Key);
        
        DECLSPEC_XFGVIRT(IApplicationCache, RemoveAll)
        HRESULT ( STDMETHODCALLTYPE *RemoveAll )( 
            IApplicationCache * This);
        
        DECLSPEC_XFGVIRT(IApplicationCache, LockKey)
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *LockKey )( 
            IApplicationCache * This,
            /* [in] */ BSTR Key);
        
        DECLSPEC_XFGVIRT(IApplicationCache, UnlockKey)
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *UnlockKey )( 
            IApplicationCache * This,
            /* [in] */ BSTR Key);
        
        DECLSPEC_XFGVIRT(IApplicationCache, ExpireKeyAt)
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *ExpireKeyAt )( 
            IApplicationCache * This,
            /* [in] */ BSTR vKey,
            /* [in] */ INT at);
        
        DECLSPEC_XFGVIRT(IApplicationCache, get_Exists)
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Exists )( 
            IApplicationCache * This,
            /* [in] */ BSTR Key,
            /* [retval][out] */ VARIANT_BOOL *pVal);
        
        DECLSPEC_XFGVIRT(IApplicationCache, get_KeyType)
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_KeyType )( 
            IApplicationCache * This,
            /* [in] */ BSTR Key,
            /* [retval][out] */ SHORT *pVal);
        
        DECLSPEC_XFGVIRT(IApplicationCache, OnStartPage)
        /* [hidden] */ HRESULT ( STDMETHODCALLTYPE *OnStartPage )( 
            IApplicationCache * This,
            /* [in] */ IUnknown *p);
        
        DECLSPEC_XFGVIRT(IApplicationCache, OnEndPage)
        /* [hidden] */ HRESULT ( STDMETHODCALLTYPE *OnEndPage )( 
            IApplicationCache * This);
        
        END_INTERFACE
    } IApplicationCacheVtbl;

    interface IApplicationCache
    {
        CONST_VTBL struct IApplicationCacheVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IApplicationCache_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IApplicationCache_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IApplicationCache_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IApplicationCache_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IApplicationCache_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IApplicationCache_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IApplicationCache_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IApplicationCache_get_Value(This,Key,pvar)	\
    ( (This)->lpVtbl -> get_Value(This,Key,pvar) ) 

#define IApplicationCache_put_Value(This,Key,pvar)	\
    ( (This)->lpVtbl -> put_Value(This,Key,pvar) ) 

#define IApplicationCache_putref_Value(This,Key,pvar)	\
    ( (This)->lpVtbl -> putref_Value(This,Key,pvar) ) 

#define IApplicationCache_get_Key(This,KeyIndex,pVal)	\
    ( (This)->lpVtbl -> get_Key(This,KeyIndex,pVal) ) 

#define IApplicationCache_get_Count(This,pVal)	\
    ( (This)->lpVtbl -> get_Count(This,pVal) ) 

#define IApplicationCache__NewEnum(This,pVal)	\
    ( (This)->lpVtbl -> _NewEnum(This,pVal) ) 

#define IApplicationCache_Remove(This,Key)	\
    ( (This)->lpVtbl -> Remove(This,Key) ) 

#define IApplicationCache_RemoveAll(This)	\
    ( (This)->lpVtbl -> RemoveAll(This) ) 

#define IApplicationCache_LockKey(This,Key)	\
    ( (This)->lpVtbl -> LockKey(This,Key) ) 

#define IApplicationCache_UnlockKey(This,Key)	\
    ( (This)->lpVtbl -> UnlockKey(This,Key) ) 

#define IApplicationCache_ExpireKeyAt(This,vKey,at)	\
    ( (This)->lpVtbl -> ExpireKeyAt(This,vKey,at) ) 

#define IApplicationCache_get_Exists(This,Key,pVal)	\
    ( (This)->lpVtbl -> get_Exists(This,Key,pVal) ) 

#define IApplicationCache_get_KeyType(This,Key,pVal)	\
    ( (This)->lpVtbl -> get_KeyType(This,Key,pVal) ) 

#define IApplicationCache_OnStartPage(This,p)	\
    ( (This)->lpVtbl -> OnStartPage(This,p) ) 

#define IApplicationCache_OnEndPage(This)	\
    ( (This)->lpVtbl -> OnEndPage(This) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IApplicationCache_INTERFACE_DEFINED__ */


#ifndef __INWCSession_INTERFACE_DEFINED__
#define __INWCSession_INTERFACE_DEFINED__

/* interface INWCSession */
/* [unique][oleautomation][dual][uuid][object] */ 


EXTERN_C const IID IID_INWCSession;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("A84D5FFF-6EB4-4211-82E6-045C74F3CB6B")
    INWCSession : public IDispatch
    {
    public:
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_SessionID( 
            /* [retval][out] */ BSTR *pbstrRet) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_Value( 
            /* [in] */ BSTR vkey,
            /* [retval][out] */ VARIANT *pVal) = 0;
        
        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_Value( 
            /* [in] */ BSTR vkey,
            /* [in] */ VARIANT newVal) = 0;
        
        virtual /* [id][propputref] */ HRESULT STDMETHODCALLTYPE putref_Value( 
            /* [in] */ BSTR vkey,
            /* [in] */ VARIANT newVal) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_TimeOut( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_TimeOut( 
            /* [in] */ LONG pVal) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Abandon( void) = 0;
        
        virtual /* [hidden][propget] */ HRESULT STDMETHODCALLTYPE get_CodePage( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [hidden][propput] */ HRESULT STDMETHODCALLTYPE put_CodePage( 
            /* [in] */ LONG newVal) = 0;
        
        virtual /* [hidden][propget] */ HRESULT STDMETHODCALLTYPE get_LCID( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [hidden][propput] */ HRESULT STDMETHODCALLTYPE put_LCID( 
            /* [in] */ LONG newVal) = 0;
        
        virtual /* [hidden][propget] */ HRESULT STDMETHODCALLTYPE get_StaticObjects( 
            /* [retval][out] */ INWCVariantDictionary **ppProperties) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Contents( 
            /* [retval][out] */ INWCVariantDictionary **pVal) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_IsExpired( 
            /* [retval][out] */ VARIANT_BOOL *pVal) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_IsNew( 
            /* [retval][out] */ VARIANT_BOOL *pVal) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_LiquidCookie( 
            /* [retval][out] */ VARIANT_BOOL *pVal) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_LiquidCookie( 
            /* [in] */ VARIANT_BOOL newVal) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_ReEntrance( 
            /* [retval][out] */ VARIANT_BOOL *pVal) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_ReEntrance( 
            /* [in] */ VARIANT_BOOL newVal) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_URL( 
            /* [in] */ VARIANT strCheckA,
            /* [retval][out] */ VARIANT *pVal) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Execute( 
            /* [in] */ BSTR ToPage) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Transfer( 
            /* [in] */ BSTR ToPage) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_CreateInstance( 
            /* [in] */ BSTR progid,
            /* [retval][out] */ IDispatch **pVal) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_Readonly( 
            /* [in] */ VARIANT_BOOL newVal) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Statistics( 
            /* [optional][in] */ VARIANT vAppKey,
            /* [optional][in] */ VARIANT vSessionID,
            /* [retval][out] */ INWCVariantDictionary **retVal) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE EnsureURLCookie( void) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_OldSessionID( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [hidden] */ HRESULT STDMETHODCALLTYPE OnStartPage( 
            /* [in] */ IUnknown *p) = 0;
        
        virtual /* [hidden] */ HRESULT STDMETHODCALLTYPE OnEndPage( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Initialize( 
            /* [in] */ IDispatch *iRequest,
            /* [in] */ IDispatch *iSErver,
            /* [in] */ IDispatch *iResponse) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct INWCSessionVtbl
    {
        BEGIN_INTERFACE
        
        DECLSPEC_XFGVIRT(IUnknown, QueryInterface)
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            INWCSession * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        DECLSPEC_XFGVIRT(IUnknown, AddRef)
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            INWCSession * This);
        
        DECLSPEC_XFGVIRT(IUnknown, Release)
        ULONG ( STDMETHODCALLTYPE *Release )( 
            INWCSession * This);
        
        DECLSPEC_XFGVIRT(IDispatch, GetTypeInfoCount)
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            INWCSession * This,
            /* [out] */ UINT *pctinfo);
        
        DECLSPEC_XFGVIRT(IDispatch, GetTypeInfo)
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            INWCSession * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        DECLSPEC_XFGVIRT(IDispatch, GetIDsOfNames)
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            INWCSession * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        DECLSPEC_XFGVIRT(IDispatch, Invoke)
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            INWCSession * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        DECLSPEC_XFGVIRT(INWCSession, get_SessionID)
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_SessionID )( 
            INWCSession * This,
            /* [retval][out] */ BSTR *pbstrRet);
        
        DECLSPEC_XFGVIRT(INWCSession, get_Value)
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Value )( 
            INWCSession * This,
            /* [in] */ BSTR vkey,
            /* [retval][out] */ VARIANT *pVal);
        
        DECLSPEC_XFGVIRT(INWCSession, put_Value)
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Value )( 
            INWCSession * This,
            /* [in] */ BSTR vkey,
            /* [in] */ VARIANT newVal);
        
        DECLSPEC_XFGVIRT(INWCSession, putref_Value)
        /* [id][propputref] */ HRESULT ( STDMETHODCALLTYPE *putref_Value )( 
            INWCSession * This,
            /* [in] */ BSTR vkey,
            /* [in] */ VARIANT newVal);
        
        DECLSPEC_XFGVIRT(INWCSession, get_TimeOut)
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_TimeOut )( 
            INWCSession * This,
            /* [retval][out] */ LONG *pVal);
        
        DECLSPEC_XFGVIRT(INWCSession, put_TimeOut)
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE *put_TimeOut )( 
            INWCSession * This,
            /* [in] */ LONG pVal);
        
        DECLSPEC_XFGVIRT(INWCSession, Abandon)
        HRESULT ( STDMETHODCALLTYPE *Abandon )( 
            INWCSession * This);
        
        DECLSPEC_XFGVIRT(INWCSession, get_CodePage)
        /* [hidden][propget] */ HRESULT ( STDMETHODCALLTYPE *get_CodePage )( 
            INWCSession * This,
            /* [retval][out] */ LONG *pVal);
        
        DECLSPEC_XFGVIRT(INWCSession, put_CodePage)
        /* [hidden][propput] */ HRESULT ( STDMETHODCALLTYPE *put_CodePage )( 
            INWCSession * This,
            /* [in] */ LONG newVal);
        
        DECLSPEC_XFGVIRT(INWCSession, get_LCID)
        /* [hidden][propget] */ HRESULT ( STDMETHODCALLTYPE *get_LCID )( 
            INWCSession * This,
            /* [retval][out] */ LONG *pVal);
        
        DECLSPEC_XFGVIRT(INWCSession, put_LCID)
        /* [hidden][propput] */ HRESULT ( STDMETHODCALLTYPE *put_LCID )( 
            INWCSession * This,
            /* [in] */ LONG newVal);
        
        DECLSPEC_XFGVIRT(INWCSession, get_StaticObjects)
        /* [hidden][propget] */ HRESULT ( STDMETHODCALLTYPE *get_StaticObjects )( 
            INWCSession * This,
            /* [retval][out] */ INWCVariantDictionary **ppProperties);
        
        DECLSPEC_XFGVIRT(INWCSession, get_Contents)
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_Contents )( 
            INWCSession * This,
            /* [retval][out] */ INWCVariantDictionary **pVal);
        
        DECLSPEC_XFGVIRT(INWCSession, get_IsExpired)
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_IsExpired )( 
            INWCSession * This,
            /* [retval][out] */ VARIANT_BOOL *pVal);
        
        DECLSPEC_XFGVIRT(INWCSession, get_IsNew)
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_IsNew )( 
            INWCSession * This,
            /* [retval][out] */ VARIANT_BOOL *pVal);
        
        DECLSPEC_XFGVIRT(INWCSession, get_LiquidCookie)
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_LiquidCookie )( 
            INWCSession * This,
            /* [retval][out] */ VARIANT_BOOL *pVal);
        
        DECLSPEC_XFGVIRT(INWCSession, put_LiquidCookie)
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE *put_LiquidCookie )( 
            INWCSession * This,
            /* [in] */ VARIANT_BOOL newVal);
        
        DECLSPEC_XFGVIRT(INWCSession, get_ReEntrance)
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_ReEntrance )( 
            INWCSession * This,
            /* [retval][out] */ VARIANT_BOOL *pVal);
        
        DECLSPEC_XFGVIRT(INWCSession, put_ReEntrance)
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE *put_ReEntrance )( 
            INWCSession * This,
            /* [in] */ VARIANT_BOOL newVal);
        
        DECLSPEC_XFGVIRT(INWCSession, get_URL)
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_URL )( 
            INWCSession * This,
            /* [in] */ VARIANT strCheckA,
            /* [retval][out] */ VARIANT *pVal);
        
        DECLSPEC_XFGVIRT(INWCSession, Execute)
        HRESULT ( STDMETHODCALLTYPE *Execute )( 
            INWCSession * This,
            /* [in] */ BSTR ToPage);
        
        DECLSPEC_XFGVIRT(INWCSession, Transfer)
        HRESULT ( STDMETHODCALLTYPE *Transfer )( 
            INWCSession * This,
            /* [in] */ BSTR ToPage);
        
        DECLSPEC_XFGVIRT(INWCSession, get_CreateInstance)
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_CreateInstance )( 
            INWCSession * This,
            /* [in] */ BSTR progid,
            /* [retval][out] */ IDispatch **pVal);
        
        DECLSPEC_XFGVIRT(INWCSession, put_Readonly)
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE *put_Readonly )( 
            INWCSession * This,
            /* [in] */ VARIANT_BOOL newVal);
        
        DECLSPEC_XFGVIRT(INWCSession, Statistics)
        HRESULT ( STDMETHODCALLTYPE *Statistics )( 
            INWCSession * This,
            /* [optional][in] */ VARIANT vAppKey,
            /* [optional][in] */ VARIANT vSessionID,
            /* [retval][out] */ INWCVariantDictionary **retVal);
        
        DECLSPEC_XFGVIRT(INWCSession, EnsureURLCookie)
        HRESULT ( STDMETHODCALLTYPE *EnsureURLCookie )( 
            INWCSession * This);
        
        DECLSPEC_XFGVIRT(INWCSession, get_OldSessionID)
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_OldSessionID )( 
            INWCSession * This,
            /* [retval][out] */ BSTR *pVal);
        
        DECLSPEC_XFGVIRT(INWCSession, OnStartPage)
        /* [hidden] */ HRESULT ( STDMETHODCALLTYPE *OnStartPage )( 
            INWCSession * This,
            /* [in] */ IUnknown *p);
        
        DECLSPEC_XFGVIRT(INWCSession, OnEndPage)
        /* [hidden] */ HRESULT ( STDMETHODCALLTYPE *OnEndPage )( 
            INWCSession * This);
        
        DECLSPEC_XFGVIRT(INWCSession, Initialize)
        HRESULT ( STDMETHODCALLTYPE *Initialize )( 
            INWCSession * This,
            /* [in] */ IDispatch *iRequest,
            /* [in] */ IDispatch *iSErver,
            /* [in] */ IDispatch *iResponse);
        
        END_INTERFACE
    } INWCSessionVtbl;

    interface INWCSession
    {
        CONST_VTBL struct INWCSessionVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define INWCSession_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define INWCSession_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define INWCSession_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define INWCSession_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define INWCSession_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define INWCSession_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define INWCSession_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define INWCSession_get_SessionID(This,pbstrRet)	\
    ( (This)->lpVtbl -> get_SessionID(This,pbstrRet) ) 

#define INWCSession_get_Value(This,vkey,pVal)	\
    ( (This)->lpVtbl -> get_Value(This,vkey,pVal) ) 

#define INWCSession_put_Value(This,vkey,newVal)	\
    ( (This)->lpVtbl -> put_Value(This,vkey,newVal) ) 

#define INWCSession_putref_Value(This,vkey,newVal)	\
    ( (This)->lpVtbl -> putref_Value(This,vkey,newVal) ) 

#define INWCSession_get_TimeOut(This,pVal)	\
    ( (This)->lpVtbl -> get_TimeOut(This,pVal) ) 

#define INWCSession_put_TimeOut(This,pVal)	\
    ( (This)->lpVtbl -> put_TimeOut(This,pVal) ) 

#define INWCSession_Abandon(This)	\
    ( (This)->lpVtbl -> Abandon(This) ) 

#define INWCSession_get_CodePage(This,pVal)	\
    ( (This)->lpVtbl -> get_CodePage(This,pVal) ) 

#define INWCSession_put_CodePage(This,newVal)	\
    ( (This)->lpVtbl -> put_CodePage(This,newVal) ) 

#define INWCSession_get_LCID(This,pVal)	\
    ( (This)->lpVtbl -> get_LCID(This,pVal) ) 

#define INWCSession_put_LCID(This,newVal)	\
    ( (This)->lpVtbl -> put_LCID(This,newVal) ) 

#define INWCSession_get_StaticObjects(This,ppProperties)	\
    ( (This)->lpVtbl -> get_StaticObjects(This,ppProperties) ) 

#define INWCSession_get_Contents(This,pVal)	\
    ( (This)->lpVtbl -> get_Contents(This,pVal) ) 

#define INWCSession_get_IsExpired(This,pVal)	\
    ( (This)->lpVtbl -> get_IsExpired(This,pVal) ) 

#define INWCSession_get_IsNew(This,pVal)	\
    ( (This)->lpVtbl -> get_IsNew(This,pVal) ) 

#define INWCSession_get_LiquidCookie(This,pVal)	\
    ( (This)->lpVtbl -> get_LiquidCookie(This,pVal) ) 

#define INWCSession_put_LiquidCookie(This,newVal)	\
    ( (This)->lpVtbl -> put_LiquidCookie(This,newVal) ) 

#define INWCSession_get_ReEntrance(This,pVal)	\
    ( (This)->lpVtbl -> get_ReEntrance(This,pVal) ) 

#define INWCSession_put_ReEntrance(This,newVal)	\
    ( (This)->lpVtbl -> put_ReEntrance(This,newVal) ) 

#define INWCSession_get_URL(This,strCheckA,pVal)	\
    ( (This)->lpVtbl -> get_URL(This,strCheckA,pVal) ) 

#define INWCSession_Execute(This,ToPage)	\
    ( (This)->lpVtbl -> Execute(This,ToPage) ) 

#define INWCSession_Transfer(This,ToPage)	\
    ( (This)->lpVtbl -> Transfer(This,ToPage) ) 

#define INWCSession_get_CreateInstance(This,progid,pVal)	\
    ( (This)->lpVtbl -> get_CreateInstance(This,progid,pVal) ) 

#define INWCSession_put_Readonly(This,newVal)	\
    ( (This)->lpVtbl -> put_Readonly(This,newVal) ) 

#define INWCSession_Statistics(This,vAppKey,vSessionID,retVal)	\
    ( (This)->lpVtbl -> Statistics(This,vAppKey,vSessionID,retVal) ) 

#define INWCSession_EnsureURLCookie(This)	\
    ( (This)->lpVtbl -> EnsureURLCookie(This) ) 

#define INWCSession_get_OldSessionID(This,pVal)	\
    ( (This)->lpVtbl -> get_OldSessionID(This,pVal) ) 

#define INWCSession_OnStartPage(This,p)	\
    ( (This)->lpVtbl -> OnStartPage(This,p) ) 

#define INWCSession_OnEndPage(This)	\
    ( (This)->lpVtbl -> OnEndPage(This) ) 

#define INWCSession_Initialize(This,iRequest,iSErver,iResponse)	\
    ( (This)->lpVtbl -> Initialize(This,iRequest,iSErver,iResponse) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __INWCSession_INTERFACE_DEFINED__ */



#ifndef __ISPCSession_LIBRARY_DEFINED__
#define __ISPCSession_LIBRARY_DEFINED__

/* library ISPCSession */
/* [helpstring][uuid][version] */ 


EXTERN_C const IID LIBID_ISPCSession;

EXTERN_C const CLSID CLSID_CVariantDictionary;

#ifdef __cplusplus

class DECLSPEC_UUID("94764678-7F98-4CD8-88E9-FA83D776135C")
CVariantDictionary;
#endif

EXTERN_C const CLSID CLSID_NWCSession;

#ifdef __cplusplus

class DECLSPEC_UUID("D64ABC73-3B0E-4E85-92CD-215B03CAC996")
NWCSession;
#endif

EXTERN_C const CLSID CLSID_NWCApplication;

#ifdef __cplusplus

class DECLSPEC_UUID("D64ABC73-3B0E-4E85-92CD-215B03CAC998")
NWCApplication;
#endif
#endif /* __ISPCSession_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

unsigned long             __RPC_USER  BSTR_UserSize(     unsigned long *, unsigned long            , BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserMarshal(  unsigned long *, unsigned char *, BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserUnmarshal(unsigned long *, unsigned char *, BSTR * ); 
void                      __RPC_USER  BSTR_UserFree(     unsigned long *, BSTR * ); 

unsigned long             __RPC_USER  VARIANT_UserSize(     unsigned long *, unsigned long            , VARIANT * ); 
unsigned char * __RPC_USER  VARIANT_UserMarshal(  unsigned long *, unsigned char *, VARIANT * ); 
unsigned char * __RPC_USER  VARIANT_UserUnmarshal(unsigned long *, unsigned char *, VARIANT * ); 
void                      __RPC_USER  VARIANT_UserFree(     unsigned long *, VARIANT * ); 

unsigned long             __RPC_USER  BSTR_UserSize64(     unsigned long *, unsigned long            , BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserMarshal64(  unsigned long *, unsigned char *, BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserUnmarshal64(unsigned long *, unsigned char *, BSTR * ); 
void                      __RPC_USER  BSTR_UserFree64(     unsigned long *, BSTR * ); 

unsigned long             __RPC_USER  VARIANT_UserSize64(     unsigned long *, unsigned long            , VARIANT * ); 
unsigned char * __RPC_USER  VARIANT_UserMarshal64(  unsigned long *, unsigned char *, VARIANT * ); 
unsigned char * __RPC_USER  VARIANT_UserUnmarshal64(unsigned long *, unsigned char *, VARIANT * ); 
void                      __RPC_USER  VARIANT_UserFree64(     unsigned long *, VARIANT * ); 

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


