

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 8.01.0622 */
/* at Tue Jan 19 11:14:07 2038
 */
/* Compiler settings for TabExplorerExt.idl:
    Oicf, W1, Zp8, env=Win64 (32b run), target_arch=AMD64 8.01.0622 
    protocol : all , ms_ext, c_ext, robust
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

#ifndef __TabExplorerExt_h__
#define __TabExplorerExt_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __ITabExplorerShlExt_FWD_DEFINED__
#define __ITabExplorerShlExt_FWD_DEFINED__
typedef interface ITabExplorerShlExt ITabExplorerShlExt;

#endif 	/* __ITabExplorerShlExt_FWD_DEFINED__ */


#ifndef __TabExplorerShlExt_FWD_DEFINED__
#define __TabExplorerShlExt_FWD_DEFINED__

#ifdef __cplusplus
typedef class TabExplorerShlExt TabExplorerShlExt;
#else
typedef struct TabExplorerShlExt TabExplorerShlExt;
#endif /* __cplusplus */

#endif 	/* __TabExplorerShlExt_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 


#ifndef __ITabExplorerShlExt_INTERFACE_DEFINED__
#define __ITabExplorerShlExt_INTERFACE_DEFINED__

/* interface ITabExplorerShlExt */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_ITabExplorerShlExt;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("2A0D8430-F0ED-4103-B627-666763B6F80F")
    ITabExplorerShlExt : public IDispatch
    {
    public:
    };
    
    
#else 	/* C style interface */

    typedef struct ITabExplorerShlExtVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ITabExplorerShlExt * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ITabExplorerShlExt * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ITabExplorerShlExt * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            ITabExplorerShlExt * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            ITabExplorerShlExt * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            ITabExplorerShlExt * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            ITabExplorerShlExt * This,
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
        
        END_INTERFACE
    } ITabExplorerShlExtVtbl;

    interface ITabExplorerShlExt
    {
        CONST_VTBL struct ITabExplorerShlExtVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ITabExplorerShlExt_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define ITabExplorerShlExt_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define ITabExplorerShlExt_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define ITabExplorerShlExt_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define ITabExplorerShlExt_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define ITabExplorerShlExt_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define ITabExplorerShlExt_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __ITabExplorerShlExt_INTERFACE_DEFINED__ */



#ifndef __TABEXPLOREREXTLib_LIBRARY_DEFINED__
#define __TABEXPLOREREXTLib_LIBRARY_DEFINED__

/* library TABEXPLOREREXTLib */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_TABEXPLOREREXTLib;

EXTERN_C const CLSID CLSID_TabExplorerShlExt;

#ifdef __cplusplus

class DECLSPEC_UUID("9FD4B86D-2BB8-423d-ACB0-E6D1C9A7B47D")
TabExplorerShlExt;
#endif
#endif /* __TABEXPLOREREXTLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


