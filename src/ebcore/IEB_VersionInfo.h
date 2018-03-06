// IEB_VersionInfo.h file here
#ifndef __IEB_VersionInfo_h__
#define __IEB_VersionInfo_h__
#include "EBUnknwn.h"
#include "ebdisp.h"

class CIEB_VersionInfo : public IEB_VersionInfo
	, public CEBDispatch
	, public EB_VersionInfo
{
public:
	virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Version( 
		/* [retval][out] */ BSTR *pVal)
	{
		*pVal = m_sVersion.CopyBSTR();
		return S_OK;
	}
	virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Version( 
		/* [in] */ BSTR newVal)
	{
		m_sVersion = newVal;
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
	virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_UpdateTime( 
		/* [retval][out] */ BSTR *pVal)
	{
		*pVal = m_sUpdateTime.CopyBSTR();
		return S_OK;
	}
	virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_UpdateTime( 
		/* [in] */ BSTR newVal)
	{
		m_sUpdateTime = newVal;
		return S_OK;
	}
	virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_UpdateType( 
		/* [retval][out] */ SHORT *pVal)
	{
		*pVal = m_nUpdateType;
		return S_OK;
	}
	virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_UpdateType( 
		/* [in] */ SHORT newVal)
	{
		m_nUpdateType = newVal;
		return S_OK;
	}
	virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_VersionFile( 
		/* [retval][out] */ BSTR *pVal)
	{
		*pVal = m_sVersionFile.CopyBSTR();
		return S_OK;
	}
	virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_VersionFile( 
		/* [in] */ BSTR newVal)
	{
		m_sVersionFile = newVal;
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

	virtual HRESULT STDMETHODCALLTYPE QueryInterface( 
		/* [in] */ REFIID riid,
		/* [iid_is][out] */ __RPC__deref_out void __RPC_FAR *__RPC_FAR *ppvObject)
	{
		if (riid == IID_IEB_VersionInfo)
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
		if (bstrName==L"0" || bstrName == L"VERSION")
		{
			*rgDispId = EB_DISP_VERSION;
		}
		else if (bstrName==L"1" || bstrName == L"DESCRIPTION")
		{
			*rgDispId = EB_DISP_DESCRIPTION;
		}
		else if (bstrName==L"2" || bstrName == L"UPDATETIME")
		{
			*rgDispId = EB_DISP_UPDATETIME;
		}
		else if (bstrName==L"3" || bstrName == L"UPDATETYPE")
		{
			*rgDispId = EB_DISP_UPDATETYPE;
		}
		else if (bstrName==L"4" || bstrName == L"VERSIONFILE")
		{
			*rgDispId = EB_DISP_VERSIONFILE;
		}
		else if (bstrName==L"5" || bstrName == L"RESID")
		{
			*rgDispId = EB_DISP_RESID;
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
			case EB_DISP_VERSION:
				pVarResult->vt = VT_BSTR;
				return this->get_Version(&pVarResult->bstrVal);
			case EB_DISP_DESCRIPTION:
				pVarResult->vt = VT_BSTR;
				return this->get_Description(&pVarResult->bstrVal);
			case EB_DISP_UPDATETIME:
				pVarResult->vt = VT_BSTR;
				return this->get_UpdateTime(&pVarResult->bstrVal);
			case EB_DISP_UPDATETYPE:
				pVarResult->vt = VT_I2;
				return this->get_UpdateType(&pVarResult->iVal);
			case EB_DISP_VERSIONFILE:
				pVarResult->vt = VT_BSTR;
				return this->get_VersionFile(&pVarResult->bstrVal);
			case EB_DISP_RESID:
				pVarResult->vt = VT_UI8;
				return this->get_ResId(&pVarResult->ullVal);
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
			case EB_DISP_VERSION:
				return this->put_Version(pDispParams->rgvarg[0].bstrVal);
			case EB_DISP_DESCRIPTION:
				return this->put_Description(pDispParams->rgvarg[0].bstrVal);
			case EB_DISP_UPDATETIME:
				return this->put_UpdateTime(pDispParams->rgvarg[0].bstrVal);
			case EB_DISP_UPDATETYPE:
				return this->put_UpdateType(pDispParams->rgvarg[0].iVal);
			case EB_DISP_VERSIONFILE:
				return this->put_VersionFile(pDispParams->rgvarg[0].bstrVal);
			case EB_DISP_RESID:
				return this->put_ResId(pDispParams->rgvarg[0].ullVal);
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
	CIEB_VersionInfo(void)
	{
	}
	CIEB_VersionInfo(const EB_VersionInfo& pObj)
		: EB_VersionInfo(pObj)
	{
	}
};

#endif // __IEB_VersionInfo_h__
