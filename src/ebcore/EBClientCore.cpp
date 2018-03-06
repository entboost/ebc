// EBClientCore.cpp : Implementation of CEBClientCore

#include "stdafx.h"
#include "EBClientCore.h"
#include "atlsafe.h"
#include "DispEx.h"

#ifdef _DEBUG
#pragma comment(lib, "libebumd.lib")
#else
#pragma comment(lib, "libebum.lib")
#endif
#pragma comment(lib, "comsuppw.lib")

// CEBClientCore

//#define BSTR2A_EX(bstr) (bstr==NULL?"":W2A(bstr))

//HRESULT GetArrayVariantOfIndex(IDispatch* pDisp, int index, VARIANT * pOutVariant)
//{
//	CComVariant varName(index, VT_I4);   // 数组下标
//	DISPPARAMS noArgs = {NULL, NULL, 0, 0};
//	DISPID dispId;
//	HRESULT hr = 0;       
//	varName.ChangeType(VT_BSTR);         // 将数组下标转为数字型，以进行GetIDsOfNames
//	//
//	// 获取通过下标访问数组的过程，将过程名保存在dispId中
//	//
//	hr = pDisp->GetIDsOfNames(IID_NULL, &varName.bstrVal, 1, LOCALE_USER_DEFAULT, &dispId);
//	if (FAILED(hr))
//	{
//		return hr;
//	}
//	//
//	// 调用COM过程，访问指定下标数组元素，根据dispId 将元素值保存在varValue中
//	//
//	VariantInit(pOutVariant);
//	hr = pDisp->Invoke(dispId, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_PROPERTYGET , &noArgs, pOutVariant, NULL, NULL);
//	return hr;
//}
//HRESULT GetArrayVariantOfName(IDispatch* pDisp, BSTR bstrName, VARIANT * pOutVariant)
//{
//	DISPPARAMS noArgs = {NULL, NULL, 0, 0};
//	DISPID dispId;
//	HRESULT hr = 0;
//	hr = pDisp->GetIDsOfNames(IID_NULL, &bstrName, 1, LOCALE_USER_DEFAULT, &dispId);
//	if (FAILED(hr))
//	{
//		//char lpsztext[20];
//		//sprintf(lpsztext,"1 %x",hr);
//		//MessageBoxA(NULL,lpsztext,"",MB_OK);
//		return hr;
//	}
//	VariantInit(pOutVariant);
//	hr = pDisp->Invoke(dispId, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_PROPERTYGET , &noArgs, pOutVariant, NULL, NULL);
//	return hr;
//}

//HRESULT AddArrayElement(IDispatch* pDisp, int value)
//{
//	HRESULT hr = 0;
//	DISPID      dispid[2] = {0};
//	CComBSTR funcName(L"push");
//	hr = pDisp->GetIDsOfNames(IID_NULL, &funcName, 1, LOCALE_USER_DEFAULT, dispid);
//	if (FAILED(hr))
//		return hr;
//
//	DISPID dispidNamed = DISPID_UNKNOWN;
//	DISPPARAMS params;
//	params.rgdispidNamedArgs = NULL;
//	params.cArgs = 1;
//	params.cNamedArgs = 0;
//	params.rgvarg = new VARIANTARG[1];
//	params.rgvarg[0].vt = VT_I4;
//	params.rgvarg[0].intVal = value;
//
//	hr = pDisp->Invoke(dispid[0], IID_NULL, LOCALE_SYSTEM_DEFAULT, DISPATCH_METHOD, &params, NULL, NULL, NULL);        
//	return hr;
//}
HRESULT SetArrayNumberOfIndex(IDispatch* pDisp, int index, int value)
{
	CComVariant varName(index, VT_I4);
	DISPID dispId;
	CComVariant varValue;
	HRESULT hr = 0;
	varName.ChangeType(VT_BSTR); // 将数组下标转为数字型，以进行GetIDsOfNames
	hr = pDisp->GetIDsOfNames
		(IID_NULL, &varName.bstrVal, 1, LOCALE_USER_DEFAULT, &dispId);
	if (FAILED(hr)) 
		return hr;

	DISPID dispidPut = DISPID_PROPERTYPUT;             // put操作
	DISPPARAMS dispparams;
	dispparams.rgvarg = new VARIANTARG[1];                    // 初始化rgvarg
	dispparams.rgvarg[0].vt = VT_I4;                                      // 数据类型
	dispparams.rgvarg[0].intVal = value;                                  // 更新值
	dispparams.cArgs = 1;                                                       // 参数数量
	dispparams.cNamedArgs = 1;                                             // 参数名称
	dispparams.rgdispidNamedArgs = &dispidPut;         // 操作DispId，表明本参数适用于put操作
	hr = pDisp->Invoke(dispId, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_PROPERTYPUT, &dispparams, NULL, NULL, NULL);
	return hr;
}

STDMETHODIMP CEBClientCore::EB_Init(BSTR sAddress, BSTR sInitParameter)
{
	//MessageBox(NULL,sAddress,L"",MB_OK);
	USES_CONVERSION_EX;
	theEBAppClient.EB_Init(BSTR2A_EX(sAddress),BSTR2A_EX(sInitParameter));
	theEBAppClient.EB_SetCallBack((CEBCallbackInterface*)this);
	return S_OK;
}

STDMETHODIMP CEBClientCore::get_EB_IsInited(VARIANT_BOOL* pVal)
{
	*pVal = theEBAppClient.EB_IsInited()?VARIANT_TRUE:VARIANT_FALSE;
	return S_OK;
}
//
//STDMETHODIMP CEBClientCore::EB_SetCallBack(IDispatch* pCallback)
//{
//	m_pCallback = pCallback;
//	return m_pCallback==NULL?S_FALSE:S_OK;
//}

//STDMETHODIMP CEBClientCore::EB_SetMsgHwnd(ULONG hWnd)
//{
//	theEBAppClient.EB_SetMsgHwnd((HWND)hWnd);
//	return S_OK;
//}

STDMETHODIMP CEBClientCore::EB_UnInit(void)
{
	//MessageBox(NULL,L"EB_UnInit",L"",MB_OK);
	m_pSearchCallback.Release();
	theEBAppClient.EB_UnInit();
	return S_OK;
}

STDMETHODIMP CEBClientCore::get_EB_LastStateCode(LONG* pVal)
{
	*pVal = theEBAppClient.EB_GetLastStateCode();
	return S_OK;
}

STDMETHODIMP CEBClientCore::get_EB_Address(BSTR* pVal)
{
	CEBString sAddress = theEBAppClient.EB_GetAddress();
	*pVal = sAddress.CopyBSTR();
	return S_OK;
}
STDMETHODIMP CEBClientCore::put_EB_Address(BSTR newVal)
{
	USES_CONVERSION_EX;
	theEBAppClient.EB_Init(BSTR2A_EX(newVal),"");
	return S_OK;
}

STDMETHODIMP CEBClientCore::get_EB_AppPath(BSTR* pVal)
{
	CEBString sString = theEBAppClient.EB_GetAppPath();
	*pVal = sString.CopyBSTR();
	return S_OK;
}

STDMETHODIMP CEBClientCore::get_EB_ResourcePath(BSTR* pVal)
{
	CEBString sString = theEBAppClient.EB_GetResourcePath();
	*pVal = sString.CopyBSTR();
	return S_OK;
}

STDMETHODIMP CEBClientCore::get_EB_AppDataPath(BSTR* pVal)
{
	CEBString sString = theEBAppClient.EB_GetAppDataPath();
	*pVal = sString.CopyBSTR();
	return S_OK;
}

STDMETHODIMP CEBClientCore::get_EB_AppDataTempPath(BSTR* pVal)
{
	CEBString sString = theEBAppClient.EB_GetAppDataTempPath();
	USES_CONVERSION_EX;
	*pVal = sString.CopyBSTR();
	return S_OK;
}

STDMETHODIMP CEBClientCore::EB_CheckVersion(BSTR sClientVersion)
{
	USES_CONVERSION_EX;
	theEBAppClient.EB_CheckVersion(BSTR2A_EX(sClientVersion));
	return S_OK;
}

STDMETHODIMP CEBClientCore::EB_GetAreaInfo(LONG nParent, ULONG nParameter)
{
	USES_CONVERSION_EX;
	const int ret = theEBAppClient.EB_GetAreaInfo(nParent, nParameter);
	return ret==0?S_OK:S_FALSE;
}


STDMETHODIMP CEBClientCore::EB_SetDevAppId(ULONGLONG sAppId, BSTR sAppKey, VARIANT_BOOL bReLoadAppOnlineKey)
{
	USES_CONVERSION_EX;
	int ret = theEBAppClient.EB_SetDevAppId(sAppId,BSTR2A_EX(sAppKey),bReLoadAppOnlineKey?true:false);
	return ret==0?S_OK:S_FALSE;
}

STDMETHODIMP CEBClientCore::EB_SetDevAppOnlineKey(ULONGLONG sAppId, BSTR sAppOnlineKey)
{
	USES_CONVERSION_EX;
	theEBAppClient.EB_SetDevAppOnlineKey(sAppId,BSTR2A_EX(sAppOnlineKey));
	return S_OK;
}
STDMETHODIMP CEBClientCore::EB_GetDevAppId(ULONGLONG* pVal)
{
	*pVal =theEBAppClient.EB_GetDevAppId();
	return S_OK;
}
STDMETHODIMP CEBClientCore::EB_GetDevAppOnlineKey(BSTR* pVal)
{
	CEBString sString1 =theEBAppClient.EB_GetDevAppOnlineKey();
	*pVal = sString1.CopyBSTR();
	return S_OK;
}
STDMETHODIMP CEBClientCore::EB_GetSystemParameter(LONG nParameter, ULONG* pParameterVal)
{
	theEBAppClient.EB_GetSystemParameter((EB_SYSTEM_PARAMETER)nParameter,pParameterVal);
	return S_OK;
}
STDMETHODIMP CEBClientCore::EB_FreeSystemParameter(LONG nParameter, ULONG pParameterVal)
{
	theEBAppClient.EB_FreeSystemParameter((EB_SYSTEM_PARAMETER)nParameter,pParameterVal);
	return S_OK;
}
STDMETHODIMP CEBClientCore::EB_LogonAppCenter(void)
{
	int ret = theEBAppClient.EB_LogonAppCenter();
	return ret==0?S_OK:S_FALSE;
}

STDMETHODIMP CEBClientCore::EB_LogoutAppCenter(void)
{
	theEBAppClient.EB_LogoutAppCenter();
	return S_OK;
}

STDMETHODIMP CEBClientCore::EB_SendAPMsg(IDispatch* pAPMsgInfoInterface, VARIANT_BOOL bSaveOffMsg)
{
	if (pAPMsgInfoInterface == NULL) return E_INVALIDARG;
	USES_CONVERSION_EX;
	EB_APMsgInfo pAPMsgInfo;
	CComPtr<IEB_APMsgInfo> pInterface;
	pAPMsgInfoInterface->QueryInterface(IID_IEB_APMsgInfo,(void**)&pInterface);
	if (pInterface != NULL)
	{
		ULONGLONG sFromAppId = 0;
		CComBSTR sAccount;
		ULONGLONG nMsgId = 0;
		LONG nMsgType = 0;
		CComBSTR sMsgName;
		CComBSTR sMsgContent;
		CComBSTR sMsgTime;
		pInterface->get_FromAppId(&sFromAppId);
		pInterface->get_Account(&sAccount);
		pInterface->get_MsgId(&nMsgId);
		pInterface->get_MsgType(&nMsgType);
		pInterface->get_MsgName(&sMsgName);
		pInterface->get_MsgContent(&sMsgContent);
		pInterface->get_MsgTime(&sMsgTime);
		pAPMsgInfo.m_sFromAppId = sFromAppId;
		pAPMsgInfo.m_sAccount = BSTR2A_EX(sAccount);
		pAPMsgInfo.m_nMsgId = nMsgId;
		pAPMsgInfo.m_nMsgType = nMsgType;
		pAPMsgInfo.m_sMsgName = BSTR2A_EX(sMsgName);
		pAPMsgInfo.m_sMsgContent = BSTR2A_EX(sMsgContent);
		pAPMsgInfo.m_sMsgTime = BSTR2A_EX(sMsgTime);
	}else
	{
		CComVariant vValue;
		HRESULT hr;
		hr = GetArrayVariantOfName(pAPMsgInfoInterface,L"FromAppId",&vValue);
		if (SUCCEEDED(hr))
		{
			pAPMsgInfo.m_sFromAppId = vValue.ullVal;
			VariantClear(&vValue);
			//MessageBoxA(NULL,pAPMsgInfo.m_sFromAppId.c_str(),"",MB_OK);
		}
		hr = GetArrayVariantOfName(pAPMsgInfoInterface,L"Account",&vValue);
		if (SUCCEEDED(hr))
		{
			pAPMsgInfo.m_sAccount = vValue.bstrVal;
			VariantClear(&vValue);
			//MessageBoxA(NULL,pAPMsgInfo.m_sAccount.c_str(),"",MB_OK);
		}
		hr = GetArrayVariantOfName(pAPMsgInfoInterface,L"MsgId",&vValue);
		if (SUCCEEDED(hr))
		{
			pAPMsgInfo.m_nMsgId = vValue.ullVal;
			VariantClear(&vValue);
		}
		hr = GetArrayVariantOfName(pAPMsgInfoInterface,L"MsgType",&vValue);
		if (SUCCEEDED(hr))
		{
			pAPMsgInfo.m_nMsgType = vValue.lVal;
			VariantClear(&vValue);
		}
		hr = GetArrayVariantOfName(pAPMsgInfoInterface,L"MsgName",&vValue);
		if (SUCCEEDED(hr))
		{
			pAPMsgInfo.m_sMsgName = vValue.bstrVal;
			VariantClear(&vValue);
		}
		hr = GetArrayVariantOfName(pAPMsgInfoInterface,L"MsgContent",&vValue);
		if (SUCCEEDED(hr))
		{
			pAPMsgInfo.m_sMsgContent = vValue.bstrVal;
			VariantClear(&vValue);
		}
		//hr = GetArrayVariantOfName(pAPMsgInfoInterface,L"MsgTime",&vValue);
		//if (SUCCEEDED(hr))
		//{
		//	pAPMsgInfo.m_sMsgTime = vValue.bstrVal;
		//	VariantClear(&vValue);
		//}
	}
	int ret = theEBAppClient.EB_SendAPMsg(pAPMsgInfo,bSaveOffMsg?true:false);
	return ret==0?S_OK:S_FALSE;
}
STDMETHODIMP CEBClientCore::EB_SendAPMsg2(SEB_APMsgInfo* pAPMsgInfoStruct, VARIANT_BOOL bSaveOffMsg)
{
	if (pAPMsgInfoStruct == NULL) return E_INVALIDARG;
	USES_CONVERSION_EX;
	EB_APMsgInfo pAPMsgInfo;
	pAPMsgInfo.m_sFromAppId = pAPMsgInfoStruct->sFromAppId;
	pAPMsgInfo.m_sAccount = BSTR2A_EX(pAPMsgInfoStruct->sAccount);
	pAPMsgInfo.m_nMsgId = pAPMsgInfoStruct->nMsgId;
	pAPMsgInfo.m_nMsgType = pAPMsgInfoStruct->nMsgType;
	pAPMsgInfo.m_sMsgName = BSTR2A_EX(pAPMsgInfoStruct->sMsgName);
	pAPMsgInfo.m_sMsgContent = BSTR2A_EX(pAPMsgInfoStruct->sMsgContent);
	pAPMsgInfo.m_sMsgTime = BSTR2A_EX(pAPMsgInfoStruct->sMsgTime);
	theEBAppClient.EB_SendAPMsg(pAPMsgInfo,bSaveOffMsg?true:false);
	return S_OK;
}
//
//STDMETHODIMP CEBClientCore::EB_SendAPMsg3(VARIANT* pAPMsgInfoVariant, VARIANT_BOOL bSaveOffMsg)
//{
//	if (pAPMsgInfoVariant == NULL) return E_INVALIDARG;
//	//如果传入的类型不是VT_DISPATCH就退回出错信息
//	if(pAPMsgInfoVariant->vt!=VT_DISPATCH)
//		return E_INVALIDARG;
//	HRESULT hr;
//
//	//hr = GetArrayVariantOfIndex(pDispEx,0,&vValue);
//	USES_CONVERSION_EX;
//	EB_APMsgInfo pAPMsgInfo;
//	CComVariant vValue;
//	hr = GetArrayVariantOfName(pAPMsgInfoVariant->pdispVal,L"FromAppId",&vValue);
//	if (SUCCEEDED(hr))
//	{
//		pAPMsgInfo.m_sFromAppId = vValue.bstrVal;
//		VariantClear(&vValue);
//	}
//	hr = GetArrayVariantOfName(pAPMsgInfoVariant->pdispVal,L"Account",&vValue);
//	if (SUCCEEDED(hr))
//	{
//		pAPMsgInfo.m_sAccount = vValue.bstrVal;
//		VariantClear(&vValue);
//	}
//	hr = GetArrayVariantOfName(pAPMsgInfoVariant->pdispVal,L"MsgId",&vValue);
//	if (SUCCEEDED(hr))
//	{
//		pAPMsgInfo.m_sMsgId = vValue.bstrVal;
//		VariantClear(&vValue);
//	}
//	hr = GetArrayVariantOfName(pAPMsgInfoVariant->pdispVal,L"MsgType",&vValue);
//	if (SUCCEEDED(hr))
//	{
//		pAPMsgInfo.m_nMsgType = vValue.lVal;
//		VariantClear(&vValue);
//	}
//	hr = GetArrayVariantOfName(pAPMsgInfoVariant->pdispVal,L"MsgName",&vValue);
//	if (SUCCEEDED(hr))
//	{
//		pAPMsgInfo.m_sMsgName = vValue.bstrVal;
//		VariantClear(&vValue);
//	}
//	hr = GetArrayVariantOfName(pAPMsgInfoVariant->pdispVal,L"MsgContent",&vValue);
//	if (SUCCEEDED(hr))
//	{
//		pAPMsgInfo.m_sMsgContent = vValue.bstrVal;
//		VariantClear(&vValue);
//	}
//	//hr = GetArrayVariantOfName(pAPMsgInfoVariant->pdispVal,L"MsgTime",&vValue);
//	//if (SUCCEEDED(hr))
//	//{
//	//	pAPMsgInfo.m_sMsgTime = vValue.bstrVal;
//	//	VariantClear(&vValue);
//	//}
//
//	/*
//	//取得IDispatchEx接口
//	CComPtr<IDispatchEx> pDispEx;
//	hr=pAPMsgInfoVariant->pdispVal->QueryInterface(IID_IDispatchEx,(void**)&pDispEx);
//	if(FAILED(hr))
//		return E_INVALIDARG;
//	//枚举所有的属性
//	DISPID dispid;
//	//取得第一个元素的DISPID  
//	hr = pDispEx->GetNextDispID(fdexEnumAll, DISPID_STARTENUM, &dispid);
//	while (hr == NOERROR)
//	{
//		//元素名称
//		CComBSTR bstrName;
//		hr = pDispEx->GetMemberName(dispid, &bstrName);
//		if (FAILED(hr))
//			return S_FALSE;
//
//		//元素值
//		CComVariant vValue;
//		DISPPARAMS dispparamsNoArgs = {NULL, NULL, 0, 0};
//		hr = pDispEx->InvokeEx(dispid, LOCALE_USER_DEFAULT,
//			DISPATCH_PROPERTYGET, &dispparamsNoArgs,
//			&vValue, NULL, NULL);
//		if(FAILED(hr))
//			return S_FALSE;
//
//		if (bstrName == L"FromAppId")
//		{
//			pAPMsgInfo.m_sFromAppId = vValue.bstrVal;
//		}else if (bstrName == L"Account")
//		{
//			pAPMsgInfo.m_sAccount = vValue.bstrVal;
//		}else if (bstrName == L"MsgId")
//		{
//			pAPMsgInfo.m_sMsgId = vValue.bstrVal;
//		}else if (bstrName == L"MsgType")
//		{
//			pAPMsgInfo.m_nMsgType = vValue.lVal;
//		}else if (bstrName == L"MsgName")
//		{
//			pAPMsgInfo.m_sMsgName = vValue.bstrVal;
//		}else if (bstrName == L"MsgContent")
//		{
//			pAPMsgInfo.m_sMsgContent = vValue.bstrVal;
//		//}else if (bstrName == L"MsgTime")
//		//{
//		//	pAPMsgInfo.m_sMsgTime = vValue.bstrVal;
//		}
//
//		//取得下一个元素的DISPID   
//		hr = pDispEx->GetNextDispID(fdexEnumAll, dispid, &dispid);
//	}
//	*/
//	int ret = theEBAppClient.EB_SendAPMsg(pAPMsgInfo,bSaveOffMsg?true:false);
//	return ret==0?S_OK:S_FALSE;
//}

