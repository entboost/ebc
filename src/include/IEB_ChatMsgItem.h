// IEB_ChatMsgItem.h file here
#ifndef __IEB_ChatMsgItem_h__
#define __IEB_ChatMsgItem_h__
#include <comutil.h>
#include "EBUnknwn.h"
#include "ebdisp.h"

class CIEB_ChatMsgItem : public IEB_ChatMsgItem
	, public CEBDispatch
	, public EB_ChatRoomMsgItem
{
public:
	virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Type( 
		/* [retval][out] */ SHORT *pVal)
	{
		*pVal = (SHORT)EB_ChatRoomMsgItem::GetType();
		return S_OK;
	}
	virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Type( 
		/* [in] */ SHORT newVal)
	{
		EB_ChatRoomMsgItem::SetType((EB_ChatRoomMsgItem::MsgItemType)newVal);
		return S_OK;
	}
	virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Text( 
		/* [retval][out] */ BSTR *pVal)
	{
		*pVal = EB_ChatRoomMsgItem::GetText().CopyBSTR(CP_UTF8);
		return S_OK;
	}
	virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Text( 
		/* [in] */ BSTR newVal)
	{
		EB_ChatRoomMsgItem::SetText(CEBString(newVal,CP_UTF8));
		return S_OK;
	}
	virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Data( 
		/* [retval][out] */ BYTE **pVal)
	{
		ULONG nSize = EB_ChatRoomMsgItem::GetSize();
		if (nSize > 0 && EB_ChatRoomMsgItem::GetData() != NULL)
		{
			*pVal = new BYTE[nSize+1];
			memcpy((void*)(*pVal),EB_ChatRoomMsgItem::GetData(),nSize);
			//(*pVal)[nSize] = '0';
		}
		return S_OK;
	}
	virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Data( 
		/* [in] */ BYTE *newVal)
	{
		EB_ChatRoomMsgItem::SetData((const char*)newVal);
		return S_OK;
	}
	virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Size( 
		/* [retval][out] */ ULONG *pVal)
	{
		//*pVal = nSize;
		*pVal = EB_ChatRoomMsgItem::GetSize();
		return S_OK;
	}
	virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Size( 
		/* [in] */ ULONG newVal)
	{
		EB_ChatRoomMsgItem::SetSize(newVal);
		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE QueryInterface( 
		/* [in] */ REFIID riid,
		/* [iid_is][out] */ __RPC__deref_out void __RPC_FAR *__RPC_FAR *ppvObject)
	{
#ifdef IID_IEB_ChatMsgItem
		if (InlineIsEqualGUID(riid, IID_IEB_ChatMsgItem))
#else
		if (InlineIsEqualGUID(riid, __uuidof(IEB_ChatMsgItem)))
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
		if (bstrName==L"0" || bstrName == L"TYPE")
		{
			*rgDispId = EB_DISP_TYPE;
		}
		else if (bstrName==L"1" || bstrName == L"TEXT")
		{
			*rgDispId = EB_DISP_TEXT;
		}
		else if (bstrName==L"2" || bstrName == L"DATA")
		{
			*rgDispId = EB_DISP_DATA;
		}
		else if (bstrName==L"3" || bstrName == L"SIZE")
		{
			*rgDispId = EB_DISP_SIZE;
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
			case EB_DISP_TYPE:
				pVarResult->vt = VT_I2;
				return this->get_Type(&pVarResult->iVal);
			case EB_DISP_TEXT:
				pVarResult->vt = VT_BSTR;
				return this->get_Text(&pVarResult->bstrVal);
			case EB_DISP_DATA:
				{
					pVarResult->vt = VT_UI1|VT_BYREF;
					return this->get_Data((BYTE**)&pVarResult->byref);
					//CComVariant var;
					//if (!PutBinaryIntoVariant(&var,(const BYTE*)EB_ChatRoomMsgItem::GetData(),EB_ChatRoomMsgItem::GetSize()))
					//{
					//	return E_FAIL;
					//}
					//var.Detach(pVarResult);
					//return S_OK;
				}
			case EB_DISP_SIZE:
				pVarResult->vt = VT_UI4;
				return this->get_Size(&pVarResult->ulVal);
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
			case EB_DISP_TYPE:
				return this->put_Type(pDispParams->rgvarg[0].iVal);
			case EB_DISP_TEXT:
				return this->put_Text(pDispParams->rgvarg[0].bstrVal);
			case EB_DISP_DATA:
				{
					return this->put_Data((BYTE*)pDispParams->rgvarg[0].byref);
					//CComVariant var(pDispParams->rgvarg[0]);
					//BYTE * pData = NULL;
					//ULONG nSize = 0;
					//if (!GetBinaryFromVariant(var,&pData,&nSize))
					//{
					//	return E_FAIL;
					//}
					//EB_ChatRoomMsgItem::SetSize(nSize);
					//EB_ChatRoomMsgItem::SetData((const char*)pData);
					//return S_OK;
				}break;
			case EB_DISP_SIZE:
				return this->put_Size(pDispParams->rgvarg[0].ulVal);
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
	CIEB_ChatMsgItem(void)
	{
	}
	CIEB_ChatMsgItem(const EB_ChatRoomMsgItem& pObj)
		: EB_ChatRoomMsgItem(pObj)
	{
	}
};

#endif // __IEB_ChatMsgItem_h__
