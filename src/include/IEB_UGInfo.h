// IEB_UGInfo.h file here
#ifndef __IEB_UGInfo_h__
#define __IEB_UGInfo_h__
#include "EBUnknwn.h"
#include "ebdisp.h"

class CIEB_UGInfo : public IEB_UGInfo
	, public CEBDispatch
	, public EB_UGInfo
{
public:
	virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_UGId( 
		/* [retval][out] */ ULONGLONG *pVal)
	{
		*pVal = m_nUGId;
		return S_OK;
	}
	virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_UGId( 
		/* [in] */ ULONGLONG newVal)
	{
		m_nUGId = newVal;
		return S_OK;
	}
	virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_GroupName( 
		/* [retval][out] */ BSTR *pVal)
	{
		*pVal = m_sGroupName.CopyBSTR();
		return S_OK;
	}
	virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_GroupName( 
		/* [in] */ BSTR newVal)
	{
		m_sGroupName = newVal;
		return S_OK;
	}


	virtual HRESULT STDMETHODCALLTYPE QueryInterface( 
		/* [in] */ REFIID riid,
		/* [iid_is][out] */ __RPC__deref_out void __RPC_FAR *__RPC_FAR *ppvObject)
	{
#ifdef IID_IEB_UGInfo
		if (InlineIsEqualGUID(riid, IID_IEB_UGInfo))
#else
		if (InlineIsEqualGUID(riid, __uuidof(IEB_UGInfo)))
#endif
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
		if (bstrName==L"0" || bstrName == L"UGID")
		{
			*rgDispId = EB_DISP_UGID;
		}
		else if (bstrName==L"1" || bstrName == L"GROUPNAME")
		{
			*rgDispId = EB_DISP_GROUPNAME;
		}
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
			case EB_DISP_UGID:
				pVarResult->vt = VT_UI8;
				return this->get_UGId(&pVarResult->ullVal);
			case EB_DISP_GROUPNAME:
				pVarResult->vt = VT_BSTR;
				return this->get_GroupName(&pVarResult->bstrVal);
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
			case EB_DISP_UGID:
				return this->put_UGId(pDispParams->rgvarg[0].ullVal);
			case EB_DISP_GROUPNAME:
				return this->put_GroupName(pDispParams->rgvarg[0].bstrVal);
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
	CIEB_UGInfo(void)
	{
	}
	CIEB_UGInfo(const EB_UGInfo& pObj)
		: EB_UGInfo(&pObj)
	{
	}
};

#endif // __IEB_UGInfo_h__
