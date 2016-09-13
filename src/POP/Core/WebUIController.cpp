#include "stdafx.h"
#include "WebUIController.h"


CWebUIController::CWebUIController(CWebUICallback * pWebUICallback)
: m_uRefCount(1), m_pWebBrowser2(NULL), m_dwCookie(0)
, m_pExtMethodCallback(NULL)
, m_dwParam(0)
, m_pWebUICallback(pWebUICallback)

{       
	//m_eventhandler = new CHTMLEventHandlerEx();
	m_nAutoSize = 0;
	m_nEnableMax = 1;
	m_nWidth = 1024;
	m_nHeight = 720;

	m_nSysToolBar = 1;
	m_nContextMenu = 0;
	m_n3DBorder = 0;
	m_nScrollBar = 1;
	m_nTopMost = 1;
} 
CWebUIController::~CWebUIController(void)
{
	m_pExtMethods.clear();
	ClearWebBrowser();
	//delete m_eventhandler;
	//m_eventhandler = NULL;
}

inline HRESULT _CoAdvise(IUnknown* pUnkCP, IUnknown* pUnk, const IID& iid, LPDWORD pdw)
{    
	IConnectionPointContainer* pCPC = NULL;
	IConnectionPoint* pCP = NULL;
	HRESULT hRes = pUnkCP->QueryInterface(IID_IConnectionPointContainer, (void**)&pCPC);
	if(SUCCEEDED(hRes) && pCPC != NULL)    
	{        
		hRes = pCPC->FindConnectionPoint(iid, &pCP);
		if(SUCCEEDED(hRes) && pCP != NULL)
		{            
			hRes = pCP->Advise(pUnk, pdw);
			pCP->Release();
		}         
		pCPC->Release();    
	}     
	return hRes;
}

inline HRESULT _CoUnadvise(IUnknown* pUnkCP, const IID& iid, DWORD dw)
{    
	IConnectionPointContainer* pCPC = NULL;    
	IConnectionPoint* pCP = NULL;    
	HRESULT hRes = pUnkCP->QueryInterface(IID_IConnectionPointContainer, (void**)&pCPC);
	if(SUCCEEDED(hRes) && pCPC != NULL)    
	{        
		hRes = pCPC->FindConnectionPoint(iid, &pCP);
		if(SUCCEEDED(hRes) && pCP != NULL)        
		{            
			hRes = pCP->Unadvise(dw);
			pCP->Release();
		}         
		pCPC->Release();    
	}     
	return hRes;
}

HRESULT CWebUIController::QueryInterface(REFIID riid, void** ppvObject)
{       
	*ppvObject = NULL;
	if(IsEqualGUID(riid, DIID_DWebBrowserEvents2) || IsEqualGUID(riid, IID_IDispatch))
	{            
		*ppvObject = (DWebBrowserEvents2*)this;
		AddRef();
		return S_OK;
	}        
	else if(IsEqualGUID(riid, IID_IDocHostUIHandler) || IsEqualGUID(riid, IID_IUnknown))
	{            
		*ppvObject = (IDocHostUIHandler*)this;
		AddRef();
		return S_OK;
	}         
	return E_NOINTERFACE;    
}

HRESULT CWebUIController::SetWebBrowser(IWebBrowser2* pWebBrowser2)   
{        
	// Unadvise the event sink, if there was a    
	// previous reference to the WebBrowser control.   
	ClearWebBrowser();
	m_pWebBrowser2 = pWebBrowser2;  
	if(pWebBrowser2 == NULL)   
		return S_OK;  
	m_pWebBrowser2->AddRef();  
	return _CoAdvise(m_pWebBrowser2, (IDispatch*)this, DIID_DWebBrowserEvents2, &m_dwCookie);   
}
void CWebUIController::ClearWebBrowser(void)
{
	if(m_pWebBrowser2)
	{           
		IDispatch* pDoc = NULL;   
		HRESULT hr = m_pWebBrowser2->get_Document(&pDoc);       
		if(SUCCEEDED(hr) && pDoc!=NULL)      
		{
			// SetUIHandler
			ICustomDoc* pCustDoc = NULL;    
			hr = pDoc->QueryInterface(IID_ICustomDoc, (void**)&pCustDoc); 
			if(SUCCEEDED(hr) && pCustDoc != NULL)    
			{                      
				pCustDoc->SetUIHandler(NULL);
				pCustDoc->Release();       
			}              
			pDoc->Release();    
		}       

		_CoUnadvise(m_pWebBrowser2, DIID_DWebBrowserEvents2, m_dwCookie); 
		m_dwCookie = 0;   
		m_pWebBrowser2->Release();     
		m_pWebBrowser2 = NULL;
	}    
}