STDMETHODIMP CEBClientCore::EB_APMsgAck(ULONGLONG nMsgId)
{
	theEBAppClient.EB_APMsgAck(nMsgId);
	return S_OK;
}

STDMETHODIMP CEBClientCore::EB_Register(BSTR sAccount, BSTR sPassword, BSTR sUserName, BSTR sEnterpriseName)
{
	int ret = theEBAppClient.EB_Register(CEBString(sAccount).c_str(),CEBString(sPassword).c_str(),
		CEBString(sUserName).c_str(),CEBString(sEnterpriseName).c_str());
	return ret==0?S_OK:S_FALSE;
}

STDMETHODIMP CEBClientCore::EB_LogonByVisitor(BSTR sReqCode)
{
	int ret = theEBAppClient.EB_LogonByVisitor(CEBString(sReqCode).c_str());
	return ret==0?S_OK:S_FALSE;
}

STDMETHODIMP CEBClientCore::EB_LogonByAccount(BSTR sAccount, BSTR sPassword, BSTR sReqCode, BSTR sOAuthKey, LONG nNewLineState, LONG nLogonType)
{
	int ret = theEBAppClient.EB_LogonByAccount(CEBString(sAccount).c_str(),
		CEBString(sPassword).c_str(),
		CEBString(sReqCode).c_str(),
		CEBString(sOAuthKey).c_str(),
		(EB_USER_LINE_STATE)nNewLineState,
		(EB_LOGON_TYPE)nLogonType);
	return ret==0?S_OK:S_FALSE;
}

STDMETHODIMP CEBClientCore::EB_LogonOAuth(ULONGLONG nUserId, BSTR sOAuthKey, LONG nNewLineState)
{
	USES_CONVERSION_EX;
	int ret = theEBAppClient.EB_LogonOAuth(nUserId, BSTR2A_EX(sOAuthKey),(EB_USER_LINE_STATE)nNewLineState);
	return ret==0?S_OK:S_FALSE;
}

STDMETHODIMP CEBClientCore::EB_ReLogon(void)
{
	int ret = theEBAppClient.EB_ReLogon();
	return ret==0?S_OK:S_FALSE;
}

STDMETHODIMP CEBClientCore::get_EB_IsLogoned(VARIANT_BOOL* pVal)
{
	*pVal = theEBAppClient.EB_IsLogoned()?VARIANT_TRUE:VARIANT_FALSE;
	return S_OK;
}

STDMETHODIMP CEBClientCore::get_EB_IsLogonVisitor(VARIANT_BOOL* pVal)
{
	*pVal = theEBAppClient.EB_IsLogonVisitor()?VARIANT_TRUE:VARIANT_FALSE;
	return S_OK;
}

STDMETHODIMP CEBClientCore::EB_Logout(void)
{
	theEBAppClient.EB_Logout();
	return S_OK;
}

STDMETHODIMP CEBClientCore::EB_LoadInfo(void)
{
	theEBAppClient.EB_LoadInfo();
	return S_OK;
}

STDMETHODIMP CEBClientCore::EB_GetMyAccountInfo(IEB_AccountInfo** pOutAccountInterface)
{
	//CIEB_AccountInfo * pIEBAccountInfo = new CIEB_AccountInfo();
	//pIEBAccountInfo->put_Account(L"test1@entboost.com");
	//return pIEBAccountInfo->QueryInterface(IID_IEB_AccountInfo,(void**)pOutAccountInterface);
	CIEB_AccountInfo * pIEBAccountInfo = new CIEB_AccountInfo();
	if (!theEBAppClient.EB_GetMyAccountInfo(pIEBAccountInfo))
	{
		delete pIEBAccountInfo;
		return S_FALSE;
	}
	return pIEBAccountInfo->QueryInterface(IID_IEB_AccountInfo,(void**)pOutAccountInterface);
}

STDMETHODIMP CEBClientCore::EB_GetMyAccountInfo2(SEB_AccountInfo* pOutAccountStruct)
{
	EB_AccountInfo pAccountInfo;
	if (!theEBAppClient.EB_GetMyAccountInfo(&pAccountInfo))
	{
		return S_FALSE;
	}
	pOutAccountStruct->sAccount = pAccountInfo.GetAccount().CopyBSTR();
	pOutAccountStruct->sPassword = pAccountInfo.GetPasswd().CopyBSTR();
	pOutAccountStruct->sUsername = pAccountInfo.GetUserName().CopyBSTR();
	pOutAccountStruct->sDescription = pAccountInfo.GetDescription().CopyBSTR();
	return S_OK;
}

//STDMETHODIMP CEBClientCore::EB_GetMyAccountInfo3(VARIANT* pOutAccountVariant)
//{
//	// OK
//	CIEB_AccountInfo * pAccountInfo = new CIEB_AccountInfo();
//	pAccountInfo->put_Account(L"abc");
//	CComVariant var(pAccountInfo);
//	var.Detach(pOutAccountVariant);
//
//	//CComDispatchDriver s;
//	//SetArrayNumberOfIndex(s,0,12);
//	//SetArrayNumberOfIndex(s,1,13);
//	//CComVariant var(s);
//	//var.Detach(pOutAccountVariant);
//
//	//EB_AccountInfo pAccountInfo;
//	//if (!theEBAppClient.EB_GetMyAccountInfo(&pAccountInfo))
//	//{
//	//	return S_FALSE;
//	//}
//	//USES_CONVERSION_EX;
//	//CComSafeArray<VARIANT> m_sa;
//	//m_sa.Add(CComVariant(A2W(pAccountInfo.GetAccount().c_str())));
//	////m_sa.Add(CComVariant(A2W(pAccountInfo.GetPasswd().c_str())));
//	//m_sa.Add(CComVariant(_T("")));
//	//m_sa.Add(CComVariant(A2W(pAccountInfo.GetUserName().c_str())));
//	//VariantInit(pOutAccountVariant);
//	//CComVariant var(m_sa);
//	//var.Detach(pOutAccountVariant);
//	return S_OK;
//}

STDMETHODIMP CEBClientCore::get_EB_MyDefaultMemberCode(ULONGLONG* pVal)
{
	eb::bigint bMemberCode = 0;
	theEBAppClient.EB_GetMyDefaultMemberCode(bMemberCode);
	*pVal = bMemberCode;
	return S_OK;
}

STDMETHODIMP CEBClientCore::put_EB_MyDefaultMemberCode(ULONGLONG newVal)
{
	bool ret = theEBAppClient.EB_SetMyDefaultMemberCode(newVal);
	return ret==true?S_OK:S_FALSE;
}

STDMETHODIMP CEBClientCore::EB_GetMyDefaultMemberInfo(IEB_MemberInfo** pOutMemberInfoInterface)
{
	CIEB_MemberInfo * pIEBMemberInfo = new CIEB_MemberInfo();
	if (!theEBAppClient.EB_GetMyDefaultMemberInfo(pIEBMemberInfo))
	{
		delete pIEBMemberInfo;
		return S_FALSE;
	}
	return pIEBMemberInfo->QueryInterface(IID_IEB_MemberInfo,(void**)pOutMemberInfoInterface);
}

STDMETHODIMP CEBClientCore::EB_GetMyDefaultMemberInfo2(SEB_MemberInfo* pOutMemberInfoStruct)
{
	EB_MemberInfo pMemberInfo;
	if (!theEBAppClient.EB_GetMyDefaultMemberInfo(&pMemberInfo))
	{
		return S_FALSE;
	}
	pOutMemberInfoStruct->sMemberCode = pMemberInfo.m_sMemberCode;
	pOutMemberInfoStruct->sGroupCode = pMemberInfo.m_sGroupCode;
	pOutMemberInfoStruct->sMemberAccount = pMemberInfo.m_sMemberAccount.CopyBSTR();
	pOutMemberInfoStruct->sUserName = pMemberInfo.m_sUserName.CopyBSTR();
	pOutMemberInfoStruct->sJobTitle = pMemberInfo.m_sJobTitle.CopyBSTR();
	pOutMemberInfoStruct->nJobPosition = pMemberInfo.m_nJobPosition;
	pOutMemberInfoStruct->sCellPhone = pMemberInfo.m_sCellPhone.CopyBSTR();
	pOutMemberInfoStruct->sFax = pMemberInfo.m_sFax.CopyBSTR();
	pOutMemberInfoStruct->sWorkPhone = pMemberInfo.m_sWorkPhone.CopyBSTR();
	pOutMemberInfoStruct->sEmail = pMemberInfo.m_sEmail.CopyBSTR();
	pOutMemberInfoStruct->sAddress = pMemberInfo.m_sAddress.CopyBSTR();
	pOutMemberInfoStruct->sDescription = pMemberInfo.m_sDescription.CopyBSTR();
	pOutMemberInfoStruct->nManagerLevel = pMemberInfo.m_nManagerLevel;
	pOutMemberInfoStruct->nGender = pMemberInfo.m_nGender;
	pOutMemberInfoStruct->nBirthday = pMemberInfo.m_nBirthday;
	pOutMemberInfoStruct->nLineState = pMemberInfo.m_nLineState;
	pOutMemberInfoStruct->sHeadResourceId = pMemberInfo.m_sHeadResourceId;
	pOutMemberInfoStruct->sHeadResourceFile = pMemberInfo.m_sHeadResourceFile.CopyBSTR();
	return S_OK;
}
STDMETHODIMP CEBClientCore::EB_GetMyDefaultMemberHeadFile(BSTR* pVal)
{
	CEBString sString = theEBAppClient.EB_GetMyDefaultMemberHeadFile();
	*pVal = sString.CopyBSTR();
	return S_OK;
}

STDMETHODIMP CEBClientCore::get_EB_MyAccountSetting(LONG* pVal)
{
	*pVal = theEBAppClient.EB_GetMyAccountSetting();
	return S_OK;
}

STDMETHODIMP CEBClientCore::put_EB_MyAccountSetting(LONG newVal)
{
	bool ret = theEBAppClient.EB_SetMyAccountSetting(newVal);
	return ret==true?S_OK:S_FALSE;
}

STDMETHODIMP CEBClientCore::get_EB_LogonAccount(BSTR* pVal)
{
	CEBString str = theEBAppClient.EB_GetLogonAccount();
	*pVal = str.CopyBSTR();
	return S_OK;
}
STDMETHODIMP CEBClientCore::get_EB_LogonUserId(ULONGLONG* pVal)
{
	*pVal = theEBAppClient.EB_GetLogonUserId();
	return S_OK;
}

STDMETHODIMP CEBClientCore::get_EB_Description(BSTR* pVal)
{
	CEBString sString = theEBAppClient.EB_GetDescription();
	*pVal = sString.CopyBSTR();
	return S_OK;
}

STDMETHODIMP CEBClientCore::put_EB_Description(BSTR newVal)
{
	bool ret = theEBAppClient.EB_SetDescription(CEBString(newVal).c_str());
	return ret==true?S_OK:S_FALSE;
}

STDMETHODIMP CEBClientCore::get_EB_LineState(LONG* pVal)
{
	*pVal = (LONG)theEBAppClient.EB_GetLineState();
	return S_OK;
}

STDMETHODIMP CEBClientCore::put_EB_LineState(LONG newVal)
{
	bool ret = theEBAppClient.EB_SetLineState((EB_USER_LINE_STATE)newVal);
	return ret==true?S_OK:S_FALSE;
}

STDMETHODIMP CEBClientCore::get_EB_UserName(BSTR* pVal)
{
	CEBString sString = theEBAppClient.EB_GetUserName();
	*pVal = sString.CopyBSTR();
	return S_OK;
}

STDMETHODIMP CEBClientCore::put_EB_UserName(BSTR newVal)
{
	bool ret = theEBAppClient.EB_SetUserName(CEBString(newVal).c_str());
	return ret==true?S_OK:S_FALSE;
}

STDMETHODIMP CEBClientCore::get_EB_Password(BSTR* pVal)
{
	CEBString sString = theEBAppClient.EB_GetPassword();
	*pVal = sString.CopyBSTR();
	return S_OK;
}

//STDMETHODIMP CEBClientCore::put_EB_Password(BSTR newVal)
//{
//	bool ret = theEBAppClient.EB_SetPassword(CEBString(newVal).c_str());
//	return ret==true?S_OK:S_FALSE;
//}
STDMETHODIMP CEBClientCore::EB_SetPassword(BSTR sOldPassword,BSTR sNewPassword)
{
	bool ret = theEBAppClient.EB_SetPassword(CEBString(sOldPassword).c_str(),CEBString(sNewPassword).c_str());
	return ret==true?S_OK:S_FALSE;
}

STDMETHODIMP CEBClientCore::get_EB_CanEditEnterprise(ULONGLONG sEnterpriseCode, VARIANT_BOOL* pVal)
{
	bool ret = theEBAppClient.EB_CanEditEnterprise(sEnterpriseCode);
	*pVal = ret==true?VARIANT_TRUE:VARIANT_FALSE;
	return S_OK;
}

STDMETHODIMP CEBClientCore::get_EB_CanEditGroupInfo(ULONGLONG sEnterpriseCode, ULONGLONG sGroupCode, VARIANT_BOOL* pVal)
{
	bool ret = theEBAppClient.EB_CanEditGroupInfo(sEnterpriseCode,sGroupCode);
	*pVal = ret==true?VARIANT_TRUE:VARIANT_FALSE;
	return S_OK;
}

STDMETHODIMP CEBClientCore::get_EB_CanDeleteGroupInfo(ULONGLONG sGroupCode, VARIANT_BOOL* pVal)
{
	bool ret = theEBAppClient.EB_CanDeleteGroupInfo(sGroupCode);
	*pVal = ret==true?VARIANT_TRUE:VARIANT_FALSE;
	return S_OK;
}

STDMETHODIMP CEBClientCore::get_EB_CanEditMyBaseMemberInfo(ULONGLONG sGroupCode, VARIANT_BOOL* pVal)
{
	bool ret = theEBAppClient.EB_CanEditMyBaseMemberInfo(sGroupCode);
	*pVal = ret==true?VARIANT_TRUE:VARIANT_FALSE;
	return S_OK;
}
STDMETHODIMP CEBClientCore::get_EB_CanEditMemberInfo(ULONGLONG sGroupCode, ULONGLONG nMemberUserId, VARIANT_BOOL* pVal)
{
	bool ret = theEBAppClient.EB_CanEditMemberInfo(sGroupCode,nMemberUserId);
	*pVal = ret==true?VARIANT_TRUE:VARIANT_FALSE;
	return S_OK;
}

STDMETHODIMP CEBClientCore::get_EB_CanDeleteMemberInfo(ULONGLONG sGroupCode, ULONGLONG nMemberUserId, VARIANT_BOOL* pVal)
{
	bool ret = theEBAppClient.EB_CanDeleteMemberInfo(sGroupCode,nMemberUserId);
	*pVal = ret==true?VARIANT_TRUE:VARIANT_FALSE;
	return S_OK;
}

