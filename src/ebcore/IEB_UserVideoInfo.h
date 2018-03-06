// IEB_UserVideoInfo.h file here
#ifndef __IEB_UserVideoInfo_h__
#define __IEB_UserVideoInfo_h__
#include "EBUnknwn.h"
#include "ebdisp.h"

class CIEB_UserVideoInfo : public IEB_UserVideoInfo
	, public CEBDispatch
	, public EB_UserVideoInfo
{
public:
	virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_UserId( 
		/* [retval][out] */ ULONGLONG *pVal)
	{
		*pVal = m_sUserAccount;
		return S_OK;
	}
	virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_UserId( 
		/* [in] */ ULONGLONG newVal)
	{
		m_sUserAccount = newVal;
		return S_OK;
	}
	virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_OnVideo( 
		/* [retval][out] */ VARIANT_BOOL *pVal)
	{
		*pVal = m_bOnVideo?VARIANT_TRUE:VARIANT_FALSE;
		return S_OK;
	}
	virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_OnVideo( 
		/* [in] */ VARIANT_BOOL newVal)
	{
		m_bOnVideo = newVal==VARIANT_TRUE?true:false;
		return S_OK;
	}
	//virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_UserVideoId( 
	//	/* [retval][out] */ ULONG *pVal)
	//{
	//	*pVal = m_nUserVideoId;
	//	return S_OK;
	//}
	//virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_UserVideoId( 
	//	/* [in] */ ULONG newVal)
	//{
	//	m_nUserVideoId = newVal;
	//	return S_OK;
	//}

	virtual HRESULT STDMETHODCALLTYPE QueryInterface( 
		/* [in] */ REFIID riid,
		/* [iid_is][out] */ __RPC__deref_out void __RPC_FAR *__RPC_FAR *ppvObject)
	{
		if (riid == IID_IEB_UserVideoInfo)
		{
			*ppvObject = this;
			AddRef();
			return S_OK;
		}
		return CEBDispatch::QueryInterface(riid,ppvObject);
	}
private:
	virtual HRESULT STDMETHODCALLTYPE GetIDsOfNames( 
		/* [in] */ __RPC__in REFIID riid,
		/* [size_is][in] */ __RPC__in_ecount_full(cNames) LPOLESTR *rgszNames,
		/* [range][in] */ UINT cNames,
		/* [in] */ LCID lcid,
		/* [size_is][out] */ __RPC__out_ecount_full(cNames) DISPID *rgDispId)
	{
		if (rgszNames==NULL)
			return E_INVALIDARG;
		CComBSTR bstrName(*rgszNames);
		bstrName.ToUpper();
		if (bstrName==L"0" || bstrName == L"USERID")
		{
			*rgDispId = EB_DISP_USERID;
		}
		else if (bstrName==L"1" || bstrName == L"ONVIDEO")
		{
			*rgDispId = EB_DISP_ONVIDEO;
		}
		//else if (bstrName==L"1" || bstrName == L"USERVIDEOID")
		//{
		//	*rgDispId = EB_DISP_USERVIDEOID;
		//}
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
		if (wFlags&DISPATCH_PROPERTYGET)
		{
			if (pVarResult==NULL)
				return E_INVALIDARG;
			VariantInit(pVarResult);
			switch (dispIdMember)
			{
			case EB_DISP_USERID:
				pVarResult->vt = VT_UI8;
				return this->get_UserId(&pVarResult->ullVal);
			case EB_DISP_ONVIDEO:
				pVarResult->vt = VT_BOOL;
				return this->get_OnVideo(&pVarResult->boolVal);
			//case EB_DISP_USERVIDEOID:
			//	pVarResult->vt = VT_UI4;
			//	return this->get_UserVideoId(&pVarResult->ulVal);
			default:
				break;
			}
		}
		else if (wFlags&DISPATCH_PROPERTYPUT)
		{
			if (pDispParams == NULL || pDispParams->cArgs != 1 || pDispParams->rgvarg == NULL)
				return E_INVALIDARG;
			switch (dispIdMember)
			{
			case EB_DISP_USERID:
				return this->put_UserId(pDispParams->rgvarg[0].ullVal);
			case EB_DISP_ONVIDEO:
				return this->put_OnVideo(pDispParams->rgvarg[0].boolVal);
			//case EB_DISP_USERVIDEOID:
			//	return this->put_UserVideoId(pDispParams->rgvarg[0].ulVal);
			default:
				break;
			}
		}
		return S_OK;
	}

	virtual ULONG STDMETHODCALLTYPE AddRef( void)
	{
		return CEBDispatch::AddRef();
	}
	virtual ULONG STDMETHODCALLTYPE Release( void)
	{
		return CEBDispatch::Release();
	}
	virtual HRESULT STDMETHODCALLTYPE GetTypeInfoCount( 
		/* [out] */ __RPC__out UINT *pctinfo)
	{
		return CEBDispatch::GetTypeInfoCount(pctinfo);
	}
	virtual HRESULT STDMETHODCALLTYPE GetTypeInfo( 
		/* [in] */ UINT iTInfo,
		/* [in] */ LCID lcid,
		/* [out] */ __RPC__deref_out_opt ITypeInfo **ppTInfo)
	{
		return CEBDispatch::GetTypeInfo(iTInfo,lcid,ppTInfo);
	}
public:
	CIEB_UserVideoInfo(void)
	{
	}
	CIEB_UserVideoInfo(const EB_UserVideoInfo& pObj)
		: EB_UserVideoInfo(pObj)
	{
	}
};

#endif // __IEB_UserVideoInfo_h__
