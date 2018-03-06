// IEB_CallInfo.h file here
#ifndef __IEB_CallInfo_h__
#define __IEB_CallInfo_h__
#include "EBUnknwn.h"
#include "ebdisp.h"

class CIEB_CallInfo : public IEB_CallInfo
	, public CEBDispatch
	, public EB_CallInfo
{
public:
	virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_GroupCode( 
		/* [retval][out] */ ULONGLONG *pVal)
	{
		*pVal = m_sGroupCode;
		return S_OK;
	}
	virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_GroupCode( 
		/* [in] */ ULONGLONG newVal)
	{
		m_sGroupCode = newVal;
		return S_OK;
	}
	virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_OldCallId( 
		/* [retval][out] */ ULONGLONG *pVal)
	{
		*pVal = m_sOldCallId;
		return S_OK;
	}
	virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_OldCallId( 
		/* [in] */ ULONGLONG newVal)
	{
		m_sOldCallId = newVal;
		return S_OK;
	}
	virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_CallId( 
		/* [retval][out] */ ULONGLONG *pVal)
	{
		*pVal = m_sCallId;
		return S_OK;
	}
	virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_CallId( 
		/* [in] */ ULONGLONG newVal)
	{
		m_sCallId = newVal;
		return S_OK;
	}
	virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_FromUserId( 
		/* [retval][out] */ ULONGLONG *pVal)
	{
		*pVal = m_nFromUserId;
		return S_OK;
	}
	virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_FromUserId( 
		/* [in] */ ULONGLONG newVal)
	{
		m_nFromUserId = newVal;
		return S_OK;
	}
	virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_FromAccount( 
		/* [retval][out] */ BSTR *pVal)
	{
		*pVal = m_sFromAccount.CopyBSTR();
		return S_OK;
	}
	virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_FromAccount( 
		/* [in] */ BSTR newVal)
	{
		m_sFromAccount = newVal;
		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE QueryInterface( 
		/* [in] */ REFIID riid,
		/* [iid_is][out] */ __RPC__deref_out void __RPC_FAR *__RPC_FAR *ppvObject)
	{
#ifdef IID_IEB_CallInfo
		if (InlineIsEqualGUID(riid, IID_IEB_CallInfo))
#else
		if (InlineIsEqualGUID(riid, __uuidof(IEB_CallInfo)))
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
		if (bstrName==L"0" || bstrName == L"GROUPCODE")
		{
			*rgDispId = EB_DISP_GROUPCODE;
		}
		else if (bstrName==L"1" || bstrName == L"OLDCALLID")
		{
			*rgDispId = EB_DISP_OLDCALLID;
		}
		else if (bstrName==L"2" || bstrName == L"CALLID")
		{
			*rgDispId = EB_DISP_CALLID;
		}
		else if (bstrName==L"3" || bstrName == L"FROMUSERID")
		{
			*rgDispId = EB_DISP_FROMUSERID;
		}
		else if (bstrName==L"4" || bstrName == L"FROMACCOUNT")
		{
			*rgDispId = EB_DISP_FROMACCOUNT;
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
			case EB_DISP_GROUPCODE:
				pVarResult->vt = VT_UI8;
				return this->get_GroupCode(&pVarResult->ullVal);
			case EB_DISP_OLDCALLID:
				pVarResult->vt = VT_UI8;
				return this->get_OldCallId(&pVarResult->ullVal);
			case EB_DISP_CALLID:
				pVarResult->vt = VT_UI8;
				return this->get_CallId(&pVarResult->ullVal);
			case EB_DISP_FROMUSERID:
				pVarResult->vt = VT_UI8;
				return this->get_FromUserId(&pVarResult->ullVal);
			case EB_DISP_FROMACCOUNT:
				pVarResult->vt = VT_BSTR;
				return this->get_FromAccount(&pVarResult->bstrVal);
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
			case EB_DISP_GROUPCODE:
				return this->put_GroupCode(pDispParams->rgvarg[0].ullVal);
			case EB_DISP_OLDCALLID:
				return this->put_OldCallId(pDispParams->rgvarg[0].ullVal);
			case EB_DISP_CALLID:
				return this->put_CallId(pDispParams->rgvarg[0].ullVal);
			case EB_DISP_FROMUSERID:
				return this->put_FromUserId(pDispParams->rgvarg[0].ullVal);
			case EB_DISP_FROMACCOUNT:
				return this->put_FromAccount(pDispParams->rgvarg[0].bstrVal);
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
	CIEB_CallInfo(void)
	{
	}
	CIEB_CallInfo(const EB_CallInfo& pObj)
		: EB_CallInfo(&pObj)
	{
	}
};

#endif // __IEB_CallInfo_h__
