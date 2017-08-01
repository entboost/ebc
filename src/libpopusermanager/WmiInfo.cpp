//#include "StdAfx.h"
#include "WmiInfo.h"
#ifdef USES_WMIINFO
#ifdef _QT_MAKE_
#include <QString>
#endif

#ifndef MIN
#define MIN(a, b)  (((a) < (b)) ? (a) : (b))
#endif // MIN

#ifdef WIN32
CWmiInfo::CWmiInfo(void)  
{  
	m_pWbemSvc=NULL;  
	m_pWbemLoc=NULL;  
	m_pEnumClsObj = NULL;  
}  

CWmiInfo::~CWmiInfo(void)  
{  
	ReleaseWmi();
	m_pWbemSvc=NULL;  
	m_pWbemLoc=NULL;  
	m_pEnumClsObj = NULL;  
}  

HRESULT CWmiInfo::InitWmi()  
{  
	HRESULT hr;  

	//一、初始化COM组件  
	//初始化COM  
	hr=::CoInitializeEx(0,COINIT_MULTITHREADED);  
	if (SUCCEEDED(hr) || RPC_E_CHANGED_MODE == hr)  
	{  
		//设置进程的安全级别，（调用COM组件时在初始化COM之后要调用CoInitializeSecurity设置进程安全级别，否则会被系统识别为病毒）  
		hr=CoInitializeSecurity(NULL,  
			-1,  
			NULL,                     
			NULL,  
			RPC_C_AUTHN_LEVEL_PKT,  
			RPC_C_IMP_LEVEL_IMPERSONATE,  
			NULL,  
			EOAC_NONE,  
			NULL);  
		//VERIFY(SUCCEEDED(hr));  

		//二、创建一个WMI命名空间连接  
		//创建一个CLSID_WbemLocator对象  
		hr=CoCreateInstance(CLSID_WbemLocator,  
			0,  
			CLSCTX_INPROC_SERVER,  
			IID_IWbemLocator,  
			(LPVOID*)&m_pWbemLoc);  
		//VERIFY(SUCCEEDED(hr));  

		//使用m_pWbemLoc连接到"root\cimv2"并设置m_pWbemSvc的指针  
#ifdef _QT_MAKE_
        hr=m_pWbemLoc->ConnectServer(L"ROOT\\CIMV2",
#else
        hr=m_pWbemLoc->ConnectServer(CComBSTR(L"ROOT\\CIMV2"),
#endif
            NULL,
			NULL,  
			0,  
			NULL,  
			0,  
			0,  
			&m_pWbemSvc);  
		//VERIFY(SUCCEEDED(hr));  

		//三、设置WMI连接的安全性  
		hr=CoSetProxyBlanket(m_pWbemSvc,  
			RPC_C_AUTHN_WINNT,  
			RPC_C_AUTHZ_NONE,  
			NULL,  
			RPC_C_AUTHN_LEVEL_CALL,  
			RPC_C_IMP_LEVEL_IMPERSONATE,  
			NULL,  
			EOAC_NONE);  
		//VERIFY(SUCCEEDED(hr));  

	}  
	return(hr);  
}  

HRESULT CWmiInfo::ReleaseWmi()  
{  
	HRESULT hr;  

	if (NULL != m_pWbemSvc)  
	{  
		hr=m_pWbemSvc->Release();  
	}  
	if (NULL != m_pWbemLoc)  
	{  
		hr=m_pWbemLoc->Release();  
	}  
	if (NULL != m_pEnumClsObj)  
	{  
		hr=m_pEnumClsObj->Release();  
	}  

	::CoUninitialize();  

	return(hr);  
}  

bool CWmiInfo::GetSingleItemInfo(const char* ClassName,const char* ClassMember,tstring &chRetValue)  
{  
#ifdef _QT_MAKE_
    QString query("SELECT * FROM ");
#else
	USES_CONVERSION;  
	CComBSTR query("SELECT * FROM ");  
#endif
    VARIANT vtProp;
	ULONG uReturn;  
	HRESULT hr;  
	bool bRet = false;  

	if (NULL != m_pWbemSvc)  
	{  
		//查询类ClassName中的所有字段,保存到m_pEnumClsObj中  
#ifdef _QT_MAKE_
        query+=ClassName;
        BSTR bstr_str = SysAllocString(query.toStdWString().c_str());
//        BSTR bstr_str = SysAllocString(query.utf16());
        hr=m_pWbemSvc->ExecQuery(L"WQL",bstr_str,WBEM_FLAG_FORWARD_ONLY|WBEM_FLAG_RETURN_IMMEDIATELY,0,&m_pEnumClsObj);
        SysFreeString(bstr_str);
#else
        query+=CComBSTR(ClassName);
        hr=m_pWbemSvc->ExecQuery(CComBSTR("WQL"),query,WBEM_FLAG_FORWARD_ONLY|WBEM_FLAG_RETURN_IMMEDIATELY,0,&m_pEnumClsObj);
#endif
        if (SUCCEEDED(hr))
		{  
			//初始化vtProp值  
			VariantInit(&vtProp);  
			uReturn=0;  

			//返回从当前位置起的第一个对象到m_pWbemClsObj中  
			hr=m_pEnumClsObj->Next(WBEM_INFINITE,1,&m_pWbemClsObj,&uReturn);  
			if(SUCCEEDED(hr)&&uReturn>0)   
			{  
				//从m_pWbemClsObj中找出ClassMember标识的成员属性值,并保存到vtProp变量中  
#ifdef _QT_MAKE_
                BSTR bstr_str = SysAllocString(QString(ClassMember).toStdWString().c_str());
                hr=m_pWbemClsObj->Get(bstr_str,0,&vtProp,0,0);
                SysFreeString(bstr_str);
#else
                hr=m_pWbemClsObj->Get(CComBSTR(ClassMember),0,&vtProp,0,0);
#endif
				if (SUCCEEDED(hr))  
				{  
					VariantToString(&vtProp,chRetValue);  
					VariantClear(&vtProp);//清空vtProp  
					bRet = true;  
				}  
			}  
		}  
	}  
	if(NULL != m_pEnumClsObj)   
	{  
		hr=m_pEnumClsObj->Release();  
		m_pEnumClsObj = NULL;  
	}  
	if(NULL != m_pWbemClsObj)   
	{  
		hr=m_pWbemClsObj->Release();  
		m_pWbemClsObj = NULL;  
	}  
	return bRet;  
}  

bool CWmiInfo::GetGroupItemInfo(const char* ClassName,tstring ClassMember[],int n,tstring& chRetValue)  
{
#ifdef _QT_MAKE_
    QString query("SELECT * FROM ");
#else
	USES_CONVERSION;  
	CComBSTR query("SELECT * FROM ");  
#endif
	tstring result,info;  
	VARIANT vtProp;  
	ULONG uReturn;  
	HRESULT hr;  
	int i;  
	bool bRet = false;  
	if (NULL  != m_pWbemSvc)  
	{  
#ifdef _QT_MAKE_
        query+=ClassName;
        BSTR bstr_str = SysAllocString(query.toStdWString().c_str());
//        BSTR bstr_str = SysAllocString(query.utf16());
        hr=m_pWbemSvc->ExecQuery(L"WQL",bstr_str,WBEM_FLAG_FORWARD_ONLY|WBEM_FLAG_RETURN_IMMEDIATELY,0,&m_pEnumClsObj);
        SysFreeString(bstr_str);
#else
        query+=CComBSTR(ClassName);
        hr=m_pWbemSvc->ExecQuery(CComBSTR("WQL"),query,WBEM_FLAG_FORWARD_ONLY|WBEM_FLAG_RETURN_IMMEDIATELY,0,&m_pEnumClsObj);
#endif
		if (SUCCEEDED(hr))  
		{  
			VariantInit(&vtProp); //初始化vtProp变量  
			if(m_pEnumClsObj)    
			{  
				Sleep(10);  
				uReturn=0;  
				hr=m_pEnumClsObj->Next(WBEM_INFINITE,1,&m_pWbemClsObj,&uReturn);  
				if (SUCCEEDED(hr) &&uReturn>0)  
				{  
					for(i=0;i<n;++i)  
                    {
#ifdef _QT_MAKE_
                        BSTR bstr_str = SysAllocString(QString(ClassMember[i]).toStdWString().c_str());
                        hr=m_pWbemClsObj->Get(bstr_str,0,&vtProp,0,0);
                        SysFreeString(bstr_str);
#else
						hr=m_pWbemClsObj->Get(CComBSTR(ClassMember[i].c_str()),0,&vtProp,0,0);  
#endif
						if (SUCCEEDED(hr))  
						{  
							VariantToString(&vtProp,info);  
                            chRetValue+=(info+"\t");
							VariantClear(&vtProp);  
							bRet = true;  
						}  
					}  
                    chRetValue+="\r\n";
				}  
			}  
		}  
	}  

	if(NULL != m_pEnumClsObj)  
	{  
		hr=m_pEnumClsObj->Release();  
		m_pEnumClsObj=NULL;  
	}  
	if(NULL != m_pWbemClsObj)  
	{  
		hr=m_pWbemClsObj->Release();  
		m_pWbemClsObj=NULL;  
	}  
	return bRet;  
}  

void CWmiInfo::VariantToString(const LPVARIANT pVar,tstring &chRetValue) const  
{  
#ifdef _QT_MAKE_
    QString query("SELECT * FROM ");
    BSTR* pBstr;
#else
	USES_CONVERSION;  
    CComBSTR HUGEP* pBstr;
#endif
	BYTE HUGEP* pBuf;  
	LONG low,high,i;  
	HRESULT hr;  

	switch(pVar->vt)  
	{  
	case VT_BSTR:  
		{  
#ifdef _QT_MAKE_
        chRetValue=QString::fromUtf16((const ushort*)pVar->bstrVal).toStdString();
#else
        chRetValue=W2A(pVar->bstrVal);
#endif
		}  
		break;  
	case VT_BOOL:  
		{  
			if(VARIANT_TRUE==pVar->boolVal)   
				chRetValue="是";  
			else  
				chRetValue="否";  
		}  
		break;  
	case VT_I4:  
		{  
			char lpszBuffer[20];
			sprintf(lpszBuffer,"%d",pVar->lVal);
			chRetValue = lpszBuffer;
		}  
		break;  
	case VT_UI1:  
		{  
			char lpszBuffer[20];
			sprintf(lpszBuffer,"%d",pVar->bVal);
			chRetValue = lpszBuffer;
		}  
		break;  
	case VT_UI4:  
		{  
			char lpszBuffer[20];
			sprintf(lpszBuffer,"%d",pVar->ulVal);
			chRetValue = lpszBuffer;
		}  
		break;  

	case VT_BSTR|VT_ARRAY:  
		{  
#ifdef _QT_MAKE_
        hr=SafeArrayAccessData(pVar->parray,(void**)&pBstr);
        hr=SafeArrayUnaccessData(pVar->parray);
        chRetValue=QString::fromUtf16((const ushort*)*pBstr).toStdString();
#else
        hr=SafeArrayAccessData(pVar->parray,(void HUGEP**)&pBstr);
        hr=SafeArrayUnaccessData(pVar->parray);
        chRetValue=W2A(pBstr->m_str);
#endif
		}  
		break;  

	case VT_I4|VT_ARRAY:  
		{  
			SafeArrayGetLBound(pVar->parray,1,&low);   
			SafeArrayGetUBound(pVar->parray,1,&high);  

			hr=SafeArrayAccessData(pVar->parray,(void HUGEP**)&pBuf);  
			hr=SafeArrayUnaccessData(pVar->parray);  
			char lpszBuffer[20];
            high=MIN(high,260*2-1);
			for(i=low;i<=high;++i)  
			{  
				sprintf(lpszBuffer,"%02X",pBuf[i]);
				chRetValue+=lpszBuffer;  
			}  
		}  
		break;  
	default:  
		break;  
	}  
}  

#endif
#endif // USES_WMIINFO