class ExternalMethod
{
public:
	void callback(DWORD dwParam, const cgcValueInfo::pointer & pInValueInfo)
	{
		m_pCallback(dwParam, m_pExtMethodInfo, pInValueInfo);
	}

	ExternalMethod(JsExternalMethod_Callback * pCallback, CExtMethodInfo::pointer pExtMethodInfo)
		: m_pCallback(pCallback)
		, m_pExtMethodInfo(pExtMethodInfo)
	{
	}
private:
	JsExternalMethod_Callback * m_pCallback;
	CExtMethodInfo::pointer m_pExtMethodInfo;
};


HRESULT CWebUIController::GetIDsOfNames(REFIID riid, OLECHAR FAR* FAR* rgszNames, unsigned int cNames, LCID lcid, DISPID FAR* rgDispId)    
{
	CString sExtMethod(rgszNames[0]);
	CExtMethodInfo::pointer pExtMethodInfo;
	if (m_pExtMethods.find(sExtMethod, pExtMethodInfo))
	{
		*rgDispId = (LONG)new ExternalMethod(m_pExtMethodCallback, pExtMethodInfo);
		m_pDispIds.insert(*rgDispId, true);
		return S_OK;
	}
	return E_NOTIMPL;
}    


inline cgcValueInfo::pointer GetValueInfo(const COleVariant &pVaraint)
{
	cgcValueInfo::pointer pValueInfo;
	switch (pVaraint.vt)
	{
	case VT_I2:
		pValueInfo = CGC_VALUEINFO((int)pVaraint.iVal);
		break;
	case VT_I4:
		pValueInfo = CGC_VALUEINFO((int)pVaraint.lVal);
		break;
	case VT_I8:
		pValueInfo = CGC_VALUEINFO(pVaraint.llVal);
		break;
	case VT_R4:
		pValueInfo = CGC_VALUEINFO((double)pVaraint.fltVal);
		break;
	case VT_R8:
		pValueInfo = CGC_VALUEINFO(pVaraint.dblVal);
		break;
	case VT_BSTR:
		{
			tstring sValue = (LPCTSTR)(CString)pVaraint.bstrVal;
			pValueInfo = CGC_VALUEINFO(sValue);
		}break;
	case VT_BOOL:
		pValueInfo = CGC_VALUEINFO((pVaraint.boolVal?true:false));
		break;
	default:
		break;
	}
	return pValueInfo;
}

