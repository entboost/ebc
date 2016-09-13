// IEB_ChatRichInfo.h file here
#ifndef __IEB_ChatRichInfo_h__
#define __IEB_ChatRichInfo_h__
#include "EBUnknwn.h"
#include "ebdisp.h"
#include "atlsafe.h"
#include "IEB_ChatMsgItem.h"

class CIEB_ChatRichInfo : public IEB_ChatRichInfo
	, public CEBDispatch
	, public CCrRichInfo
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
	virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_RichMsg( 
		/* [retval][out] */ VARIANT *pVal)
	{
		if (m_pRichMsg == NULL) return S_FALSE;
		CComSafeArray<VARIANT> m_sa;
		const std::vector<EB_ChatRoomMsgItem*> & pList = m_pRichMsg->GetList();
		for (size_t i=0;i<pList.size();i++)
		{
			const EB_ChatRoomMsgItem* pItem = pList[i];
			CIEB_ChatMsgItem* pIEBChatMsgItem = new CIEB_ChatMsgItem(*pItem);
			CComPtr<IEB_ChatMsgItem> pOutInterface;
			pIEBChatMsgItem->QueryInterface(__uuidof(IEB_ChatMsgItem),(void**)&pOutInterface);
			m_sa.Add(CComVariant(pOutInterface));
		}
		VariantInit(pVal);
		CComVariant var(m_sa);
		var.Detach(pVal);
		return S_OK;
	}
	virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_RichMsg( 
		/* [in] */ VARIANT *newVal)
	{
		if (newVal==NULL || newVal->parray==NULL)// || newVal->vt != VT_ARRAY)
		{
			return E_INVALIDARG;
		}
		if (m_pRichMsg == NULL)
		{
			m_bNewRichMsg = true;
			m_pRichMsg = new EB_ChatRoomRichMsg();
		}
		m_pRichMsg->clear();
		CComSafeArray<VARIANT> m_sa(newVal->parray);
		for (ULONG i=0;i<m_sa.GetCount();i++)
		{
			CComVariant var = m_sa.GetAt(i);
			if (var.vt != VT_DISPATCH)
				return E_INVALIDARG;
			SHORT nType = 0;
			CComBSTR sText;
			BYTE* pData = NULL;
			ULONG nSize = 0;
			CComPtr<IEB_ChatMsgItem> pInterface;
			var.pdispVal->QueryInterface(__uuidof(IEB_ChatMsgItem),(void**)&pInterface);
			if (pInterface != NULL)
			{
				pInterface->get_Type(&nType);
				pInterface->get_Text(&sText);
				pInterface->get_Data(&pData);
				pInterface->get_Size(&nSize);
				if (nType!=EB_ChatRoomMsgItem::MIT_OBJECT && pData != NULL)
				{
					delete []pData;
					pData = NULL;
				}
			}else
			{
				CComVariant vValue;
				HRESULT hr;
				hr = GetArrayVariantOfName(var.pdispVal,L"Type",&vValue);
				if (SUCCEEDED(hr))
				{
					nType = vValue.iVal;
					VariantClear(&vValue);
				}
				hr = GetArrayVariantOfName(var.pdispVal,L"Text",&vValue);
				if (SUCCEEDED(hr))
				{
					sText = vValue.bstrVal;
					VariantClear(&vValue);
				}
				hr = GetArrayVariantOfName(var.pdispVal,L"Data",&vValue);
				if (SUCCEEDED(hr))
				{
					pData = (BYTE*)vValue.byref;
					//if (!GetBinaryFromVariant(vValue,&pData,&nSize))
					//{
					//	return E_INVALIDARG;
					//}
					VariantClear(&vValue);
				}
				hr = GetArrayVariantOfName(var.pdispVal,L"Size",&vValue);
				if (SUCCEEDED(hr))
				{
					nSize = vValue.ulVal;
					VariantClear(&vValue);
				}
			}
			if (nType==EB_ChatRoomMsgItem::MIT_TEXT)
			{
				//// http://blog.csdn.net/collin1211/article/details/4836892
				m_pRichMsg->AddText(CEBString((BSTR)sText,CP_UTF8).c_str());
			}
			else if (nType==EB_ChatRoomMsgItem::MIT_OBJECT)
			{
				EB_ChatRoomMsgItem* pMsgItem = (EB_ChatRoomMsgItem*)m_pRichMsg->AddObject((char*)pData,nSize);
				if (pMsgItem)
					pMsgItem->SetText(CEBString(sText).c_str());
			}
			else if (nType==EB_ChatRoomMsgItem::MIT_RESOURCE)
			{
				m_pRichMsg->AddResource(CEBString(sText).c_str());
			}
		}
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

	virtual HRESULT STDMETHODCALLTYPE QueryInterface( 
		/* [in] */ REFIID riid,
		/* [iid_is][out] */ __RPC__deref_out void __RPC_FAR *__RPC_FAR *ppvObject)
	{
#ifdef IID_IEB_ChatRichInfo
		if (InlineIsEqualGUID(riid, IID_IEB_ChatRichInfo))
#else
		if (InlineIsEqualGUID(riid, __uuidof(IEB_ChatRichInfo)))
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
		else if (bstrName==L"3" || bstrName == L"RICHMSG")
		{
			*rgDispId = EB_DISP_RICHMSG;
		}
		else if (bstrName==L"4" || bstrName == L"SENDFROM")
		{
			*rgDispId = EB_DISP_SENDFROM;
		}
		else if (bstrName==L"5" || bstrName == L"SENDTO")
		{
			*rgDispId = EB_DISP_SENDTO;
		}
		else if (bstrName==L"6" || bstrName == L"PRIVATE")
		{
			*rgDispId = EB_DISP_PRIVATE;
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
			case EB_DISP_RICHMSG:
				pVarResult->vt = VT_ARRAY|VT_DISPATCH;
				return this->get_RichMsg(pVarResult);
			case EB_DISP_SENDFROM:
				pVarResult->vt = VT_UI8;
				return this->get_SendFrom(&pVarResult->ullVal);
			case EB_DISP_SENDTO:
				pVarResult->vt = VT_UI8;
				return this->get_SendTo(&pVarResult->ullVal);
			case EB_DISP_PRIVATE:
				pVarResult->vt = VT_BOOL;
				return this->get_Private(&pVarResult->boolVal);
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
			case EB_DISP_RICHMSG:
				return this->put_RichMsg(&pDispParams->rgvarg[0]);
			case EB_DISP_SENDFROM:
				return this->put_SendFrom(pDispParams->rgvarg[0].ullVal);
			case EB_DISP_SENDTO:
				return this->put_SendTo(pDispParams->rgvarg[0].ullVal);
			case EB_DISP_PRIVATE:
				return this->put_Private(pDispParams->rgvarg[0].boolVal);
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
	CIEB_ChatRichInfo(void)
		: m_bNewRichMsg(false)
	{
	}
	CIEB_ChatRichInfo(const CCrRichInfo& pObj)
		: CCrRichInfo(pObj)
		, m_bNewRichMsg(false)
	{
	}
	virtual ~CIEB_ChatRichInfo(void)
	{
		if (m_bNewRichMsg && m_pRichMsg != NULL)
		{
			delete m_pRichMsg;
			m_pRichMsg = NULL;
		}
	}
private:
	bool m_bNewRichMsg;
};

#endif // __IEB_ChatRichInfo_h__
