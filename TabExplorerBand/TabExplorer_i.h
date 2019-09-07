

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 8.01.0622 */
/* at Tue Jan 19 11:14:07 2038
 */
/* Compiler settings for TabExplorer.idl:
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

#ifndef __TabExplorer_i_h__
#define __TabExplorer_i_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __ITabExplorerBHO_FWD_DEFINED__
#define __ITabExplorerBHO_FWD_DEFINED__
typedef interface ITabExplorerBHO ITabExplorerBHO;

#endif 	/* __ITabExplorerBHO_FWD_DEFINED__ */


#ifndef __ITabbarBand_FWD_DEFINED__
#define __ITabbarBand_FWD_DEFINED__
typedef interface ITabbarBand ITabbarBand;

#endif 	/* __ITabbarBand_FWD_DEFINED__ */


#ifndef __TabExplorerBHO_FWD_DEFINED__
#define __TabExplorerBHO_FWD_DEFINED__

#ifdef __cplusplus
typedef class TabExplorerBHO TabExplorerBHO;
#else
typedef struct TabExplorerBHO TabExplorerBHO;
#endif /* __cplusplus */

#endif 	/* __TabExplorerBHO_FWD_DEFINED__ */


#ifndef __TabbarBand_FWD_DEFINED__
#define __TabbarBand_FWD_DEFINED__

#ifdef __cplusplus
typedef class TabbarBand TabbarBand;
#else
typedef struct TabbarBand TabbarBand;
#endif /* __cplusplus */

#endif 	/* __TabbarBand_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 


#ifndef __ITabExplorerBHO_INTERFACE_DEFINED__
#define __ITabExplorerBHO_INTERFACE_DEFINED__

/* interface ITabExplorerBHO */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_ITabExplorerBHO;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("9442648A-428E-4F22-B03A-51D6EE9FCBFA")
    ITabExplorerBHO : public IDispatch
    {
    public:
    };
    
    
#else 	/* C style interface */

    typedef struct ITabExplorerBHOVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ITabExplorerBHO * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ITabExplorerBHO * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ITabExplorerBHO * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            ITabExplorerBHO * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            ITabExplorerBHO * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            ITabExplorerBHO * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            ITabExplorerBHO * This,
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
    } ITabExplorerBHOVtbl;

    interface ITabExplorerBHO
    {
        CONST_VTBL struct ITabExplorerBHOVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ITabExplorerBHO_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define ITabExplorerBHO_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define ITabExplorerBHO_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define ITabExplorerBHO_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define ITabExplorerBHO_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define ITabExplorerBHO_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define ITabExplorerBHO_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __ITabExplorerBHO_INTERFACE_DEFINED__ */


#ifndef __ITabbarBand_INTERFACE_DEFINED__
#define __ITabbarBand_INTERFACE_DEFINED__

/* interface ITabbarBand */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_ITabbarBand;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("931743A6-9BAF-420C-9CF6-78459D9F7720")
    ITabbarBand : public IDispatch
    {
    public:
    };
    
    
#else 	/* C style interface */

    typedef struct ITabbarBandVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ITabbarBand * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ITabbarBand * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ITabbarBand * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            ITabbarBand * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            ITabbarBand * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            ITabbarBand * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            ITabbarBand * This,
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
    } ITabbarBandVtbl;

    interface ITabbarBand
    {
        CONST_VTBL struct ITabbarBandVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ITabbarBand_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define ITabbarBand_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define ITabbarBand_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define ITabbarBand_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define ITabbarBand_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define ITabbarBand_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define ITabbarBand_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __ITabbarBand_INTERFACE_DEFINED__ */



#ifndef __TabExplorerLib_LIBRARY_DEFINED__
#define __TabExplorerLib_LIBRARY_DEFINED__

/* library TabExplorerLib */
/* [version][uuid] */ 


EXTERN_C const IID LIBID_TabExplorerLib;

EXTERN_C const CLSID CLSID_TabExplorerBHO;

#ifdef __cplusplus

class DECLSPEC_UUID("70AF1B13-ACC5-4108-909D-514D3EA15C88")
TabExplorerBHO;
#endif

EXTERN_C const CLSID CLSID_TabbarBand;

#ifdef __cplusplus

class DECLSPEC_UUID("C43B8324-773C-4CFF-8676-5DA1C2CEC6BB")
TabbarBand;
#endif
#endif /* __TabExplorerLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


