// IEB_AreaInfo.h file here
#ifndef __IEB_AreaInfo_h__
#define __IEB_AreaInfo_h__
#include "EBUnknwn.h"
#include "ebdisp.h"
#include "IEB_ECardInfo.h"

class CIEB_AreaInfo : public IEB_AreaInfo
	, public CEBDispatch
	, public EB_AreaInfo
{
public:
	virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_AreaCode( 
		/* [retval][out] */ LONG *pVal)
	{
		*pVal = m_nAreaCode;
		return S_OK;
	}
	virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_AreaCode( 
		/* [in] */ LONG newVal)
	{
		m_nAreaCode = newVal;
		return S_OK;
	}
	virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ParentCode( 
		/* [retval][out] */ LONG *pVal)
	{
		*pVal = m_nParentCode;
		return S_OK;
	}
	virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_ParentCode( 
		/* [in] */ LONG newVal)
	{
		m_nParentCode = newVal;
		return S_OK;
	}
	virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_AreaName( 
		/* [retval][out] */ BSTR *pVal)
	{
		*pVal = m_sAreaName.CopyBSTR();
		return S_OK;
	}
	virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_AreaName( 
		/* [in] */ BSTR newVal)
	{
		m_sAreaName = newVal;
		return S_OK;
	}
	virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_AreaDigit( 
		/* [retval][out] */ BSTR *pVal)
	{
		*pVal = m_sAreaDigit.CopyBSTR();
		return S_OK;
	}
	virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_AreaDigit( 
		/* [in] */ BSTR newVal)
	{
		m_sAreaDigit = newVal;
		return S_OK;
	}


	virtual HRESULT STDMETHODCALLTYPE QueryInterface( 
		/* [in] */ REFIID riid,
		/* [iid_is][out] */ __RPC__deref_out void __RPC_FAR *__RPC_FAR *ppvObject)
	{
#ifdef IID_IEB_AreaInfo
		if (InlineIsEqualGUID(riid, IID_IEB_AreaInfo))
#else
		if (InlineIsEqualGUID(riid, __uuidof(IEB_AreaInfo)))
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
		if (bstrName==L"0" || bstrName == L"AREACODE")
		{
			*rgDispId = EB_DISP_AREACODE;
		}
		else if (bstrName==L"1" || bstrName == L"PARENTCODE")
		{
			*rgDispId = EB_DISP_PARENTCODE;
		}
		else if (bstrName==L"2" || bstrName == L"AREANAME")
		{
			*rgDispId = EB_DISP_AREANAME;
		}
		else if (bstrName==L"3" || bstrName == L"AREADIGIT")
		{
			*rgDispId = EB_DISP_AREADIGIT;
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
			case EB_DISP_AREACODE:
				pVarResult->vt = VT_I4;
				return this->get_AreaCode(&pVarResult->lVal);
			case EB_DISP_PARENTCODE:
				pVarResult->vt = VT_I4;
				return this->get_ParentCode(&pVarResult->lVal);
			case EB_DISP_AREANAME:
				pVarResult->vt = VT_BSTR;
				return this->get_AreaName(&pVarResult->bstrVal);
			case EB_DISP_AREADIGIT:
				pVarResult->vt = VT_BSTR;
				return this->get_AreaDigit(&pVarResult->bstrVal);
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
			case EB_DISP_AREACODE:
				return this->put_AreaCode(pDispParams->rgvarg[0].lVal);
			case EB_DISP_PARENTCODE:
				return this->put_ParentCode(pDispParams->rgvarg[0].lVal);
			case EB_DISP_AREANAME:
				return this->put_AreaName(pDispParams->rgvarg[0].bstrVal);
			case EB_DISP_AREADIGIT:
				return this->put_AreaDigit(pDispParams->rgvarg[0].bstrVal);
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
	CIEB_AreaInfo(void)
	{
	}
	CIEB_AreaInfo(const EB_AreaInfo& pObj)
		: EB_AreaInfo(&pObj)
	{
	}
	virtual ~CIEB_AreaInfo(void)
	{
	}
};

#endif // __IEB_AreaInfo_h__
