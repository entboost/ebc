// IEB_APMsgInfo.h file here
#ifndef __IEB_APMsgInfo_h__
#define __IEB_APMsgInfo_h__
#include "EBUnknwn.h"
#include "ebdisp.h"
#include "../include/eb_object.h"
using namespace entboost;

class CIEB_APMsgInfo : public IEB_APMsgInfo
	, public CEBDispatch
	, public EB_APMsgInfo
{
public:
	virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_FromAppId( 
		/* [retval][out] */ ULONGLONG *pVal)
	{
		*pVal = m_sFromAppId;
		return S_OK;
	}
	virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_FromAppId( 
		/* [in] */ ULONGLONG newVal)
	{
		m_sFromAppId = newVal;
		return S_OK;
	}
	virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_GroupId( 
		/* [retval][out] */ ULONGLONG *pVal)
	{
		*pVal = m_nGroupId;
		return S_OK;
	}
	virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_GroupId( 
		/* [in] */ ULONGLONG newVal)
	{
		m_nGroupId = newVal;
		return S_OK;
	}
	virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_UserId( 
		/* [retval][out] */ ULONGLONG *pVal)
	{
		*pVal = m_nUserId;
		return S_OK;
	}
	virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_UserId( 
		/* [in] */ ULONGLONG newVal)
	{
		m_nUserId = newVal;
		return S_OK;
	}
	virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Account( 
		/* [retval][out] */ BSTR *pVal)
	{
		*pVal = m_sAccount.CopyBSTR();
		return S_OK;
	}
	virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Account( 
		/* [in] */ BSTR newVal)
	{
		m_sAccount = newVal;
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
	virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_MsgType( 
		/* [retval][out] */ LONG *pVal)
	{
		*pVal = (LONG)m_nMsgType;
		return S_OK;
	}
	virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_MsgType( 
		/* [in] */ LONG newVal)
	{
		m_nMsgType = newVal;
		return S_OK;
	}
	virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_MsgName( 
		/* [retval][out] */ BSTR *pVal)
	{
		*pVal = m_sMsgName.CopyBSTR();
		return S_OK;
	}
	virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_MsgName( 
		/* [in] */ BSTR newVal)
	{
		m_sMsgName = newVal;
		return S_OK;
	}
	virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_MsgContent( 
		/* [retval][out] */ BSTR *pVal)
	{
		*pVal = m_sMsgContent.CopyBSTR();
		return S_OK;
	}
	virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_MsgContent( 
		/* [in] */ BSTR newVal)
	{
		m_sMsgContent = newVal;
		return S_OK;
	}
	virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_MsgTime( 
		/* [retval][out] */ BSTR *pVal)
	{
		*pVal = m_sMsgTime.CopyBSTR();
		return S_OK;
	}
	virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_MsgTime( 
		/* [in] */ BSTR newVal)
	{
		m_sMsgTime = newVal;
		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE QueryInterface( 
		/* [in] */ REFIID riid,
		/* [iid_is][out] */ __RPC__deref_out void __RPC_FAR *__RPC_FAR *ppvObject)
	{
		if (riid == IID_IEB_APMsgInfo)
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
		if (bstrName==L"0" || bstrName == L"FROMAPPID")
		{
			*rgDispId = EB_DISP_FROMAPPID;
		}
		else if (bstrName==L"1" || bstrName == L"GROUPID")
		{
			*rgDispId = EB_DISP_GROUPID;
		}
		else if (bstrName==L"2" || bstrName == L"USERID")
		{
			*rgDispId = EB_DISP_USERID;
		}
		else if (bstrName==L"3" || bstrName == L"ACCOUNT")
		{
			*rgDispId = EB_DISP_ACCOUNT;
		}
		else if (bstrName==L"4" || bstrName == L"MSGID")
		{
			*rgDispId = EB_DISP_MSGID;
		}
		else if (bstrName==L"5" || bstrName == L"MSGTYPE")
		{
			*rgDispId = EB_DISP_MSGTYPE;
		}
		else if (bstrName==L"6" || bstrName == L"MSGTNAME")
		{
			*rgDispId = EB_DISP_MSGNAME;
		}
		else if (bstrName==L"7" || bstrName == L"MSGTCONTENT")
		{
			*rgDispId = EB_DISP_MSGCONTENT;
		}
		else if (bstrName==L"8" || bstrName == L"MSGTTIME")
		{
			*rgDispId = EB_DISP_MSGTIME;
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
			case EB_DISP_FROMAPPID:
				pVarResult->vt = VT_UI8;
				return this->get_FromAppId(&pVarResult->ullVal);
			case EB_DISP_GROUPID:
				pVarResult->vt = VT_UI8;
				return this->get_GroupId(&pVarResult->ullVal);
			case EB_DISP_USERID:
				pVarResult->vt = VT_UI8;
				return this->get_UserId(&pVarResult->ullVal);
			case EB_DISP_ACCOUNT:
				pVarResult->vt = VT_BSTR;
				return this->get_Account(&pVarResult->bstrVal);
			case EB_DISP_MSGID:
				pVarResult->vt = VT_UI8;
				return this->get_MsgId(&pVarResult->ullVal);
			case EB_DISP_MSGTYPE:
				pVarResult->vt = VT_I4;
				return this->get_MsgType(&pVarResult->lVal);
			case EB_DISP_MSGNAME:
				pVarResult->vt = VT_BSTR;
				return this->get_MsgName(&pVarResult->bstrVal);
			case EB_DISP_MSGCONTENT:
				pVarResult->vt = VT_BSTR;
				return this->get_MsgContent(&pVarResult->bstrVal);
			case EB_DISP_MSGTIME:
				pVarResult->vt = VT_BSTR;
				return this->get_MsgTime(&pVarResult->bstrVal);
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
			case EB_DISP_FROMAPPID:
				return this->put_FromAppId(pDispParams->rgvarg[0].ullVal);
			case EB_DISP_GROUPID:
				return this->put_GroupId(pDispParams->rgvarg[0].ullVal);
			case EB_DISP_USERID:
				return this->put_UserId(pDispParams->rgvarg[0].ullVal);
			case EB_DISP_ACCOUNT:
				return this->put_Account(pDispParams->rgvarg[0].bstrVal);
			case EB_DISP_MSGID:
				return this->put_MsgId(pDispParams->rgvarg[0].ullVal);
			case EB_DISP_MSGTYPE:
				return this->put_MsgType(pDispParams->rgvarg[0].lVal);
			case EB_DISP_MSGNAME:
				return this->put_MsgName(pDispParams->rgvarg[0].bstrVal);
			case EB_DISP_MSGCONTENT:
				return this->put_MsgContent(pDispParams->rgvarg[0].bstrVal);
			case EB_DISP_MSGTIME:
				return this->put_MsgTime(pDispParams->rgvarg[0].bstrVal);
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
	CIEB_APMsgInfo(void)
	{
	}
	CIEB_APMsgInfo(const EB_APMsgInfo& pObj)
		: EB_APMsgInfo(pObj)
	{
	}
};

#endif // __IEB_APMsgInfo_h__