STDMETHODIMP CEBClientCore::get_EB_CanDeleteMemberInfo2(ULONGLONG sMemberCode, VARIANT_BOOL* pVal)
{
	bool ret = theEBAppClient.EB_CanDeleteMemberInfo(sMemberCode);
	*pVal = ret==true?VARIANT_TRUE:VARIANT_FALSE;
	return S_OK;
}

STDMETHODIMP CEBClientCore::get_EB_CanEditGroupRes(ULONGLONG sGroupCode, ULONGLONG nResourceId, VARIANT_BOOL* pVal)
{
	bool ret = theEBAppClient.EB_CanEditGroupRes(sGroupCode, nResourceId);
	*pVal = ret==true?VARIANT_TRUE:VARIANT_FALSE;
	return S_OK;
}

STDMETHODIMP CEBClientCore::get_EB_CanDeleteGroupRes(ULONGLONG sGroupCode, ULONGLONG nResourceId, VARIANT_BOOL* pVal)
{
	bool ret = theEBAppClient.EB_CanDeleteGroupRes(sGroupCode, nResourceId);
	*pVal = ret==true?VARIANT_TRUE:VARIANT_FALSE;
	return S_OK;
}

STDMETHODIMP CEBClientCore::EB_LoadRes(ULONGLONG sGroupCode)
{
	int ret = theEBAppClient.EB_LoadRes(sGroupCode);
	return ret==0?S_OK:S_FALSE;
}

STDMETHODIMP CEBClientCore::EB_GetDirAllRes(ULONGLONG sParentResId, VARIANT* pOutResourceList)
{
	std::vector<EB_ResourceInfo> pResourceList;
	theEBAppClient.EB_GetDirAllRes(sParentResId,pResourceList);
	// TEST CODE
	//EB_ResourceInfo s1;
	//s1.m_sResId = "RES1";
	//pResourceList.push_back(s1);
	//s1.m_sResId = "RES2";
	//pResourceList.push_back(s1);
	//s1.m_sResId = "RES3";
	//pResourceList.push_back(s1);
	if (pResourceList.empty())
	{
		return S_FALSE;
	}
	CComSafeArray<VARIANT> m_sa;
	for (size_t i=0; i<pResourceList.size(); i++)
	{
		CIEB_ResourceInfo * pIEBResoureInfo = new CIEB_ResourceInfo(pResourceList[i]);
		CComPtr<IEB_ResourceInfo> pOutResourceInfoInterface;
		pIEBResoureInfo->QueryInterface(IID_IEB_ResourceInfo,(void**)&pOutResourceInfoInterface);
		m_sa.Add(CComVariant(pOutResourceInfoInterface));
	}
	VariantInit(pOutResourceList);
	CComVariant var(m_sa);
	var.Detach(pOutResourceList);
	return S_OK;
}
STDMETHODIMP CEBClientCore::EB_GetNotDirAllRes(ULONGLONG sGroupCode, VARIANT* pOutResourceList)
{
	std::vector<EB_ResourceInfo> pResourceList;
	theEBAppClient.EB_GetNotDirAllRes(pResourceList,sGroupCode);
	if (pResourceList.empty())
	{
		return S_FALSE;
	}
	CComSafeArray<VARIANT> m_sa;
	for (size_t i=0; i<pResourceList.size(); i++)
	{
		CIEB_ResourceInfo * pIEBResoureInfo = new CIEB_ResourceInfo(pResourceList[i]);
		CComPtr<IEB_ResourceInfo> pOutResourceInfoInterface;
		pIEBResoureInfo->QueryInterface(IID_IEB_ResourceInfo,(void**)&pOutResourceInfoInterface);
		m_sa.Add(CComVariant(pOutResourceInfoInterface));
	}
	VariantInit(pOutResourceList);
	CComVariant var(m_sa);
	var.Detach(pOutResourceList);
	return S_OK;
}
STDMETHODIMP CEBClientCore::EB_HasSubRes(ULONGLONG sParentResId, VARIANT_BOOL* pVal)
{
	bool ret = theEBAppClient.EB_HasSubRes(sParentResId);
	*pVal = ret==true?VARIANT_TRUE:VARIANT_FALSE;
	return S_OK;
}
STDMETHODIMP CEBClientCore::EB_GetAllRes(ULONGLONG sGroupCode, VARIANT* pOutResourceList)
{
	std::vector<EB_ResourceInfo> pResourceList;
	theEBAppClient.EB_GetAllRes(pResourceList,sGroupCode);
	if (pResourceList.empty())
	{
		return S_FALSE;
	}
	CComSafeArray<VARIANT> m_sa;
	for (size_t i=0; i<pResourceList.size(); i++)
	{
		CIEB_ResourceInfo * pIEBResoureInfo = new CIEB_ResourceInfo(pResourceList[i]);
		CComPtr<IEB_ResourceInfo> pOutResourceInfoInterface;
		pIEBResoureInfo->QueryInterface(IID_IEB_ResourceInfo,(void**)&pOutResourceInfoInterface);
		m_sa.Add(CComVariant(pOutResourceInfoInterface));
	}
	VariantInit(pOutResourceList);
	CComVariant var(m_sa);
	var.Detach(pOutResourceList);
	return S_OK;
}
STDMETHODIMP CEBClientCore::EB_GetAllFileRes(ULONGLONG sGroupCode, VARIANT* pOutResourceList)
{
	std::vector<EB_ResourceInfo> pResourceList;
	theEBAppClient.EB_GetAllFileRes(pResourceList,sGroupCode);
	if (pResourceList.empty())
	{
		return S_FALSE;
	}
	CComSafeArray<VARIANT> m_sa;
	for (size_t i=0; i<pResourceList.size(); i++)
	{
		CIEB_ResourceInfo * pIEBResoureInfo = new CIEB_ResourceInfo(pResourceList[i]);
		CComPtr<IEB_ResourceInfo> pOutResourceInfoInterface;
		pIEBResoureInfo->QueryInterface(IID_IEB_ResourceInfo,(void**)&pOutResourceInfoInterface);
		m_sa.Add(CComVariant(pOutResourceInfoInterface));
	}
	VariantInit(pOutResourceList);
	CComVariant var(m_sa);
	var.Detach(pOutResourceList);
	return S_OK;
}
STDMETHODIMP CEBClientCore::EB_GetAllNoteRes(ULONGLONG sGroupCode, VARIANT* pOutResourceList)
{
	std::vector<EB_ResourceInfo> pResourceList;
	theEBAppClient.EB_GetAllNoteRes(pResourceList,sGroupCode);
	if (pResourceList.empty())
	{
		return S_FALSE;
	}
	CComSafeArray<VARIANT> m_sa;
	for (size_t i=0; i<pResourceList.size(); i++)
	{
		CIEB_ResourceInfo * pIEBResoureInfo = new CIEB_ResourceInfo(pResourceList[i]);
		CComPtr<IEB_ResourceInfo> pOutResourceInfoInterface;
		pIEBResoureInfo->QueryInterface(IID_IEB_ResourceInfo,(void**)&pOutResourceInfoInterface);
		m_sa.Add(CComVariant(pOutResourceInfoInterface));
	}
	VariantInit(pOutResourceList);
	CComVariant var(m_sa);
	var.Detach(pOutResourceList);
	return S_OK;
}


STDMETHODIMP CEBClientCore::EB_AddDirRes(BSTR sDirName, ULONGLONG sParentResId, ULONGLONG sGroupCode)
{
	int ret = theEBAppClient.EB_AddDirRes(CEBString(sDirName).c_str(),sParentResId,sGroupCode);
	return ret==0?S_OK:S_FALSE;
}

STDMETHODIMP CEBClientCore::EB_AddFileRes(BSTR sFilePath, BSTR sFileName, BSTR sDescription, ULONGLONG sParentResId, ULONGLONG sGroupCode)
{
	int ret = theEBAppClient.EB_AddFileRes(CEBString(sFilePath).c_str(),CEBString(sFileName).c_str(),
		CEBString(sDescription).c_str(),sParentResId,sGroupCode);
	return ret==0?S_OK:S_FALSE;
}
STDMETHODIMP CEBClientCore::EB_AddTempGroupFileRes(BSTR sFilePath, BSTR sFileName, ULONGLONG sGroupCode)
{
	int ret = theEBAppClient.EB_AddTempGroupFileRes(CEBString(sFilePath).c_str(),CEBString(sFileName).c_str(),sGroupCode);
	return ret==0?S_OK:S_FALSE;
}

STDMETHODIMP CEBClientCore::EB_AddNoteRes(BSTR sNoteName, BSTR sDescription, ULONGLONG sParentResId, ULONGLONG sGroupCode)
{
	int ret = theEBAppClient.EB_AddNoteRes(CEBString(sNoteName).c_str(),CEBString(sDescription).c_str(),sParentResId,sGroupCode);
	return ret==0?S_OK:S_FALSE;
}

STDMETHODIMP CEBClientCore::EB_EditRes(ULONGLONG sResId, BSTR sName, BSTR sDescription)
{
	int ret = theEBAppClient.EB_EditRes(sResId,CEBString(sName).c_str(),CEBString(sDescription).c_str());
	return ret==0?S_OK:S_FALSE;
}

STDMETHODIMP CEBClientCore::EB_MoveRes2Dir(ULONGLONG sResId, ULONGLONG sParentResId)
{
	int ret = theEBAppClient.EB_MoveRes2Dir(sResId,sParentResId);
	return ret==0?S_OK:S_FALSE;
}

STDMETHODIMP CEBClientCore::EB_DeleteRes(ULONGLONG sResId)
{
	int ret = theEBAppClient.EB_DeleteRes(sResId);
	return ret==0?S_OK:S_FALSE;
}
STDMETHODIMP CEBClientCore::EB_GetResourceInfo(ULONGLONG sResId, IEB_ResourceInfo** pVal)
{
	CIEB_ResourceInfo * pIEBResoureInfo = new CIEB_ResourceInfo();
	if (!theEBAppClient.EB_GetResourceInfo(sResId,pIEBResoureInfo))
	{
		delete pIEBResoureInfo;
		return S_FALSE;
	}
	return pIEBResoureInfo->QueryInterface(IID_IEB_ResourceInfo,(void**)pVal);
}
STDMETHODIMP CEBClientCore::EB_GetResourceCmInfo(ULONGLONG sResId, BSTR* pVal)
{
	tstring sString;
	if (!theEBAppClient.EB_GetResourceCmInfo(sResId,sString))
		return S_FALSE;
	USES_CONVERSION_EX;
	*pVal = ::SysAllocString(A2W_EX(sString.c_str(),0));
	return S_OK;
}
STDMETHODIMP CEBClientCore::EB_DownloadFileRes(ULONGLONG sResId, BSTR sSaveTo)
{
	int ret = theEBAppClient.EB_DownloadFileRes(sResId,CEBString(sSaveTo).c_str());
	return ret==0?S_OK:S_FALSE;
}
STDMETHODIMP CEBClientCore::EB_CancelFileRes(ULONGLONG sResId, ULONGLONG nMsgId)
{
	int ret = theEBAppClient.EB_CancelFileRes(sResId,nMsgId);
	return ret==0?S_OK:S_FALSE;
}

STDMETHODIMP CEBClientCore::EB_CallMember(ULONGLONG sMemberCode, ULONGLONG sExistCallId)
{
	int ret = theEBAppClient.EB_CallMember(sMemberCode,sExistCallId);
	return ret==0?S_OK:S_FALSE;
}
STDMETHODIMP CEBClientCore::EB_CallAccount(BSTR sToAccount, ULONGLONG sExistCallId)
{
	int ret = theEBAppClient.EB_CallAccount(CEBString(sToAccount).c_str(),sExistCallId);
	return ret==0?S_OK:S_FALSE;
}
STDMETHODIMP CEBClientCore::EB_CallUserId(ULONGLONG nUserId, ULONGLONG sExistCallId)
{
	int ret = theEBAppClient.EB_CallUserId(nUserId,sExistCallId);
	return ret==0?S_OK:S_FALSE;
}
STDMETHODIMP CEBClientCore::EB_Call2Group(ULONGLONG sCallId, BSTR sToAccount)
{
	int ret = theEBAppClient.EB_Call2Group(sCallId,CEBString(sToAccount).c_str());
	return ret==0?S_OK:S_FALSE;
}
STDMETHODIMP CEBClientCore::EB_CallGroup(ULONGLONG sGroupCode)
{
	int ret = theEBAppClient.EB_CallGroup(sGroupCode);
	return ret==0?S_OK:S_FALSE;
}
STDMETHODIMP CEBClientCore::EB_CallAnswer(ULONGLONG sCallId,VARIANT_BOOL bAccept)
{
	int ret = theEBAppClient.EB_CallAnswer(sCallId,bAccept?true:false);
	return ret==0?S_OK:S_FALSE;
}
STDMETHODIMP CEBClientCore::EB_CallHangup(ULONGLONG sCallId)
{
	int ret = theEBAppClient.EB_CallHangup(sCallId);
	return ret==0?S_OK:S_FALSE;
}
STDMETHODIMP CEBClientCore::EB_CallExit(ULONGLONG sCallId)
{
	theEBAppClient.EB_CallExit(sCallId);
	return S_OK;
}
//STDMETHODIMP CEBClientCore::EB_CheckReCall(ULONGLONG sCallId,VARIANT_BOOL bOffReCall)
//{
//	int ret = theEBAppClient.EB_CheckReCall(sCallId,bOffReCall?true:false);
//	if (ret==1)
//		return S_FALSE;
//	return ret==0?S_OK:S_FALSE;
//}
STDMETHODIMP CEBClientCore::EB_RichBufferAddText(ULONGLONG sCallId, BSTR sText)
{
	theEBAppClient.EB_RichBufferAddText(sCallId,CEBString(sText).c_str());
	return S_OK;
}
STDMETHODIMP CEBClientCore::EB_RichBufferAddObject(ULONGLONG sCallId, BYTE* pData, ULONG nSize)
{
	theEBAppClient.EB_RichBufferAddObject(sCallId,(const char*)pData,nSize);
	return S_OK;
}
STDMETHODIMP CEBClientCore::EB_RichBufferAddObject2(ULONGLONG sCallId, VARIANT* pVar, ULONG nSize)
{
	if (pVar==NULL) return E_INVALIDARG;
	BYTE* pData = pVar->pbVal;
	//BYTE* pData = NULL;
	//ULONG nSize = 0;
	//if (!GetBinaryFromVariant(*pVar,&pData,&nSize))
	//{
	//	return E_INVALIDARG;
	//}
	if (pData==NULL || nSize==0)
		return E_INVALIDARG;
	theEBAppClient.EB_RichBufferAddObject(sCallId,(const char*)pData,nSize);
	return S_OK;
}
STDMETHODIMP CEBClientCore::EB_RichBufferAddResource(ULONGLONG sCallId, BSTR sResource)
{
	theEBAppClient.EB_RichBufferAddResource(sCallId,CEBString(sResource).c_str());
	return S_OK;
}
STDMETHODIMP CEBClientCore::EB_RichBufferClear(ULONGLONG sCallId)
{
	theEBAppClient.EB_RichBufferClear(sCallId);
	return S_OK;
}
STDMETHODIMP CEBClientCore::EB_SendRichBuffer(ULONGLONG sCallId, ULONGLONG sToUserId, VARIANT_BOOL bPrivate)
{
	int ret = theEBAppClient.EB_SendRichBuffer(sCallId,sToUserId,bPrivate?true:false);
	return ret==0?S_OK:S_FALSE;
}
STDMETHODIMP CEBClientCore::EB_SendText(ULONGLONG sCallId, BSTR sTextMsg, ULONGLONG sToUserId, VARIANT_BOOL bPrivate)
{
	int ret = theEBAppClient.EB_SendText(sCallId,CEBString(sTextMsg).c_str(),sToUserId,bPrivate?true:false);
	return ret==0?S_OK:S_FALSE;
}

