// EBUnknwn.h file here
#ifndef __EBUnknwn_h__
#define __EBUnknwn_h__
#include "Unknwn.h"

HRESULT GetArrayVariantOfName(IDispatch* pDisp, BSTR bstrName, VARIANT * pOutVariant)
{
	DISPPARAMS noArgs = {NULL, NULL, 0, 0};
	DISPID dispId;
	HRESULT hr = 0;
	hr = pDisp->GetIDsOfNames(IID_NULL, &bstrName, 1, LOCALE_USER_DEFAULT, &dispId);
	if (FAILED(hr))
	{
		//char lpsztext[20];
		//sprintf(lpsztext,"1 %x",hr);
		//MessageBoxA(NULL,lpsztext,"",MB_OK);
		return hr;
	}
	VariantInit(pOutVariant);
	hr = pDisp->Invoke(dispId, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_PROPERTYGET , &noArgs, pOutVariant, NULL, NULL);
	return hr;
}
//
//BOOL GetBinaryFromVariant(const VARIANT & ovData, BYTE ** ppBuf, unsigned long * pcBufLen)
//{
//	BOOL fRetVal = FALSE;
//
//	//Binary data is stored in the variant as an array of unsigned char
//	if(ovData.vt == (VT_ARRAY|VT_UI1))  // (OLE SAFEARRAY)
//	{
//		//Retrieve size of array
//		*pcBufLen = ovData.parray->rgsabound[0].cElements;
//
//		*ppBuf = new BYTE[*pcBufLen]; //Allocate a buffer to store the data
//		if(*ppBuf != NULL)
//		{
//			void * pArrayData;
//
//			//Obtain safe pointer to the array
//			SafeArrayAccessData(ovData.parray,&pArrayData);
//
//			//Copy the bitmap into our buffer
//			memcpy(*ppBuf, pArrayData, *pcBufLen);
//
//			//Unlock the variant data
//			SafeArrayUnaccessData(ovData.parray);
//			fRetVal = TRUE;
//		}
//	}
//	return fRetVal;
//}

//BOOL PutBinaryIntoVariant(CComVariant * ovData, const BYTE * pBuf, unsigned long cBufLen)
//{
//	BOOL fRetVal = FALSE;
//	VARIANT var;
//	VariantInit(&var);  //Initialize our variant
//
//	//Set the type to an array of unsigned chars (OLE SAFEARRAY)
//	var.vt = VT_ARRAY | VT_UI1;
//
//	//Set up the bounds structure
//	SAFEARRAYBOUND  rgsabound[1];
//	rgsabound[0].cElements = cBufLen;
//	rgsabound[0].lLbound = 0;
//
//	//Create an OLE SAFEARRAY
//	var.parray = SafeArrayCreate(VT_UI1,1,rgsabound);
//	if(var.parray != NULL)
//	{
//		void * pArrayData = NULL;
//
//		//Get a safe pointer to the array
//		SafeArrayAccessData(var.parray,&pArrayData);
//
//		//Copy bitmap to it
//		memcpy(pArrayData, pBuf, cBufLen);
//
//		//Unlock the variant data
//		SafeArrayUnaccessData(var.parray);
//
//		*ovData = var;  // Create a COleVariant based on our variant
//		VariantClear(&var);
//
//		fRetVal = TRUE;
//	}
//	return fRetVal;
//}

class CEBUnknown : public IUnknown
{
public:
	virtual HRESULT STDMETHODCALLTYPE QueryInterface( 
		/* [in] */ REFIID riid,
		/* [iid_is][out] */ __RPC__deref_out void __RPC_FAR *__RPC_FAR *ppvObject)
	{
		if (InlineIsEqualGUID(riid,IID_IUnknown))
		{
			*ppvObject = this;
			AddRef();
			return S_OK;
		}
		return E_NOINTERFACE;
	}
	virtual ULONG STDMETHODCALLTYPE AddRef( void)
	{
		return InterlockedIncrement(&m_comRef);
	}
	virtual ULONG STDMETHODCALLTYPE Release( void)
	{
		if(InterlockedDecrement(&m_comRef)==0)
		{
			delete this;
			return 0;
		}
		return m_comRef;
	}

	CEBUnknown(void)
		: m_comRef(0)
	{
	}
	virtual ~CEBUnknown(void)
	{
	}
private:
	LONG m_comRef;
};

class CEBDispatch : public IDispatch, public CEBUnknown
{
public:
	virtual HRESULT STDMETHODCALLTYPE QueryInterface( 
		/* [in] */ REFIID riid,
		/* [iid_is][out] */ __RPC__deref_out void __RPC_FAR *__RPC_FAR *ppvObject)
	{
		if (InlineIsEqualGUID(riid, IID_IDispatch))
		{
			*ppvObject = this;
			AddRef();
			return S_OK;
		}
		return CEBUnknown::QueryInterface(riid,ppvObject);
	}
	virtual ULONG STDMETHODCALLTYPE AddRef( void)
	{
		return CEBUnknown::AddRef();
	}
	virtual ULONG STDMETHODCALLTYPE Release( void)
	{
		return CEBUnknown::Release();
	}
	virtual HRESULT STDMETHODCALLTYPE GetTypeInfoCount( 
		/* [out] */ __RPC__out UINT *pctinfo)
	{
		return S_OK;
	}
	virtual HRESULT STDMETHODCALLTYPE GetTypeInfo( 
		/* [in] */ UINT iTInfo,
		/* [in] */ LCID lcid,
		/* [out] */ __RPC__deref_out_opt ITypeInfo **ppTInfo)
	{
		return E_NOTIMPL;
		return S_OK;
	}
	virtual HRESULT STDMETHODCALLTYPE GetIDsOfNames( 
		/* [in] */ __RPC__in REFIID riid,
		/* [size_is][in] */ __RPC__in_ecount_full(cNames) LPOLESTR *rgszNames,
		/* [range][in] */ UINT cNames,
		/* [in] */ LCID lcid,
		/* [size_is][out] */ __RPC__out_ecount_full(cNames) DISPID *rgDispId)
	{
		return E_NOTIMPL;
		return S_OK;
	}
	virtual /* [local] */ HRESULT STDMETHODCALLTYPE Invoke( 
		/* [in] */ DISPID dispIdMember,
		/* [in] */ REFIID riid,
		/* [in] */ LCID lcid,
		/* [in] */ WORD wFlags,
		/* [out][in] */ DISPPARAMS *pDispParams,
		/* [out] */ VARIANT *pVarResult,
		/* [out] */ EXCEPINFO *pExcepInfo,
		/* [out] */ UINT *puArgErr)
	{
		return E_NOTIMPL;
		return S_OK;
	}

};

#endif // __EBUnknwn_h__
