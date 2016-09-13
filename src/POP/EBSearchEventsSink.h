// EBSearchEventsSink.h file here
#ifndef __EBSearchEventsSink_h__
#define __EBSearchEventsSink_h__

#include "../include/EBUnknwn.h"
//#ifdef USES_EBCOM

class CEBSearchEventsSink
	: public CEBDispatch
{
public:
	CEBSearchEventsSink() {}
	virtual ~CEBSearchEventsSink() {};

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
		switch (dispIdMember)
		{
		case 1:
			{
				if (pDispParams == NULL || pDispParams->cArgs != 3 || pDispParams->rgvarg == NULL)
					return E_INVALIDARG;
				IDispatch * pGroupInfo = pDispParams->rgvarg[2].pdispVal;
				IDispatch * pEnterpriseInfo = pDispParams->rgvarg[1].pdispVal;
				ULONG dwParam = pDispParams->rgvarg[0].ulVal;
				return onSearchGroupInfo(pGroupInfo, pEnterpriseInfo, dwParam);
			}break;
		case 2:
			{
				if (pDispParams == NULL || pDispParams->cArgs != 3 || pDispParams->rgvarg == NULL)
					return E_INVALIDARG;
				IDispatch * pGroupInfo = pDispParams->rgvarg[2].pdispVal;
				IDispatch * pMemberInfo = pDispParams->rgvarg[1].pdispVal;
				ULONG dwParam = pDispParams->rgvarg[0].ulVal;
				return onSearchMemberInfo(pGroupInfo, pMemberInfo, dwParam);
			}break;
		case 3:
			{
				if (pDispParams == NULL || pDispParams->cArgs != 2 || pDispParams->rgvarg == NULL)
					return E_INVALIDARG;
				IDispatch * pContactInfo = pDispParams->rgvarg[1].pdispVal;
				ULONG dwParam = pDispParams->rgvarg[0].ulVal;
				return onSearchContactInfo(pContactInfo, dwParam);
			}break;
		default:
			break;
		}
		return S_OK;
	}
	HRESULT _stdcall onSearchGroupInfo(IDispatch* pGroupInfo, IDispatch* pEnterpriseInfo, ULONG dwParam)
	{
		if (pGroupInfo == NULL || pEnterpriseInfo == NULL) return E_INVALIDARG;
		IEB_GroupInfoPtr pInterface1;
		pGroupInfo->QueryInterface(__uuidof(IEB_GroupInfo),(void**)&pInterface1);
		if (pInterface1==NULL) return S_FALSE;
		IEB_EnterpriseInfoPtr pInterface2;
		pEnterpriseInfo->QueryInterface(__uuidof(IEB_EnterpriseInfo),(void**)&pInterface2);
		if (pInterface2==NULL) return S_FALSE;
		Fire_onSearchGroupInfo(pInterface1, pInterface2, dwParam);
		return S_OK;
	}
	HRESULT _stdcall onSearchMemberInfo(IDispatch* pGroupInfo, IDispatch* pMemberInfo, ULONG dwParam)
	{
		if (pGroupInfo == NULL || pMemberInfo == NULL) return E_INVALIDARG;
		IEB_GroupInfoPtr pInterface1;
		pGroupInfo->QueryInterface(__uuidof(IEB_GroupInfo),(void**)&pInterface1);
		if (pInterface1==NULL) return S_FALSE;
		IEB_MemberInfoPtr pInterface2;
		pMemberInfo->QueryInterface(__uuidof(IEB_MemberInfo),(void**)&pInterface2);
		if (pInterface2==NULL) return S_FALSE;
		Fire_onSearchMemberInfo(pInterface1, pInterface2, dwParam);
		return S_OK;
	}
	HRESULT _stdcall onSearchContactInfo(IDispatch* pContactInfo, ULONG dwParam)
	{
		if (pContactInfo == NULL) return E_INVALIDARG;
		IEB_ContactInfoPtr pInterface1;
		pContactInfo->QueryInterface(__uuidof(IEB_ContactInfo),(void**)&pInterface1);
		if (pInterface1==NULL) return S_FALSE;
		Fire_onSearchContactInfo(pInterface1, dwParam);
		return S_OK;
	}
private:
	virtual void Fire_onSearchGroupInfo(IEB_GroupInfo* pGroupInfo, IEB_EnterpriseInfo* pEnterpriseInfo, ULONG dwParam) {}
	virtual void Fire_onSearchMemberInfo(IEB_GroupInfo* pGroupInfo, IEB_MemberInfo* pMemberInfo, ULONG dwParam) {}
	virtual void Fire_onSearchContactInfo(IEB_ContactInfo* pContactInfo, ULONG dwParam) {}
};

//#endif

#endif // __EBSearchEventsSink_h__
