// IEB_P2PInfo.h file here
#ifndef __IEB_P2PInfo_h__
#define __IEB_P2PInfo_h__
#include "EBUnknwn.h"
#include "ebdisp.h"

class CIEB_P2PInfo : public IEB_P2PInfo
	, public CEBDispatch
	, public CCrP2PInfo
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
	virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_FromAccount( 
		/* [retval][out] */ ULONGLONG *pVal)
	{
		*pVal = m_sFromAccount;
		return S_OK;
	}
	virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_FromAccount( 
		/* [in] */ ULONGLONG newVal)
	{
		m_sFromAccount = newVal;
		return S_OK;
	}
	virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_FromLocalAddress( 
		/* [retval][out] */ BSTR *pVal)
	{
		*pVal = m_sFromLocalAddress.CopyBSTR();
		return S_OK;
	}
	virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_FromLocalAddress( 
		/* [in] */ BSTR newVal)
	{
		m_sFromLocalAddress = newVal;
		return S_OK;
	}
	virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_FromRemoteAddress( 
		/* [retval][out] */ BSTR *pVal)
	{
		*pVal = m_sFromRemoteAddress.CopyBSTR();
		return S_OK;
	}
	virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_FromRemoteAddress( 
		/* [in] */ BSTR newVal)
	{
		m_sFromRemoteAddress = newVal;
		return S_OK;
	}
	virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_P2PData( 
		/* [retval][out] */ ULONG *pVal)
	{
		*pVal = m_nP2PData;
		return S_OK;
	}
	virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_P2PData( 
		/* [in] */ ULONG newVal)
	{
		m_nP2PData = newVal;
		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE QueryInterface( 
		/* [in] */ REFIID riid,
		/* [iid_is][out] */ __RPC__deref_out void __RPC_FAR *__RPC_FAR *ppvObject)
	{
		if (riid == IID_IEB_P2PInfo)
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
		else if (bstrName==L"3" || bstrName == L"FROMACCOUNT")
		{
			*rgDispId = EB_DISP_FROMACCOUNT;
		}
		else if (bstrName==L"4" || bstrName == L"FROMLOCALADDRESS")
		{
			*rgDispId = EB_DISP_FROMLOCALADDRESS;
		}
		else if (bstrName==L"5" || bstrName == L"FROMREMOTEADDRESS")
		{
			*rgDispId = EB_DISP_FROMREMOTEADDRESS;
		}
		else if (bstrName==L"6" || bstrName == L"P2PDATA")
		{
			*rgDispId = EB_DISP_P2PDATA;
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
			case EB_DISP_FROMACCOUNT:
				pVarResult->vt = VT_UI8;
				return this->get_FromAccount(&pVarResult->ullVal);
			case EB_DISP_FROMLOCALADDRESS:
				pVarResult->vt = VT_BSTR;
				return this->get_FromLocalAddress(&pVarResult->bstrVal);
			case EB_DISP_FROMREMOTEADDRESS:
				pVarResult->vt = VT_BSTR;
				return this->get_FromRemoteAddress(&pVarResult->bstrVal);
			case EB_DISP_P2PDATA:
				pVarResult->vt = VT_UI4;
				return this->get_P2PData(&pVarResult->ulVal);
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
			case EB_DISP_FROMACCOUNT:
				return this->put_FromAccount(pDispParams->rgvarg[0].ullVal);
			case EB_DISP_FROMLOCALADDRESS:
				return this->put_FromLocalAddress(pDispParams->rgvarg[0].bstrVal);
			case EB_DISP_FROMREMOTEADDRESS:
				return this->put_FromRemoteAddress(pDispParams->rgvarg[0].bstrVal);
			case EB_DISP_P2PDATA:
				return this->put_P2PData(pDispParams->rgvarg[0].ulVal);
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
	CIEB_P2PInfo(void)
	{
	}
	CIEB_P2PInfo(const CCrP2PInfo& pObj)
		: CCrP2PInfo(pObj)
	{
	}
};

#endif // __IEB_P2PInfo_h__
