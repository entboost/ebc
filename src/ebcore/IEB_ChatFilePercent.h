// IEB_ChatFilePercent.h file here
#ifndef __IEB_ChatFilePercent_h__
#define __IEB_ChatFilePercent_h__
#include "EBUnknwn.h"
#include "ebdisp.h"

class CIEB_ChatFilePercent : public IEB_ChatFilePercent
	, public CEBDispatch
	, public CChatRoomFilePercent
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
	virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ResId( 
		/* [retval][out] */ ULONGLONG *pVal)
	{
		*pVal = m_sResId;
		return S_OK;
	}
	virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_ResId( 
		/* [in] */ ULONGLONG newVal)
	{
		m_sResId = newVal;
		return S_OK;
	}
	virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_MsgId( 
		/* [retval][out] */ ULONGLONG *pVal)
	{
		*pVal = m_nMsgId;
		return S_OK;
	}
	virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_MsgId( 
		/* [in] */ ULONGLONG newVal)
	{
		m_nMsgId = newVal;
		return S_OK;
	}
	virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Percent( 
		/* [retval][out] */ DOUBLE *pVal)
	{
		*pVal = m_percent;
		return S_OK;
	}
	virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Percent( 
		/* [in] */ DOUBLE newVal)
	{
		m_percent = newVal;
		return S_OK;
	}
	virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_TranSeconds( 
		/* [retval][out] */ ULONG *pVal)
	{
		*pVal = m_nTranSeconds;
		return S_OK;
	}
	virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_TranSeconds( 
		/* [in] */ ULONG newVal)
	{
		m_nTranSeconds = newVal;
		return S_OK;
	}
	virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_LeftSeconds( 
		/* [retval][out] */ ULONG *pVal)
	{
		*pVal = m_nLeftSeconds;
		return S_OK;
	}
	virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_LeftSeconds( 
		/* [in] */ ULONG newVal)
	{
		m_nLeftSeconds = newVal;
		return S_OK;
	}
	virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_CurSpeed( 
		/* [retval][out] */ ULONG *pVal)
	{
		*pVal = m_nCurSpeed;
		return S_OK;
	}
	virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_CurSpeed( 
		/* [in] */ ULONG newVal)
	{
		m_nCurSpeed = newVal;
		return S_OK;
	}
	virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_AvrSpeed( 
		/* [retval][out] */ ULONG *pVal)
	{
		*pVal = m_nAvrSpeed;
		return S_OK;
	}
	virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_AvrSpeed( 
		/* [in] */ ULONG newVal)
	{
		m_nAvrSpeed = newVal;
		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE QueryInterface( 
		/* [in] */ REFIID riid,
		/* [iid_is][out] */ __RPC__deref_out void __RPC_FAR *__RPC_FAR *ppvObject)
	{
		if (riid == IID_IEB_ChatFilePercent)
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
		else if (bstrName==L"3" || bstrName == L"RESID")
		{
			*rgDispId = EB_DISP_RESID;
		}
		else if (bstrName==L"4" || bstrName == L"MSGID")
		{
			*rgDispId = EB_DISP_MSGID;
		}
		else if (bstrName==L"5" || bstrName == L"PERCENT")
		{
			*rgDispId = EB_DISP_PERCENT;
		}
		else if (bstrName==L"6" || bstrName == L"TRANSECONDS")
		{
			*rgDispId = EB_DISP_TRANSECONDS;
		}
		else if (bstrName==L"7" || bstrName == L"LEFTSECONDS")
		{
			*rgDispId = EB_DISP_LEFTSECONDS;
		}
		else if (bstrName==L"8" || bstrName == L"CURSPEED")
		{
			*rgDispId = EB_DISP_CURSPEED;
		}
		else if (bstrName==L"9" || bstrName == L"AVRSPEED")
		{
			*rgDispId = EB_DISP_AVRSPEED;
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
			case EB_DISP_RESID:
				pVarResult->vt = VT_UI8;
				return this->get_ResId(&pVarResult->ullVal);
			case EB_DISP_MSGID:
				pVarResult->vt = VT_UI8;
				return this->get_MsgId(&pVarResult->ullVal);
			case EB_DISP_PERCENT:
				pVarResult->vt = VT_R8;
				return this->get_Percent(&pVarResult->dblVal);
			case EB_DISP_TRANSECONDS:
				pVarResult->vt = VT_UI4;
				return this->get_TranSeconds(&pVarResult->ulVal);
			case EB_DISP_LEFTSECONDS:
				pVarResult->vt = VT_UI4;
				return this->get_LeftSeconds(&pVarResult->ulVal);
			case EB_DISP_CURSPEED:
				pVarResult->vt = VT_UI4;
				return this->get_CurSpeed(&pVarResult->ulVal);
			case EB_DISP_AVRSPEED:
				pVarResult->vt = VT_UI4;
				return this->get_AvrSpeed(&pVarResult->ulVal);
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
			case EB_DISP_RESID:
				return this->put_ResId(pDispParams->rgvarg[0].ullVal);
			case EB_DISP_MSGID:
				return this->put_MsgId(pDispParams->rgvarg[0].ullVal);
			case EB_DISP_PERCENT:
				return this->put_Percent(pDispParams->rgvarg[0].dblVal);
			case EB_DISP_TRANSECONDS:
				return this->put_TranSeconds(pDispParams->rgvarg[0].ulVal);
			case EB_DISP_LEFTSECONDS:
				return this->put_LeftSeconds(pDispParams->rgvarg[0].ulVal);
			case EB_DISP_CURSPEED:
				return this->put_CurSpeed(pDispParams->rgvarg[0].ulVal);
			case EB_DISP_AVRSPEED:
				return this->put_AvrSpeed(pDispParams->rgvarg[0].ulVal);
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
	CIEB_ChatFilePercent(void)
	{
	}
	CIEB_ChatFilePercent(const CChatRoomFilePercent& pObj)
		: CChatRoomFilePercent(pObj)
	{
	}
};

#endif // __IEB_ChatFilePercent_h__
