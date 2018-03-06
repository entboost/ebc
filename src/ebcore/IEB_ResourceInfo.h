// IEB_ResourceInfo.h file here
#ifndef __IEB_ResourceInfo_h__
#define __IEB_ResourceInfo_h__
#include "EBUnknwn.h"
#include "ebdisp.h"

class CIEB_ResourceInfo : public IEB_ResourceInfo
	, public CEBDispatch
	, public EB_ResourceInfo
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
	virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ParentResId( 
		/* [retval][out] */ ULONGLONG *pVal)
	{
		*pVal = m_sParentResId;
		return S_OK;
	}
	virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_ParentResId( 
		/* [in] */ ULONGLONG newVal)
	{
		m_sParentResId = newVal;
		return S_OK;
	}
	virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Name( 
		/* [retval][out] */ BSTR *pVal)
	{
		*pVal = m_sName.CopyBSTR();
		return S_OK;
	}
	virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Name( 
		/* [in] */ BSTR newVal)
	{
		m_sName = newVal;
		return S_OK;
	}
	virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ResType( 
		/* [retval][out] */ LONG *pVal)
	{
		*pVal = (EB_RESOURCE_TYPE)m_nResType;
		return S_OK;
	}
	virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_ResType( 
		/* [in] */ LONG newVal)
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
	virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_FromId( 
		/* [retval][out] */ ULONGLONG *pVal)
	{
		*pVal = m_nFromId;
		return S_OK;
	}
	virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_FromId( 
		/* [in] */ ULONGLONG newVal)
	{
		m_nFromId = newVal;
		return S_OK;
	}
	virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_FromType( 
		/* [retval][out] */ LONG *pVal)
	{
		*pVal = (LONG)m_nFromType;
		return S_OK;
	}
	virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_FromType( 
		/* [in] */ LONG newVal)
	{
		m_nFromType = (EB_RESOURCE_FROM_TYPE)newVal;
		return S_OK;
	}
	//virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_EnterpriseCode( 
	//	/* [retval][out] */ ULONGLONG *pVal)
	//{
	//	*pVal = m_sEnterpriseCode;
	//	return S_OK;
	//}
	//virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_EnterpriseCode( 
	//	/* [in] */ ULONGLONG newVal)
	//{
	//	m_sEnterpriseCode = newVal;
	//	return S_OK;
	//}
	//virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_GroupCode( 
	//	/* [retval][out] */ ULONGLONG *pVal)
	//{
	//	*pVal = m_sGroupCode;
	//	return S_OK;
	//}
	//virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_GroupCode( 
	//	/* [in] */ ULONGLONG newVal)
	//{
	//	m_sGroupCode = newVal;
	//	return S_OK;
	//}
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
	virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Time( 
		/* [retval][out] */ BSTR *pVal)
	{
		*pVal = m_sTime.CopyBSTR();
		return S_OK;
	}
	virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Time( 
		/* [in] */ BSTR newVal)
	{
		m_sTime = newVal;
		return S_OK;
	}
	virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Downloads( 
		/* [retval][out] */ ULONG *pVal)
	{
		*pVal = m_nDownloads;
		return S_OK;
	}
	virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Downloads( 
		/* [in] */ ULONG newVal)
	{
		m_nDownloads = newVal;
		return S_OK;
	}
	virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Size( 
		/* [retval][out] */ ULONGLONG *pVal)
	{
		*pVal = m_nSize;
		return S_OK;
	}
	virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Size( 
		/* [in] */ ULONGLONG newVal)
	{
		m_nSize = newVal;
		return S_OK;
	}
	virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Share( 
		/* [retval][out] */ LONG *pVal)
	{
		*pVal = m_nShare;
		return S_OK;
	}
	virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Share( 
		/* [in] */ LONG newVal)
	{
		m_nSize = (EB_RESOURCE_SHARE_TYPE)newVal;
		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE QueryInterface( 
		/* [in] */ REFIID riid,
		/* [iid_is][out] */ __RPC__deref_out void __RPC_FAR *__RPC_FAR *ppvObject)
	{
		if (riid == IID_IEB_ResourceInfo)
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
		if (bstrName=L"0" || bstrName == L"RESID")
		{
			*rgDispId = EB_DISP_RESID;
		}
		else if (bstrName=L"1" || bstrName == L"PARENTRESID")
		{
			*rgDispId = EB_DISP_PARENTRESID;
		}
		else if (bstrName=L"2" || bstrName == L"NAME")
		{
			*rgDispId = EB_DISP_NAME;
		}
		else if (bstrName=L"3" || bstrName == L"RESTYPE")
		{
			*rgDispId = EB_DISP_RESTYPE;
		}
		else if (bstrName=L"4" || bstrName == L"DESCRIPTION")
		{
			*rgDispId = EB_DISP_DESCRIPTION;
		}
		else if (bstrName=L"5" || bstrName == L"FROM_ID")
		{
			*rgDispId = EB_DISP_FROM_ID;
		}
		else if (bstrName=L"6" || bstrName == L"FROM_TYPE")
		{
			*rgDispId = EB_DISP_FROM_TYPE;
		}
		//else if (bstrName=L"5" || bstrName == L"ENTERPRISECODE")
		//{
		//	*rgDispId = EB_DISP_ENTERPRISECODE;
		//}
		//else if (bstrName=L"6" || bstrName == L"GROUPCODE")
		//{
		//	*rgDispId = EB_DISP_GROUPCODE;
		//}
		else if (bstrName=L"7" || bstrName == L"CREATEUSERID")
		{
			*rgDispId = EB_DISP_CREATEUSERID;
		}
		else if (bstrName=L"8" || bstrName == L"TIME")
		{
			*rgDispId = EB_DISP_TIME;
		}
		else if (bstrName=L"9" || bstrName == L"DOWNLOADS")
		{
			*rgDispId = EB_DISP_DOWNLOADS;
		}
		else if (bstrName=L"10" || bstrName == L"SIZE")
		{
			*rgDispId = EB_DISP_SIZE;
		}
		else if (bstrName=L"11" || bstrName == L"SHARE")
		{
			*rgDispId = EB_DISP_SHARE;
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
			case EB_DISP_PARENTRESID:
				pVarResult->vt = VT_UI8;
				return this->get_ParentResId(&pVarResult->ullVal);
			case EB_DISP_NAME:
				pVarResult->vt = VT_BSTR;
				return this->get_Name(&pVarResult->bstrVal);
			case EB_DISP_RESTYPE:
				pVarResult->vt = VT_INT;
				pVarResult->lVal = (LONG)this->m_nResType;
				return S_OK;
			case EB_DISP_DESCRIPTION:
				pVarResult->vt = VT_BSTR;
				return this->get_Description(&pVarResult->bstrVal);
			case EB_DISP_FROM_ID:
				pVarResult->vt = VT_UI8;
				return this->get_FromId(&pVarResult->ullVal);
			case EB_DISP_FROM_TYPE:
				pVarResult->vt = VT_INT;
				return this->get_FromType(&pVarResult->lVal);
			//case EB_DISP_ENTERPRISECODE:
			//	pVarResult->vt = VT_UI8;
			//	return this->get_EnterpriseCode(&pVarResult->ullVal);
			//case EB_DISP_GROUPCODE:
			//	pVarResult->vt = VT_UI8;
			//	return this->get_GroupCode(&pVarResult->ullVal);
			case EB_DISP_CREATEUSERID:
				pVarResult->vt = VT_UI8;
				return this->get_CreateUserId(&pVarResult->ullVal);
			case EB_DISP_TIME:
				pVarResult->vt = VT_BSTR;
				return this->get_Time(&pVarResult->bstrVal);
			case EB_DISP_DOWNLOADS:
				pVarResult->vt = VT_UINT;
				return this->get_Downloads(&pVarResult->ulVal);
			case EB_DISP_SIZE:
				pVarResult->vt = VT_UI8;
				return this->get_Size(&pVarResult->ullVal);
			case EB_DISP_SHARE:
				pVarResult->vt = VT_I4;
				return this->get_Share(&pVarResult->lVal);
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
			case EB_DISP_PARENTRESID:
				return this->put_ParentResId(pDispParams->rgvarg[0].ullVal);
			case EB_DISP_NAME:
				return this->put_Name(pDispParams->rgvarg[0].bstrVal);
			case EB_DISP_RESTYPE:
				return this->put_ResType(pDispParams->rgvarg[0].lVal);
			case EB_DISP_DESCRIPTION:
				return this->put_Description(pDispParams->rgvarg[0].bstrVal);
			case EB_DISP_FROM_ID:
				return this->put_FromId(pDispParams->rgvarg[0].ullVal);
			case EB_DISP_FROM_TYPE:
				return this->put_FromType(pDispParams->rgvarg[0].lVal);
			//case EB_DISP_ENTERPRISECODE:
			//	return this->put_EnterpriseCode(pDispParams->rgvarg[0].ullVal);
			//case EB_DISP_GROUPCODE:
			//	return this->put_GroupCode(pDispParams->rgvarg[0].ullVal);
			case EB_DISP_CREATEUSERID:
				return this->put_CreateUserId(pDispParams->rgvarg[0].ullVal);
			case EB_DISP_TIME:
				return this->put_Time(pDispParams->rgvarg[0].bstrVal);
			case EB_DISP_DOWNLOADS:
				return this->put_Downloads(pDispParams->rgvarg[0].ulVal);
			case EB_DISP_SIZE:
				return this->put_Size(pDispParams->rgvarg[0].ullVal);
			case EB_DISP_SHARE:
				return this->put_Share(pDispParams->rgvarg[0].lVal);
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
	CIEB_ResourceInfo(void)
	{
	}
	CIEB_ResourceInfo(const EB_ResourceInfo& pObj)
		: EB_ResourceInfo(pObj)
	{
	}
};

#endif // __IEB_ResourceInfo_h__
