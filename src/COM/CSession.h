

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 8.00.0603 */
/* at Thu Dec 01 10:18:28 2016
 */
/* Compiler settings for CSession.idl:
    Oicf, W1, Zp8, env=Win32 (32b run), target_arch=X86 8.00.0603 
    protocol : dce , ms_ext, app_config, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */

#pragma warning( disable: 4049 )  /* more than 64k source lines */


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 475
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __CSession_h__
#define __CSession_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
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


#ifndef __ISessionObject2_FWD_DEFINED__
#define __ISessionObject2_FWD_DEFINED__
typedef interface ISessionObject2 ISessionObject2;

#endif 	/* __ISessionObject2_FWD_DEFINED__ */


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
        virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Item( 
            /* [in] */ VARIANT varKey,
            /* [retval][out] */ VARIANT *pVal) = 0;
        
        virtual /* [id][helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_Item( 
            /* [in] */ VARIANT varKey,
            /* [in] */ VARIANT newVal) = 0;
        
        virtual /* [defaultcollelem][id][helpstring][propputref] */ HRESULT STDMETHODCALLTYPE putref_Item( 
            /* [in] */ VARIANT varKey,
            /* [in] */ VARIANT newVal) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Key( 
            /* [in] */ VARIANT KeyIndex,
            /* [retval][out] */ VARIANT *pVal) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Count( 
            /* [retval][out] */ int *pVal) = 0;
        
        virtual /* [restricted][id] */ HRESULT STDMETHODCALLTYPE _NewEnum( 
            /* [retval][out] */ IUnknown **pVal) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Remove( 
            /* [in] */ VARIANT varKey) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE RemoveAll( void) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Exists( 
            /* [in] */ VARIANT vKey,
            /* [retval][out] */ VARIANT_BOOL *pVal) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_VarType( 
            /* [in] */ VARIANT vKey,
            /* [retval][out] */ SHORT *pVal) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_CaseSensitive( 
            /* [retval][out] */ VARIANT_BOOL *pVal) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_CaseSensitive( 
            /* [in] */ VARIANT_BOOL newVal) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Persist( 
            VARIANT varKey) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct INWCVariantDictionaryVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            INWCVariantDictionary * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            INWCVariantDictionary * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            INWCVariantDictionary * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            INWCVariantDictionary * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            INWCVariantDictionary * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            INWCVariantDictionary * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
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
        
        /* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Item )( 
            INWCVariantDictionary * This,
            /* [in] */ VARIANT varKey,
            /* [retval][out] */ VARIANT *pVal);
        
        /* [id][helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Item )( 
            INWCVariantDictionary * This,
            /* [in] */ VARIANT varKey,
            /* [in] */ VARIANT newVal);
        
        /* [defaultcollelem][id][helpstring][propputref] */ HRESULT ( STDMETHODCALLTYPE *putref_Item )( 
            INWCVariantDictionary * This,
            /* [in] */ VARIANT varKey,
            /* [in] */ VARIANT newVal);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Key )( 
            INWCVariantDictionary * This,
            /* [in] */ VARIANT KeyIndex,
            /* [retval][out] */ VARIANT *pVal);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Count )( 
            INWCVariantDictionary * This,
            /* [retval][out] */ int *pVal);
        
        /* [restricted][id] */ HRESULT ( STDMETHODCALLTYPE *_NewEnum )( 
            INWCVariantDictionary * This,
            /* [retval][out] */ IUnknown **pVal);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *Remove )( 
            INWCVariantDictionary * This,
            /* [in] */ VARIANT varKey);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *RemoveAll )( 
            INWCVariantDictionary * This);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Exists )( 
            INWCVariantDictionary * This,
            /* [in] */ VARIANT vKey,
            /* [retval][out] */ VARIANT_BOOL *pVal);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_VarType )( 
            INWCVariantDictionary * This,
            /* [in] */ VARIANT vKey,
            /* [retval][out] */ SHORT *pVal);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_CaseSensitive )( 
            INWCVariantDictionary * This,
            /* [retval][out] */ VARIANT_BOOL *pVal);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE *put_CaseSensitive )( 
            INWCVariantDictionary * This,
            /* [in] */ VARIANT_BOOL newVal);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *Persist )( 
            INWCVariantDictionary * This,
            VARIANT varKey);
        
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


#define INWCVariantDictionary_get_Item(This,varKey,pVal)	\
    ( (This)->lpVtbl -> get_Item(This,varKey,pVal) ) 

