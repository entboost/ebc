// IEB_ChatFileInfo.h file here
#ifndef __IEB_ChatFileInfo_h__
#define __IEB_ChatFileInfo_h__
#include "EBUnknwn.h"
#include "ebdisp.h"

class CIEB_ChatFileInfo : public IEB_ChatFileInfo
	, public CEBDispatch
	, public CCrFileInfo
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
	virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ResCmServer( 
		/* [retval][out] */ BSTR *pVal)
	{
		*pVal = m_sResCmServer.CopyBSTR();
		return S_OK;
	}
	virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_ResCmServer( 
		/* [in] */ BSTR newVal)
	{
		m_sResCmServer = newVal;
		return S_OK;
	}
	virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ResCmAppName( 
		/* [retval][out] */ BSTR *pVal)
	{
		*pVal = m_sResCmAppName.CopyBSTR();
		return S_OK;
	}
	virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_ResCmAppName( 
		/* [in] */ BSTR newVal)
	{
		m_sResCmAppName = newVal;
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
	virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_FileName( 
		/* [retval][out] */ BSTR *pVal)
	{
		*pVal = m_sFileName.CopyBSTR();
		return S_OK;
	}
	virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_FileName( 
		/* [in] */ BSTR newVal)
	{
		m_sFileName = newVal;
		return S_OK;
	}
	virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_FileSize( 
		/* [retval][out] */ ULONGLONG *pVal)
	{
		*pVal = m_nFileSize;
		return S_OK;
	}
	virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_FileSize( 
		/* [in] */ ULONGLONG newVal)
	{
		m_nFileSize = newVal;
		return S_OK;
	}
	virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_FileTime( 
		/* [retval][out] */ BSTR *pVal)
	{
		*pVal = m_sFileTime.CopyBSTR();
		return S_OK;
	}
	virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_FileTime( 
		/* [in] */ BSTR newVal)
	{
		m_sFileTime = newVal;
		return S_OK;
	}
	virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_OffFile( 
		/* [retval][out] */ VARIANT_BOOL *pVal)
	{
		*pVal = m_bOffFile?VARIANT_TRUE:VARIANT_FALSE;
		return S_OK;
	}
	virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_OffFile( 
		/* [in] */ VARIANT_BOOL newVal)
	{
		m_bOffFile = newVal?true:false;
		return S_OK;
	}
	virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_SendFrom( 
		/* [retval][out] */ ULONGLONG *pVal)
	{
		*pVal = m_sSendFrom;
		return S_OK;
	}
	virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_SendFrom( 
		/* [in] */ ULONGLONG newVal)
	{
		m_sSendFrom = newVal;
		return S_OK;
	}
	virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_SendTo( 
		/* [retval][out] */ ULONGLONG *pVal)
	{
		*pVal = m_sSendTo;
		return S_OK;
	}
	virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_SendTo( 
		/* [in] */ ULONGLONG newVal)
	{
		m_sSendTo = newVal;
		return S_OK;
	}
	virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Private( 
		/* [retval][out] */ VARIANT_BOOL *pVal)
	{
		*pVal = m_bPrivate?VARIANT_TRUE:VARIANT_FALSE;
		return S_OK;
	}
	virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Private( 
		/* [in] */ VARIANT_BOOL newVal)
	{
		m_bPrivate = newVal?true:false;
		return S_OK;
	}
	virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ReceiveAccount( 
		/* [retval][out] */ ULONGLONG *pVal)
	{
		*pVal = m_sReceiveAccount;
		return S_OK;
	}
	virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_ReceiveAccount( 
		/* [in] */ ULONGLONG newVal)
	{
		m_sReceiveAccount = newVal;
		return S_OK;
	}
	//virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE CopyRawData( 
	//	/* [in] */ void *pVal)
	//{
	//	if (pVal==NULL) return E_INVALIDARG;
	//	CIEB_ChatFileInfo * copyVal = (CIEB_ChatFileInfo*)pVal;
	//	((CCrFileInfo*)copyVal)->operator =((const CCrFileInfo*)this);
	//	return S_OK;
	//}
	//virtual HRESULT STDMETHODCALLTYPE raw_CopyRawData (
	//	/*[in]*/ void * pVal )
	//{
	//	return CopyRawData(pVal);
	//}
	//virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE FromRawData( 
	//	/* [in] */ void *pVal)
	//{
	//	if (pVal==NULL) return E_INVALIDARG;
	//	CIEB_ChatFileInfo * fromVal = (CIEB_ChatFileInfo*)pVal;
	//	((CCrFileInfo*)this)->operator =((const CCrFileInfo*)fromVal);
	//	return S_OK;
	//}
	//virtual HRESULT STDMETHODCALLTYPE raw_FromRawData (
	//	/*[in]*/ void * pVal )
	//{
	//	return FromRawData(pVal);
	//}

	//virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_RawData( 
	//	/* [retval][out] */ void *pVal)
	//{
	//	if (pVal==NULL) 
	//	CIEB_ChatFileInfo * newVal = (CIEB_ChatFileInfo*)pVal;
	//	((CCrFileInfo*)newVal)->operator =((const CCrFileInfo*)this);
	//	return S_OK;
	//}

	virtual HRESULT STDMETHODCALLTYPE QueryInterface( 
		/* [in] */ REFIID riid,
		/* [iid_is][out] */ __RPC__deref_out void __RPC_FAR *__RPC_FAR *ppvObject)
	{
#ifdef IID_IEB_ChatFileInfo
		if (InlineIsEqualGUID(riid, IID_IEB_ChatFileInfo))
#else
		if (InlineIsEqualGUID(riid, __uuidof(IEB_ChatFileInfo)))
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
		else if (bstrName==L"4" || bstrName == L"RESCMSERVER")
		{
			*rgDispId = EB_DISP_RESCMSERVER;
		}
		else if (bstrName==L"5" || bstrName == L"RESCMAPPNAME")
		{
			*rgDispId = EB_DISP_RESCMAPPNAME;
		}
		else if (bstrName==L"6" || bstrName == L"MSGID")
		{
			*rgDispId = EB_DISP_MSGID;
		}
		else if (bstrName==L"7" || bstrName == L"FILENAME")
		{
			*rgDispId = EB_DISP_FILENAME;
		}
		else if (bstrName==L"8" || bstrName == L"FILESIZE")
		{
			*rgDispId = EB_DISP_FILESIZE;
		}
		else if (bstrName==L"9" || bstrName == L"FILETIME")
		{
			*rgDispId = EB_DISP_FILETIME;
		}
		else if (bstrName==L"10" || bstrName == L"OFFFILE")
		{
			*rgDispId = EB_DISP_OFFFILE;
		}
		else if (bstrName==L"11" || bstrName == L"SENDFROM")
		{
			*rgDispId = EB_DISP_SENDFROM;
		}
		else if (bstrName==L"12" || bstrName == L"SENDTO")
		{
			*rgDispId = EB_DISP_SENDTO;
		}
		else if (bstrName==L"13" || bstrName == L"PRIVATE")
		{
			*rgDispId = EB_DISP_PRIVATE;
		}
		else if (bstrName==L"14" || bstrName == L"RECEIVEACCOUNT")
		{
			*rgDispId = EB_DISP_RECEIVEACCOUNT;
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
			case EB_DISP_RESCMSERVER:
				pVarResult->vt = VT_BSTR;
				return this->get_ResCmServer(&pVarResult->bstrVal);
			case EB_DISP_RESCMAPPNAME:
				pVarResult->vt = VT_BSTR;
				return this->get_ResCmAppName(&pVarResult->bstrVal);
			case EB_DISP_MSGID:
				pVarResult->vt = VT_UI8;
				return this->get_MsgId(&pVarResult->ullVal);
			case EB_DISP_FILENAME:
				pVarResult->vt = VT_BSTR;
				return this->get_FileName(&pVarResult->bstrVal);
			case EB_DISP_FILESIZE:
				pVarResult->vt = VT_UI8;
				return this->get_FileSize(&pVarResult->ullVal);
			case EB_DISP_FILETIME:
				pVarResult->vt = VT_BSTR;
				return this->get_FileTime(&pVarResult->bstrVal);
			case EB_DISP_OFFFILE:
				pVarResult->vt = VT_BOOL;
				return this->get_OffFile(&pVarResult->boolVal);
			case EB_DISP_SENDFROM:
				pVarResult->vt = VT_UI8;
				return this->get_SendFrom(&pVarResult->ullVal);
			case EB_DISP_SENDTO:
				pVarResult->vt = VT_UI8;
				return this->get_SendTo(&pVarResult->ullVal);
			case EB_DISP_PRIVATE:
				pVarResult->vt = VT_BOOL;
				return this->get_Private(&pVarResult->boolVal);
			case EB_DISP_RECEIVEACCOUNT:
				pVarResult->vt = VT_UI8;
				return this->get_ReceiveAccount(&pVarResult->ullVal);
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
			case EB_DISP_RESCMSERVER:
				return this->put_ResCmServer(pDispParams->rgvarg[0].bstrVal);
			case EB_DISP_RESCMAPPNAME:
				return this->put_ResCmAppName(pDispParams->rgvarg[0].bstrVal);
			case EB_DISP_MSGID:
				return this->put_MsgId(pDispParams->rgvarg[0].ullVal);
			case EB_DISP_FILENAME:
				return this->put_FileName(pDispParams->rgvarg[0].bstrVal);
			case EB_DISP_FILESIZE:
				return this->put_FileSize(pDispParams->rgvarg[0].ullVal);
			case EB_DISP_FILETIME:
				return this->put_FileTime(pDispParams->rgvarg[0].bstrVal);
			case EB_DISP_OFFFILE:
				return this->put_OffFile(pDispParams->rgvarg[0].boolVal);
			case EB_DISP_SENDFROM:
				return this->put_SendFrom(pDispParams->rgvarg[0].ullVal);
			case EB_DISP_SENDTO:
				return this->put_SendTo(pDispParams->rgvarg[0].ullVal);
			case EB_DISP_PRIVATE:
				return this->put_Private(pDispParams->rgvarg[0].boolVal);
			case EB_DISP_RECEIVEACCOUNT:
				return this->put_ReceiveAccount(pDispParams->rgvarg[0].ullVal);
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
	CIEB_ChatFileInfo(void)
	{
	}
	CIEB_ChatFileInfo(const CCrFileInfo& pObj)
		: CCrFileInfo(pObj)
	{
	}
};

#endif // __IEB_ChatFileInfo_h__
