// IEB_AccountInfo.h file here
#ifndef __IEB_AccountInfo_h__
#define __IEB_AccountInfo_h__
#include "EBUnknwn.h"
#include "ebdisp.h"
#include "IEB_ECardInfo.h"

class CIEB_AccountInfo : public IEB_AccountInfo
	, public CEBDispatch
	, public EB_AccountInfo
{
public:
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
	virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Account( 
		/* [retval][out] */ BSTR *pVal)
	{
		*pVal = m_sAccount.CopyBSTR();
		return S_OK;
	}
	virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Account( 
		/* [in] */ BSTR newVal)
	{
		m_sAccount = newVal;
		return S_OK;
	}
	virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Passwd( 
		/* [retval][out] */ BSTR *pVal)
	{
		*pVal = m_sPasswd.CopyBSTR();
		return S_OK;
	}
	virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Passwd( 
		/* [in] */ BSTR newVal)
	{
		m_sPasswd = newVal;
		return S_OK;
	}
	virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_UserName( 
		/* [retval][out] */ BSTR *pVal)
	{
		*pVal = m_sUserName.CopyBSTR();
		return S_OK;
	}
	virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_UserName( 
		/* [in] */ BSTR newVal)
	{
		m_sUserName = newVal;
		return S_OK;
	}
	virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Gender( 
		/* [retval][out] */ SHORT *pVal)
	{
		*pVal = (SHORT)m_nGender;
		return S_OK;
	}
	virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Gender( 
		/* [in] */ SHORT newVal)
	{
		m_nGender = (EB_GENDER_TYPE)newVal;
		return S_OK;
	}
	virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Birthday( 
		/* [retval][out] */ LONG *pVal)
	{
		*pVal = (LONG)m_nBirthday;
		return S_OK;
	}
	virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Birthday( 
		/* [in] */ LONG newVal)
	{
		m_nBirthday = newVal;
		return S_OK;
	}
	virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Mobile( 
		/* [retval][out] */ BSTR *pVal)
	{
		*pVal = m_sMobile.CopyBSTR();
		return S_OK;
	}
	virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Mobile( 
		/* [in] */ BSTR newVal)
	{
		m_sMobile = newVal;
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
	virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Area1( 
		/* [retval][out] */ LONG *pVal)
	{
		*pVal = (LONG)m_nArea1;
		return S_OK;
	}
	virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Area1( 
		/* [in] */ LONG newVal)
	{
		m_nArea1 = newVal;
		return S_OK;
	}
	virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Area1String( 
		/* [retval][out] */ BSTR *pVal)
	{
		*pVal = m_sArea1.CopyBSTR();
		return S_OK;
	}
	virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Area1String( 
		/* [in] */ BSTR newVal)
	{
		m_sArea1 = newVal;
		return S_OK;
	}
	virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Area2( 
		/* [retval][out] */ LONG *pVal)
	{
		*pVal = (LONG)m_nArea2;
		return S_OK;
	}
	virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Area2( 
		/* [in] */ LONG newVal)
	{
		m_nArea2 = newVal;
		return S_OK;
	}
	virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Area2String( 
		/* [retval][out] */ BSTR *pVal)
	{
		*pVal = m_sArea2.CopyBSTR();
		return S_OK;
	}
	virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Area2String( 
		/* [in] */ BSTR newVal)
	{
		m_sArea2 = newVal;
		return S_OK;
	}
	virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Area3( 
		/* [retval][out] */ LONG *pVal)
	{
		*pVal = (LONG)m_nArea3;
		return S_OK;
	}
	virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Area3( 
		/* [in] */ LONG newVal)
	{
		m_nArea3 = newVal;
		return S_OK;
	}
	virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Area3String( 
		/* [retval][out] */ BSTR *pVal)
	{
		*pVal = m_sArea3.CopyBSTR();
		return S_OK;
	}
	virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Area3String( 
		/* [in] */ BSTR newVal)
	{
		m_sArea3 = newVal;
		return S_OK;
	}
	virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Area4( 
		/* [retval][out] */ LONG *pVal)
	{
		*pVal = (LONG)m_nArea4;
		return S_OK;
	}
	virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Area4( 
		/* [in] */ LONG newVal)
	{
		m_nArea4 = newVal;
		return S_OK;
	}
	virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Area4String( 
		/* [retval][out] */ BSTR *pVal)
	{
		*pVal = m_sArea4.CopyBSTR();
		return S_OK;
	}
	virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Area4String( 
		/* [in] */ BSTR newVal)
	{
		m_sArea4 = newVal;
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
	virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ZipCode( 
		/* [retval][out] */ BSTR *pVal)
	{
		*pVal = m_sZipCode.CopyBSTR();
		return S_OK;
	}
	virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_ZipCode( 
		/* [in] */ BSTR newVal)
	{
		m_sZipCode = newVal;
		return S_OK;
	}
	virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_LogonType( 
		/* [retval][out] */ ULONG *pVal)
	{
		*pVal = (ULONG)m_nLogonType;
		return S_OK;
	}
	virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_LogonType( 
		/* [in] */ ULONG newVal)
	{
		m_nLogonType = newVal;
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
	virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ECardInfo( 
		/* [retval][out] */ IEB_ECardInfo **pVal)
	{
		CIEB_ECardInfo* pECardInfo = new CIEB_ECardInfo(m_pFromCardInfo);
		return pECardInfo->QueryInterface(IID_IEB_ECardInfo,(void**)pVal);
	}
	virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_ECardInfo( 
		/* [in] */ IEB_ECardInfo *newVal)
	{
		if (newVal == NULL) return E_INVALIDARG;
#ifdef USES_EBCOM_TEST
		m_pFromCardInfo.operator=(newVal);
		return S_OK;
#else
		return S_FALSE;
#endif
	}

	virtual HRESULT STDMETHODCALLTYPE QueryInterface( 
		/* [in] */ REFIID riid,
		/* [iid_is][out] */ __RPC__deref_out void __RPC_FAR *__RPC_FAR *ppvObject)
	{
#ifdef IID_IEB_AccountInfo
		if (InlineIsEqualGUID(riid, IID_IEB_AccountInfo))
#else
		if (InlineIsEqualGUID(riid, __uuidof(IEB_AccountInfo)))
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
		if (bstrName==L"0" || bstrName == L"USERID")
		{
			*rgDispId = EB_DISP_USERID;
		}
		else if (bstrName==L"1" || bstrName == L"ACCOUNT")
		{
			*rgDispId = EB_DISP_ACCOUNT;
		}
		else if (bstrName==L"2" || bstrName == L"PASSWD")
		{
			*rgDispId = EB_DISP_PASSWD;
		}
		else if (bstrName==L"3" || bstrName == L"USERNAME")
		{
			*rgDispId = EB_DISP_USERNAME;
		}
		else if (bstrName==L"4" || bstrName == L"GENDER")
		{
			*rgDispId = EB_DISP_GENDER;
		}
		else if (bstrName==L"5" || bstrName == L"BIRTHDAY")
		{
			*rgDispId = EB_DISP_BIRTHDAY;
		}
		else if (bstrName==L"6" || bstrName == L"MOBILE")
		{
			*rgDispId = EB_DISP_MOBILE;
		}
		else if (bstrName==L"7" || bstrName == L"TEL")
		{
			*rgDispId = EB_DISP_TEL;
		}
		else if (bstrName==L"8" || bstrName == L"EMAIL")
		{
			*rgDispId = EB_DISP_EMAIL;
		}
		else if (bstrName==L"9" || bstrName == L"AREA1")
		{
			*rgDispId = EB_DISP_AREA1;
		}
		else if (bstrName==L"10" || bstrName == L"AREA1STRING")
		{
			*rgDispId = EB_DISP_AREA1STRING;
		}
		else if (bstrName==L"11" || bstrName == L"AREA2")
		{
			*rgDispId = EB_DISP_AREA2;
		}
		else if (bstrName==L"12" || bstrName == L"AREA2STRING")
		{
			*rgDispId = EB_DISP_AREA2STRING;
		}
		else if (bstrName==L"13" || bstrName == L"AREA3")
		{
			*rgDispId = EB_DISP_AREA3;
		}
		else if (bstrName==L"14" || bstrName == L"AREA3STRING")
		{
			*rgDispId = EB_DISP_AREA3STRING;
		}
		else if (bstrName==L"15" || bstrName == L"AREA4")
		{
			*rgDispId = EB_DISP_AREA4;
		}
		else if (bstrName==L"16" || bstrName == L"AREA4STRING")
		{
			*rgDispId = EB_DISP_AREA4STRING;
		}
		else if (bstrName==L"17" || bstrName == L"ADDRESS")
		{
			*rgDispId = EB_DISP_ADDRESS;
		}
		else if (bstrName==L"18" || bstrName == L"URL")
		{
			*rgDispId = EB_DISP_URL;
		}
		else if (bstrName==L"19" || bstrName == L"ZIPCODE")
		{
			*rgDispId = EB_DISP_ZIPCODE;
		}
		else if (bstrName==L"20" || bstrName == L"LOGONTYPE")
		{
			*rgDispId = EB_DISP_LOGONTYPE;
		}
		else if (bstrName==L"21" || bstrName == L"DESCRIPTION")
		{
			*rgDispId = EB_DISP_DESCRIPTION;
		}
		else if (bstrName==L"22" || bstrName == L"ECARDINFO")
		{
			*rgDispId = EB_DISP_ECARDINFO;
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
			case EB_DISP_USERID:
				pVarResult->vt = VT_UI8;
				return this->get_UserId(&pVarResult->ullVal);
			case EB_DISP_ACCOUNT:
				pVarResult->vt = VT_BSTR;
				return this->get_Account(&pVarResult->bstrVal);
			case EB_DISP_PASSWD:
				pVarResult->vt = VT_BSTR;
				return this->get_Passwd(&pVarResult->bstrVal);
			case EB_DISP_USERNAME:
				pVarResult->vt = VT_BSTR;
				return this->get_UserName(&pVarResult->bstrVal);
			case EB_DISP_GENDER:
				pVarResult->vt = VT_I2;
				return this->get_Gender(&pVarResult->iVal);
			case EB_DISP_BIRTHDAY:
				pVarResult->vt = VT_I4;
				return this->get_Birthday(&pVarResult->lVal);
			case EB_DISP_MOBILE:
				pVarResult->vt = VT_BSTR;
				return this->get_Mobile(&pVarResult->bstrVal);
			case EB_DISP_TEL:
				pVarResult->vt = VT_BSTR;
				return this->get_Tel(&pVarResult->bstrVal);
			case EB_DISP_EMAIL:
				pVarResult->vt = VT_BSTR;
				return this->get_Email(&pVarResult->bstrVal);
			case EB_DISP_AREA1:
				pVarResult->vt = VT_I4;
				return this->get_Area1(&pVarResult->lVal);
			case EB_DISP_AREA1STRING:
				pVarResult->vt = VT_BSTR;
				return this->get_Area1String(&pVarResult->bstrVal);
			case EB_DISP_AREA2:
				pVarResult->vt = VT_I4;
				return this->get_Area2(&pVarResult->lVal);
			case EB_DISP_AREA2STRING:
				pVarResult->vt = VT_BSTR;
				return this->get_Area2String(&pVarResult->bstrVal);
			case EB_DISP_AREA3:
				pVarResult->vt = VT_I4;
				return this->get_Area3(&pVarResult->lVal);
			case EB_DISP_AREA3STRING:
				pVarResult->vt = VT_BSTR;
				return this->get_Area3String(&pVarResult->bstrVal);
			case EB_DISP_AREA4:
				pVarResult->vt = VT_I4;
				return this->get_Area4(&pVarResult->lVal);
			case EB_DISP_AREA4STRING:
				pVarResult->vt = VT_BSTR;
				return this->get_Area4String(&pVarResult->bstrVal);
			case EB_DISP_ADDRESS:
				pVarResult->vt = VT_BSTR;
				return this->get_Address(&pVarResult->bstrVal);
			case EB_DISP_URL:
				pVarResult->vt = VT_BSTR;
				return this->get_Url(&pVarResult->bstrVal);
			case EB_DISP_ZIPCODE:
				pVarResult->vt = VT_BSTR;
				return this->get_ZipCode(&pVarResult->bstrVal);
			case EB_DISP_LOGONTYPE:
				pVarResult->vt = VT_UI4;
				return this->get_LogonType(&pVarResult->ulVal);
			case EB_DISP_DESCRIPTION:
				pVarResult->vt = VT_BSTR;
				return this->get_Description(&pVarResult->bstrVal);
			case EB_DISP_ECARDINFO:
				pVarResult->vt = VT_DISPATCH;
				return this->get_ECardInfo((IEB_ECardInfo**)&pVarResult->pdispVal);
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
			case EB_DISP_USERID:
				return this->put_UserId(pDispParams->rgvarg[0].ullVal);
			case EB_DISP_ACCOUNT:
				return this->put_Account(pDispParams->rgvarg[0].bstrVal);
			case EB_DISP_PASSWD:
				return this->put_Passwd(pDispParams->rgvarg[0].bstrVal);
			case EB_DISP_USERNAME:
				return this->put_UserName(pDispParams->rgvarg[0].bstrVal);
			case EB_DISP_GENDER:
				return this->put_Gender(pDispParams->rgvarg[0].iVal);
			case EB_DISP_BIRTHDAY:
				return this->put_Birthday(pDispParams->rgvarg[0].lVal);
			case EB_DISP_MOBILE:
				return this->put_Mobile(pDispParams->rgvarg[0].bstrVal);
			case EB_DISP_TEL:
				return this->put_Tel(pDispParams->rgvarg[0].bstrVal);
			case EB_DISP_EMAIL:
				return this->put_Email(pDispParams->rgvarg[0].bstrVal);
			case EB_DISP_AREA1:
				return this->put_Area1(pDispParams->rgvarg[0].lVal);
			case EB_DISP_AREA1STRING:
				return this->put_Area1String(pDispParams->rgvarg[0].bstrVal);
			case EB_DISP_AREA2:
				return this->put_Area2(pDispParams->rgvarg[0].lVal);
			case EB_DISP_AREA2STRING:
				return this->put_Area2String(pDispParams->rgvarg[0].bstrVal);
			case EB_DISP_AREA3:
				return this->put_Area3(pDispParams->rgvarg[0].lVal);
			case EB_DISP_AREA3STRING:
				return this->put_Area3String(pDispParams->rgvarg[0].bstrVal);
			case EB_DISP_AREA4:
				return this->put_Area4(pDispParams->rgvarg[0].lVal);
			case EB_DISP_AREA4STRING:
				return this->put_Area4String(pDispParams->rgvarg[0].bstrVal);
			case EB_DISP_ADDRESS:
				return this->put_Address(pDispParams->rgvarg[0].bstrVal);
			case EB_DISP_URL:
				return this->put_Url(pDispParams->rgvarg[0].bstrVal);
			case EB_DISP_ZIPCODE:
				return this->put_ZipCode(pDispParams->rgvarg[0].bstrVal);
			case EB_DISP_LOGONTYPE:
				return this->put_LogonType(pDispParams->rgvarg[0].ulVal);
			case EB_DISP_DESCRIPTION:
				return this->put_Description(pDispParams->rgvarg[0].bstrVal);
			case EB_DISP_ECARDINFO:
				{
					if (pDispParams->rgvarg[0].pdispVal == NULL) return E_INVALIDARG;
					CComPtr<IEB_ECardInfo> pECardInfo;
					pDispParams->rgvarg[0].pdispVal->QueryInterface(IID_IEB_ECardInfo,(void**)&pECardInfo);
					return this->put_ECardInfo(pECardInfo);
				}break;
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
	CIEB_AccountInfo(void)
	{
	}
	CIEB_AccountInfo(const EB_AccountInfo& pObj)
		: EB_AccountInfo(&pObj)
	{
	}
	virtual ~CIEB_AccountInfo(void)
	{
	}
};

#endif // __IEB_AccountInfo_h__