#define INWCVariantDictionary_put_Item(This,varKey,newVal)	\
    ( (This)->lpVtbl -> put_Item(This,varKey,newVal) ) 

#define INWCVariantDictionary_putref_Item(This,varKey,newVal)	\
    ( (This)->lpVtbl -> putref_Item(This,varKey,newVal) ) 

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

#define INWCVariantDictionary_get_CaseSensitive(This,pVal)	\
    ( (This)->lpVtbl -> get_CaseSensitive(This,pVal) ) 

#define INWCVariantDictionary_put_CaseSensitive(This,newVal)	\
    ( (This)->lpVtbl -> put_CaseSensitive(This,newVal) ) 

#define INWCVariantDictionary_Persist(This,varKey)	\
    ( (This)->lpVtbl -> Persist(This,varKey) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __INWCVariantDictionary_INTERFACE_DEFINED__ */


#ifndef __IVariantDictionary2_INTERFACE_DEFINED__
#define __IVariantDictionary2_INTERFACE_DEFINED__

/* interface IVariantDictionary2 */
/* [object][oleautomation][hidden][uuid] */ 


EXTERN_C const IID IID_IVariantDictionary2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("4A7DEB90-B069-11D0-B373-00A0C90C2BD8")
    IVariantDictionary2 : public IUnknown
    {
    public:
        virtual /* [restricted] */ HRESULT STDMETHODCALLTYPE GetTypeInfoCount2( 
            UINT *pctinfo) = 0;
        
        virtual /* [restricted] */ HRESULT STDMETHODCALLTYPE GetTypeInfo2( 
            UINT iTInfo,
            LCID lcid,
            UINT **ppTInfo) = 0;
        
        virtual /* [restricted] */ HRESULT STDMETHODCALLTYPE GetIDsOfNames2( 
            UINT *riid,
            LPOLESTR *rgszNames,
            UINT cNames,
            LCID lcid,
            DISPID *rgDispId) = 0;
        
        virtual /* [restricted] */ HRESULT STDMETHODCALLTYPE Invoke2( 
            DISPID dispIdMember,
            UINT *riid,
            LCID lcid,
            WORD wFlags,
            UINT *pDispParams,
            VARIANT *pVarResult,
            UINT *pExcepInfo,
            UINT *puArgErr) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Item2( 
            /* [in] */ VARIANT VarKey,
            /* [retval][out] */ VARIANT *pvar) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_Item2( 
            /* [in] */ VARIANT VarKey,
            /* [in] */ VARIANT pvar) = 0;
        
        virtual /* [propputref] */ HRESULT STDMETHODCALLTYPE putref_Item2( 
            /* [in] */ VARIANT VarKey,
            /* [in] */ VARIANT pvar) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Key2( 
            /* [in] */ VARIANT VarKey,
            /* [retval][out] */ VARIANT *pvar) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Count2( 
            /* [retval][out] */ int *cStrRet) = 0;
        
        virtual /* [restricted][propget] */ HRESULT STDMETHODCALLTYPE get__NewEnum2( 
            /* [retval][out] */ IUnknown **ppEnumReturn) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Remove2( 
            /* [in] */ VARIANT VarKey) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE RemoveAll2( void) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IVariantDictionary2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IVariantDictionary2 * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IVariantDictionary2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IVariantDictionary2 * This);
        
        /* [restricted] */ HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount2 )( 
            IVariantDictionary2 * This,
            UINT *pctinfo);
        
        /* [restricted] */ HRESULT ( STDMETHODCALLTYPE *GetTypeInfo2 )( 
            IVariantDictionary2 * This,
            UINT iTInfo,
            LCID lcid,
            UINT **ppTInfo);
        
        /* [restricted] */ HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames2 )( 
            IVariantDictionary2 * This,
            UINT *riid,
            LPOLESTR *rgszNames,
            UINT cNames,
            LCID lcid,
            DISPID *rgDispId);
        
        /* [restricted] */ HRESULT ( STDMETHODCALLTYPE *Invoke2 )( 
            IVariantDictionary2 * This,
            DISPID dispIdMember,
            UINT *riid,
            LCID lcid,
            WORD wFlags,
            UINT *pDispParams,
            VARIANT *pVarResult,
            UINT *pExcepInfo,
            UINT *puArgErr);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_Item2 )( 
            IVariantDictionary2 * This,
            /* [in] */ VARIANT VarKey,
            /* [retval][out] */ VARIANT *pvar);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE *put_Item2 )( 
            IVariantDictionary2 * This,
            /* [in] */ VARIANT VarKey,
            /* [in] */ VARIANT pvar);
        
        /* [propputref] */ HRESULT ( STDMETHODCALLTYPE *putref_Item2 )( 
            IVariantDictionary2 * This,
            /* [in] */ VARIANT VarKey,
            /* [in] */ VARIANT pvar);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_Key2 )( 
            IVariantDictionary2 * This,
            /* [in] */ VARIANT VarKey,
            /* [retval][out] */ VARIANT *pvar);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_Count2 )( 
            IVariantDictionary2 * This,
            /* [retval][out] */ int *cStrRet);
        
        /* [restricted][propget] */ HRESULT ( STDMETHODCALLTYPE *get__NewEnum2 )( 
            IVariantDictionary2 * This,
            /* [retval][out] */ IUnknown **ppEnumReturn);
        
        HRESULT ( STDMETHODCALLTYPE *Remove2 )( 
            IVariantDictionary2 * This,
            /* [in] */ VARIANT VarKey);
        
        HRESULT ( STDMETHODCALLTYPE *RemoveAll2 )( 
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


#define IVariantDictionary2_GetTypeInfoCount2(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount2(This,pctinfo) ) 

#define IVariantDictionary2_GetTypeInfo2(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo2(This,iTInfo,lcid,ppTInfo) ) 

#define IVariantDictionary2_GetIDsOfNames2(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames2(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IVariantDictionary2_Invoke2(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke2(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#define IVariantDictionary2_get_Item2(This,VarKey,pvar)	\
    ( (This)->lpVtbl -> get_Item2(This,VarKey,pvar) ) 

#define IVariantDictionary2_put_Item2(This,VarKey,pvar)	\
    ( (This)->lpVtbl -> put_Item2(This,VarKey,pvar) ) 

#define IVariantDictionary2_putref_Item2(This,VarKey,pvar)	\
    ( (This)->lpVtbl -> putref_Item2(This,VarKey,pvar) ) 

#define IVariantDictionary2_get_Key2(This,VarKey,pvar)	\
    ( (This)->lpVtbl -> get_Key2(This,VarKey,pvar) ) 

#define IVariantDictionary2_get_Count2(This,cStrRet)	\
    ( (This)->lpVtbl -> get_Count2(This,cStrRet) ) 

#define IVariantDictionary2_get__NewEnum2(This,ppEnumReturn)	\
    ( (This)->lpVtbl -> get__NewEnum2(This,ppEnumReturn) ) 

#define IVariantDictionary2_Remove2(This,VarKey)	\
    ( (This)->lpVtbl -> Remove2(This,VarKey) ) 

#define IVariantDictionary2_RemoveAll2(This)	\
    ( (This)->lpVtbl -> RemoveAll2(This) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IVariantDictionary2_INTERFACE_DEFINED__ */


#ifndef __ISessionObject2_INTERFACE_DEFINED__
#define __ISessionObject2_INTERFACE_DEFINED__

/* interface ISessionObject2 */
/* [object][oleautomation][hidden][uuid] */ 


EXTERN_C const IID IID_ISessionObject2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("D97A6DA0-A865-11CF-83AF-00A0C90C2BD8")
    ISessionObject2 : public IUnknown
    {
    public:
        virtual /* [restricted] */ HRESULT STDMETHODCALLTYPE GetTypeInfoCount2( 
            UINT *pctinfo) = 0;
        
        virtual /* [restricted] */ HRESULT STDMETHODCALLTYPE GetTypeInfo2( 
            UINT iTInfo,
            LCID lcid,
            UINT **ppTInfo) = 0;
        
        virtual /* [restricted] */ HRESULT STDMETHODCALLTYPE GetIDsOfNames2( 
            UINT *riid,
            LPOLESTR *rgszNames,
            UINT cNames,
            LCID lcid,
            DISPID *rgDispId) = 0;
        
        virtual /* [restricted] */ HRESULT STDMETHODCALLTYPE Invoke2( 
            DISPID dispIdMember,
            UINT *riid,
            LCID lcid,
            WORD wFlags,
            UINT *pDispParams,
            VARIANT *pVarResult,
            UINT *pExcepInfo,
            UINT *puArgErr) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Value2( 
            /* [in] */ BSTR bstrValue,
            /* [retval][out] */ VARIANT *pvar) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_Value2( 
            /* [in] */ BSTR bstrValue,
            /* [in] */ VARIANT pvar) = 0;
        
        virtual /* [propputref] */ HRESULT STDMETHODCALLTYPE putref_Value2( 
            /* [in] */ BSTR bstrValue,
            /* [in] */ VARIANT pvar) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Timeout2( 
            /* [retval][out] */ LONG *plvar) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_Timeout2( 
            /* [in] */ LONG plvar) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Abandon2( void) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_CodePage2( 
            /* [retval][out] */ LONG *plvar) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_CodePage2( 
            /* [in] */ LONG plvar) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_LCID2( 
            /* [retval][out] */ LONG *plvar) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_LCID2( 
            /* [in] */ LONG plvar) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_StaticObjects2( 
            /* [retval][out] */ IVariantDictionary2 **ppTaggedObjects) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Contents2( 
            /* [retval][out] */ IVariantDictionary2 **ppProperties) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct ISessionObject2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ISessionObject2 * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ISessionObject2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ISessionObject2 * This);
        
        /* [restricted] */ HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount2 )( 
            ISessionObject2 * This,
            UINT *pctinfo);
        
        /* [restricted] */ HRESULT ( STDMETHODCALLTYPE *GetTypeInfo2 )( 
            ISessionObject2 * This,
            UINT iTInfo,
            LCID lcid,
            UINT **ppTInfo);
        
        /* [restricted] */ HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames2 )( 
            ISessionObject2 * This,
            UINT *riid,
            LPOLESTR *rgszNames,
            UINT cNames,
            LCID lcid,
            DISPID *rgDispId);
        
        /* [restricted] */ HRESULT ( STDMETHODCALLTYPE *Invoke2 )( 
            ISessionObject2 * This,
            DISPID dispIdMember,
            UINT *riid,
            LCID lcid,
            WORD wFlags,
            UINT *pDispParams,
            VARIANT *pVarResult,
            UINT *pExcepInfo,
            UINT *puArgErr);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_Value2 )( 
            ISessionObject2 * This,
            /* [in] */ BSTR bstrValue,
            /* [retval][out] */ VARIANT *pvar);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE *put_Value2 )( 
            ISessionObject2 * This,
            /* [in] */ BSTR bstrValue,
            /* [in] */ VARIANT pvar);
        
        /* [propputref] */ HRESULT ( STDMETHODCALLTYPE *putref_Value2 )( 
            ISessionObject2 * This,
            /* [in] */ BSTR bstrValue,
            /* [in] */ VARIANT pvar);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_Timeout2 )( 
            ISessionObject2 * This,
            /* [retval][out] */ LONG *plvar);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE *put_Timeout2 )( 
            ISessionObject2 * This,
            /* [in] */ LONG plvar);
        
        HRESULT ( STDMETHODCALLTYPE *Abandon2 )( 
            ISessionObject2 * This);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_CodePage2 )( 
            ISessionObject2 * This,
            /* [retval][out] */ LONG *plvar);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE *put_CodePage2 )( 
            ISessionObject2 * This,
            /* [in] */ LONG plvar);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_LCID2 )( 
            ISessionObject2 * This,
            /* [retval][out] */ LONG *plvar);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE *put_LCID2 )( 
            ISessionObject2 * This,
            /* [in] */ LONG plvar);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_StaticObjects2 )( 
            ISessionObject2 * This,
            /* [retval][out] */ IVariantDictionary2 **ppTaggedObjects);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_Contents2 )( 
            ISessionObject2 * This,
            /* [retval][out] */ IVariantDictionary2 **ppProperties);
        
        END_INTERFACE
    } ISessionObject2Vtbl;

    interface ISessionObject2
    {
        CONST_VTBL struct ISessionObject2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ISessionObject2_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define ISessionObject2_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define ISessionObject2_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define ISessionObject2_GetTypeInfoCount2(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount2(This,pctinfo) ) 

#define ISessionObject2_GetTypeInfo2(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo2(This,iTInfo,lcid,ppTInfo) ) 

#define ISessionObject2_GetIDsOfNames2(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames2(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define ISessionObject2_Invoke2(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke2(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#define ISessionObject2_get_Value2(This,bstrValue,pvar)	\
    ( (This)->lpVtbl -> get_Value2(This,bstrValue,pvar) ) 

#define ISessionObject2_put_Value2(This,bstrValue,pvar)	\
    ( (This)->lpVtbl -> put_Value2(This,bstrValue,pvar) ) 

#define ISessionObject2_putref_Value2(This,bstrValue,pvar)	\
    ( (This)->lpVtbl -> putref_Value2(This,bstrValue,pvar) ) 

#define ISessionObject2_get_Timeout2(This,plvar)	\
    ( (This)->lpVtbl -> get_Timeout2(This,plvar) ) 

#define ISessionObject2_put_Timeout2(This,plvar)	\
    ( (This)->lpVtbl -> put_Timeout2(This,plvar) ) 

#define ISessionObject2_Abandon2(This)	\
    ( (This)->lpVtbl -> Abandon2(This) ) 

#define ISessionObject2_get_CodePage2(This,plvar)	\
    ( (This)->lpVtbl -> get_CodePage2(This,plvar) ) 

#define ISessionObject2_put_CodePage2(This,plvar)	\
    ( (This)->lpVtbl -> put_CodePage2(This,plvar) ) 

#define ISessionObject2_get_LCID2(This,plvar)	\
    ( (This)->lpVtbl -> get_LCID2(This,plvar) ) 

#define ISessionObject2_put_LCID2(This,plvar)	\
    ( (This)->lpVtbl -> put_LCID2(This,plvar) ) 

#define ISessionObject2_get_StaticObjects2(This,ppTaggedObjects)	\
    ( (This)->lpVtbl -> get_StaticObjects2(This,ppTaggedObjects) ) 

#define ISessionObject2_get_Contents2(This,ppProperties)	\
    ( (This)->lpVtbl -> get_Contents2(This,ppProperties) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __ISessionObject2_INTERFACE_DEFINED__ */


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
        virtual HRESULT STDMETHODCALLTYPE OnStartPage( 
            /* [in] */ IUnknown *p) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE OnEndPage( void) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_SessionID( 
            /* [retval][out] */ BSTR *pbstrRet) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Value( 
            /* [in] */ BSTR vkey,
            /* [retval][out] */ VARIANT *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Value( 
            /* [in] */ BSTR vkey,
            /* [in] */ VARIANT newVal) = 0;
        
        virtual /* [helpstring][id][propputref] */ HRESULT STDMETHODCALLTYPE putref_Value( 
            /* [in] */ BSTR vkey,
            /* [in] */ VARIANT newVal) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_TimeOut( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_TimeOut( 
            /* [in] */ LONG pVal) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Abandon( void) = 0;
        
        virtual /* [helpstring][hidden][propget] */ HRESULT STDMETHODCALLTYPE get_CodePage( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [helpstring][hidden][propput] */ HRESULT STDMETHODCALLTYPE put_CodePage( 
            /* [in] */ LONG newVal) = 0;
        
        virtual /* [helpstring][hidden][propget] */ HRESULT STDMETHODCALLTYPE get_LCID( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [helpstring][hidden][propput] */ HRESULT STDMETHODCALLTYPE put_LCID( 
            /* [in] */ LONG newVal) = 0;
        
        virtual /* [helpstring][hidden][propget] */ HRESULT STDMETHODCALLTYPE get_StaticObjects( 
            /* [retval][out] */ INWCVariantDictionary **ppProperties) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Contents( 
            /* [retval][out] */ INWCVariantDictionary **pVal) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_IsExpired( 
            /* [retval][out] */ VARIANT_BOOL *pVal) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_IsNew( 
            /* [retval][out] */ VARIANT_BOOL *pVal) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_LiquidCookie( 
            /* [retval][out] */ VARIANT_BOOL *pVal) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_LiquidCookie( 
            /* [in] */ VARIANT_BOOL newVal) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_ReEntrance( 
            /* [retval][out] */ VARIANT_BOOL *pVal) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_ReEntrance( 
            /* [in] */ VARIANT_BOOL newVal) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_URL( 
            /* [in] */ VARIANT strCheckA,
            /* [retval][out] */ VARIANT *pVal) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Execute( 
            /* [in] */ BSTR ToPage) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Transfer( 
            /* [in] */ BSTR ToPage) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_CreateInstance( 
            /* [in] */ BSTR progid,
            /* [retval][out] */ IDispatch **pVal) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_Readonly( 
            /* [in] */ VARIANT_BOOL newVal) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Statistics( 
            /* [optional][in] */ VARIANT vAppKey,
            /* [optional][in] */ VARIANT vSessionID,
            /* [retval][out] */ INWCVariantDictionary **retVal) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE EnsureURLCookie( void) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_OldSessionID( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct INWCSessionVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            INWCSession * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            INWCSession * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            INWCSession * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            INWCSession * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            INWCSession * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            INWCSession * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
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
        
        HRESULT ( STDMETHODCALLTYPE *OnStartPage )( 
            INWCSession * This,
            /* [in] */ IUnknown *p);
        
        HRESULT ( STDMETHODCALLTYPE *OnEndPage )( 
            INWCSession * This);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_SessionID )( 
            INWCSession * This,
            /* [retval][out] */ BSTR *pbstrRet);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Value )( 
            INWCSession * This,
            /* [in] */ BSTR vkey,
            /* [retval][out] */ VARIANT *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Value )( 
            INWCSession * This,
            /* [in] */ BSTR vkey,
            /* [in] */ VARIANT newVal);
        
        /* [helpstring][id][propputref] */ HRESULT ( STDMETHODCALLTYPE *putref_Value )( 
            INWCSession * This,
            /* [in] */ BSTR vkey,
            /* [in] */ VARIANT newVal);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_TimeOut )( 
            INWCSession * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE *put_TimeOut )( 
            INWCSession * This,
            /* [in] */ LONG pVal);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *Abandon )( 
            INWCSession * This);
        
        /* [helpstring][hidden][propget] */ HRESULT ( STDMETHODCALLTYPE *get_CodePage )( 
            INWCSession * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [helpstring][hidden][propput] */ HRESULT ( STDMETHODCALLTYPE *put_CodePage )( 
            INWCSession * This,
            /* [in] */ LONG newVal);
        
        /* [helpstring][hidden][propget] */ HRESULT ( STDMETHODCALLTYPE *get_LCID )( 
            INWCSession * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [helpstring][hidden][propput] */ HRESULT ( STDMETHODCALLTYPE *put_LCID )( 
            INWCSession * This,
            /* [in] */ LONG newVal);
        
        /* [helpstring][hidden][propget] */ HRESULT ( STDMETHODCALLTYPE *get_StaticObjects )( 
            INWCSession * This,
            /* [retval][out] */ INWCVariantDictionary **ppProperties);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Contents )( 
            INWCSession * This,
            /* [retval][out] */ INWCVariantDictionary **pVal);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_IsExpired )( 
            INWCSession * This,
            /* [retval][out] */ VARIANT_BOOL *pVal);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_IsNew )( 
            INWCSession * This,
            /* [retval][out] */ VARIANT_BOOL *pVal);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_LiquidCookie )( 
            INWCSession * This,
            /* [retval][out] */ VARIANT_BOOL *pVal);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE *put_LiquidCookie )( 
            INWCSession * This,
            /* [in] */ VARIANT_BOOL newVal);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ReEntrance )( 
            INWCSession * This,
            /* [retval][out] */ VARIANT_BOOL *pVal);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE *put_ReEntrance )( 
            INWCSession * This,
            /* [in] */ VARIANT_BOOL newVal);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_URL )( 
            INWCSession * This,
            /* [in] */ VARIANT strCheckA,
            /* [retval][out] */ VARIANT *pVal);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *Execute )( 
            INWCSession * This,
            /* [in] */ BSTR ToPage);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *Transfer )( 
            INWCSession * This,
            /* [in] */ BSTR ToPage);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_CreateInstance )( 
            INWCSession * This,
            /* [in] */ BSTR progid,
            /* [retval][out] */ IDispatch **pVal);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Readonly )( 
            INWCSession * This,
            /* [in] */ VARIANT_BOOL newVal);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *Statistics )( 
            INWCSession * This,
            /* [optional][in] */ VARIANT vAppKey,
            /* [optional][in] */ VARIANT vSessionID,
            /* [retval][out] */ INWCVariantDictionary **retVal);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *EnsureURLCookie )( 
            INWCSession * This);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_OldSessionID )( 
            INWCSession * This,
            /* [retval][out] */ BSTR *pVal);
        
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


#define INWCSession_OnStartPage(This,p)	\
    ( (This)->lpVtbl -> OnStartPage(This,p) ) 

#define INWCSession_OnEndPage(This)	\
    ( (This)->lpVtbl -> OnEndPage(This) ) 

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

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


