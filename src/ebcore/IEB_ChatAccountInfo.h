// IEB_ChatAccountInfo.h file here
#ifndef __IEB_ChatAccountInfo_h__
#define __IEB_ChatAccountInfo_h__
#include "EBUnknwn.h"
#include "ebdisp.h"
#include "ebdisp.h"

class CIEB_ChatAccountInfo : public IEB_ChatAccountInfo
	, public CEBDispatch
	, public CCrAccountInfo
{
public:
	virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Param( 
		/* [retval][out] */ ULONG *pVal)
	{
		*pVal = m_nParam;
		return S_OK;
	}
	virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Param( 
		/* [in] */ ULONG newVal)
	{
		m_nParam = newVal;
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
	virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ChatId( 
		/* [retval][out] */ ULONGLONG *pVal)
	{
		*pVal = m_sChatId;
		return S_OK;
	}
	virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_ChatId( 
		/* [in] */ ULONGLONG newVal)
	{
		m_sChatId = newVal;
		return S_OK;
	}
	virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_UserId( 
		/* [retval][out] */ ULONGLONG *pVal)
	{
		*pVal = m_sAccount;
		return S_OK;
	}
	virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_UserId( 
		/* [in] */ ULONGLONG newVal)
	{
		m_sAccount = newVal;
		return S_OK;
	}
	virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_FromInfo( 
		/* [retval][out] */ BSTR *pVal)
	{
		*pVal = m_sFromInfo.CopyBSTR();
		return S_OK;
	}
	virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_FromInfo( 
		/* [in] */ BSTR newVal)
	{
		m_sFromInfo = newVal;
		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE QueryInterface( 
		/* [in] */ REFIID riid,
		/* [iid_is][out] */ __RPC__deref_out void __RPC_FAR *__RPC_FAR *ppvObject)
	{
		if (riid == IID_IEB_ChatAccountInfo)
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
		if (bstrName==L"0" || bstrName == L"PARAM")
		{
			*rgDispId = EB_DISP_PARAM;
		}
		else if (bstrName==L"1" || bstrName == L"CALLID")
		{
			*rgDispId = EB_DISP_CALLID;
		}
		else if (bstrName==L"2" || bstrName == L"CHATID")
		{
			*rgDispId = EB_DISP_CHATID;
		}
		else if (bstrName==L"3" || bstrName == L"USERID")
		{
			*rgDispId = EB_DISP_USERID;
		}
		else if (bstrName==L"4" || bstrName == L"FROMINFO")
		{
			*rgDispId = EB_DISP_FROMINFO;
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
			case EB_DISP_PARAM:
				pVarResult->vt = VT_UI4;
				return this->get_Param(&pVarResult->ulVal);
			case EB_DISP_CALLID:
				pVarResult->vt = VT_UI8;
				return this->get_CallId(&pVarResult->ullVal);
			case EB_DISP_CHATID:
				pVarResult->vt = VT_UI8;
				return this->get_ChatId(&pVarResult->ullVal);
			case EB_DISP_USERID:
				pVarResult->vt = VT_UI8;
				return this->get_UserId(&pVarResult->ullVal);
			case EB_DISP_FROMINFO:
				pVarResult->vt = VT_BSTR;
				return this->get_FromInfo(&pVarResult->bstrVal);
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
			case EB_DISP_PARAM:
				return this->put_Param(pDispParams->rgvarg[0].ulVal);
			case EB_DISP_CALLID:
				return this->put_CallId(pDispParams->rgvarg[0].ullVal);
			case EB_DISP_CHATID:
				return this->put_ChatId(pDispParams->rgvarg[0].ullVal);
			case EB_DISP_USERID:
				return this->put_UserId(pDispParams->rgvarg[0].ullVal);
			case EB_DISP_FROMINFO:
				return this->put_FromInfo(pDispParams->rgvarg[0].bstrVal);
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
	CIEB_ChatAccountInfo(void)
	{
	}
	CIEB_ChatAccountInfo(const CCrAccountInfo& pObj)
		: CCrAccountInfo(pObj)
	{
	}
};

#endif // __IEB_ChatAccountInfo_h__