STDMETHODIMP CEBClientCore::EB_SendRich(ULONGLONG sCallId, VARIANT* pRichMsg, ULONGLONG sToUserId, VARIANT_BOOL bPrivate)
{
	CIEB_ChatRichInfo pEBRichInfo;
	HRESULT hr = pEBRichInfo.put_RichMsg(pRichMsg);
	if (FAILED(hr)) return S_FALSE;
	int ret = theEBAppClient.EB_SendRich(sCallId,pEBRichInfo.m_pRichMsg,sToUserId,bPrivate?true:false);
	return ret==0?S_OK:S_FALSE;
}
STDMETHODIMP CEBClientCore::EB_SendFile(ULONGLONG sCallId, BSTR sFilePath, ULONGLONG sToUserId, VARIANT_BOOL bPrivate, VARIANT_BOOL bOffFile)
{
	int ret = theEBAppClient.EB_SendFile(sCallId,CEBString(sFilePath).c_str(),sToUserId,
		bPrivate?true:false,bOffFile?true:false);
	return ret==0?S_OK:S_FALSE;
}
STDMETHODIMP CEBClientCore::EB_AcceptFile(ULONGLONG sCallId, ULONGLONG nMsgId, BSTR sSaveTo)
{
	int ret = theEBAppClient.EB_AcceptFile(sCallId,nMsgId,CEBString(sSaveTo).c_str());
	return ret==0?S_OK:S_FALSE;
}
STDMETHODIMP CEBClientCore::EB_Save2CloudDrive(ULONGLONG sCallId, ULONGLONG nMsgId)
{
	int ret = theEBAppClient.EB_Save2CloudDrive(sCallId,nMsgId);
	return ret==0?S_OK:S_FALSE;
}
//STDMETHODIMP CEBClientCore::EB_AcceptFileRes(ULONGLONG sCallId, ULONGLONG sResId, BSTR sSaveTo)
//{
//	int ret = theEBAppClient.EB_AcceptFileRes(sCallId,sResId,CEBString(sSaveTo).c_str());
//	return ret==0?S_OK:S_FALSE;
//}
STDMETHODIMP CEBClientCore::EB_CancelFile(ULONGLONG sCallId, ULONGLONG nMsgId)
{
	int ret = theEBAppClient.EB_CancelFile(sCallId,nMsgId);
	return ret==0?S_OK:S_FALSE;
}
STDMETHODIMP CEBClientCore::EB_GetCallAccountInfoList(ULONGLONG sCallId, VARIANT* pVal)
{
	std::vector<EB_AccountInfo> pUserList;
	theEBAppClient.EB_GetCallAccountInfoList(sCallId,pUserList);
	if (pUserList.empty())
	{
		return S_FALSE;
	}
	CComSafeArray<VARIANT> m_sa;
	for (size_t i=0; i<pUserList.size(); i++)
	{
		CIEB_AccountInfo * pIEBAccountInfo = new CIEB_AccountInfo(pUserList[i]);
		CComPtr<IEB_AccountInfo> pOutAccountInfoInterface;
		pIEBAccountInfo->QueryInterface(IID_IEB_AccountInfo,(void**)&pOutAccountInfoInterface);
		m_sa.Add(CComVariant(pOutAccountInfoInterface));
	}
	VariantInit(pVal);
	CComVariant var(m_sa);
	var.Detach(pVal);
	return S_OK;
}
STDMETHODIMP CEBClientCore::EB_GetCallUserIdList(ULONGLONG sCallId, VARIANT* pVal)
{
	std::vector<eb::bigint> pUserList;
	theEBAppClient.EB_GetCallUserIdList(sCallId,pUserList);
	if (pUserList.empty())
	{
		return S_FALSE;
	}
	CComSafeArray<VARIANT> m_sa;
	for (size_t i=0; i<pUserList.size(); i++)
	{
		m_sa.Add(CComVariant((ULONGLONG)pUserList[i]));
	}
	VariantInit(pVal);
	CComVariant var(m_sa);
	var.Detach(pVal);
	return S_OK;
}
STDMETHODIMP CEBClientCore::EB_GetCallAccountList(ULONGLONG sCallId, VARIANT* pVal)
{
	std::vector<tstring> pUserList;
	theEBAppClient.EB_GetCallAccountList(sCallId,pUserList);
	if (pUserList.empty())
	{
		return S_FALSE;
	}
	CComSafeArray<VARIANT> m_sa;
	for (size_t i=0; i<pUserList.size(); i++)
	{
		m_sa.Add(CComVariant(CEBString(pUserList[i]).CopyBSTR()));
	}
	VariantInit(pVal);
	CComVariant var(m_sa);
	var.Detach(pVal);
	return S_OK;
}
STDMETHODIMP CEBClientCore::EB_GetCallAccountInfo(ULONGLONG sCallId, ULONGLONG nUserId, IEB_AccountInfo** pVal)
{
	CIEB_AccountInfo * pIEBAccountInfo = new CIEB_AccountInfo();
	if (!theEBAppClient.EB_GetCallAccountInfo(sCallId,nUserId,pIEBAccountInfo))
	{
		delete pIEBAccountInfo;
		return S_FALSE;
	}
	return pIEBAccountInfo->QueryInterface(IID_IEB_AccountInfo,(void**)pVal);
}
STDMETHODIMP CEBClientCore::EB_GetCallAccountInfo2(ULONGLONG sCallId, ULONGLONG nUserId, SEB_AccountInfo* pVal)
{
	EB_AccountInfo pAccountInfo;
	if (!theEBAppClient.EB_GetCallAccountInfo(sCallId,nUserId,&pAccountInfo))
	{
		return S_FALSE;
	}
	pVal->nUserId = pAccountInfo.GetUserId();
	pVal->sAccount = pAccountInfo.GetAccount().CopyBSTR();
	pVal->sPassword = pAccountInfo.GetPasswd().CopyBSTR();
	pVal->sUsername = pAccountInfo.GetUserName().CopyBSTR();
	pVal->sDescription = pAccountInfo.GetDescription().CopyBSTR();
	return S_OK;
}

STDMETHODIMP CEBClientCore::EB_LoadContact(void)
{
	int ret = theEBAppClient.EB_LoadContact();
	return ret==0?S_OK:S_FALSE;
}
STDMETHODIMP CEBClientCore::EB_EditUGInfo(ULONGLONG nUGId, BSTR bstrGroupName)
{
	int ret = theEBAppClient.EB_EditUGInfo(nUGId,CEBString(bstrGroupName).c_str());
	return ret==0?S_OK:S_FALSE;
}
STDMETHODIMP CEBClientCore::EB_DeleteUGInfo(ULONGLONG nUGId)
{
	int ret = theEBAppClient.EB_DeleteUGInfo(nUGId);
	return ret==0?S_OK:S_FALSE;
}
STDMETHODIMP CEBClientCore::EB_GetUGInfoList(VARIANT* pVal)
{
	std::vector<EB_UGInfo> pUGInfoList;
	theEBAppClient.EB_GetUGInfoList(pUGInfoList);
	if (pUGInfoList.empty())
	{
		return S_FALSE;
	}
	CComSafeArray<VARIANT> m_sa;
	for (size_t i=0; i<pUGInfoList.size(); i++)
	{
		CIEB_UGInfo * pIEBUGInfo = new CIEB_UGInfo(pUGInfoList[i]);
		CComPtr<IEB_UGInfo> pOutInterface;
		pIEBUGInfo->QueryInterface(IID_IEB_UGInfo,(void**)&pOutInterface);
		m_sa.Add(CComVariant(pOutInterface));
	}
	VariantInit(pVal);
	CComVariant var(m_sa);
	var.Detach(pVal);
	return S_OK;
}
STDMETHODIMP CEBClientCore::EB_ReqAddContact(ULONGLONG nContactUserId, BSTR bstrDescription)
{
	int ret = theEBAppClient.EB_ReqAddContact(nContactUserId,CEBString(bstrDescription).c_str());
	return ret==0?S_OK:S_FALSE;
}

STDMETHODIMP CEBClientCore::EB_EditContact(IDispatch* newVal)
{
	if (newVal == NULL) return E_INVALIDARG;
	USES_CONVERSION_EX;
	EB_ContactInfo pContactInfo;
	CComPtr<IEB_ContactInfo> pInterface;
	newVal->QueryInterface(IID_IEB_ContactInfo,(void**)&pInterface);
	if (pInterface != NULL)
	{
		ULONGLONG nContactId = 0;
		SHORT nContactType = 0;
		ULONGLONG nUGId = 0;
		CComBSTR sGroupName;
		CComBSTR sContact;
		CComBSTR sName;
		CComBSTR sPhone;
		CComBSTR sEmail;
		CComBSTR sAddress;
		CComBSTR sDescription;
		CComBSTR sCompany;
		CComBSTR sJobTitle;
		CComBSTR sTel;
		CComBSTR sFax;
		CComBSTR sUrl;
		ULONGLONG nContactUserId = 0;
		pInterface->get_ContactId(&nContactId);
		pInterface->get_ContactType(&nContactType);
		pInterface->get_UGId(&nUGId);
		pInterface->get_GroupName(&sGroupName);
		pInterface->get_Contact(&sContact);
		pInterface->get_Name(&sName);
		pInterface->get_Phone(&sPhone);
		pInterface->get_Email(&sEmail);
		pInterface->get_Address(&sAddress);
		pInterface->get_Description(&sDescription);
		pInterface->get_Company(&sCompany);
		pInterface->get_JobTitle(&sJobTitle);
		pInterface->get_Tel(&sTel);
		pInterface->get_Fax(&sFax);
		pInterface->get_Url(&sUrl);
		pInterface->get_ContactUserId(&nContactUserId);
		pContactInfo.m_nContactId = nContactId;
		pContactInfo.m_nContactType = nContactType;
		pContactInfo.m_nUGId = nUGId;
		pContactInfo.m_sGroupName = BSTR2A_EX(sGroupName);
		pContactInfo.m_nContactUserId = nContactUserId;
		pContactInfo.m_sContact = BSTR2A_EX(sContact);
		pContactInfo.m_sName = BSTR2A_EX(sName);
		pContactInfo.m_sPhone = BSTR2A_EX(sPhone);
		pContactInfo.m_sEmail = BSTR2A_EX(sEmail);
		pContactInfo.m_sAddress = BSTR2A_EX(sAddress);
		pContactInfo.m_sDescription = BSTR2A_EX(sDescription);
		pContactInfo.m_sCompany = BSTR2A_EX(sCompany);
		pContactInfo.m_sJobTitle = BSTR2A_EX(sJobTitle);
		pContactInfo.m_sTel = BSTR2A_EX(sTel);
		pContactInfo.m_sFax = BSTR2A_EX(sFax);
		pContactInfo.m_sUrl = BSTR2A_EX(sUrl);
	}else
	{
		CComVariant vValue;
		HRESULT hr;

		hr = GetArrayVariantOfName(newVal,L"ContactId",&vValue);
		if (SUCCEEDED(hr))
		{
			pContactInfo.m_nContactId = vValue.ullVal;
			VariantClear(&vValue);
		}
		hr = GetArrayVariantOfName(newVal,L"ContactType",&vValue);
		if (SUCCEEDED(hr))
		{
			pContactInfo.m_nContactType = (int)vValue.iVal;
			VariantClear(&vValue);
		}
		hr = GetArrayVariantOfName(newVal,L"UGId",&vValue);
		if (SUCCEEDED(hr))
		{
			pContactInfo.m_nUGId = vValue.llVal;
			VariantClear(&vValue);
		}
		hr = GetArrayVariantOfName(newVal,L"GroupName",&vValue);
		if (SUCCEEDED(hr))
		{
			pContactInfo.m_sGroupName = vValue.bstrVal;
			VariantClear(&vValue);
		}
		hr = GetArrayVariantOfName(newVal,L"ContactUserId",&vValue);
		if (SUCCEEDED(hr))
		{
			pContactInfo.m_nContactUserId = vValue.ullVal;
			VariantClear(&vValue);
		}
		hr = GetArrayVariantOfName(newVal,L"Contact",&vValue);
		if (SUCCEEDED(hr))
		{
			pContactInfo.m_sContact = vValue.bstrVal;
			VariantClear(&vValue);
		}
		hr = GetArrayVariantOfName(newVal,L"Name",&vValue);
		if (SUCCEEDED(hr))
		{
			pContactInfo.m_sName = vValue.bstrVal;
			VariantClear(&vValue);
		}
		hr = GetArrayVariantOfName(newVal,L"Phone",&vValue);
		if (SUCCEEDED(hr))
		{
			pContactInfo.m_sPhone = vValue.bstrVal;
			VariantClear(&vValue);
		}
		hr = GetArrayVariantOfName(newVal,L"Email",&vValue);
		if (SUCCEEDED(hr))
		{
			pContactInfo.m_sEmail = vValue.bstrVal;
			VariantClear(&vValue);
		}
		hr = GetArrayVariantOfName(newVal,L"Address",&vValue);
		if (SUCCEEDED(hr))
		{
			pContactInfo.m_sAddress = vValue.bstrVal;
			VariantClear(&vValue);
		}
		hr = GetArrayVariantOfName(newVal,L"Description",&vValue);
		if (SUCCEEDED(hr))
		{
			pContactInfo.m_sDescription = vValue.bstrVal;
			VariantClear(&vValue);
		}
		hr = GetArrayVariantOfName(newVal,L"Company",&vValue);
		if (SUCCEEDED(hr))
		{
			pContactInfo.m_sCompany = vValue.bstrVal;
			VariantClear(&vValue);
		}
		hr = GetArrayVariantOfName(newVal,L"JobTitle",&vValue);
		if (SUCCEEDED(hr))
		{
			pContactInfo.m_sJobTitle = vValue.bstrVal;
			VariantClear(&vValue);
		}
		hr = GetArrayVariantOfName(newVal,L"Tel",&vValue);
		if (SUCCEEDED(hr))
		{
			pContactInfo.m_sTel = vValue.bstrVal;
			VariantClear(&vValue);
		}
		hr = GetArrayVariantOfName(newVal,L"Fax",&vValue);
		if (SUCCEEDED(hr))
		{
			pContactInfo.m_sFax = vValue.bstrVal;
			VariantClear(&vValue);
		}
		hr = GetArrayVariantOfName(newVal,L"Url",&vValue);
		if (SUCCEEDED(hr))
		{
			pContactInfo.m_sUrl = vValue.bstrVal;
			VariantClear(&vValue);
		}
	}
	int ret = theEBAppClient.EB_EditContact(&pContactInfo);
	return ret==0?S_OK:S_FALSE;
}
STDMETHODIMP CEBClientCore::EB_EditContact2(SEB_ContactInfo* newVal)
{
	if (newVal == NULL) return E_INVALIDARG;
	EB_ContactInfo pContactInfo;
	pContactInfo.m_nContactId = newVal->nContactId;
	pContactInfo.m_nContactType = newVal->nContactType;
	pContactInfo.m_nUGId = newVal->nUGId;
	pContactInfo.m_sGroupName = newVal->sGroupName;
	pContactInfo.m_nContactUserId = newVal->nContactUserId;
	pContactInfo.m_sContact = newVal->sContact;
	pContactInfo.m_sName = newVal->sName;
	pContactInfo.m_sPhone = newVal->sPhone;
	pContactInfo.m_sEmail = newVal->sEmail;
	pContactInfo.m_sAddress = newVal->sAddress;
	pContactInfo.m_sDescription = newVal->sDescription;
	pContactInfo.m_sCompany = newVal->sCompany;
	pContactInfo.m_sJobTitle = newVal->sJobTitle;
	pContactInfo.m_sTel = newVal->sTel;
	pContactInfo.m_sFax = newVal->sFax;
	pContactInfo.m_sUrl = newVal->sUrl;
	int ret = theEBAppClient.EB_EditContact(&pContactInfo);
	return ret==0?S_OK:S_FALSE;
}
STDMETHODIMP CEBClientCore::EB_DeleteContact(ULONGLONG nContactId,VARIANT_BOOL bDeleteDest)
{
	const int ret = theEBAppClient.EB_DeleteContact(nContactId,bDeleteDest==VARIANT_TRUE?true:false);
	return ret==0?S_OK:S_FALSE;
}
STDMETHODIMP CEBClientCore::EB_GetContactList(VARIANT* pVal)
{
	std::vector<EB_ContactInfo> pContactList;
	theEBAppClient.EB_GetContactList(pContactList);
	if (pContactList.empty())
	{
		return S_FALSE;
	}
	CComSafeArray<VARIANT> m_sa;
	for (size_t i=0; i<pContactList.size(); i++)
	{
		CIEB_ContactInfo * pIEBContactInfo = new CIEB_ContactInfo(pContactList[i]);
		CComPtr<IEB_ContactInfo> pOutInterface;
		pIEBContactInfo->QueryInterface(IID_IEB_ContactInfo,(void**)&pOutInterface);
		m_sa.Add(CComVariant(pOutInterface));
	}
	VariantInit(pVal);
	CComVariant var(m_sa);
	var.Detach(pVal);
	return S_OK;
}
STDMETHODIMP CEBClientCore::EB_GetContactInfo1(ULONGLONG nContactId, IEB_ContactInfo** pVal)
{
	CIEB_ContactInfo * pIEBContactInfo = new CIEB_ContactInfo();
	if (!theEBAppClient.EB_GetContactInfo1(nContactId,pIEBContactInfo))
	{
		delete pIEBContactInfo;
		return S_FALSE;
	}
	return pIEBContactInfo->QueryInterface(IID_IEB_ContactInfo,(void**)pVal);
}
STDMETHODIMP CEBClientCore::EB_GetContactInfo2(ULONGLONG nContactUserId, IEB_ContactInfo** pVal)
{
	CIEB_ContactInfo * pIEBContactInfo = new CIEB_ContactInfo();
	if (!theEBAppClient.EB_GetContactInfo2(nContactUserId,pIEBContactInfo))
	{
		delete pIEBContactInfo;
		return S_FALSE;
	}
	return pIEBContactInfo->QueryInterface(IID_IEB_ContactInfo,(void**)pVal);
}

//
//STDMETHODIMP CEBClientCore::EB_GetContactInfo2(BSTR sContactAccount, SEB_ContactInfo* pVal)
//{
//	EB_ContactInfo pContactInfo;
//	if (!theEBAppClient.EB_GetContactInfo(CEBString(sContactAccount).c_str(),&pContactInfo))
//	{
//		return S_FALSE;
//	}
//	pVal->sContact = pContactInfo.m_sContact.CopyBSTR();
//	pVal->sGroup = pContactInfo.m_sGroup.CopyBSTR();
//	pVal->sName = pContactInfo.m_sName.CopyBSTR();
//	pVal->sCompany = pContactInfo.m_sCompany.CopyBSTR();
//	pVal->sJobTitle = pContactInfo.m_sJobTitle.CopyBSTR();
//	pVal->sTel = pContactInfo.m_sTel.CopyBSTR();
//	pVal->sFax = pContactInfo.m_sFax.CopyBSTR();
//	pVal->sPhone = pContactInfo.m_sPhone.CopyBSTR();
//	pVal->sEmail = pContactInfo.m_sEmail.CopyBSTR();
//	pVal->sUrl = pContactInfo.m_sUrl.CopyBSTR();
//	pVal->sAddress = pContactInfo.m_sAddress.CopyBSTR();
//	pVal->sDescription = pContactInfo.m_sDescription.CopyBSTR();
//	return S_OK;
//}
STDMETHODIMP CEBClientCore::EB_IsMyContactAccount1(ULONGLONG nContactId, VARIANT_BOOL* pVal)
{
	bool ret = theEBAppClient.EB_IsMyContactAccount1(nContactId);
	*pVal = ret==true?VARIANT_TRUE:VARIANT_FALSE;
	return S_OK;
}
STDMETHODIMP CEBClientCore::EB_IsMyContactAccount2(ULONGLONG nContactUserId, VARIANT_BOOL* pVal)
{
	bool ret = theEBAppClient.EB_IsMyContactAccount2(nContactUserId);
	*pVal = ret==true?VARIANT_TRUE:VARIANT_FALSE;
	return S_OK;
}