HRESULT CWebUIController::Invoke(DISPID dispidMember,REFIID riid, LCID lcid, WORD wFlags,        DISPPARAMS* pDispParams, VARIANT* pvarResult,        EXCEPINFO* pexcepinfo, UINT* puArgErr)
{
	if(!pDispParams)
		return E_INVALIDARG;                 
	switch(dispidMember)        
	{
	case DISPID_COMMANDSTATECHANGE:
		{
			int i=0;
		}break;
	case DISPID_STATUSTEXTCHANGE:
		{
			if (m_pWebUICallback!=NULL && pDispParams->cArgs==1)
			{
				m_pWebUICallback->OnStatusTextChange(pDispParams->rgvarg[0].bstrVal);
			}
		}break;
	//case DISPID_TITLEICONCHANGE:	// ?
	//	{
	//		//if (m_pWebUICallback!=NULL && pDispParams->cArgs==1)
	//		//{
	//		//	//m_pWebUICallback->OnTitleChange(pDispParams->rgvarg[0].bstrVal);
	//		//}
	//	}break;
	case DISPID_PROGRESSCHANGE:
	case DISPID_PROPERTYCHANGE:
	case DISPID_BEFORENAVIGATE2:
	case DISPID_DOWNLOADCOMPLETE:
		//case DISPID_COMMANDSTATECHANGE:
	case DISPID_DOWNLOADBEGIN:
	case DISPID_SETSECURELOCKICON:
	case DISPID_FILEDOWNLOAD:
		break;
	case DISPID_TITLECHANGE:
		{
			if (m_pWebUICallback!=NULL && pDispParams->cArgs==1)
			{
				m_pWebUICallback->OnTitleChange(pDispParams->rgvarg[0].bstrVal);
			}
		}break;
	case DISPID_WINDOWSETWIDTH:
		{
			int i=0;
		}break;
	case DISPID_WINDOWRESIZE:
		{
			int i=0;
		}break;
	//case DISPID_TITLEICONCHANGE:
	//	{
	//		if(pDispParams->cArgs >= 2 && pDispParams->rgvarg[1].vt == VT_DISPATCH)
	//			OnDocumentComplete(pDispParams->rgvarg[1].pdispVal);

	//	}break;
		//            
		// The parameters for this DISPID are as follows:
		// [0]: URL to navigate to - VT_BYREF|VT_VARIANT
		// [1]: An object that evaluates to the top-level or frame 
		//      WebBrowser object corresponding to the event.
	case DISPID_DOCUMENTCOMPLETE:
		{
			if(pDispParams->cArgs >= 2 && pDispParams->rgvarg[1].vt == VT_DISPATCH)
				OnDocumentComplete(pDispParams->rgvarg[1].pdispVal);
		}break;
	case DISPID_NAVIGATECOMPLETE2:
		//            
		// The IDocHostUIHandler association must be set
		// up every time we navigate to a new page.
		//            
		if(pDispParams->cArgs >= 2 && pDispParams->rgvarg[1].vt == VT_DISPATCH)
		{
			//USES_CONVERSION;
			//CString sUrl = W2T(pDispParams->rgvarg[0].pvarVal->bstrVal);
			//sUrl = sUrl.Right(3);
			//sUrl.MakeLower();
			//if (sUrl == _T("csp"))
				SetCustomDoc(pDispParams->rgvarg[1].pdispVal);
		}
		else                
			return E_INVALIDARG;
		break;
	case DISPID_NEWWINDOW3:
		{
			if (m_pWebUICallback!=NULL && pDispParams->cArgs==5)
			{
				m_pWebUICallback->OnNewWindow3(pDispParams->rgvarg[4].ppdispVal,pDispParams->rgvarg[3].pboolVal,pDispParams->rgvarg[2].ulVal,pDispParams->rgvarg[1].bstrVal,pDispParams->rgvarg[0].bstrVal);
			}
		}break;
	case DISPID_NEWWINDOW2:
		{
			// Set the cancel flag to block popups
			//pDispParams->rgvarg[0].pvarVal->vt = VT_BOOL;
			//pDispParams->rgvarg[0].pvarVal->boolVal = VARIANT_TRUE;
		}break;
	default:
		{
			bool bValue = false;
			if (dispidMember > 0xffff && m_pDispIds.find(dispidMember, bValue, true))
			{
				cgcValueInfo::pointer pInValueInfo;
				if (pDispParams->cArgs == 1)
				{
					COleVariant pVaraint(pDispParams->rgvarg[0]);
					pInValueInfo = GetValueInfo(pVaraint);
				}else if (pDispParams->cArgs > 1)
				{
					pInValueInfo = CGC_VALUEINFO(cgcValueInfo::TYPE_VECTOR);
					for (int i=pDispParams->cArgs-1; i>=0; i--)
					{
						COleVariant pVaraint(pDispParams->rgvarg[i]);
						pInValueInfo->addVector(GetValueInfo(pVaraint));
					}
				}

				ExternalMethod * pExternalMethod = (ExternalMethod*)dispidMember;
				pExternalMethod->callback(m_dwParam, pInValueInfo);
				delete pExternalMethod;
			}else
			{
				int i=0;

			}

		}
		break;       
	}                
	return S_OK;    
}     

//int CWebUIController::GetElementAttribute(IHTMLElement* pElement, BSTR strAttributeName, int nDefault) const
//{
//	int result = nDefault;
//	if (pElement != NULL)
//	{
//		VARIANT pOutValue;
//		HRESULT hr = pElement->getAttribute(strAttributeName, 0, &pOutValue);
//		if (SUCCEEDED(hr) && pOutValue.vt == VT_BSTR)
//		{
//			result = _wtoi(pOutValue.bstrVal);
//		}
//	}
//	return result;
//}

