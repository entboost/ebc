// IEB_ECardInfo.h file here
#ifndef __IEB_ECardInfo_h__
#define __IEB_ECardInfo_h__
#include "EBUnknwn.h"
#include "ebdisp.h"

class CIEB_ECardInfo : public IEB_ECardInfo
	, public CEBDispatch
	, public EB_ECardInfo
{
public:
	virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_AccountType( 
		/* [retval][out] */ SHORT *pVal)
	{
		*pVal = m_nAccountType;
		return S_OK;
	}
	virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_AccountType( 
		/* [in] */ SHORT newVal)
	{
		m_nAccountType = (EB_ACCOUNT_TYPE)newVal;
		return S_OK;
	}
	virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_MemberCode( 
		/* [retval][out] */ ULONGLONG *pVal)
	{
		*pVal = m_sMemberCode;
		return S_OK;
	}
	virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_MemberCode( 
		/* [in] */ ULONGLONG newVal)
	{
		m_sMemberCode = newVal;
		return S_OK;
	}
	virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_MemberUserId( 
		/* [retval][out] */ ULONGLONG *pVal)
	{
		*pVal = m_nMemberUserId;
		return S_OK;
	}
	virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_MemberUserId( 
		/* [in] */ ULONGLONG newVal)
	{
		m_nMemberUserId = newVal;
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
	virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Tel( 
		/* [retval][out] */ BSTR *pVal)
	{
		*pVal = m_sTel.CopyBSTR();
		return S_OK;
	}
	virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Tel( 
		/* [in] */ BSTR newVal)
	{
		m_sTel = newVal;
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
	virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Title( 
		/* [retval][out] */ BSTR *pVal)
	{
		*pVal = m_sTitle.CopyBSTR();
		return S_OK;
	}
	virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Title( 
		/* [in] */ BSTR newVal)
	{
		m_sTitle = newVal;
		return S_OK;
	}
	virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_GroupName( 
		/* [retval][out] */ BSTR *pVal)
	{
		*pVal = m_sGroupName.CopyBSTR();
		return S_OK;
	}
	virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_GroupName( 
		/* [in] */ BSTR newVal)
	{
		m_sGroupName = newVal;
		return S_OK;
	}
	virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_GroupCode( 
		/* [retval][out] */ ULONGLONG *pVal)
	{
		*pVal = m_sGroupCode;
		return S_OK;
	}
	virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_GroupCode( 
		/* [in] */ ULONGLONG newVal)
	{
		m_sGroupCode = newVal;
		return S_OK;
	}
	virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Enterprise( 
		/* [retval][out] */ BSTR *pVal)
	{
		*pVal = m_sEnterprise.CopyBSTR();
		return S_OK;
	}
	virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Enterprise( 
		/* [in] */ BSTR newVal)
	{
		m_sEnterprise = newVal;
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
	virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_UserSignId( 
		/* [retval][out] */ ULONGLONG *pVal)
	{
		*pVal = m_nUserSignId;
		return S_OK;
	}
	virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_UserSignId( 
		/* [in] */ ULONGLONG newVal)
	{
		m_nUserSignId = newVal;
		return S_OK;
	}
	virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_FromIp( 
		/* [retval][out] */ ULONGLONG *pVal)
	{
		*pVal = m_nFromIp;
		return S_OK;
	}
	virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_FromIp( 
		/* [in] */ ULONGLONG newVal)
	{
		m_nFromIp = newVal;
		return S_OK;
	}
	virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_HeadResId( 
		/* [retval][out] */ ULONGLONG *pVal)
	{
		*pVal = m_nHeadResId;
		return S_OK;
	}
	virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_HeadResId( 
		/* [in] */ ULONGLONG newVal)
	{
		m_nHeadResId = newVal;
		return S_OK;
	}
	virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_HeadCmServer( 
		/* [retval][out] */ BSTR *pVal)
	{
		*pVal = m_sHeadCmServer.CopyBSTR();
		return S_OK;
	}
	virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_HeadCmServer( 
		/* [in] */ BSTR newVal)
	{
		m_sHeadCmServer = newVal;
		return S_OK;
	}
	virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_HeadHttpServer( 
		/* [retval][out] */ BSTR *pVal)
	{
		*pVal = m_sHeadHttpServer.CopyBSTR();
		return S_OK;
	}
	virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_HeadHttpServer( 
		/* [in] */ BSTR newVal)
	{
		m_sHeadHttpServer = newVal;
		return S_OK;
	}
	virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_HeadMd5( 
		/* [retval][out] */ BSTR *pVal)
	{
		*pVal = m_sHeadMd5.CopyBSTR();
		return S_OK;
	}
	virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_HeadMd5( 
		/* [in] */ BSTR newVal)
	{
		m_sHeadMd5 = newVal;
		return S_OK;
	}
	virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_HeadResourceFile( 
		/* [retval][out] */ BSTR *pVal)
	{
		*pVal = m_sHeadResourceFile.CopyBSTR();
		return S_OK;
	}
	virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_HeadResourceFile( 
		/* [in] */ BSTR newVal)
	{
		m_sHeadResourceFile = newVal;
		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE QueryInterface( 
		/* [in] */ REFIID riid,
		/* [iid_is][out] */ __RPC__deref_out void __RPC_FAR *__RPC_FAR *ppvObject)
	{
		if (riid == IID_IEB_ECardInfo)
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
		if (bstrName==L"0" || bstrName == L"ACCOUNTTYPE")
		{
			*rgDispId = EB_DISP_ACCOUNTTYPE;
		}
		else if (bstrName==L"1" || bstrName == L"MEMBERCODE")
		{
			*rgDispId = EB_DISP_MEMBERCODE;
		}
		else if (bstrName==L"2" || bstrName == L"MEMBERUSERID")
		{
			*rgDispId = EB_DISP_MEMBERUSERID;
		}
		else if (bstrName==L"3" || bstrName == L"NAME")
		{
			*rgDispId = EB_DISP_NAME;
		}
		else if (bstrName==L"4" || bstrName == L"PHONE")
		{
			*rgDispId = EB_DISP_PHONE;
		}
		else if (bstrName==L"5" || bstrName == L"TEL")
		{
			*rgDispId = EB_DISP_TEL;
		}
		else if (bstrName==L"6" || bstrName == L"EMAIL")
		{
			*rgDispId = EB_DISP_EMAIL;
		}
		else if (bstrName==L"7" || bstrName == L"TITLE")
		{
			*rgDispId = EB_DISP_TITLE;
		}
		else if (bstrName==L"8" || bstrName == L"GROUPNAME")
		{
			*rgDispId = EB_DISP_GROUPNAME;
		}
		else if (bstrName==L"9" || bstrName == L"GROUPCODE")
		{
			*rgDispId = EB_DISP_GROUPCODE;
		}
		else if (bstrName==L"10" || bstrName == L"ENTERPRISE")
		{
			*rgDispId = EB_DISP_ENTERPRISE;
		}
		else if (bstrName==L"11" || bstrName == L"ADDRESS")
		{
			*rgDispId = EB_DISP_ADDRESS;
		}
		else if (bstrName==L"12" || bstrName == L"USERSIGNID")
		{
			*rgDispId = EB_DISP_USERSIGNID;
		}
		else if (bstrName==L"13" || bstrName == L"FROMIP")
		{
			*rgDispId = EB_DISP_FROMIP;
		}
		else if (bstrName==L"14" || bstrName == L"HEADRESID")
		{
			*rgDispId = EB_DISP_HEADRESID;
		}
		else if (bstrName==L"15" || bstrName == L"HEADCMSERVER")
		{
			*rgDispId = EB_DISP_HEADCMSERVER;
		}
		else if (bstrName==L"16" || bstrName == L"HEADHTTPSERVER")
		{
			*rgDispId = EB_DISP_HEADHTTPSERVER;
		}
		else if (bstrName==L"17" || bstrName == L"HEADMD5")
		{
			*rgDispId = EB_DISP_HEADMD5;
		}
		else if (bstrName==L"18" || bstrName == L"HEADRESOURCEFILE")
		{
			*rgDispId = EB_DISP_HEADRESOURCEFILE;
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
			case EB_DISP_ACCOUNTTYPE:
				pVarResult->vt = VT_I2;
				return this->get_AccountType(&pVarResult->iVal);
			case EB_DISP_MEMBERCODE:
				pVarResult->vt = VT_UI8;
				return this->get_MemberCode(&pVarResult->ullVal);
			case EB_DISP_MEMBERUSERID:
				pVarResult->vt = VT_UI8;
				return this->get_MemberUserId(&pVarResult->ullVal);
			case EB_DISP_NAME:
				pVarResult->vt = VT_BSTR;
				return this->get_Name(&pVarResult->bstrVal);
			case EB_DISP_PHONE:
				pVarResult->vt = VT_BSTR;
				return this->get_Phone(&pVarResult->bstrVal);
			case EB_DISP_TEL:
				pVarResult->vt = VT_BSTR;
				return this->get_Tel(&pVarResult->bstrVal);
			case EB_DISP_EMAIL:
				pVarResult->vt = VT_BSTR;
				return this->get_Email(&pVarResult->bstrVal);
			case EB_DISP_TITLE:
				pVarResult->vt = VT_BSTR;
				return this->get_Title(&pVarResult->bstrVal);
			case EB_DISP_GROUPNAME:
				pVarResult->vt = VT_BSTR;
				return this->get_GroupName(&pVarResult->bstrVal);
			case EB_DISP_GROUPCODE:
				pVarResult->vt = VT_UI8;
				return this->get_GroupCode(&pVarResult->ullVal);
			case EB_DISP_ENTERPRISE:
				pVarResult->vt = VT_BSTR;
				return this->get_Enterprise(&pVarResult->bstrVal);
			case EB_DISP_ADDRESS:
				pVarResult->vt = VT_BSTR;
				return this->get_Address(&pVarResult->bstrVal);
			case EB_DISP_USERSIGNID:
				pVarResult->vt = VT_UI8;
				return this->get_UserSignId(&pVarResult->ullVal);
			case EB_DISP_FROMIP:
				pVarResult->vt = VT_UI8;
				return this->get_FromIp(&pVarResult->ullVal);
			case EB_DISP_HEADRESID:
				pVarResult->vt = VT_UI8;
				return this->get_HeadResId(&pVarResult->ullVal);
			case EB_DISP_HEADCMSERVER:
				pVarResult->vt = VT_BSTR;
				return this->get_HeadCmServer(&pVarResult->bstrVal);
			case EB_DISP_HEADHTTPSERVER:
				pVarResult->vt = VT_BSTR;
				return this->get_HeadHttpServer(&pVarResult->bstrVal);
			case EB_DISP_HEADMD5:
				pVarResult->vt = VT_BSTR;
				return this->get_HeadMd5(&pVarResult->bstrVal);
			case EB_DISP_HEADRESOURCEFILE:
				pVarResult->vt = VT_BSTR;
				return this->get_HeadResourceFile(&pVarResult->bstrVal);
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
			case EB_DISP_ACCOUNTTYPE:
				return this->put_AccountType(pDispParams->rgvarg[0].iVal);
			case EB_DISP_MEMBERCODE:
				return this->put_MemberCode(pDispParams->rgvarg[0].ullVal);
			case EB_DISP_MEMBERUSERID:
				return this->put_MemberUserId(pDispParams->rgvarg[0].ullVal);
			case EB_DISP_NAME:
				return this->put_Name(pDispParams->rgvarg[0].bstrVal);
			case EB_DISP_PHONE:
				return this->put_Phone(pDispParams->rgvarg[0].bstrVal);
			case EB_DISP_TEL:
				return this->put_Tel(pDispParams->rgvarg[0].bstrVal);
			case EB_DISP_EMAIL:
				return this->put_Email(pDispParams->rgvarg[0].bstrVal);
			case EB_DISP_TITLE:
				return this->put_Title(pDispParams->rgvarg[0].bstrVal);
			case EB_DISP_GROUPNAME:
				return this->put_GroupName(pDispParams->rgvarg[0].bstrVal);
			case EB_DISP_GROUPCODE:
				return this->put_GroupCode(pDispParams->rgvarg[0].ullVal);
			case EB_DISP_ENTERPRISE:
				return this->put_Enterprise(pDispParams->rgvarg[0].bstrVal);
			case EB_DISP_ADDRESS:
				return this->put_Address(pDispParams->rgvarg[0].bstrVal);
			case EB_DISP_USERSIGNID:
				return this->put_UserSignId(pDispParams->rgvarg[0].ullVal);
			case EB_DISP_FROMIP:
				return this->put_FromIp(pDispParams->rgvarg[0].ullVal);
			case EB_DISP_HEADRESID:
				return this->put_HeadResId(pDispParams->rgvarg[0].ullVal);
			case EB_DISP_HEADCMSERVER:
				return this->put_HeadCmServer(pDispParams->rgvarg[0].bstrVal);
			case EB_DISP_HEADHTTPSERVER:
				return this->put_HeadHttpServer(pDispParams->rgvarg[0].bstrVal);
			case EB_DISP_HEADMD5:
				return this->put_HeadMd5(pDispParams->rgvarg[0].bstrVal);
			case EB_DISP_HEADRESOURCEFILE:
				return this->put_HeadResourceFile(pDispParams->rgvarg[0].bstrVal);
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
	CIEB_ECardInfo(void)
	{
	}
	CIEB_ECardInfo(const EB_ECardInfo& pObj)
		: EB_ECardInfo(pObj)
	{
	}
};

#endif // __IEB_ECardInfo_h__