STDMETHODIMP CEBClientCore::EB_LoadOrg(void)
{
	int ret = theEBAppClient.EB_LoadOrg();
	return ret==0?S_OK:S_FALSE;
}
STDMETHODIMP CEBClientCore::EB_LoadGroup(ULONGLONG sGroupCode, VARIANT_BOOL bLoadMember)
{
	int ret = theEBAppClient.EB_LoadGroup(sGroupCode,bLoadMember==VARIANT_TRUE?true:false);
	return ret==0?S_OK:S_FALSE;
}
STDMETHODIMP CEBClientCore::EB_GetEnterpriseInfo(ULONGLONG sEnterpriseCode, IEB_EnterpriseInfo** pVal)
{
	CIEB_EnterpriseInfo* pIEBEnterpriseInfo = new CIEB_EnterpriseInfo();
	if (!theEBAppClient.EB_GetEnterpriseInfo(pIEBEnterpriseInfo,sEnterpriseCode))
	{
		delete pIEBEnterpriseInfo;
		return S_FALSE;
	}
	return pIEBEnterpriseInfo->QueryInterface(IID_IEB_EnterpriseInfo,(void**)pVal);
}
STDMETHODIMP CEBClientCore::EB_GetEnterpriseInfo2(ULONGLONG sEnterpriseCode, SEB_EnterpriseInfo* pVal)
{
	EB_EnterpriseInfo pEnterpriseInfo;
	if (!theEBAppClient.EB_GetEnterpriseInfo(&pEnterpriseInfo,sEnterpriseCode))
	{
		return S_FALSE;
	}
	pVal->sEnterpriseCode = pEnterpriseInfo.m_sEnterpriseCode;
	pVal->sEnterpriseName = pEnterpriseInfo.m_sEnterpriseName.CopyBSTR();
	pVal->sDescription = pEnterpriseInfo.m_sDescription.CopyBSTR();
	pVal->sPhone = pEnterpriseInfo.m_sPhone.CopyBSTR();
	pVal->sFax = pEnterpriseInfo.m_sFax.CopyBSTR();
	pVal->sEmail = pEnterpriseInfo.m_sEmail.CopyBSTR();
	pVal->sUrl = pEnterpriseInfo.m_sUrl.CopyBSTR();
	pVal->sAddress = pEnterpriseInfo.m_sAddress.CopyBSTR();
	pVal->sCreateTime = pEnterpriseInfo.m_sCreateTime.CopyBSTR();
	pVal->nCreateUserId = pEnterpriseInfo.m_nCreateUserId;
	return S_OK;
}
STDMETHODIMP CEBClientCore::EB_IsEnterpriseUser(ULONGLONG sEnterpriseCode, VARIANT_BOOL* pVal)
{
	const bool ret = theEBAppClient.EB_IsEnterpriseUser(sEnterpriseCode);
	*pVal = ret?VARIANT_TRUE:VARIANT_FALSE;
	return S_OK;
}

STDMETHODIMP CEBClientCore::EB_GetEnterpriseName(ULONGLONG sEnterpriseCode, BSTR* pVal)
{
	CEBString sString;
	bool ret = theEBAppClient.EB_GetEnterpriseName(sString,sEnterpriseCode);
	*pVal = sString.CopyBSTR();
	return ret==true?S_OK:S_FALSE;
}
STDMETHODIMP CEBClientCore::EB_GetEnterpriseMemberSize(ULONGLONG sEnterpriseCode, LONG* pOutMemberSize, LONG* pOutOnlineSize)
{
	int nMemberSize = 0;
	int nOnlineSize = 0;
	theEBAppClient.EB_GetEnterpriseMemberSize(sEnterpriseCode,nMemberSize,nOnlineSize);
	*pOutMemberSize = nMemberSize;
	*pOutOnlineSize = nOnlineSize;
	return S_OK;
}
STDMETHODIMP CEBClientCore::EB_EditEnterprise(IDispatch* newVal)
{
	if (newVal == NULL) return E_INVALIDARG;
	USES_CONVERSION_EX;
	EB_EnterpriseInfo pEnterpriseInfo;
	CComPtr<IEB_EnterpriseInfo> pInterface;
	newVal->QueryInterface(IID_IEB_EnterpriseInfo,(void**)&pInterface);
	if (pInterface != NULL)
	{
		ULONGLONG sEnterpriseCode = 0;
		CComBSTR sEnterpriseName;
		CComBSTR sDescription;
		CComBSTR sPhone;
		CComBSTR sFax;
		CComBSTR sEmail;
		CComBSTR sUrl;
		CComBSTR sAddress;
		CComBSTR sCreateTime;
		ULONGLONG nCreateUserId;
		pInterface->get_EnterpriseCode(&sEnterpriseCode);
		pInterface->get_EnterpriseName(&sEnterpriseName);
		pInterface->get_Description(&sDescription);
		pInterface->get_Phone(&sPhone);
		pInterface->get_Fax(&sFax);
		pInterface->get_Email(&sEmail);
		pInterface->get_Url(&sUrl);
		pInterface->get_Address(&sAddress);
		pInterface->get_CreateTime(&sCreateTime);
		pInterface->get_CreateUserId(&nCreateUserId);
		pEnterpriseInfo.m_sEnterpriseCode = sEnterpriseCode;
		pEnterpriseInfo.m_sEnterpriseName = sEnterpriseName;
		pEnterpriseInfo.m_sDescription = sDescription;
		pEnterpriseInfo.m_sPhone = sPhone;
		pEnterpriseInfo.m_sFax = sFax;
		pEnterpriseInfo.m_sEmail = sEmail;
		pEnterpriseInfo.m_sUrl = sUrl;
		pEnterpriseInfo.m_sAddress = sAddress;
		pEnterpriseInfo.m_sCreateTime = sCreateTime;
		pEnterpriseInfo.m_nCreateUserId = nCreateUserId;
	}else
	{
		CComVariant vValue;
		HRESULT hr;
		hr = GetArrayVariantOfName(newVal,L"EnterpriseCode",&vValue);
		if (SUCCEEDED(hr))
		{
			pEnterpriseInfo.m_sEnterpriseCode = vValue.ullVal;
			VariantClear(&vValue);
		}
		hr = GetArrayVariantOfName(newVal,L"EnterpriseName",&vValue);
		if (SUCCEEDED(hr))
		{
			pEnterpriseInfo.m_sEnterpriseName = vValue.bstrVal;
			VariantClear(&vValue);
		}
		hr = GetArrayVariantOfName(newVal,L"Description",&vValue);
		if (SUCCEEDED(hr))
		{
			pEnterpriseInfo.m_sDescription = vValue.bstrVal;
			VariantClear(&vValue);
		}
		hr = GetArrayVariantOfName(newVal,L"Phone",&vValue);
		if (SUCCEEDED(hr))
		{
			pEnterpriseInfo.m_sPhone = vValue.bstrVal;
			VariantClear(&vValue);
		}
		hr = GetArrayVariantOfName(newVal,L"Fax",&vValue);
		if (SUCCEEDED(hr))
		{
			pEnterpriseInfo.m_sFax = vValue.bstrVal;
			VariantClear(&vValue);
		}
		hr = GetArrayVariantOfName(newVal,L"Email",&vValue);
		if (SUCCEEDED(hr))
		{
			pEnterpriseInfo.m_sEmail = vValue.bstrVal;
			VariantClear(&vValue);
		}
		hr = GetArrayVariantOfName(newVal,L"Url",&vValue);
		if (SUCCEEDED(hr))
		{
			pEnterpriseInfo.m_sUrl = vValue.bstrVal;
			VariantClear(&vValue);
		}
		hr = GetArrayVariantOfName(newVal,L"Address",&vValue);
		if (SUCCEEDED(hr))
		{
			pEnterpriseInfo.m_sAddress = vValue.bstrVal;
			VariantClear(&vValue);
		}
		hr = GetArrayVariantOfName(newVal,L"CreateTime",&vValue);
		if (SUCCEEDED(hr))
		{
			pEnterpriseInfo.m_sCreateTime = vValue.bstrVal;
			VariantClear(&vValue);
		}
		hr = GetArrayVariantOfName(newVal,L"CreateUserId",&vValue);
		if (SUCCEEDED(hr))
		{
			pEnterpriseInfo.m_nCreateUserId = vValue.ullVal;
			VariantClear(&vValue);
		}
	}
	int ret = theEBAppClient.EB_EditEnterprise(&pEnterpriseInfo);
	return ret==0?S_OK:S_FALSE;
}
STDMETHODIMP CEBClientCore::EB_EditEnterprise2(SEB_EnterpriseInfo* newVal)
{
	if (newVal == NULL) return E_INVALIDARG;
	EB_EnterpriseInfo pEnterpriseInfo;
	pEnterpriseInfo.m_sEnterpriseCode = newVal->sEnterpriseCode;
	pEnterpriseInfo.m_sEnterpriseName = newVal->sEnterpriseName;
	pEnterpriseInfo.m_sDescription = newVal->sDescription;
	pEnterpriseInfo.m_sPhone = newVal->sPhone;
	pEnterpriseInfo.m_sFax = newVal->sFax;
	pEnterpriseInfo.m_sEmail = newVal->sEmail;
	pEnterpriseInfo.m_sUrl = newVal->sUrl;
	pEnterpriseInfo.m_sAddress = newVal->sAddress;
	pEnterpriseInfo.m_sCreateTime = newVal->sCreateTime;
	pEnterpriseInfo.m_nCreateUserId = newVal->nCreateUserId;
	int ret = theEBAppClient.EB_EditEnterprise(&pEnterpriseInfo);
	return ret==0?S_OK:S_FALSE;
}
STDMETHODIMP CEBClientCore::EB_EditGroup(IDispatch* newVal)
{
	if (newVal == NULL) return E_INVALIDARG;
	USES_CONVERSION_EX;
	EB_GroupInfo pGroupInfo;
	CComPtr<IEB_GroupInfo> pInterface;
	newVal->QueryInterface(IID_IEB_EnterpriseInfo,(void**)&pInterface);
	if (pInterface != NULL)
	{
		ULONGLONG sEnterpriseCode = 0;
		ULONGLONG sGroupCode = 0;
		CComBSTR sGroupName;
		ULONGLONG sParentCode=0;
		CComBSTR sDescription;
		CComBSTR sPhone;
		CComBSTR sFax;
		CComBSTR sEmail;
		CComBSTR sUrl;
		CComBSTR sAddress;
		CComBSTR sCreateTime;
		ULONGLONG nCreateUserId;
		long nDisplayIndex = 0;
		SHORT nGroupType;
		pInterface->get_EnterpriseCode(&sEnterpriseCode);
		pInterface->get_GroupCode(&sGroupCode);
		pInterface->get_GroupName(&sGroupName);
		pInterface->get_ParentCode(&sParentCode);
		pInterface->get_Description(&sDescription);
		pInterface->get_Phone(&sPhone);
		pInterface->get_Fax(&sFax);
		pInterface->get_Email(&sEmail);
		pInterface->get_Url(&sUrl);
		pInterface->get_Address(&sAddress);
		pInterface->get_CreateTime(&sCreateTime);
		pInterface->get_CreateUserId(&nCreateUserId);
		pInterface->get_DisplayIndex(&nDisplayIndex);
		pInterface->get_GroupType(&nGroupType);
		pGroupInfo.m_sEnterpriseCode = sEnterpriseCode;
		pGroupInfo.m_sGroupCode = sGroupCode;
		pGroupInfo.m_sGroupName = sGroupName;
		pGroupInfo.m_sParentCode = sParentCode;
		pGroupInfo.m_sDescription = sDescription;
		pGroupInfo.m_sPhone = sPhone;
		pGroupInfo.m_sFax = sFax;
		pGroupInfo.m_sEmail = sEmail;
		pGroupInfo.m_sUrl = sUrl;
		pGroupInfo.m_sAddress = sAddress;
		pGroupInfo.m_sCreateTime = sCreateTime;
		pGroupInfo.m_nCreateUserId = nCreateUserId;
		pGroupInfo.m_nDisplayIndex = (int)nDisplayIndex;
		pGroupInfo.m_nGroupType = (EB_GROUP_TYPE)nGroupType;
	}else
	{
		CComVariant vValue;
		HRESULT hr;
		hr = GetArrayVariantOfName(newVal,L"EnterpriseCode",&vValue);
		if (SUCCEEDED(hr))
		{
			pGroupInfo.m_sEnterpriseCode = vValue.ullVal;
			VariantClear(&vValue);
		}
		hr = GetArrayVariantOfName(newVal,L"GroupCode",&vValue);
		if (SUCCEEDED(hr))
		{
			pGroupInfo.m_sGroupCode = vValue.ullVal;
			VariantClear(&vValue);
		}
		hr = GetArrayVariantOfName(newVal,L"GroupName",&vValue);
		if (SUCCEEDED(hr))
		{
			pGroupInfo.m_sGroupName = vValue.bstrVal;
			VariantClear(&vValue);
		}
		hr = GetArrayVariantOfName(newVal,L"ParentCode",&vValue);
		if (SUCCEEDED(hr))
		{
			pGroupInfo.m_sParentCode = vValue.ullVal;
			VariantClear(&vValue);
		}
		hr = GetArrayVariantOfName(newVal,L"Description",&vValue);
		if (SUCCEEDED(hr))
		{
			pGroupInfo.m_sDescription = vValue.bstrVal;
			VariantClear(&vValue);
		}
		hr = GetArrayVariantOfName(newVal,L"Phone",&vValue);
		if (SUCCEEDED(hr))
		{
			pGroupInfo.m_sPhone = vValue.bstrVal;
			VariantClear(&vValue);
		}
		hr = GetArrayVariantOfName(newVal,L"Fax",&vValue);
		if (SUCCEEDED(hr))
		{
			pGroupInfo.m_sFax = vValue.bstrVal;
			VariantClear(&vValue);
		}
		hr = GetArrayVariantOfName(newVal,L"Email",&vValue);
		if (SUCCEEDED(hr))
		{
			pGroupInfo.m_sEmail = vValue.bstrVal;
			VariantClear(&vValue);
		}
		hr = GetArrayVariantOfName(newVal,L"Url",&vValue);
		if (SUCCEEDED(hr))
		{
			pGroupInfo.m_sUrl = vValue.bstrVal;
			VariantClear(&vValue);
		}
		hr = GetArrayVariantOfName(newVal,L"Address",&vValue);
		if (SUCCEEDED(hr))
		{
			pGroupInfo.m_sAddress = vValue.bstrVal;
			VariantClear(&vValue);
		}
		hr = GetArrayVariantOfName(newVal,L"CreateTime",&vValue);
		if (SUCCEEDED(hr))
		{
			pGroupInfo.m_sCreateTime = vValue.bstrVal;
			VariantClear(&vValue);
		}
		hr = GetArrayVariantOfName(newVal,L"CreateUserId",&vValue);
		if (SUCCEEDED(hr))
		{
			pGroupInfo.m_nCreateUserId = vValue.ullVal;
			VariantClear(&vValue);
		}
		hr = GetArrayVariantOfName(newVal,L"GroupType",&vValue);
		if (SUCCEEDED(hr))
		{
			pGroupInfo.m_nGroupType = (EB_GROUP_TYPE)vValue.iVal;
			VariantClear(&vValue);
		}
		hr = GetArrayVariantOfName(newVal,L"DisplayIndex",&vValue);
		if (SUCCEEDED(hr))
		{
			pGroupInfo.m_nDisplayIndex = (int)vValue.lVal;
			VariantClear(&vValue);
		}
	}
	int ret = theEBAppClient.EB_EditGroup(&pGroupInfo);
	return ret==0?S_OK:S_FALSE;
}
STDMETHODIMP CEBClientCore::EB_EditGroup2(SEB_GroupInfo* newVal)
{
	if (newVal == NULL) return E_INVALIDARG;
	EB_GroupInfo pGroupInfo;
	pGroupInfo.m_sEnterpriseCode = newVal->sEnterpriseCode;
	pGroupInfo.m_sGroupCode = newVal->sGroupCode;
	pGroupInfo.m_sGroupName = newVal->sGroupName;
	pGroupInfo.m_sParentCode = newVal->sParentCode;
	pGroupInfo.m_sDescription = newVal->sDescription;
	pGroupInfo.m_sPhone = newVal->sPhone;
	pGroupInfo.m_sFax = newVal->sFax;
	pGroupInfo.m_sEmail = newVal->sEmail;
	pGroupInfo.m_sUrl = newVal->sUrl;
	pGroupInfo.m_sAddress = newVal->sAddress;
	pGroupInfo.m_sCreateTime = newVal->sCreateTime;
	pGroupInfo.m_nCreateUserId = newVal->nCreateUserId;
	pGroupInfo.m_nGroupType = (EB_GROUP_TYPE)newVal->nGroupType;
	pGroupInfo.m_nDisplayIndex = newVal->nDisplayIndex;
	int ret = theEBAppClient.EB_EditGroup(&pGroupInfo);
	return ret==0?S_OK:S_FALSE;
}
STDMETHODIMP CEBClientCore::EB_DeleteGroup(ULONGLONG sGroupCode)
{
	int ret = theEBAppClient.EB_DeleteGroup(sGroupCode);
	return ret==0?S_OK:S_FALSE;
}
STDMETHODIMP CEBClientCore::EB_ExitGroup(ULONGLONG sGroupCode)
{
	int ret = theEBAppClient.EB_ExitGroup(sGroupCode);
	return ret==0?S_OK:S_FALSE;
}
STDMETHODIMP CEBClientCore::EB_SetMyGroupHeadFile(ULONGLONG sGroupCode, BSTR sImagePath)
{
	int ret = theEBAppClient.EB_SetMyGroupHeadFile(sGroupCode,CEBString(sImagePath).c_str());
	return ret==0?S_OK:S_FALSE;
}
STDMETHODIMP CEBClientCore::EB_SetMyGroupHeadRes(ULONGLONG sGroupCode, ULONGLONG sResId)
{
	int ret = theEBAppClient.EB_SetMyGroupHeadRes(sGroupCode,sResId);
	return ret==0?S_OK:S_FALSE;
}
STDMETHODIMP CEBClientCore::EB_GetGroupInfo(ULONGLONG sGroupCode, IEB_GroupInfo** pVal)
{
	CIEB_GroupInfo* pIEBGroupInfo = new CIEB_GroupInfo();
	if (!theEBAppClient.EB_GetGroupInfo(sGroupCode,pIEBGroupInfo))
	{
		delete pIEBGroupInfo;
		return S_FALSE;
	}
	return pIEBGroupInfo->QueryInterface(IID_IEB_GroupInfo,(void**)pVal);
}
STDMETHODIMP CEBClientCore::EB_GetGroupInfo2(ULONGLONG sGroupCode, SEB_GroupInfo* pVal)
{
	EB_GroupInfo pGroupInfo;
	if (!theEBAppClient.EB_GetGroupInfo(sGroupCode,&pGroupInfo))
	{
		return S_FALSE;
	}
	pVal->sEnterpriseCode = pGroupInfo.m_sEnterpriseCode;
	pVal->sGroupCode = pGroupInfo.m_sGroupCode;
	pVal->sGroupName = pGroupInfo.m_sGroupName.CopyBSTR();
	pVal->sParentCode = pGroupInfo.m_sParentCode;
	pVal->sDescription = pGroupInfo.m_sDescription.CopyBSTR();
	pVal->sPhone = pGroupInfo.m_sPhone.CopyBSTR();
	pVal->sFax = pGroupInfo.m_sFax.CopyBSTR();
	pVal->sEmail = pGroupInfo.m_sEmail.CopyBSTR();
	pVal->sUrl = pGroupInfo.m_sUrl.CopyBSTR();
	pVal->sAddress = pGroupInfo.m_sAddress.CopyBSTR();
	pVal->sCreateTime = pGroupInfo.m_sCreateTime.CopyBSTR();
	pVal->nCreateUserId = pGroupInfo.m_nCreateUserId;
	pVal->nGroupType = pGroupInfo.m_nGroupType;
	return S_OK;
}
STDMETHODIMP CEBClientCore::EB_GetGroupName(ULONGLONG sGroupCode, BSTR* pVal)
{
	CEBString sGroupName;
	bool ret = theEBAppClient.EB_GetGroupName(sGroupCode,sGroupName);
	*pVal = sGroupName.CopyBSTR();
	return ret==true?S_OK:S_FALSE;
}
STDMETHODIMP CEBClientCore::EB_GetGroupCreator(ULONGLONG sGroupCode, ULONGLONG* pVal)
{
	eb::bigint nCreateUserId = 0;
	bool ret = theEBAppClient.EB_GetGroupCreator(sGroupCode,nCreateUserId);
	*pVal = nCreateUserId;
	return ret==true?S_OK:S_FALSE;
}
STDMETHODIMP CEBClientCore::EB_GetGroupType(ULONGLONG sGroupCode, SHORT* pVal)
{
	EB_GROUP_TYPE nGroupType = EB_GROUP_TYPE_DEPARTMENT;
	bool ret = theEBAppClient.EB_GetGroupType(sGroupCode,nGroupType);
	if (ret)
	{
		*pVal = nGroupType;
		return S_OK;
	}
	*pVal = -1;
	return S_FALSE;
}
STDMETHODIMP CEBClientCore::EB_GetGroupMemberInfoList(ULONGLONG sGroupCode, VARIANT* pVal)
{
	std::vector<EB_MemberInfo> pMemberList;
	theEBAppClient.EB_GetGroupMemberInfoList(sGroupCode,pMemberList);
	if (pMemberList.empty())
	{
		return S_FALSE;
	}
	CComSafeArray<VARIANT> m_sa;
	for (size_t i=0; i<pMemberList.size(); i++)
	{
		CIEB_MemberInfo * pIEBMemberInfo = new CIEB_MemberInfo(pMemberList[i]);
		CComPtr<IEB_MemberInfo> pOutInterface;
		pIEBMemberInfo->QueryInterface(IID_IEB_MemberInfo,(void**)&pOutInterface);
		m_sa.Add(CComVariant(pOutInterface));
	}
	VariantInit(pVal);
	CComVariant var(m_sa);
	var.Detach(pVal);
	return S_OK;
}
STDMETHODIMP CEBClientCore::EB_GetGroupMemberUserIdList(ULONGLONG sGroupCode, VARIANT* pVal)
{
	std::vector<eb::bigint> pMemberList;
	theEBAppClient.EB_GetGroupMemberUserIdList(sGroupCode,pMemberList);
	if (pMemberList.empty())
	{
		return S_FALSE;
	}
	CComSafeArray<VARIANT> m_sa;
	for (size_t i=0; i<pMemberList.size(); i++)
	{
		m_sa.Add(CComVariant(ULONGLONG(pMemberList[i])));
	}
	VariantInit(pVal);
	CComVariant var(m_sa);
	var.Detach(pVal);
	return S_OK;
}
STDMETHODIMP CEBClientCore::EB_GetGroupMemberCodeList(ULONGLONG sGroupCode, VARIANT* pVal)
{
	std::vector<eb::bigint> pMemberList;
	theEBAppClient.EB_GetGroupMemberCodeList(sGroupCode,pMemberList);
	if (pMemberList.empty())
	{
		return S_FALSE;
	}
	CComSafeArray<VARIANT> m_sa;
	for (size_t i=0; i<pMemberList.size(); i++)
	{
		m_sa.Add(CComVariant(ULONGLONG(pMemberList[i])));
	}
	VariantInit(pVal);
	CComVariant var(m_sa);
	var.Detach(pVal);
	return S_OK;
}
STDMETHODIMP CEBClientCore::EB_GetGroupMemberAccountList(ULONGLONG sGroupCode, VARIANT* pVal)
{
	std::vector<tstring> pMemberList;
	theEBAppClient.EB_GetGroupMemberAccountList(sGroupCode,pMemberList);
	if (pMemberList.empty())
	{
		return S_FALSE;
	}
	CComSafeArray<VARIANT> m_sa;
	for (size_t i=0; i<pMemberList.size(); i++)
	{
		m_sa.Add(CComVariant(CEBString(pMemberList[i]).CopyBSTR()));
	}
	VariantInit(pVal);
	CComVariant var(m_sa);
	var.Detach(pVal);
	return S_OK;
}
STDMETHODIMP CEBClientCore::EB_GetGroupMemberSize(ULONGLONG sGroupCode, SHORT nGetType, ULONG* pVal)
{
	*pVal = theEBAppClient.EB_GetGroupMemberSize(sGroupCode, (int)nGetType);
	return S_OK;
}
STDMETHODIMP CEBClientCore::EB_GetGroupOnlineSize(ULONGLONG sGroupCode, SHORT nGetType, ULONG* pVal)
{
	*pVal = theEBAppClient.EB_GetGroupOnlineSize(sGroupCode, (int)nGetType);
	return S_OK;
}

