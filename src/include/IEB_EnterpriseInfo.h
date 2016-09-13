// IEB_EnterpriseInfo.h file here
#ifndef __IEB_EnterpriseInfo_h__
#define __IEB_EnterpriseInfo_h__
#include "EBUnknwn.h"
#include "ebdisp.h"

class CIEB_EnterpriseInfo : public IEB_EnterpriseInfo
	, public CEBDispatch
	, public EB_EnterpriseInfo
{
public:
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
	virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_EnterpriseName( 
		/* [retval][out] */ BSTR *pVal)
	{
		*pVal = m_sEnterpriseName.CopyBSTR();
		return S_OK;
	}
	virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_EnterpriseName( 
		/* [in] */ BSTR newVal)
	{
		m_sEnterpriseName = newVal;
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
	virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Phone( 
		/* [retval][out] */ BSTR *pVal)
	{
		*pVal = m_sPhone.CopyBSTR();
		return S_OK;
	}
	virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Phone( 
		/* [in] */ BSTR newVal)
	{
		m_sPhone = newVal;
		return S_OK;
	}
	virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Fax( 
		/* [retval][out] */ BSTR *pVal)
	{
		*pVal = m_sFax.CopyBSTR();
		return S_OK;
	}
	virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Fax( 
		/* [in] */ BSTR newVal)
	{
		m_sFax = newVal;
		return S_OK;
	}
	virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Email( 
		/* [retval][out] */ BSTR *pVal)
	{
		*pVal = m_sEmail.CopyBSTR();
		return S_OK;
	}
	virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Email( 
		/* [in] */ BSTR newVal)
	{
		m_sEmail = newVal;
		return S_OK;
	}
	virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Url( 
		/* [retval][out] */ BSTR *pVal)
	{
		*pVal = m_sUrl.CopyBSTR();
		return S_OK;
	}
	virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Url( 
		/* [in] */ BSTR newVal)
	{
		m_sUrl = newVal;
		return S_OK;
	}
	virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Address( 
		/* [retval][out] */ BSTR *pVal)
	{
		*pVal = m_sAddress.CopyBSTR();
		return S_OK;
	}
	virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Address( 
		/* [in] */ BSTR newVal)
	{
		m_sAddress = newVal;
		return S_OK;
	}
	virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_CreateTime( 
		/* [retval][out] */ BSTR *pVal)
	{
		*pVal = m_sCreateTime.CopyBSTR();
		return S_OK;
	}
	virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_CreateTime( 
		/* [in] */ BSTR newVal)
	{
		m_sCreateTime = newVal;
		return S_OK;
	}
	virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_CreateUserId( 
		/* [retval][out] */ ULONGLONG *pVal)
	{
		*pVal = m_nCreateUserId;
		return S_OK;
	}
	virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_CreateUserId( 
		/* [in] */ ULONGLONG newVal)
	{
		m_nCreateUserId = newVal;
		return S_OK;
	}
	virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_EntExt( 
		/* [retval][out] */ ULONG *pVal)
	{
		*pVal = m_nEntExt;
		return S_OK;
	}
	virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_EntExt( 
		/* [in] */ ULONG newVal)
	{
		m_nEntExt = newVal;
		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE QueryInterface( 
		/* [in] */ REFIID riid,
		/* [iid_is][out] */ __RPC__deref_out void __RPC_FAR *__RPC_FAR *ppvObject)
	{
#ifdef IID_IEB_EnterpriseInfo
		if (InlineIsEqualGUID(riid, IID_IEB_EnterpriseInfo))
#else
		if (InlineIsEqualGUID(riid, __uuidof(IEB_EnterpriseInfo)))
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
		if (bstrName==L"0" || bstrName == L"ENTERPRISECODE")
		{
			*rgDispId = EB_DISP_ENTERPRISECODE;
		}
		else if (bstrName==L"1" || bstrName == L"ENTERPRISENAME")
		{
			*rgDispId = EB_DISP_ENTERPRISENAME;
		}
		else if (bstrName==L"2" || bstrName == L"DESCRIPTION")
		{
			*rgDispId = EB_DISP_DESCRIPTION;
		}
		else if (bstrName==L"3" || bstrName == L"PHONE")
		{
			*rgDispId = EB_DISP_PHONE;
		}
		else if (bstrName==L"4" || bstrName == L"FAX")
		{
			*rgDispId = EB_DISP_FAX;
		}
		else if (bstrName==L"5" || bstrName == L"EMAIL")
		{
			*rgDispId = EB_DISP_EMAIL;
		}
		else if (bstrName==L"6" || bstrName == L"URL")
		{
			*rgDispId = EB_DISP_URL;
		}
		else if (bstrName==L"7" || bstrName == L"ADDRESS")
		{
			*rgDispId = EB_DISP_ADDRESS;
		}
		else if (bstrName==L"8" || bstrName == L"CREATETIME")
		{
			*rgDispId = EB_DISP_CREATETIME;
		}
		else if (bstrName==L"9" || bstrName == L"CREATEUSERID")
		{
			*rgDispId = EB_DISP_CREATEUSERID;
		}
		else if (bstrName==L"10" || bstrName == L"ENTEXT")
		{
			*rgDispId = EB_DISP_ENTEXT;
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
			case EB_DISP_ENTERPRISECODE:
				pVarResult->vt = VT_UI8;
				return this->get_EnterpriseCode(&pVarResult->ullVal);
			case EB_DISP_ENTERPRISENAME:
				pVarResult->vt = VT_BSTR;
				return this->get_EnterpriseName(&pVarResult->bstrVal);
			case EB_DISP_DESCRIPTION:
				pVarResult->vt = VT_BSTR;
				return this->get_Description(&pVarResult->bstrVal);
			case EB_DISP_PHONE:
				pVarResult->vt = VT_BSTR;
				return this->get_Phone(&pVarResult->bstrVal);
			case EB_DISP_FAX:
				pVarResult->vt = VT_BSTR;
				return this->get_Fax(&pVarResult->bstrVal);
			case EB_DISP_EMAIL:
				pVarResult->vt = VT_BSTR;
				return this->get_Email(&pVarResult->bstrVal);
			case EB_DISP_URL:
				pVarResult->vt = VT_BSTR;
				return this->get_Url(&pVarResult->bstrVal);
			case EB_DISP_ADDRESS:
				pVarResult->vt = VT_BSTR;
				return this->get_Address(&pVarResult->bstrVal);
			case EB_DISP_CREATETIME:
				pVarResult->vt = VT_BSTR;
				return this->get_CreateTime(&pVarResult->bstrVal);
			case EB_DISP_CREATEUSERID:
				pVarResult->vt = VT_UI8;
				return this->get_CreateUserId(&pVarResult->ullVal);
			case EB_DISP_ENTEXT:
				pVarResult->vt = VT_UI4;
				return this->get_EntExt(&pVarResult->ulVal);
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
			case EB_DISP_ENTERPRISECODE:
				return this->put_EnterpriseCode(pDispParams->rgvarg[0].ullVal);
			case EB_DISP_ENTERPRISENAME:
				return this->put_EnterpriseName(pDispParams->rgvarg[0].bstrVal);
			case EB_DISP_DESCRIPTION:
				return this->put_Description(pDispParams->rgvarg[0].bstrVal);
			case EB_DISP_PHONE:
				return this->put_Phone(pDispParams->rgvarg[0].bstrVal);
			case EB_DISP_FAX:
				return this->put_Fax(pDispParams->rgvarg[0].bstrVal);
			case EB_DISP_EMAIL:
				return this->put_Email(pDispParams->rgvarg[0].bstrVal);
			case EB_DISP_URL:
				return this->put_Url(pDispParams->rgvarg[0].bstrVal);
			case EB_DISP_ADDRESS:
				return this->put_Address(pDispParams->rgvarg[0].bstrVal);
			case EB_DISP_CREATETIME:
				return this->put_CreateTime(pDispParams->rgvarg[0].bstrVal);
			case EB_DISP_CREATEUSERID:
				return this->put_CreateUserId(pDispParams->rgvarg[0].ullVal);
			case EB_DISP_ENTEXT:
				return this->put_EntExt(pDispParams->rgvarg[0].ulVal);
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
	CIEB_EnterpriseInfo(void)
	{
	}
	CIEB_EnterpriseInfo(const EB_EnterpriseInfo& pObj)
		: EB_EnterpriseInfo(&pObj)
	{
	}
};

#endif // __IEB_EnterpriseInfo_h__
