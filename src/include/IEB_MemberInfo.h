// IEB_MemberInfo.h file here
#ifndef __IEB_MemberInfo_h__
#define __IEB_MemberInfo_h__
#include "EBUnknwn.h"
#include "ebdisp.h"

class CIEB_MemberInfo : public IEB_MemberInfo
	, public CEBDispatch
	, public EB_MemberInfo
{
public:
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
	virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_MemberAccount( 
		/* [retval][out] */ BSTR *pVal)
	{
		*pVal = m_sMemberAccount.CopyBSTR();
		return S_OK;
	}
	virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_MemberAccount( 
		/* [in] */ BSTR newVal)
	{
		m_sMemberAccount = newVal;
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
	virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_JobPosition( 
		/* [retval][out] */ LONG *pVal)
	{
		*pVal = m_nJobPosition;
		return S_OK;
	}
	virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_JobPosition( 
		/* [in] */ LONG newVal)
	{
		m_nJobPosition = newVal;
		return S_OK;
	}
	virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_CellPhone( 
		/* [retval][out] */ BSTR *pVal)
	{
		*pVal = m_sCellPhone.CopyBSTR();
		return S_OK;
	}
	virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_CellPhone( 
		/* [in] */ BSTR newVal)
	{
		m_sCellPhone = newVal;
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
	virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_WorkPhone( 
		/* [retval][out] */ BSTR *pVal)
	{
		*pVal = m_sWorkPhone.CopyBSTR();
		return S_OK;
	}
	virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_WorkPhone( 
		/* [in] */ BSTR newVal)
	{
		m_sWorkPhone = newVal;
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
	virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Gender( 
		/* [retval][out] */ SHORT *pVal)
	{
		*pVal = m_nGender;
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
		*pVal = m_nBirthday;
		return S_OK;
	}
	virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Birthday( 
		/* [in] */ LONG newVal)
	{
		m_nBirthday = newVal;
		return S_OK;
	}
	virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_LineState( 
		/* [retval][out] */ LONG *pVal)
	{
		*pVal = m_nLineState;
		return S_OK;
	}
	virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_LineState( 
		/* [in] */ LONG newVal)
	{
		m_nLineState = (EB_USER_LINE_STATE)newVal;
		return S_OK;
	}
	virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_HeadResourceId( 
		/* [retval][out] */ ULONGLONG *pVal)
	{
		*pVal = m_sHeadResourceId;
		return S_OK;
	}
	virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_HeadResourceId( 
		/* [in] */ ULONGLONG newVal)
	{
		m_sHeadResourceId = newVal;
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
	virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_CSId( 
		/* [retval][out] */ ULONGLONG *pVal)
	{
		*pVal = m_nCSId;
		return S_OK;
	}
	virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_CSId( 
		/* [in] */ ULONGLONG newVal)
	{
		m_nCSId = newVal;
		return S_OK;
	}
	virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_CSExt( 
		/* [retval][out] */ LONG *pVal)
	{
		*pVal = m_nCSExt;
		return S_OK;
	}
	virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_CSExt( 
		/* [in] */ LONG newVal)
	{
		m_nCSExt = newVal;
		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE QueryInterface( 
		/* [in] */ REFIID riid,
		/* [iid_is][out] */ __RPC__deref_out void __RPC_FAR *__RPC_FAR *ppvObject)
	{
#ifdef IID_IEB_MemberInfo
		if (InlineIsEqualGUID(riid, IID_IEB_MemberInfo))
#else
		if (InlineIsEqualGUID(riid, __uuidof(IEB_MemberInfo)))
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
		if (bstrName==L"0" || bstrName == L"MEMBERCODE")
		{
			*rgDispId = EB_DISP_MEMBERCODE;
		}
		else if (bstrName==L"1" || bstrName == L"GROUPCODE")
		{
			*rgDispId = EB_DISP_GROUPCODE;
		}
		else if (bstrName==L"2" || bstrName == L"MEMBERUSERID")
		{
			*rgDispId = EB_DISP_MEMBERUSERID;
		}
		else if (bstrName==L"3" || bstrName == L"MEMBERACCOUNT")
		{
			*rgDispId = EB_DISP_MEMBERACCOUNT;
		}
		else if (bstrName==L"4" || bstrName == L"USERNAME")
		{
			*rgDispId = EB_DISP_USERNAME;
		}
		else if (bstrName==L"5" || bstrName == L"JOBTITLE")
		{
			*rgDispId = EB_DISP_JOBTITLE;
		}
		else if (bstrName==L"6" || bstrName == L"JOBPOSITION")
		{
			*rgDispId = EB_DISP_JOBPOSITION;
		}
		else if (bstrName==L"7" || bstrName == L"CELLPHONE")
		{
			*rgDispId = EB_DISP_CELLPHONE;
		}
		else if (bstrName==L"8" || bstrName == L"FAX")
		{
			*rgDispId = EB_DISP_FAX;
		}
		else if (bstrName==L"9" || bstrName == L"WORKPHONE")
		{
			*rgDispId = EB_DISP_WORKPHONE;
		}
		else if (bstrName==L"10" || bstrName == L"EMAIL")
		{
			*rgDispId = EB_DISP_EMAIL;
		}
		else if (bstrName==L"11" || bstrName == L"DESCRIPTION")
		{
			*rgDispId = EB_DISP_DESCRIPTION;
		}
		else if (bstrName==L"12" || bstrName == L"MANAGERLEVEL")
		{
			*rgDispId = EB_DISP_MANAGERLEVEL;
		}
		else if (bstrName==L"13" || bstrName == L"GENDER")
		{
			*rgDispId = EB_DISP_GENDER;
		}
		else if (bstrName==L"14" || bstrName == L"BIRTHDAY")
		{
			*rgDispId = EB_DISP_BIRTHDAY;
		}
		else if (bstrName==L"15" || bstrName == L"LINESTATE")
		{
			*rgDispId = EB_DISP_LINESTATE;
		}
		else if (bstrName==L"16" || bstrName == L"HEADRESOURCEID")
		{
			*rgDispId = EB_DISP_HEADRESOURCEID;
		}
		else if (bstrName==L"17" || bstrName == L"HEADRESOURCEFILE")
		{
			*rgDispId = EB_DISP_HEADRESOURCEFILE;
		}
		else if (bstrName==L"18" || bstrName == L"CSID")
		{
			*rgDispId = EB_DISP_CSID;
		}
		else if (bstrName==L"19" || bstrName == L"CSEXT")
		{
			*rgDispId = EB_DISP_CSEXT;
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
			case EB_DISP_MEMBERCODE:
				pVarResult->vt = VT_UI8;
				return this->get_MemberCode(&pVarResult->ullVal);
			case EB_DISP_GROUPCODE:
				pVarResult->vt = VT_UI8;
				return this->get_GroupCode(&pVarResult->ullVal);
			case EB_DISP_MEMBERUSERID:
				pVarResult->vt = VT_UI8;
				return this->get_MemberUserId(&pVarResult->ullVal);
			case EB_DISP_MEMBERACCOUNT:
				pVarResult->vt = VT_BSTR;
				return this->get_MemberAccount(&pVarResult->bstrVal);
			case EB_DISP_USERNAME:
				pVarResult->vt = VT_BSTR;
				return this->get_UserName(&pVarResult->bstrVal);
			case EB_DISP_JOBTITLE:
				pVarResult->vt = VT_BSTR;
				return this->get_JobTitle(&pVarResult->bstrVal);
			case EB_DISP_JOBPOSITION:
				{
					pVarResult->vt = VT_INT;
					pVarResult->lVal = m_nJobPosition;
					return S_OK;
				}break;
			case EB_DISP_CELLPHONE:
				pVarResult->vt = VT_BSTR;
				return this->get_CellPhone(&pVarResult->bstrVal);
			case EB_DISP_FAX:
				pVarResult->vt = VT_BSTR;
				return this->get_Fax(&pVarResult->bstrVal);
			case EB_DISP_WORKPHONE:
				pVarResult->vt = VT_BSTR;
				return this->get_WorkPhone(&pVarResult->bstrVal);
			case EB_DISP_EMAIL:
				pVarResult->vt = VT_BSTR;
				return this->get_Email(&pVarResult->bstrVal);
			case EB_DISP_ADDRESS:
				pVarResult->vt = VT_BSTR;
				return this->get_Address(&pVarResult->bstrVal);
			case EB_DISP_DESCRIPTION:
				pVarResult->vt = VT_BSTR;
				return this->get_Description(&pVarResult->bstrVal);
			case EB_DISP_MANAGERLEVEL:
				{
					pVarResult->vt = VT_INT;
					pVarResult->lVal = m_nManagerLevel;
					return S_OK;
				}break;
			case EB_DISP_GENDER:
				{
					pVarResult->vt = VT_INT;
					pVarResult->lVal = m_nGender;
					return S_OK;
				}break;
			case EB_DISP_BIRTHDAY:
				{
					pVarResult->vt = VT_INT;
					pVarResult->lVal = m_nBirthday;
					return S_OK;
				}break;
			case EB_DISP_LINESTATE:
				{
					pVarResult->vt = VT_INT;
					pVarResult->lVal = m_nLineState;
					return S_OK;
				}break;
			case EB_DISP_HEADRESOURCEID:
				pVarResult->vt = VT_UI8;
				return this->get_HeadResourceId(&pVarResult->ullVal);
			case EB_DISP_HEADRESOURCEFILE:
				pVarResult->vt = VT_BSTR;
				return this->get_HeadResourceFile(&pVarResult->bstrVal);
			case EB_DISP_CSID:
				pVarResult->vt = VT_UI8;
				return this->get_CSId(&pVarResult->ullVal);
			case EB_DISP_CSEXT:
				pVarResult->vt = VT_I4;
				return this->get_CSExt(&pVarResult->lVal);
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
			case EB_DISP_MEMBERCODE:
				return this->put_MemberCode(pDispParams->rgvarg[0].ullVal);
			case EB_DISP_GROUPCODE:
				return this->put_GroupCode(pDispParams->rgvarg[0].ullVal);
			case EB_DISP_MEMBERUSERID:
				return this->put_MemberUserId(pDispParams->rgvarg[0].ullVal);
			case EB_DISP_MEMBERACCOUNT:
				return this->put_MemberAccount(pDispParams->rgvarg[0].bstrVal);
			case EB_DISP_USERNAME:
				return this->put_UserName(pDispParams->rgvarg[0].bstrVal);
			case EB_DISP_JOBTITLE:
				return this->put_JobTitle(pDispParams->rgvarg[0].bstrVal);
			case EB_DISP_JOBPOSITION:
				return this->put_JobPosition(pDispParams->rgvarg[0].lVal);
			case EB_DISP_CELLPHONE:
				return this->put_CellPhone(pDispParams->rgvarg[0].bstrVal);
			case EB_DISP_FAX:
				return this->put_Fax(pDispParams->rgvarg[0].bstrVal);
			case EB_DISP_WORKPHONE:
				return this->put_WorkPhone(pDispParams->rgvarg[0].bstrVal);
			case EB_DISP_EMAIL:
				return this->put_Email(pDispParams->rgvarg[0].bstrVal);
			case EB_DISP_ADDRESS:
				return this->put_Address(pDispParams->rgvarg[0].bstrVal);
			case EB_DISP_DESCRIPTION:
				return this->put_Description(pDispParams->rgvarg[0].bstrVal);
			//case EB_DISP_MANAGERLEVEL:
			//	return this->put_ManagerLevel(pDispParams->rgvarg[0].lVal);
			case EB_DISP_GENDER:
				return this->put_Gender(pDispParams->rgvarg[0].iVal);
			case EB_DISP_BIRTHDAY:
				return this->put_Birthday(pDispParams->rgvarg[0].lVal);
			case EB_DISP_LINESTATE:
				return this->put_LineState(pDispParams->rgvarg[0].lVal);
			case EB_DISP_HEADRESOURCEID:
				return this->put_HeadResourceId(pDispParams->rgvarg[0].ullVal);
			case EB_DISP_HEADRESOURCEFILE:
				return this->put_HeadResourceFile(pDispParams->rgvarg[0].bstrVal);
			case EB_DISP_CSID:
				return this->put_CSId(pDispParams->rgvarg[0].ullVal);
			case EB_DISP_CSEXT:
				return this->put_CSExt(pDispParams->rgvarg[0].lVal);
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
	CIEB_MemberInfo(void)
	{
	}
	CIEB_MemberInfo(const EB_MemberInfo& pObj)
		: EB_MemberInfo(&pObj)
	{
	}
};

#endif // __IEB_MemberInfo_h__