CString CWebUIController::GetElementAttribute(IHTMLElement* pElement, BSTR strAttributeName, LPCTSTR sDefault) const
{
	CString result = sDefault;
	if (pElement != NULL)
	{
		VARIANT pOutValue;
		HRESULT hr = pElement->getAttribute(strAttributeName, 0, &pOutValue);
		if (SUCCEEDED(hr) && pOutValue.vt == VT_BSTR)
		{
			result = pOutValue.bstrVal;
		}
	}
	return result;
}

void CWebUIController::LoadFaviconUrl(IHTMLDocument3 * pDocument3)
{
	if(pDocument3 == NULL)         
		return;
	m_sFaviconUrl = "";
	IHTMLElementCollection * pElementCollection = NULL;
	pDocument3->getElementsByTagName(CComBSTR(L"link"), &pElementCollection);
	if (pElementCollection != NULL)
	{
		long nLength = 0;
		pElementCollection->get_length(&nLength);
		for (long i=0; i<nLength; i++)
		{
			IDispatchPtr pElementDispatch;
			_variant_t index = i;
			pElementCollection->item(index, index, (IDispatch**)&pElementDispatch);
			if (pElementDispatch == NULL)
				continue;

			IHTMLElement* pElement = NULL;
			pElementDispatch->QueryInterface(IID_IHTMLElement,(void**)&pElement);
			if (pElement != NULL)
			{
				CString sRel = GetElementAttribute(pElement, CComBSTR(L"rel"), _T(""));
				if (sRel=="shortcut icon")
				{
					m_sFaviconUrl = GetElementAttribute(pElement, CComBSTR(L"href"), _T(""));
					pElement->Release();
					break;
				}
				pElement->Release();
			}
		}

		pElementCollection->Release();
	}
}

