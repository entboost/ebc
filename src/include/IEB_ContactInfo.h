// IEB_ContactInfo.h file here
#ifndef __IEB_ContactInfo_h__
#define __IEB_ContactInfo_h__
#include "EBUnknwn.h"
#include "ebdisp.h"

class CIEB_ContactInfo : public IEB_ContactInfo
	, public CEBDispatch
	, public EB_ContactInfo
{
public:
	virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ContactId( 
		/* [retval][out] */ ULONGLONG *pVal)
	{
		*pVal = m_nContactId;
		return S_OK;
	}
	virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_ContactId( 
		/* [in] */ ULONGLONG newVal)
	{
		m_nContactId = newVal;
		return S_OK;
	}
	virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ContactType( 
		/* [retval][out] */ SHORT *pVal)
	{
		*pVal = (SHORT)m_nContactType;
		return S_OK;
	}
	virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_ContactType( 
		/* [in] */ SHORT newVal)
	{
		m_nContactType = (int)newVal;
		return S_OK;
	}
	virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_UGId( 
		/* [retval][out] */ ULONGLONG *pVal)
	{
		*pVal = m_nUGId;
		return S_OK;
	}
	virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_UGId( 
		/* [in] */ ULONGLONG newVal)
	{
		m_nUGId = newVal;
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
	virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ContactUserId( 
		/* [retval][out] */ ULONGLONG *pVal)
	{
		*pVal = m_nContactUserId;
		return S_OK;
	}
	virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_ContactUserId( 
		/* [in] */ ULONGLONG newVal)
	{
		m_nContactUserId = newVal;
		return S_OK;
	}
	virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Contact( 
		/* [retval][out] */ BSTR *pVal)
	{
		*pVal = m_sContact.CopyBSTR();
		return S_OK;
	}
	virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Contact( 
		/* [in] */ BSTR newVal)
	{
		m_sContact = newVal;
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
	virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Company( 
		/* [retval][out] */ BSTR *pVal)
	{
		*pVal = m_sCompany.CopyBSTR();
		return S_OK;
	}
	virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Company( 
		/* [in] */ BSTR newVal)
	{
		m_sCompany = newVal;
		return S_OK;
	}
	virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_JobTitle( 
		/* [retval][out] */ BSTR *pVal)
	{
		*pVal = m_sJobTitle.CopyBSTR();
		return S_OK;
	}
	virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_JobTitle( 
		/* [in] */ BSTR newVal)
	{
		m_sJobTitle = newVal;
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
	virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_LineState( 
		/* [retval][out] */ LONG *pVal)
	{
		*pVal = (LONG)m_nLineState;
		return S_OK;
	}
	virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_LineState( 
		/* [in] */ LONG newVal)
	{
		m_nLineState = (EB_USER_LINE_STATE)newVal;
		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE QueryInterface( 
		/* [in] */ REFIID riid,
		/* [iid_is][out] */ __RPC__deref_out void __RPC_FAR *__RPC_FAR *ppvObject)
	{
#ifdef IID_IEB_ContactInfo
		if (InlineIsEqualGUID(riid, IID_IEB_ContactInfo))
#else
		if (InlineIsEqualGUID(riid, __uuidof(IEB_ContactInfo)))
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
		if (bstrName==L"0" || bstrName == L"CONTACTID")
		{
			*rgDispId = EB_DISP_CONTACTID;
		}
		else if (bstrName==L"1" || bstrName == L"CONTACTTYPE")
		{
			*rgDispId = EB_DISP_CONTACTTYPE;
		}
		else if (bstrName==L"2" || bstrName == L"UGID")
		{
			*rgDispId = EB_DISP_UGID;
		}
		else if (bstrName==L"3" || bstrName == L"GROUPNAME")
		{
			*rgDispId = EB_DISP_GROUPNAME;
		}
		else if (bstrName==L"4" || bstrName == L"CONTACTUSERID")
		{
			*rgDispId = EB_DISP_CONTACTUID;
		}
		else if (bstrName==L"5" || bstrName == L"CONTACT")
		{
			*rgDispId = EB_DISP_CONTACT;
		}
		else if (bstrName==L"6" || bstrName == L"NAME")
		{
			*rgDispId = EB_DISP_NAME;
		}
		else if (bstrName==L"7" || bstrName == L"COMPANY")
		{
			*rgDispId = EB_DISP_COMPANY;
		}
		else if (bstrName==L"8" || bstrName == L"JOBTITLE")
		{
			*rgDispId = EB_DISP_JOBTITLE;
		}
		else if (bstrName==L"9" || bstrName == L"TEL")
		{
			*rgDispId = EB_DISP_TEL;
		}
		else if (bstrName==L"10" || bstrName == L"FAX")
		{
			*rgDispId = EB_DISP_FAX;
		}
		else if (bstrName==L"11" || bstrName == L"PHONE")
		{
			*rgDispId = EB_DISP_PHONE;
		}
		else if (bstrName==L"12" || bstrName == L"EMAIL")
		{
			*rgDispId = EB_DISP_EMAIL;
		}
		else if (bstrName==L"13" || bstrName == L"URL")
		{
			*rgDispId = EB_DISP_URL;
		}
		else if (bstrName==L"14" || bstrName == L"ADDRESS")
		{
			*rgDispId = EB_DISP_ADDRESS;
		}
		else if (bstrName==L"15" || bstrName == L"DESCRIPTION")
		{
			*rgDispId = EB_DISP_DESCRIPTION;
		}
		else if (bstrName==L"16" || bstrName == L"LINESTATE")
		{
			*rgDispId = EB_DISP_LINESTATE;
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
			case EB_DISP_CONTACTID:
				pVarResult->vt = VT_UI8;
				return this->get_ContactId(&pVarResult->ullVal);
			case EB_DISP_CONTACTTYPE:
				pVarResult->vt = VT_I2;
				return this->get_ContactType(&pVarResult->iVal);
			case EB_DISP_UGID:
				pVarResult->vt = VT_UI8;
				return this->get_UGId(&pVarResult->ullVal);
			case EB_DISP_GROUPNAME:
				pVarResult->vt = VT_BSTR;
				return this->get_GroupName(&pVarResult->bstrVal);
			case EB_DISP_CONTACTUID:
				pVarResult->vt = VT_UI8;
				return this->get_ContactUserId(&pVarResult->ullVal);
			case EB_DISP_CONTACT:
				pVarResult->vt = VT_BSTR;
				return this->get_Contact(&pVarResult->bstrVal);
			case EB_DISP_NAME:
				pVarResult->vt = VT_BSTR;
				return this->get_Name(&pVarResult->bstrVal);
			case EB_DISP_PHONE:
				pVarResult->vt = VT_BSTR;
				return this->get_Phone(&pVarResult->bstrVal);
			case EB_DISP_EMAIL:
				pVarResult->vt = VT_BSTR;
				return this->get_Email(&pVarResult->bstrVal);
			case EB_DISP_ADDRESS:
				pVarResult->vt = VT_BSTR;
				return this->get_Address(&pVarResult->bstrVal);
			case EB_DISP_DESCRIPTION:
				pVarResult->vt = VT_BSTR;
				return this->get_Description(&pVarResult->bstrVal);
			case EB_DISP_COMPANY:
				pVarResult->vt = VT_BSTR;
				return this->get_Company(&pVarResult->bstrVal);
			case EB_DISP_JOBTITLE:
				pVarResult->vt = VT_BSTR;
				return this->get_JobTitle(&pVarResult->bstrVal);
			case EB_DISP_TEL:
				pVarResult->vt = VT_BSTR;
				return this->get_Tel(&pVarResult->bstrVal);
			case EB_DISP_FAX:
				pVarResult->vt = VT_BSTR;
				return this->get_Fax(&pVarResult->bstrVal);
			case EB_DISP_URL:
				pVarResult->vt = VT_BSTR;
				return this->get_Url(&pVarResult->bstrVal);
			case EB_DISP_LINESTATE:
				pVarResult->vt = VT_I4;
				return this->get_LineState(&pVarResult->lVal);
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
			case EB_DISP_CONTACTID:
				return this->put_ContactId(pDispParams->rgvarg[0].ullVal);
			case EB_DISP_CONTACTTYPE:
				return this->put_ContactType(pDispParams->rgvarg[0].iVal);
			case EB_DISP_UGID:
				return this->put_UGId(pDispParams->rgvarg[0].ullVal);
			case EB_DISP_GROUPNAME:
				return this->put_GroupName(pDispParams->rgvarg[0].bstrVal);
			case EB_DISP_CONTACTUID:
				return this->put_ContactUserId(pDispParams->rgvarg[0].ullVal);
			case EB_DISP_CONTACT:
				return this->put_Contact(pDispParams->rgvarg[0].bstrVal);
			case EB_DISP_NAME:
				return this->put_Name(pDispParams->rgvarg[0].bstrVal);
			case EB_DISP_PHONE:
				return this->put_Phone(pDispParams->rgvarg[0].bstrVal);
			case EB_DISP_EMAIL:
				return this->put_Email(pDispParams->rgvarg[0].bstrVal);
			case EB_DISP_ADDRESS:
				return this->put_Address(pDispParams->rgvarg[0].bstrVal);
			case EB_DISP_DESCRIPTION:
				return this->put_Description(pDispParams->rgvarg[0].bstrVal);
			case EB_DISP_COMPANY:
				return this->put_Company(pDispParams->rgvarg[0].bstrVal);
			case EB_DISP_JOBTITLE:
				return this->put_JobTitle(pDispParams->rgvarg[0].bstrVal);
			case EB_DISP_TEL:
				return this->put_Tel(pDispParams->rgvarg[0].bstrVal);
			case EB_DISP_FAX:
				return this->put_Fax(pDispParams->rgvarg[0].bstrVal);
			case EB_DISP_URL:
				return this->put_Url(pDispParams->rgvarg[0].bstrVal);
			case EB_DISP_LINESTATE:
				return this->put_LineState(pDispParams->rgvarg[0].lVal);
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
	CIEB_ContactInfo(void)
	{
	}
	CIEB_ContactInfo(const EB_ContactInfo& pObj)
		: EB_ContactInfo(&pObj)
	{
	}
};

#endif // __IEB_ContactInfo_h__