STDMETHODIMP CEBClientCore::EB_IsMyGroup(ULONGLONG sGroupCode, VARIANT_BOOL* pVal)
{
	bool ret = theEBAppClient.EB_IsMyGroup(sGroupCode);
	*pVal = ret==true?VARIANT_TRUE:VARIANT_FALSE;
	return S_OK;
}

STDMETHODIMP CEBClientCore::EB_EditMember(IDispatch* newVal)
{
	if (newVal == NULL) return E_INVALIDARG;
	USES_CONVERSION_EX;
	EB_MemberInfo pMemberInfo;
	CComPtr<IEB_MemberInfo> pInterface;
	newVal->QueryInterface(IID_IEB_MemberInfo,(void**)&pInterface);
	if (pInterface != NULL)
	{
		ULONGLONG sMemberCode = 0;
		ULONGLONG sGroupCode = 0;
		ULONGLONG nMemberUserId = 0;
		CComBSTR sMemberAccount;
		CComBSTR sUserName;
		CComBSTR sJobTitle;
		LONG nJobPosition = 0;
		CComBSTR sCellPhone;
		CComBSTR sFax;
		CComBSTR sWorkPhone;
		CComBSTR sEmail;
		CComBSTR sAddress;
		CComBSTR sDescription;
		//LONG nManagerLevel;
		SHORT nGender = 0;
		LONG nBirthday = 0;
		ULONGLONG nCSId = 0;
		LONG nCSExt = 0;
		//LONG nLineState;
		//CComBSTR sHeadResourceId;
		//CComBSTR sHeadResourceFile;
		pInterface->get_MemberCode(&sMemberCode);
		pInterface->get_GroupCode(&sGroupCode);
		pInterface->get_MemberUserId(&nMemberUserId);
		pInterface->get_MemberAccount(&sMemberAccount);
		pInterface->get_UserName(&sUserName);
		pInterface->get_JobTitle(&sJobTitle);
		pInterface->get_JobPosition(&nJobPosition);
		pInterface->get_CellPhone(&sCellPhone);
		pInterface->get_Fax(&sFax);
		pInterface->get_WorkPhone(&sWorkPhone);
		pInterface->get_Email(&sEmail);
		pInterface->get_Address(&sAddress);
		pInterface->get_Description(&sDescription);
		//pInterface->get_ManagerLevel(&nManagerLevel);
		pInterface->get_Gender(&nGender);
		pInterface->get_Birthday(&nBirthday);
		pInterface->get_CSId(&nCSId);
		pInterface->get_CSExt(&nCSExt);
		//pInterface->get_LineState(&nLineState);
		//pInterface->get_HeadResourceId(&sHeadResourceId);
		//pInterface->get_HeadResourceFile(&sHeadResourceFile);
		pMemberInfo.m_sMemberCode = sMemberCode;
		pMemberInfo.m_sGroupCode = sGroupCode;
		pMemberInfo.m_nMemberUserId = nMemberUserId;
		pMemberInfo.m_sMemberAccount = sMemberAccount;
		pMemberInfo.m_sUserName = sUserName;
		pMemberInfo.m_sJobTitle = sJobTitle;
		pMemberInfo.m_nJobPosition = nJobPosition;
		pMemberInfo.m_sCellPhone = sCellPhone;
		pMemberInfo.m_sFax = sFax;
		pMemberInfo.m_sWorkPhone = sWorkPhone;
		pMemberInfo.m_sEmail = sEmail;
		pMemberInfo.m_sAddress = sAddress;
		pMemberInfo.m_sDescription = sDescription;
		//pMemberInfo.m_nManagerLevel = nManagerLevel;
		pMemberInfo.m_nGender = (EB_GENDER_TYPE)nGender;
		pMemberInfo.m_nBirthday = nBirthday;
		pMemberInfo.m_nCSId = nCSId;
		pMemberInfo.m_nCSExt = nCSExt;
		//pMemberInfo.m_nLineState = nLineState;
		//pMemberInfo.m_sHeadResourceId = sHeadResourceId;
		//pMemberInfo.m_sHeadResourceFile = sHeadResourceFile;
	}else
	{
		CComVariant vValue;
		HRESULT hr;
		hr = GetArrayVariantOfName(newVal,L"MemberCoce",&vValue);
		if (SUCCEEDED(hr))
		{
			pMemberInfo.m_sMemberCode = vValue.ullVal;
			VariantClear(&vValue);
		}
		hr = GetArrayVariantOfName(newVal,L"GroupCode",&vValue);
		if (SUCCEEDED(hr))
		{
			pMemberInfo.m_sGroupCode = vValue.ullVal;
			VariantClear(&vValue);
		}
		hr = GetArrayVariantOfName(newVal,L"MemberUserId",&vValue);
		if (SUCCEEDED(hr))
		{
			pMemberInfo.m_nMemberUserId = vValue.ullVal;
			VariantClear(&vValue);
		}
		hr = GetArrayVariantOfName(newVal,L"MemberAccount",&vValue);
		if (SUCCEEDED(hr))
		{
			pMemberInfo.m_sMemberAccount = vValue.bstrVal;
			VariantClear(&vValue);
		}
		hr = GetArrayVariantOfName(newVal,L"UserName",&vValue);
		if (SUCCEEDED(hr))
		{
			pMemberInfo.m_sUserName = vValue.bstrVal;
			VariantClear(&vValue);
		}
		hr = GetArrayVariantOfName(newVal,L"JobTitle",&vValue);
		if (SUCCEEDED(hr))
		{
			pMemberInfo.m_sJobTitle = vValue.bstrVal;
			VariantClear(&vValue);
		}
		hr = GetArrayVariantOfName(newVal,L"JobPosition",&vValue);
		if (SUCCEEDED(hr))
		{
			pMemberInfo.m_nJobPosition = vValue.lVal;
			VariantClear(&vValue);
		}
		hr = GetArrayVariantOfName(newVal,L"CellPhone",&vValue);
		if (SUCCEEDED(hr))
		{
			pMemberInfo.m_sCellPhone = vValue.bstrVal;
			VariantClear(&vValue);
		}
		hr = GetArrayVariantOfName(newVal,L"Fax",&vValue);
		if (SUCCEEDED(hr))
		{
			pMemberInfo.m_sFax = vValue.bstrVal;
			VariantClear(&vValue);
		}
		hr = GetArrayVariantOfName(newVal,L"WorkPhone",&vValue);
		if (SUCCEEDED(hr))
		{
			pMemberInfo.m_sWorkPhone = vValue.bstrVal;
			VariantClear(&vValue);
		}
		hr = GetArrayVariantOfName(newVal,L"Email",&vValue);
		if (SUCCEEDED(hr))
		{
			pMemberInfo.m_sEmail = vValue.bstrVal;
			VariantClear(&vValue);
		}
		hr = GetArrayVariantOfName(newVal,L"Address",&vValue);
		if (SUCCEEDED(hr))
		{
			pMemberInfo.m_sAddress = vValue.bstrVal;
			VariantClear(&vValue);
		}
		hr = GetArrayVariantOfName(newVal,L"Description",&vValue);
		if (SUCCEEDED(hr))
		{
			pMemberInfo.m_sDescription = vValue.bstrVal;
			VariantClear(&vValue);
		}
		//hr = GetArrayVariantOfName(newVal,L"ManagerLevel",&vValue);
		//if (SUCCEEDED(hr))
		//{
		//	pMemberInfo.m_nManagerLevel = vValue.lVal;
		//	VariantClear(&vValue);
		//}
		hr = GetArrayVariantOfName(newVal,L"Gender",&vValue);
		if (SUCCEEDED(hr))
		{
			pMemberInfo.m_nGender = (EB_GENDER_TYPE)vValue.iVal;
			VariantClear(&vValue);
		}
		hr = GetArrayVariantOfName(newVal,L"Birthday",&vValue);
		if (SUCCEEDED(hr))
		{
			pMemberInfo.m_nBirthday = vValue.lVal;
			VariantClear(&vValue);
		}
		hr = GetArrayVariantOfName(newVal,L"CSId",&vValue);
		if (SUCCEEDED(hr))
		{
			pMemberInfo.m_nCSId = vValue.ullVal;
			VariantClear(&vValue);
		}
		hr = GetArrayVariantOfName(newVal,L"CSExt",&vValue);
		if (SUCCEEDED(hr))
		{
			pMemberInfo.m_nCSExt = vValue.lVal;
			VariantClear(&vValue);
		}
		//hr = GetArrayVariantOfName(newVal,L"LineState",&vValue);
		//if (SUCCEEDED(hr))
		//{
		//	pMemberInfo.m_nLineState = vValue.lVal;
		//	VariantClear(&vValue);
		//}
		//hr = GetArrayVariantOfName(newVal,L"HeadResourceId",&vValue);
		//if (SUCCEEDED(hr))
		//{
		//	pMemberInfo.m_sHeadResourceId = vValue.bstrVal;
		//	VariantClear(&vValue);
		//}
		//hr = GetArrayVariantOfName(newVal,L"HeadResourceFile",&vValue);
		//if (SUCCEEDED(hr))
		//{
		//	pMemberInfo.m_sHeadResourceFile = vValue.bstrVal;
		//	VariantClear(&vValue);
		//}
	}
	int ret = theEBAppClient.EB_EditMember(&pMemberInfo);
	return ret==0?S_OK:S_FALSE;
}
STDMETHODIMP CEBClientCore::EB_EditMember2(SEB_MemberInfo* newVal)
{
	if (newVal == NULL) return E_INVALIDARG;
	EB_MemberInfo pMemberInfo;
	pMemberInfo.m_sMemberCode = newVal->sMemberCode;
	pMemberInfo.m_sGroupCode = newVal->sGroupCode;
	pMemberInfo.m_nMemberUserId = newVal->nMemberUserId;
	pMemberInfo.m_sMemberAccount = newVal->sMemberAccount;
	pMemberInfo.m_sUserName = newVal->sUserName;
	pMemberInfo.m_sJobTitle = newVal->sJobTitle;
	pMemberInfo.m_nJobPosition = newVal->nJobPosition;
	pMemberInfo.m_sCellPhone = newVal->sCellPhone;
	pMemberInfo.m_sFax = newVal->sFax;
	pMemberInfo.m_sWorkPhone = newVal->sWorkPhone;
	pMemberInfo.m_sEmail = newVal->sEmail;
	pMemberInfo.m_sAddress = newVal->sAddress;
	pMemberInfo.m_sDescription = newVal->sDescription;
	//pMemberInfo.m_nManagerLevel = newVal->nManagerLevel;
	pMemberInfo.m_nGender = (EB_GENDER_TYPE)newVal->nGender;
	pMemberInfo.m_nBirthday = newVal->nBirthday;
	pMemberInfo.m_nCSId = newVal->nCSId;
	pMemberInfo.m_nCSExt = newVal->nCSExt;
	//pMemberInfo.m_nLineState = newVal->nLineState;
	//pMemberInfo.m_sHeadResourceId = newVal->sHeadResourceId;
	//pMemberInfo.m_sHeadResourceFile = newVal->sHeadResourceFile;
	int ret = theEBAppClient.EB_EditMember(&pMemberInfo);
	return ret==0?S_OK:S_FALSE;
}

