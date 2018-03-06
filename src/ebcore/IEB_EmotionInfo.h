// IEB_EmotionInfo.h file here
#ifndef __IEB_EmotionInfo_h__
#define __IEB_EmotionInfo_h__
#include "EBUnknwn.h"
#include "ebdisp.h"

class CIEB_EmotionInfo : public IEB_EmotionInfo
	, public CEBDispatch
	, public EB_EmotionInfo
{
public:
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
	virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ResFile( 
		/* [retval][out] */ BSTR *pVal)
	{
		*pVal = m_sResFile.CopyBSTR();
		return S_OK;
	}
	virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_ResFile( 
		/* [in] */ BSTR newVal)
	{
		m_sResFile = newVal;
		return S_OK;
	}
	virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Index( 
		/* [retval][out] */ ULONG *pVal)
	{
		*pVal = m_nIndex;
		return S_OK;
	}
	virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Index( 
		/* [in] */ ULONG newVal)
	{
		m_nIndex = newVal;
		return S_OK;
	}
	virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Type( 
		/* [retval][out] */ SHORT *pVal)
	{
		*pVal = m_nType;
		return S_OK;
	}
	virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Type( 
		/* [in] */ SHORT newVal)
	{
		m_nType = newVal;
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
	virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_EnterpriseCode( 
		/* [retval][out] */ ULONGLONG *pVal)
	{
		*pVal = m_sEnterpriseCode;
		return S_OK;
	}
	virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_EnterpriseCode( 
		/* [in] */ ULONGLONG newVal)
	{
		m_sEnterpriseCode = newVal;
		return S_OK;
	}
	virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ResType( 
		/* [retval][out] */ SHORT *pVal)
	{
		*pVal = (SHORT)m_nResType;
		return S_OK;
	}
	virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_ResType( 
		/* [in] */ SHORT newVal)
	{
		m_nResType = (EB_RESOURCE_TYPE)newVal;
		return S_OK;
	}
	virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Description( 
		/* [retval][out] */ BSTR *pVal)
	{
		*pVal = m_sDescription.CopyBSTR();
		return S_OK;
	}
	virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Description( 
		/* [in] */ BSTR newVal)
	{
		m_sDescription = newVal;
		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE QueryInterface( 
		/* [in] */ REFIID riid,
		/* [iid_is][out] */ __RPC__deref_out void __RPC_FAR *__RPC_FAR *ppvObject)
	{
		if (riid == IID_IEB_EmotionInfo)
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
		if (bstrName==L"0" || bstrName == L"RESID")
		{
			*rgDispId = EB_DISP_RESID;
		}
		else if (bstrName==L"1" || bstrName == L"RESFILE")
		{
			*rgDispId = EB_DISP_RESFILE;
		}
		else if (bstrName==L"2" || bstrName == L"INDEX")
		{
			*rgDispId = EB_DISP_INDEX;
		}
		else if (bstrName==L"3" || bstrName == L"TYPE")
		{
			*rgDispId = EB_DISP_TYPE;
		}
		else if (bstrName==L"4" || bstrName == L"USERID")
		{
			*rgDispId = EB_DISP_USERID;
		}
		else if (bstrName==L"5" || bstrName == L"ENTERPRISECODE")
		{
			*rgDispId = EB_DISP_ENTERPRISECODE;
		}
		else if (bstrName==L"6" || bstrName == L"RESTYPE")
		{
			*rgDispId = EB_DISP_RESTYPE;
		}
		else if (bstrName==L"7" || bstrName == L"DESCRIPTION")
		{
			*rgDispId = EB_DISP_DESCRIPTION;
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
			case EB_DISP_RESID:
				pVarResult->vt = VT_UI8;
				return this->get_ResId(&pVarResult->ullVal);
			case EB_DISP_RESFILE:
				pVarResult->vt = VT_BSTR;
				return this->get_ResFile(&pVarResult->bstrVal);
			case EB_DISP_INDEX:
				pVarResult->vt = VT_UINT;
				return this->get_Index(&pVarResult->ulVal);
			case EB_DISP_TYPE:
				pVarResult->vt = VT_I2;
				return this->get_Type(&pVarResult->iVal);
			case EB_DISP_USERID:
				pVarResult->vt = VT_UI8;
				return this->get_UserId(&pVarResult->ullVal);
			case EB_DISP_ENTERPRISECODE:
				pVarResult->vt = VT_UI8;
				return this->get_EnterpriseCode(&pVarResult->ullVal);
			case EB_DISP_RESTYPE:
				pVarResult->vt = VT_I2;
				return this->get_ResType(&pVarResult->iVal);
			case EB_DISP_DESCRIPTION:
				pVarResult->vt = VT_BSTR;
				return this->get_Description(&pVarResult->bstrVal);
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
			case EB_DISP_RESID:
				return this->put_ResId(pDispParams->rgvarg[0].ullVal);
			case EB_DISP_RESFILE:
				return this->put_ResFile(pDispParams->rgvarg[0].bstrVal);
			case EB_DISP_INDEX:
				return this->put_Index(pDispParams->rgvarg[0].ulVal);
			case EB_DISP_TYPE:
				return this->put_Type(pDispParams->rgvarg[0].iVal);
			case EB_DISP_USERID:
				return this->put_UserId(pDispParams->rgvarg[0].ullVal);
			case EB_DISP_ENTERPRISECODE:
				return this->put_EnterpriseCode(pDispParams->rgvarg[0].ullVal);
			case EB_DISP_RESTYPE:
				return this->put_ResType(pDispParams->rgvarg[0].iVal);
			case EB_DISP_DESCRIPTION:
				return this->put_Description(pDispParams->rgvarg[0].bstrVal);
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
	CIEB_EmotionInfo(void)
	{
	}
	CIEB_EmotionInfo(const EB_EmotionInfo& pObj)
		: EB_EmotionInfo(&pObj)
	{
	}
};

#endif // __IEB_EmotionInfo_h__