//void CWebUIController::LoadCCPSetting(IHTMLDocument3 * pDocument3)
//{
//	if(pDocument3 == NULL)         
//		return;
//	HRESULT hr = S_OK;
//	IHTMLElementPtr pElement;
//	pDocument3->getElementById(CComBSTR(L"window-size"), &pElement);
//	if (pElement != NULL)
//	{
//		m_nEnableMax = GetElementAttribute(pElement, CComBSTR(L"enablemax"), 1);
//		m_nAutoSize = GetElementAttribute(pElement, CComBSTR(L"autosize"), 0);
//		if (m_nAutoSize)
//		{
//			IHTMLDocument2 * pDocument2;
//			pDocument3->QueryInterface(IID_IHTMLDocument2,(void**)&pDocument2);
//			ASSERT( SUCCEEDED( hr ) );
//			ASSERT( pDocument2 );
//
//			IHTMLElementPtr pBody = NULL;
//			hr = pDocument2->get_body( &pBody );
//			ASSERT( SUCCEEDED( hr ) );
//			ASSERT( pBody );
//
//			// 从body获得IHTMLElement2接口指针，用以访问滚动条
//			IHTMLElement2 *pElement2 = NULL;
//			hr = pBody->QueryInterface(IID_IHTMLElement2,(void**)&pElement2);
//			ASSERT(SUCCEEDED(hr));
//			ASSERT( pElement2 );
//
//			//// 向下滚动100个像素
//			//pElement2->put_scrollTop( 100 ); 
//
//			// 获得文档真正的高度，不是可见区域的高度
//			long scroll_height; 
//			pElement2->get_scrollHeight( &scroll_height );
//			m_nHeight = scroll_height;
//
//			//// 获得滚动条位置，从顶端开始
//			//long scroll_top;
//			//pElement2->get_scrollTop( &scroll_top ); 
//
//			// 获得文档真正的宽度，不是可见区域的宽度
//			long scroll_width; 
//			pElement2->get_scrollWidth( &scroll_width );
//			m_nWidth = (int)scroll_width;
//
//			pElement2->Release();
//		}else
//		{
//			m_nWidth = GetElementAttribute(pElement, CComBSTR(L"width"), 720);
//			m_nHeight = GetElementAttribute(pElement, CComBSTR(L"height"), 480);
//		}
//
//		pElement->Release();
//	}
//
//	pDocument3->getElementById(CComBSTR(L"window-setting"), &pElement);
//	if (pElement != NULL)
//	{
//		m_nSysToolBar = GetElementAttribute(pElement, CComBSTR(L"systoolbar"), 0);
//		m_nContextMenu = GetElementAttribute(pElement, CComBSTR(L"contextmenu"), 0);
//		m_n3DBorder = GetElementAttribute(pElement, CComBSTR(L"3dborder"), 0);
//		m_nScrollBar = GetElementAttribute(pElement, CComBSTR(L"scrollbar"), 0);
//		m_nTopMost = GetElementAttribute(pElement, CComBSTR(L"topmost"), 1);
//		m_ClosingTip = GetElementAttribute(pElement, CComBSTR(L"closingtip"), _T(""));
//
//		pElement->Release();
//	}
//
//	//pDocument3->getElementById(CComBSTR(L"tray-menu"), &pElement);
//	//if (pElement != NULL)
//	//{
//	//	m_sTrayIcon = GetElementAttribute(pElement, CComBSTR(L"icon"), _T(""));
//	//	m_sTrayTip = GetElementAttribute(pElement, CComBSTR(L"tip"), _T(""));
//	//	m_sTrayUrl = GetElementAttribute(pElement, CComBSTR(L"url"), _T(""));
//
//	//	pElement->Release();
//	//}
//
//}
//
//void CWebUIController::LoadExternalMethods(IHTMLDocument3 * pDocument3)
//{
//	m_pExtMethods.clear();
//
//	if(pDocument3 == NULL)         
//		return;
//	HRESULT hr = S_OK;
//	IHTMLElementCollection * pElementCollection;
//	pDocument3->getElementsByTagName(CComBSTR(L"ccp:ext:method"), &pElementCollection);
//	if (pElementCollection != NULL)
//	{
//		long nLength = 0;
//		pElementCollection->get_length(&nLength);
//		for (long i=0; i<nLength; i++)
//		{
//			IDispatchPtr pElementDispatch;
//			_variant_t index = i;
//			pElementCollection->item(index, index, (IDispatch**)&pElementDispatch);
//			if (pElementDispatch == NULL)
//				continue;
//
//			IHTMLElementPtr pElement;
//			pElementDispatch->QueryInterface(IID_IHTMLElement,(void**)&pElement);
//			ASSERT( SUCCEEDED( hr ) );
//			ASSERT( pElement );
//
//			if (pElement)
//			{
//				CString sName = GetElementAttribute(pElement, CComBSTR(L"name"), _T(""));
//				CString sApp = GetElementAttribute(pElement, CComBSTR(L"app"), _T(""));
//				CString sMethod = GetElementAttribute(pElement, CComBSTR(L"method"), _T(""));
//
//				CExtMethodInfo::pointer pExtMethodInfo = CExtMethodInfo::create(sName);
//				pExtMethodInfo->m_sAppName = sApp;
//				pExtMethodInfo->m_sAppMethod = sMethod;
//				m_pExtMethods.insert(sName, pExtMethodInfo);
//			}
//		}
//
//		pElementCollection->Release();
//	}
//}
//
//void CWebUIController::LoadCallbacks(IHTMLDocument3 * pDocument3)
//{
//	if(pDocument3 == NULL)         
//		return;
//	HRESULT hr = S_OK;
//	IHTMLElementCollection * pElementCollection;
//	pDocument3->getElementsByTagName(CComBSTR(L"ccp:callback"), &pElementCollection);
//	if (pElementCollection != NULL)
//	{
//		long nLength = 0;
//		pElementCollection->get_length(&nLength);
//		for (long i=0; i<nLength; i++)
//		{
//			IDispatchPtr pElementDispatch;
//			COleVariant varNameNull;
//			pElementCollection->item(varNameNull, COleVariant(i), (IDispatch**)&pElementDispatch);
//			if (pElementDispatch == NULL)
//				continue;
//
//			IHTMLElementPtr pElement;
//			pElementDispatch->QueryInterface(IID_IHTMLElement,(void**)&pElement);
//			ASSERT( SUCCEEDED( hr ) );
//			ASSERT( pElement );
//
//			if (pElement)
//			{
//				CString sCID = GetElementAttribute(pElement, CComBSTR(L"cid"), _T(""));
//				CString sEvent = GetElementAttribute(pElement, CComBSTR(L"event"), _T(""));
//				CString sApp = GetElementAttribute(pElement, CComBSTR(L"app"), _T(""));
//				CString sMethod = GetElementAttribute(pElement, CComBSTR(L"method"), _T(""));
//
//				//CCallbackInfo::pointer pCallbackInfo = CCallbackInfo::create();
//				//pCallbackInfo->m_sCID = sCID;
//				//pCallbackInfo->m_sEvent = sEvent;
//				//pCallbackInfo->m_sAppName = sApp;
//				//pCallbackInfo->m_sAppMethod = sMethod;
//				//m_pCallbacks.push_back(pCallbackInfo);
//			}
//		}
//
//		pElementCollection->Release();
//	}
//}