STDMETHODIMP CEBClientCore::EB_DeleteMember(ULONGLONG sMemberCode, VARIANT_BOOL bDeleteAccount)
{
	int ret = theEBAppClient.EB_DeleteMember(sMemberCode,bDeleteAccount?true:false);
	return ret==0?S_OK:S_FALSE;
}

STDMETHODIMP CEBClientCore::EB_GetMemberInfoByUserId(ULONGLONG sGroupCode, ULONGLONG nUserId, IEB_MemberInfo** pVal)
{
	CIEB_MemberInfo * pIEBMemberInfo = new CIEB_MemberInfo();
	if (!theEBAppClient.EB_GetMemberInfoByUserId(pIEBMemberInfo,sGroupCode,nUserId))
	{
		delete pIEBMemberInfo;
		return S_FALSE;
	}
	return pIEBMemberInfo->QueryInterface(IID_IEB_MemberInfo,(void**)pVal);
}
STDMETHODIMP CEBClientCore::EB_GetMemberInfoByUserId2(ULONGLONG nUserId, IEB_MemberInfo** pVal)
{
	CIEB_MemberInfo * pIEBMemberInfo = new CIEB_MemberInfo();
	if (!theEBAppClient.EB_GetMemberInfoByUserId2(pIEBMemberInfo,nUserId))
	{
		delete pIEBMemberInfo;
		return S_FALSE;
	}
	return pIEBMemberInfo->QueryInterface(IID_IEB_MemberInfo,(void**)pVal);
}

STDMETHODIMP CEBClientCore::EB_GetMemberInfoByAccount(ULONGLONG sGroupCode, BSTR sAccount, IEB_MemberInfo** pVal)
{
	CIEB_MemberInfo * pIEBMemberInfo = new CIEB_MemberInfo();
	if (!theEBAppClient.EB_GetMemberInfoByAccount(pIEBMemberInfo,sGroupCode,CEBString(sAccount).c_str()))
	{
		delete pIEBMemberInfo;
		return S_FALSE;
	}
	return pIEBMemberInfo->QueryInterface(IID_IEB_MemberInfo,(void**)pVal);
}
STDMETHODIMP CEBClientCore::EB_GetMemberInfoByAccount2(BSTR sAccount, IEB_MemberInfo** pVal)
{
	CIEB_MemberInfo * pIEBMemberInfo = new CIEB_MemberInfo();
	if (!theEBAppClient.EB_GetMemberInfoByAccount2(pIEBMemberInfo,CEBString(sAccount).c_str()))
	{
		delete pIEBMemberInfo;
		return S_FALSE;
	}
	return pIEBMemberInfo->QueryInterface(IID_IEB_MemberInfo,(void**)pVal);
}
//STDMETHODIMP CEBClientCore::EB_GetMemberInfoByAccount2(ULONGLONG sGroupCode, BSTR sAccount, SEB_MemberInfo* pVal)
//{
//	EB_MemberInfo pMemberInfo;
//	if (!theEBAppClient.EB_GetMemberInfoByAccount(&pMemberInfo,sGroupCode,CEBString(sAccount).c_str()))
//	{
//		return S_FALSE;
//	}
//	pVal->sMemberCode = pMemberInfo.m_sMemberCode;
//	pVal->sGroupCode = pMemberInfo.m_sGroupCode;
//	pVal->nMemberUserId = pMemberInfo.m_nMemberUserId;
//	pVal->sMemberAccount = pMemberInfo.m_sMemberAccount.CopyBSTR();
//	pVal->sUserName = pMemberInfo.m_sUserName.CopyBSTR();
//	pVal->sJobTitle = pMemberInfo.m_sJobTitle.CopyBSTR();
//	pVal->nJobPosition = pMemberInfo.m_nJobPosition;
//	pVal->sCellPhone = pMemberInfo.m_sCellPhone.CopyBSTR();
//	pVal->sFax = pMemberInfo.m_sFax.CopyBSTR();
//	pVal->sWorkPhone = pMemberInfo.m_sWorkPhone.CopyBSTR();
//	pVal->sEmail = pMemberInfo.m_sEmail.CopyBSTR();
//	pVal->sAddress = pMemberInfo.m_sAddress.CopyBSTR();
//	pVal->sDescription = pMemberInfo.m_sDescription.CopyBSTR();
//	pVal->nManagerLevel = pMemberInfo.m_nManagerLevel;
//	pVal->nGender = pMemberInfo.m_nGender;
//	pVal->nBirthday = pMemberInfo.m_nBirthday;
//	pVal->nLineState = pMemberInfo.m_nLineState;
//	pVal->sHeadResourceId = pMemberInfo.m_sHeadResourceId;
//	pVal->sHeadResourceFile = pMemberInfo.m_sHeadResourceFile.CopyBSTR();
//	return S_OK;
//}
STDMETHODIMP CEBClientCore::EB_GetMemberInfoByMemberCode(ULONGLONG sMemberCode, IEB_MemberInfo** pVal)
{
	CIEB_MemberInfo * pIEBMemberInfo = new CIEB_MemberInfo();
	if (!theEBAppClient.EB_GetMemberInfoByMemberCode(pIEBMemberInfo,sMemberCode))
	{
		delete pIEBMemberInfo;
		return S_FALSE;
	}
	return pIEBMemberInfo->QueryInterface(IID_IEB_MemberInfo,(void**)pVal);
}
STDMETHODIMP CEBClientCore::EB_GetMemberInfoByMemberCode2(ULONGLONG sMemberCode, SEB_MemberInfo* pVal)
{
	EB_MemberInfo pMemberInfo;
	if (!theEBAppClient.EB_GetMemberInfoByMemberCode(&pMemberInfo,sMemberCode))
	{
		return S_FALSE;
	}
	pVal->sMemberCode = pMemberInfo.m_sMemberCode;
	pVal->sGroupCode = pMemberInfo.m_sGroupCode;
	pVal->nMemberUserId = pMemberInfo.m_nMemberUserId;
	pVal->sMemberAccount = pMemberInfo.m_sMemberAccount.CopyBSTR();
	pVal->sUserName = pMemberInfo.m_sUserName.CopyBSTR();
	pVal->sJobTitle = pMemberInfo.m_sJobTitle.CopyBSTR();
	pVal->nJobPosition = pMemberInfo.m_nJobPosition;
	pVal->sCellPhone = pMemberInfo.m_sCellPhone.CopyBSTR();
	pVal->sFax = pMemberInfo.m_sFax.CopyBSTR();
	pVal->sWorkPhone = pMemberInfo.m_sWorkPhone.CopyBSTR();
	pVal->sEmail = pMemberInfo.m_sEmail.CopyBSTR();
	pVal->sAddress = pMemberInfo.m_sAddress.CopyBSTR();
	pVal->sDescription = pMemberInfo.m_sDescription.CopyBSTR();
	pVal->nManagerLevel = pMemberInfo.m_nManagerLevel;
	pVal->nGender = pMemberInfo.m_nGender;
	pVal->nBirthday = pMemberInfo.m_nBirthday;
	pVal->nLineState = pMemberInfo.m_nLineState;
	pVal->sHeadResourceId = pMemberInfo.m_sHeadResourceId;
	pVal->sHeadResourceFile = pMemberInfo.m_sHeadResourceFile.CopyBSTR();
	return S_OK;
}
STDMETHODIMP CEBClientCore::EB_GetMemberNameByUserId(ULONGLONG sGroupCode, ULONGLONG nMemberUserId, BSTR* pVal)
{
	CEBString sMemberName;
	bool ret = theEBAppClient.EB_GetMemberNameByUserId(sGroupCode,nMemberUserId,sMemberName);
	*pVal = sMemberName.CopyBSTR();
	return ret==true?S_OK:S_FALSE;
}
STDMETHODIMP CEBClientCore::EB_GetMemberNameByMemberCode(ULONGLONG sMemberCode, BSTR* pVal)
{
	CEBString sMemberName;
	bool ret = theEBAppClient.EB_GetMemberNameByMemberCode(sMemberCode,sMemberName);
	*pVal = sMemberName.CopyBSTR();
	return ret==true?S_OK:S_FALSE;
}
STDMETHODIMP CEBClientCore::EB_GetMemberLineState(ULONGLONG sMemberCode, SHORT* pVal)
{
	EB_USER_LINE_STATE pOutLineState = EB_LINE_STATE_UNKNOWN;
	bool ret = theEBAppClient.EB_GetMemberLineState(sMemberCode,pOutLineState);
	*pVal = pOutLineState;
	return ret==true?S_OK:S_FALSE;
}
STDMETHODIMP CEBClientCore::EB_GetMyMemberInfo(ULONGLONG sGroupCode, IEB_MemberInfo** pVal)
{
	CIEB_MemberInfo * pIEBMemberInfo = new CIEB_MemberInfo();
	if (!theEBAppClient.EB_GetMyMemberInfo(pIEBMemberInfo,sGroupCode))
	{
		delete pIEBMemberInfo;
		return S_FALSE;
	}
	return pIEBMemberInfo->QueryInterface(IID_IEB_MemberInfo,(void**)pVal);
}
STDMETHODIMP CEBClientCore::EB_GetMyMemberInfo2(ULONGLONG sGroupCode, SEB_MemberInfo* pVal)
{
	EB_MemberInfo pMemberInfo;
	if (!theEBAppClient.EB_GetMyMemberInfo(&pMemberInfo,sGroupCode))
	{
		return S_FALSE;
	}
	pVal->sMemberCode = pMemberInfo.m_sMemberCode;
	pVal->sGroupCode = pMemberInfo.m_sGroupCode;
	pVal->nMemberUserId = pMemberInfo.m_nMemberUserId;
	pVal->sMemberAccount = pMemberInfo.m_sMemberAccount.CopyBSTR();
	pVal->sUserName = pMemberInfo.m_sUserName.CopyBSTR();
	pVal->sJobTitle = pMemberInfo.m_sJobTitle.CopyBSTR();
	pVal->nJobPosition = pMemberInfo.m_nJobPosition;
	pVal->sCellPhone = pMemberInfo.m_sCellPhone.CopyBSTR();
	pVal->sFax = pMemberInfo.m_sFax.CopyBSTR();
	pVal->sWorkPhone = pMemberInfo.m_sWorkPhone.CopyBSTR();
	pVal->sEmail = pMemberInfo.m_sEmail.CopyBSTR();
	pVal->sAddress = pMemberInfo.m_sAddress.CopyBSTR();
	pVal->sDescription = pMemberInfo.m_sDescription.CopyBSTR();
	pVal->nManagerLevel = pMemberInfo.m_nManagerLevel;
	pVal->nGender = pMemberInfo.m_nGender;
	pVal->nBirthday = pMemberInfo.m_nBirthday;
	pVal->nLineState = pMemberInfo.m_nLineState;
	pVal->sHeadResourceId = pMemberInfo.m_sHeadResourceId;
	pVal->sHeadResourceFile = pMemberInfo.m_sHeadResourceFile.CopyBSTR();
	return S_OK;
}

STDMETHODIMP CEBClientCore::EB_GetMyMemberList(VARIANT* pVal)
{
	std::vector<EB_MemberInfo> pMemberList;
	theEBAppClient.EB_GetMyMemberList(pMemberList);
	if (pMemberList.empty())
	{
		return S_FALSE;
	}
	CComSafeArray<VARIANT> m_sa;
	for (size_t i=0; i<pMemberList.size(); i++)
	{
		CIEB_MemberInfo * pIEBMemberInfo = new CIEB_MemberInfo(pMemberList[i]);
		CComPtr<IEB_MemberInfo> pOutInterface;
		pIEBMemberInfo->QueryInterface(IID_IEB_MemberInfo,(void**)&pOutInterface);
		m_sa.Add(CComVariant(pOutInterface));
	}
	VariantInit(pVal);
	CComVariant var(m_sa);
	var.Detach(pVal);
	return S_OK;
}

STDMETHODIMP CEBClientCore::EB_IsExistMemberByUserId(ULONGLONG sGroupCode, ULONGLONG nMemberUserId, VARIANT_BOOL* pVal)
{
	bool ret = theEBAppClient.EB_IsExistMemberByUserId(sGroupCode,nMemberUserId);
	*pVal = ret==true?VARIANT_TRUE:VARIANT_FALSE;
	return S_OK;
}
STDMETHODIMP CEBClientCore::EB_IsExistMemberByAccount(ULONGLONG sGroupCode, BSTR sMemberAccount, VARIANT_BOOL* pVal)
{
	bool ret = theEBAppClient.EB_IsExistMemberByAccount(sGroupCode,CEBString(sMemberAccount).c_str());
	*pVal = ret==true?VARIANT_TRUE:VARIANT_FALSE;
	return S_OK;
}
STDMETHODIMP CEBClientCore::EB_IsExistMemberByMemberCode(ULONGLONG sMemberCode, VARIANT_BOOL* pVal)
{
	bool ret = theEBAppClient.EB_IsExistMemberByMemberCode(sMemberCode);
	*pVal = ret==true?VARIANT_TRUE:VARIANT_FALSE;
	return S_OK;
}

STDMETHODIMP CEBClientCore::EB_GetMyEmotionList(VARIANT* pVal)
{
	std::vector<EB_EmotionInfo> pEmotionList;
	theEBAppClient.EB_GetMyEmotionList(pEmotionList);
	if (pEmotionList.empty())
	{
		return S_FALSE;
	}
	CComSafeArray<VARIANT> m_sa;
	for (size_t i=0; i<pEmotionList.size(); i++)
	{
		CIEB_EmotionInfo * pIEBEmotionInfo = new CIEB_EmotionInfo(pEmotionList[i]);
		CComPtr<IEB_EmotionInfo> pOutAccountInfoInterface;
		pIEBEmotionInfo->QueryInterface(IID_IEB_EmotionInfo,(void**)&pOutAccountInfoInterface);
		m_sa.Add(CComVariant(pOutAccountInfoInterface));
	}
	VariantInit(pVal);
	CComVariant var(m_sa);
	var.Detach(pVal);
	return S_OK;
}
STDMETHODIMP CEBClientCore::EB_GetDefaultHeadList(VARIANT* pVal)
{
	std::vector<EB_EmotionInfo> pEmotionList;
	theEBAppClient.EB_GetDefaultHeadList(pEmotionList);
	if (pEmotionList.empty())
	{
		return S_FALSE;
	}
	CComSafeArray<VARIANT> m_sa;
	for (size_t i=0; i<pEmotionList.size(); i++)
	{
		CIEB_EmotionInfo * pIEBEmotionInfo = new CIEB_EmotionInfo(pEmotionList[i]);
		CComPtr<IEB_EmotionInfo> pOutAccountInfoInterface;
		pIEBEmotionInfo->QueryInterface(IID_IEB_EmotionInfo,(void**)&pOutAccountInfoInterface);
		m_sa.Add(CComVariant(pOutAccountInfoInterface));
	}
	VariantInit(pVal);
	CComVariant var(m_sa);
	var.Detach(pVal);
	return S_OK;
}
STDMETHODIMP CEBClientCore::EB_FindAllGroupInfo(IDispatch* pConnection, ULONGLONG sEnterpriseCode, ULONG ulParam)
{
	if (pConnection == NULL) return E_INVALIDARG;
	pConnection->QueryInterface(IID_IDispatch,(void**)&m_pSearchCallback);
	if (m_pSearchCallback == NULL) return E_INVALIDARG;
	theEBAppClient.EB_FindAllGroupInfo(this,sEnterpriseCode,ulParam);
	m_pSearchCallback.Release();
	return S_OK;
}
STDMETHODIMP CEBClientCore::EB_FindAllContactInfo(IDispatch* pConnection, ULONG ulParam)
{
	if (pConnection == NULL) return E_INVALIDARG;
	pConnection->QueryInterface(IID_IDispatch,(void**)&m_pSearchCallback);
	if (m_pSearchCallback == NULL) return E_INVALIDARG;
	theEBAppClient.EB_FindAllContactInfo(this,ulParam);
	m_pSearchCallback.Release();
	return S_OK;
}
STDMETHODIMP CEBClientCore::EB_SearchUserInfo(IDispatch* pConnection, BSTR sSearchKey, ULONG ulParam)
{
	if (pConnection == NULL) return E_INVALIDARG;
	m_pSearchCallback.Release();
	pConnection->QueryInterface(IID_IDispatch,(void**)&m_pSearchCallback);
	if (m_pSearchCallback == NULL) return E_INVALIDARG;
	theEBAppClient.EB_SearchUserInfo(this,CEBString(sSearchKey).c_str(),ulParam);
	return S_OK;
}

STDMETHODIMP CEBClientCore::EB_GetVideoDeviceList(VARIANT* pVal)
{
	std::vector<tstring> pDeviceList;
	theEBAppClient.EB_GetVideoDeviceList(pDeviceList);
	if (pDeviceList.empty())
	{
		return S_FALSE;
	}
	CComSafeArray<VARIANT> m_sa;
	for (size_t i=0; i<pDeviceList.size(); i++)
	{
		m_sa.Add(CComVariant(CEBString(pDeviceList[i]).CopyBSTR()));
	}
	VariantInit(pVal);
	CComVariant var(m_sa);
	var.Detach(pVal);
	return S_OK;
}
//STDMETHODIMP CEBClientCore::EB_GetDefaultVideoMediaType(LONG* pVal)
//{
//	*pVal = theEBAppClient.EB_GetDefaultVideoMediaType();
//	return S_OK;
//}

//void VideoDataCallBack(INT64 nId, BYTE* pData, int dwSize, DWORD dwParam)
//{
//	try
//	{
//		CStreamData* pStreamData = (CStreamData*)dwParam;
//		if (pStreamData != NULL)
//		{
//			DWORD dwParam = 0;
//			if (pStreamData->m_pVideoData.find(nId,dwParam))
//			{
//				CComVariant avarParams[4];
//				avarParams[3] = nId;
//				avarParams[3].vt = VT_I8;
//				avarParams[2].byref = pData;
//				avarParams[2].vt = VT_UI1|VT_BYREF;
//				avarParams[1] = dwSize;
//				avarParams[1].vt = VT_UI4;
//				avarParams[0] = dwParam;
//				avarParams[0].vt = VT_UI4;
//				CComVariant varResult;
//				DISPPARAMS params = { avarParams, NULL, 4, 0 };
//				pStreamData->m_pVideoCallback->Invoke(1, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD, &params, &varResult, NULL, NULL);
//			}
//		}
//	}catch(...)
//	{
//	}
//}

void AudioDataCallBack(eb::bigint nUserId, int nCallBackType, unsigned int lParam, unsigned int wParam, void* pUserData)
//void AudioDataCallBack(INT64 nId, BYTE* pData, int nSize, DWORD dwParam)
{
	try
	{
		//CStreamData* pStreamData = (CStreamData*)dwParam;
		//if (pStreamData != NULL)
		//{
		//	DWORD dwParam = 0;
		//	if (pStreamData->m_pAudioData.find(nId,dwParam))
		//	{
		//		CComVariant avarParams[4];
		//		avarParams[3] = nId;
		//		avarParams[3].vt = VT_I8;
		//		avarParams[2].byref = (PVOID)pData;
		//		avarParams[2].vt = VT_UI1|VT_BYREF;
		//		avarParams[1] = nSize;
		//		avarParams[1].vt = VT_UI4;
		//		avarParams[0] = dwParam;
		//		avarParams[0].vt = VT_UI4;
		//		CComVariant varResult;
		//		DISPPARAMS params = { avarParams, NULL, 4, 0 };
		//		pStreamData->m_pAudioCallback->Invoke(2, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD, &params, &varResult, NULL, NULL);
		//	}
		//}
	}catch(...)
	{
	}
}

STDMETHODIMP CEBClientCore::EB_SetVideoCallback(ULONGLONG sCallId, IDispatch* pVideoCallback, IDispatch* pAudioDataCallback)
{
	if (pVideoCallback==NULL || pAudioDataCallback==NULL) return E_INVALIDARG;
	bool bNewVideoData = false;
	CStreamData::pointer pStreamData;
	if (!m_pVideoData.find(sCallId,pStreamData))
	{
		bNewVideoData = true;
		pStreamData = CStreamData::create(this,sCallId);
		m_pVideoData.insert(sCallId,pStreamData);
	}
	pStreamData->m_pVideoCallback.Release();
	pVideoCallback->QueryInterface(IID_IDispatch,(void**)&pStreamData->m_pVideoCallback);
	if (pStreamData->m_pVideoCallback==NULL)
	{
		if (bNewVideoData)
		{
			m_pVideoData.remove(sCallId);
		}
		return E_INVALIDARG;
	}
	pStreamData->m_pAudioCallback.Release();
	pAudioDataCallback->QueryInterface(IID_IDispatch,(void**)&pStreamData->m_pAudioCallback);
	if (pStreamData->m_pAudioCallback==NULL)
	{
		if (bNewVideoData)
		{
			m_pVideoData.remove(sCallId);
		}
		return E_INVALIDARG;
	}

	theEBAppClient.EB_SetVideoCallback(sCallId,AudioDataCallBack);
	//theEBAppClient.EB_SetVideoCallback(sCallId,VideoDataCallBack,AudioDataCallBack);
	return S_OK;
}
STDMETHODIMP CEBClientCore::EB_VideoRequest(ULONGLONG sCallId, SHORT nVideoType)
{
	int ret = theEBAppClient.EB_VideoRequest(sCallId,(EB_VIDEO_TYPE)nVideoType);
	return ret==0?S_OK:S_FALSE;
}
STDMETHODIMP CEBClientCore::EB_VideoAck(ULONGLONG sCallId, VARIANT_BOOL bAccept, ULONGLONG nToUserId)
{
	int ret = theEBAppClient.EB_VideoAck(sCallId,bAccept?true:false,nToUserId);
	return ret==0?S_OK:S_FALSE;
}
STDMETHODIMP CEBClientCore::EB_VideoEnd(ULONGLONG sCallId)
{
	int ret = theEBAppClient.EB_VideoEnd(sCallId);
	return ret==0?S_OK:S_FALSE;
}
STDMETHODIMP CEBClientCore::EB_GetUserVideoInfo(ULONGLONG sCallId, VARIANT* pOutMemberVideoInfo)
{
	std::vector<EB_UserVideoInfo> pMemberVideoInfo;
	bool ret = theEBAppClient.EB_GetUserVideoInfo(sCallId,pMemberVideoInfo);
	if (!ret)
	{
		return S_FALSE;
	}

	// member
	if (!pMemberVideoInfo.empty())
	{
		CComSafeArray<VARIANT> saMember;
		for (size_t i=0; i<pMemberVideoInfo.size(); i++)
		{
			CIEB_UserVideoInfo* pIEBUserVideoInfo = new CIEB_UserVideoInfo(pMemberVideoInfo[i]);
			CComPtr<IDispatch> pOutInterface;
			pIEBUserVideoInfo->QueryInterface(IID_IEB_UserVideoInfo,(void**)&pOutInterface);
			saMember.Add(CComVariant(pOutInterface));
		}
		VariantInit(pOutMemberVideoInfo);
		CComVariant varMember(saMember);
		varMember.Detach(pOutMemberVideoInfo);
	}
	//// on video
	//if (!pOnVideoInfo.empty())
	//{
	//	CComSafeArray<VARIANT> saOnVideo;
	//	for (size_t i=0; i<pOnVideoInfo.size(); i++)
	//	{
	//		CIEB_UserVideoInfo* pIEBUserVideoInfo = new CIEB_UserVideoInfo(pOnVideoInfo[i]);
	//		CComPtr<IDispatch> pOutInterface;
	//		pIEBUserVideoInfo->QueryInterface(IID_IEB_UserVideoInfo,(void**)&pOutInterface);
	//		saOnVideo.Add(CComVariant(pOutInterface));
	//	}
	//	VariantInit(pOutOnVideoInfo);
	//	CComVariant varOnVideo(saOnVideo);
	//	varOnVideo.Detach(pOutOnVideoInfo);
	//}
	return S_OK;
}
STDMETHODIMP CEBClientCore::EB_OpenLocalVideo(ULONGLONG sCallId, SHORT nLocalVideoIndex, ULONG nVideoParentWnd, ULONG ulAudioParam)
{
	//if (pAudioDataCallback==NULL) return E_INVALIDARG;
	CStreamData::pointer pCallStreamData;
	if (!m_pVideoData.find(sCallId,pCallStreamData))
	{
		return S_FALSE;
		//pCallStreamData = CStreamData::create(this,sCallId,0);
		//m_pVideoData.insert(sCallId,pCallStreamData);
	}
	const eb::bigint nUserId = theEBAppClient.EB_GetLogonUserId();
	//pCallStreamData->m_pVideoData.insert(nUserId,ulVideoParam);
	pCallStreamData->m_pAudioData.insert(nUserId,ulAudioParam);
	//pCallStreamData->m_nVideoData = ulVideoParam;
	//pCallStreamData->m_nAudioData = ulAudioParam;
	//CStreamData::pointer pAudioStreamData = CStreamData::create(this,sCallId,nUserData);
	//pAudioDataCallback->QueryInterface(IID_IDispatch,(void**)&pAudioStreamData->m_pCallback);
	//if (pAudioStreamData->m_pCallback==NULL)
	//{
	//	return E_INVALIDARG;
	//}
	int ret = theEBAppClient.EB_OpenLocalVideo(sCallId,nLocalVideoIndex,(HWND)nVideoParentWnd,pCallStreamData.get());
	//int ret = theEBAppClient.EB_OpenLocalVideo(sCallId,nLocalVideoIndex,nVideoMediaType,(DWORD)pCallStreamData.get(),(DWORD)pCallStreamData.get());
	if (ret <= 0)
	{
		return S_FALSE;
	}
	//pCallStreamData->m_pAudioData.add(pAudioStreamData);
	return S_OK;
}
STDMETHODIMP CEBClientCore::EB_OpenUserVideo(ULONGLONG sCallId, ULONGLONG nUserId, ULONG nVideoParentWnd, ULONG ulAudioParam)
{
	//if (pAudioDataCallback==NULL) return E_INVALIDARG;
	CStreamData::pointer pCallStreamData;
	if (!m_pVideoData.find(sCallId,pCallStreamData))
	{
		return S_FALSE;
		//pCallStreamData = CStreamData::create(this,sCallId);
		//m_pVideoData.insert(sCallId,pCallStreamData);
	}
	//pCallStreamData->m_pVideoData.insert(nUserId,ulVideoParam);
	pCallStreamData->m_pAudioData.insert(nUserId,ulAudioParam);

	//CStreamData::pointer pAudioStreamData = CStreamData::create(this,sCallId,nUserData);
	//pAudioDataCallback->QueryInterface(IID_IDispatch,(void**)&pAudioStreamData->m_pCallback);
	//if (pAudioStreamData->m_pCallback==NULL)
	//{
	//	return E_INVALIDARG;
	//}
	bool ret = theEBAppClient.EB_OpenUserVideo(sCallId,nUserId,(HWND)nVideoParentWnd,pCallStreamData.get());
	//bool ret = theEBAppClient.EB_OpenVideo(sCallId,nUserId,(DWORD)pCallStreamData.get(),(DWORD)pCallStreamData.get());
	if (ret != true)
	{
		return S_FALSE;
	}
	//pCallStreamData->m_pAudioData.add(pAudioStreamData);
	return S_OK;
}
STDMETHODIMP CEBClientCore::EB_SetVideoOpenClose(ULONGLONG sCallId, ULONGLONG nUserId, VARIANT_BOOL newVal)
{
	bool ret = theEBAppClient.EB_SetVideoOpenClose(sCallId,nUserId,newVal==VARIANT_TRUE?true:false);
	return ret?S_OK:S_FALSE;
}
STDMETHODIMP CEBClientCore::EB_SetAudioOpenClose(ULONGLONG sCallId, ULONGLONG nUserId, VARIANT_BOOL newVal)
{
	bool ret = theEBAppClient.EB_SetAudioOpenClose(sCallId,nUserId,newVal==VARIANT_TRUE?true:false);
	return ret?S_OK:S_FALSE;
}

//STDMETHODIMP CEBClientCore::EB_OpenVideoByUserVideoId(ULONGLONG sCallId, ULONG nUserVideoId, IDispatch* pAudioDataCallback, ULONG nUserData)
//{
//	if (pAudioDataCallback==NULL) return E_INVALIDARG;
//	CStreamData::pointer pCallStreamData;
//	if (!m_pVideoData.find(sCallId,pCallStreamData))
//	{
//		pCallStreamData = CStreamData::create(this,sCallId,0);
//		m_pVideoData.insert(sCallId,pCallStreamData);
//	}
//	CStreamData::pointer pAudioStreamData = CStreamData::create(this,sCallId,nUserData);
//	pAudioDataCallback->QueryInterface(IID_IDispatch,(void**)&pAudioStreamData->m_pCallback);
//	if (pAudioStreamData->m_pCallback==NULL)
//	{
//		return E_INVALIDARG;
//	}
//	bool ret = theEBAppClient.EB_OpenVideoByUserVideoId(sCallId,nUserVideoId,AudioDataCallBack,(void*)pAudioStreamData.get());
//	if (ret != true)
//	{
//		return S_FALSE;
//	}
//	pCallStreamData->m_pAudioData.add(pAudioStreamData);
//	return S_OK;
//}

STDMETHODIMP CEBClientCore::EB_CloseAllVideo(ULONGLONG sCallId)
{
	theEBAppClient.EB_CloseAllVideo(sCallId);
	m_pVideoData.remove(sCallId);
	return S_OK;
}

STDMETHODIMP CEBClientCore::EB_GetSubscribeFuncInfo(ULONGLONG nSubId, IEB_SubscribeFuncInfo** pVal)
{
	CIEB_SubscribeFuncInfo * pIEBSubscribeFuncInfo= new CIEB_SubscribeFuncInfo();
	if (!theEBAppClient.EB_GetSubscribeFuncInfo(nSubId,pIEBSubscribeFuncInfo))
	{
		delete pIEBSubscribeFuncInfo;
		return S_FALSE;
	}
	return pIEBSubscribeFuncInfo->QueryInterface(IID_IEB_SubscribeFuncInfo,(void**)pVal);

	//EB_SubscribeFuncInfo pOutFuncInfo;
	//if (!theEBAppClient.EB_GetSubscribeFuncInfo(nSubId,&pOutFuncInfo))
	//{
	//	return S_FALSE;
	//}
	//CIEB_SubscribeFuncInfo * pIEBSubscribeFuncInfo= new CIEB_SubscribeFuncInfo(pOutFuncInfo);
	//CComPtr<IEB_AccountInfo> pOutFuncInfoInterface;
	//pIEBSubscribeFuncInfo->QueryInterface(IID_IEB_SubscribeFuncInfo,(void**)&pOutFuncInfoInterface);
}
STDMETHODIMP CEBClientCore::EB_GetSubscribeFuncList(ULONG nLocation, VARIANT* pVal)
{
	std::vector<EB_SubscribeFuncInfo> pOutFuncList;
	theEBAppClient.EB_GetSubscribeFuncList((EB_FUNC_LOCATION)nLocation,pOutFuncList);
	if (pOutFuncList.empty())
	{
		return S_FALSE;
	}
	CComSafeArray<VARIANT> m_sa;
	for (size_t i=0; i<pOutFuncList.size(); i++)
	{
		CIEB_SubscribeFuncInfo * pIEBSubscribeFuncInfo= new CIEB_SubscribeFuncInfo(pOutFuncList[i]);
		CComPtr<IEB_AccountInfo> pOutFuncInfoInterface;
		pIEBSubscribeFuncInfo->QueryInterface(IID_IEB_SubscribeFuncInfo,(void**)&pOutFuncInfoInterface);
		m_sa.Add(CComVariant(pOutFuncInfoInterface));
	}
	VariantInit(pVal);
	CComVariant var(m_sa);
	var.Detach(pVal);
	return S_OK;
}
STDMETHODIMP CEBClientCore::EB_GetSubscribeFuncSize(ULONG nLocation, ULONG* pVal)
{
	*pVal = theEBAppClient.EB_GetSubscribeFuncSize((EB_FUNC_LOCATION)nLocation);
	return S_OK;
}

STDMETHODIMP CEBClientCore::EB_GetSubscribeFuncUrl(ULONGLONG nSubscribeId,ULONGLONG nCallId,ULONGLONG nFromUserId,BSTR sFromAccount,ULONGLONG nGroupId,BSTR * pVal)
{
	CEBString sUrl = theEBAppClient.EB_GetSubscribeFuncUrl(nSubscribeId,nCallId,nFromUserId,CEBString(sFromAccount),nGroupId);
	*pVal = sUrl.CopyBSTR();
	return S_OK;
}
STDMETHODIMP CEBClientCore::EB_GetConversationsUrl(ULONGLONG nFromUserId,ULONGLONG nGroupId,BSTR * pVal)
{
	CEBString sUrl = theEBAppClient.EB_GetConversationsUrl(nFromUserId,nGroupId);
	*pVal = sUrl.CopyBSTR();
	return S_OK;
}
STDMETHODIMP CEBClientCore::EB_AckMsg(ULONGLONG nMsgId, VARIANT_BOOL bAccept)
{
	theEBAppClient.EB_AckMsg(nMsgId,bAccept==VARIANT_TRUE?true:false);
	return S_OK;
}

//STDMETHODIMP CEBClientCore::UTF82ACP(BSTR sUtf8String, BSTR* pVal)
//{
//	USES_CONVERSION_EX;
//	CEBString sString = entboost::UTF82ACP(BSTR2A_EX(sUtf8String));
//	*pVal = sString.CopyBSTR();
//	return S_OK;	
//}
//
//STDMETHODIMP CEBClientCore::ACP2UTF8(BSTR sCapString, BSTR* pVal)
//{
//	USES_CONVERSION_EX;
//	CEBString sString = entboost::ACP2UTF8(BSTR2A_EX(sCapString));
//	*pVal = sString.CopyBSTR();
//	return S_OK;	
//}

//STDMETHODIMP CEBClientCore::GetAccountAddress(BSTR sAccount, BSTR* pVal)
//{
//	USES_CONVERSION_EX;
//	CEBString sString;
//	entboost::GetAccountAddress(BSTR2A_EX(sAccount),sString);
//	*pVal = sString.CopyBSTR();
//	return S_OK;	
//}


STDMETHODIMP CEBClientCore::Load(BSTR sFileName)
{
	// TODO: Add your implementation code here
	bool ret = m_pSetting.load(CEBString(sFileName).c_str());
	return ret?S_OK:S_FALSE;
}

STDMETHODIMP CEBClientCore::get_ServerAddress(BSTR* pVal)
{
	CEBString sValue = m_pSetting.GetServerAddress();
	*pVal = sValue.CopyBSTR();
	return S_OK;
}

STDMETHODIMP CEBClientCore::get_DefaultColor(ULONG* pVal)
{
	*pVal = m_pSetting.GetDefaultColor();
	return S_OK;
}

STDMETHODIMP CEBClientCore::get_Version(BSTR* pVal)
{
	CEBString sValue = m_pSetting.GetVersion();
	*pVal = sValue.CopyBSTR();
	return S_OK;
}

STDMETHODIMP CEBClientCore::get_Enterprise(BSTR* pVal)
{
	CEBString sValue = m_pSetting.GetEnterprise();
	*pVal = sValue.CopyBSTR();
	return S_OK;
}