void CWebUIController::OnDocumentComplete(LPDISPATCH lpDisp) 
{
	if(lpDisp == NULL)         
		return;       
	IWebBrowser2* pWebBrowser2 = NULL; 
	HRESULT hr = lpDisp->QueryInterface(IID_IWebBrowser2, (void**)&pWebBrowser2);        
	if (SUCCEEDED(hr) && pWebBrowser2)   
	{      
		IDispatch* pDoc = NULL;   
		hr = pWebBrowser2->get_Document(&pDoc);       
		if(SUCCEEDED(hr) && pDoc)      
		{
			IHTMLDocument2 * pDocument2 = NULL;
			pDoc->QueryInterface(IID_IHTMLDocument2,(void**)&pDocument2);
			if (pDocument2 != NULL)
			{
				//IHTMLElementCollection * pElementCollection = NULL;
				//pDocument2->get_links(&pElementCollection);
				//if (pElementCollection != NULL)
				//{
				//	long nLength = 0;
				//	pElementCollection->get_length(&nLength);
				//	for (long i=0; i<nLength; i++)
				//	{
				//		IDispatchPtr pElementDispatch;
				//		_variant_t index = i;
				//		pElementCollection->item(index, index, (IDispatch**)&pElementDispatch);
				//		if (pElementDispatch == NULL)
				//			continue;

				//		IHTMLElement* pElement = NULL;
				//		pElementDispatch->QueryInterface(IID_IHTMLElement,(void**)&pElement);
				//		ASSERT( SUCCEEDED( hr ) );
				//		ASSERT( pElement );

				//		if (pElement)
				//		{
				//			CString sRel = GetElementAttribute(pElement, CComBSTR(L"rel"), _T(""));
				//			if (sRel=="shortcut icon")
				//			{
				//				CString sApp = GetElementAttribute(pElement, CComBSTR(L"href"), _T(""));
				//				break;
				//			}
				//		}
				//	}

				//	pElementCollection->Release();
				//}

				CComBSTR bstrTitle;
				pDocument2->get_title(&bstrTitle);
				m_sTitle = bstrTitle;
				pDocument2->Release();
			}

			// SetEventHandler
			IHTMLDocument3 * pDocument3;
			pDoc->QueryInterface(IID_IHTMLDocument3,(void**)&pDocument3);
			if (pDocument3 != NULL)
			{
				LoadFaviconUrl(pDocument3);

			//	// load setting
			//	//LoadCCPSetting(pDocument3);
			//	//LoadExternalMethods(pDocument3);
			//	//LoadCallbacks(pDocument3);

			//	//VARIANT_BOOL pResult;
			//	//CComBSTR bstrEventName(L"onclick");

			//	//IHTMLElementPtr pElement;
			//	//CComBSTR bstrId(L"btnevent");
			//	////CComBSTR bstrId(L"newwin");
			//	//pDocument3->getElementById(bstrId, &pElement);
			//	//if (pElement != NULL)
			//	//{
			//	//	IHTMLElement2 * pElement2;
			//	//	pElement->QueryInterface(IID_IHTMLElement2,(void**)&pElement2);

			//	//	hr = pElement2->attachEvent(bstrEventName, m_eventhandler, &pResult);
			//	//	pElement2->Release();
			//	//}
				pDocument3->Release();
			}
		}        
		pWebBrowser2->Release();  
	} 
}
