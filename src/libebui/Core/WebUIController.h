#ifndef __WEBUICONTROLLER_H__
#define __WEBUICONTROLLER_H__ 
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000 
#include <MSHTMHST.H>
#include <EXDISP.H>
#include <EXDISPID.H> 
#include <mshtml.h>
#include <comdef.h>
#include <comdefsp.h>
#include <dispex.h>

#include "Explorer.h"
#include "HTMLEventHandlerEx.h"

#include <stl/lockmap.h>
#include <CGCBase/cgcvalueinfo.h>
using namespace mycp;

class CCallbackInfo
{
public:
	typedef boost::shared_ptr<CCallbackInfo> pointer;
	static CCallbackInfo::pointer create(void)
	{
		return CCallbackInfo::pointer(new CCallbackInfo());
	}

	CString m_sCID;
	CString m_sEvent;
	CString m_sAppName;
	CString m_sAppMethod;

	CCallbackInfo(void)
	{
	}
};

class CExtMethodInfo
{
public:
	typedef boost::shared_ptr<CExtMethodInfo> pointer;
	static CExtMethodInfo::pointer create(const CString & sExtMethod)
	{
		return CExtMethodInfo::pointer(new CExtMethodInfo(sExtMethod));
	}

	CString m_sExtMethod;
	CString m_sAppName;
	CString m_sAppMethod;

	CExtMethodInfo(const CString & sExtMethod)
		: m_sExtMethod(sExtMethod)
	{
	}
};

class CWebUICallback
{
public:
	virtual void OnStatusTextChange(BSTR sStatusText) {}
	virtual void OnTitleChange(BSTR sTitle) {}
	virtual void OnNewWindow3(IDispatch **ppDisp, VARIANT_BOOL *Cancel, DWORD dwFlags, BSTR bstrUrlContext, BSTR bstrUrl) {}
};


typedef void _stdcall JsExternalMethod_Callback(DWORD dwParam, const CExtMethodInfo::pointer &pExtMethodInfo, const cgcValueInfo::pointer & pInValueInfo);

class CWebUIController : public DWebBrowserEvents2, public IDocHostUIHandler
{    
private:
	LONG m_uRefCount;     
	IWebBrowser2* m_pWebBrowser2;    
	DWORD m_dwCookie;

	JsExternalMethod_Callback * m_pExtMethodCallback;
	DWORD m_dwParam;
	//CHTMLEventHandlerEx * m_eventhandler;
	CWebUICallback * m_pWebUICallback;
public:
	CWebUIController(CWebUICallback * pWebUICallback=NULL);
	virtual ~CWebUIController(void);
	
	//std::vector<CCallbackInfo::pointer> m_pCallbacks;
	CLockMap<CString, CExtMethodInfo::pointer> m_pExtMethods;
	CLockMap<LONG, bool>	m_pDispIds;

	int EnableMax(void) const {return m_nEnableMax;}
	int Width(void) const {return m_nWidth;}
	int Height(void) const {return m_nHeight;}
	int SysToolBar(void) const {return m_nSysToolBar;}
	int TopMost(void) const {return m_nTopMost;}
	const CString & ClosingTip(void) const {return m_ClosingTip;}

	//const CString & TrayIcon(void) const {return m_sTrayIcon;}
	//const CString & TrayIip(void) const {return m_sTrayTip;}
	//const CString & TrayUrl(void) const {return m_sTrayUrl;}

	void SetExtMethodCallback(JsExternalMethod_Callback * pExtMethodCallback, DWORD dwParam) {m_pExtMethodCallback = pExtMethodCallback;m_dwParam=dwParam;}

protected:
	//IUnknown Methods    
	STDMETHOD(QueryInterface)(REFIID riid, void** ppvObject);

	STDMETHOD_(ULONG, AddRef)(void)    
	{        
		InterlockedIncrement(&m_uRefCount);
		return m_uRefCount;
	}    
	STDMETHOD_(ULONG, Release)(void)
	{        
		InterlockedDecrement(&m_uRefCount);
		ULONG uRefCount = m_uRefCount;
		//if(uRefCount == 0)
		//	delete this;
		return uRefCount; 
	}    

	// IDispatch Methods    
	STDMETHOD(GetTypeInfoCount)(unsigned int FAR* pctinfo)   
	{       
		return E_NOTIMPL;
	}         
	STDMETHOD(GetTypeInfo)(UINT itinfo, LCID lcid, ITypeInfo** pptinfo)
	{        
		return E_NOTIMPL;
	}              
	STDMETHOD(GetIDsOfNames)(REFIID riid, OLECHAR FAR* FAR* rgszNames, unsigned int cNames, LCID lcid, DISPID FAR* rgDispId);
	STDMETHOD(Invoke)(DISPID dispidMember,REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS* pDispParams, VARIANT* pvarResult, EXCEPINFO* pexcepinfo, UINT* puArgErr);

	STDMETHOD(QueryStatus)(
		/*[in]*/ const GUID *pguidCmdGroup, 
		/*[in]*/ ULONG cCmds,
		/*[in,out][size_is(cCmds)]*/ OLECMD *prgCmds,
		/*[in,out]*/ OLECMDTEXT *pCmdText)
	{
		return S_OK;    
		//return m_spDefaultOleCommandTarget->QueryStatus(pguidCmdGroup, cCmds,
		//	prgCmds, pCmdText);
	}
	// IDocHostUIHandler Methodsprotected:
	STDMETHOD(Exec)(
		/*[in]*/ const GUID *pguidCmdGroup,
		/*[in]*/ DWORD nCmdID,
		/*[in]*/ DWORD nCmdExecOpt,
		/*[in]*/ VARIANTARG *pvaIn,
		/*[in,out]*/ VARIANTARG *pvaOut)
	{
		if (nCmdID == OLECMDID_SHOWSCRIPTERROR)
		{
			// Don't show the error dialog, but

			// continue running scripts on the page.

			(*pvaOut).vt = VT_BOOL;
			(*pvaOut).boolVal = VARIANT_TRUE;
			return S_OK;
		}
		return S_OK;    
		//return m_spDefaultOleCommandTarget->Exec(pguidCmdGroup, nCmdID,
		//	nCmdExecOpt, pvaIn, pvaOut);
	}
	STDMETHOD(ShowContextMenu)(DWORD dwID, POINT FAR* ppt, IUnknown FAR* pcmdtReserved, IDispatch FAR* pdispReserved)
	{        
		return m_nContextMenu ? E_NOTIMPL : S_OK;    
	}         
	STDMETHOD(GetHostInfo)(DOCHOSTUIINFO FAR* pInfo)
	{        
		if(pInfo != NULL)
		{            
			//pInfo->dwFlags = DOCHOSTUIFLAG_THEME | DOCHOSTUIFLAG_NO3DBORDER | DOCHOSTUIFLAG_SCROLL_NO|DOCHOSTUIFLAG_URL_ENCODING_ENABLE_UTF8;
			pInfo->dwFlags |= (m_n3DBorder ? 0 : DOCHOSTUIFLAG_NO3DBORDER);
			pInfo->dwFlags |= (m_nScrollBar ? 0 : DOCHOSTUIFLAG_SCROLL_NO);
		}         
		return S_OK;    
	}         

	STDMETHOD(ShowUI)(DWORD dwID, IOleInPlaceActiveObject FAR* pActiveObject, IOleCommandTarget FAR* pCommandTarget, IOleInPlaceFrame  FAR* pFrame, IOleInPlaceUIWindow FAR* pDoc)    
	{        
		return E_NOTIMPL;    
	}         

	STDMETHOD(HideUI)(void)    
	{       
		return E_NOTIMPL;
	}      
	STDMETHOD(UpdateUI)(void)  
	{       
		return E_NOTIMPL;  
	}        
	STDMETHOD(EnableModeless)(BOOL fEnable) 
	{        
		return E_NOTIMPL; 
	}    
	STDMETHOD(OnDocWindowActivate)(BOOL fActivate)  
	{      
		return E_NOTIMPL; 
	}    
	STDMETHOD(OnFrameWindowActivate)(BOOL fActivate) 
	{       
		return E_NOTIMPL; 
	}    
	STDMETHOD(ResizeBorder)(LPCRECT prcBorder, IOleInPlaceUIWindow FAR* pUIWindow, BOOL fRameWindow)   
	{       
		return E_NOTIMPL; 
	}    
	STDMETHOD(TranslateAccelerator)(LPMSG lpMsg, const GUID FAR* pguidCmdGroup, DWORD nCmdID)    
	{       
		return E_NOTIMPL;  
	}     
	STDMETHOD(GetOptionKeyPath)(LPOLESTR FAR* pchKey, DWORD dw)  
	{       
		return E_NOTIMPL; 
	}   
	STDMETHOD(GetDropTarget)(IDropTarget* pDropTarget, IDropTarget** ppDropTarget)   
	{       
		return E_NOTIMPL;  
	}    
	STDMETHOD(GetExternal)(IDispatch** ppDispatch) 
	{
		*ppDispatch = (IDispatch*)this;
		return S_OK;
		return E_NOTIMPL;
	}    
	STDMETHOD(TranslateUrl)(DWORD dwTranslate, OLECHAR* pchURLIn, OLECHAR** ppchURLOut) 
	{      
		return E_NOTIMPL;  
	} 
	STDMETHOD(FilterDataObject)(IDataObject* pDO, IDataObject** ppDORet)  
	{   
		return E_NOTIMPL;   
	} 

public:  
	bool SetWebBrowser(CExplorer * pIE)
	{
		LPUNKNOWN pUnk = pIE->GetControlUnknown();
		if(pUnk != NULL)
		{
			IWebBrowser2* pWB2 = NULL;
			HRESULT hr = pUnk->QueryInterface(IID_IWebBrowser2, (void**)&pWB2);
			if(SUCCEEDED(hr) && pWB2 != NULL)
			{
				SetWebBrowser(pWB2);
				pWB2->Release();
				return true;
			}
		}
		return false;
	}

	HRESULT SetWebBrowser(IWebBrowser2* pWebBrowser2);
	void ClearWebBrowser(void);
	void EnableContextMenu(int bEnable = 1) {m_nContextMenu = bEnable;}
	void Enable3DBorder(int bEnable = 1) {m_n3DBorder = bEnable;}
	void EnableScrollBar(int bEnable = 1) {m_nScrollBar = bEnable;}
	const CString & GetTitle(void) const {return m_sTitle;}
	const CString & GetFaviconUrl(void) const {return m_sFaviconUrl;}

private:
	int	m_nAutoSize;
	int m_nEnableMax;
	int m_nWidth;
	int m_nHeight;
	int m_nSysToolBar;
	int m_nContextMenu;
	int m_n3DBorder;
	int m_nScrollBar;
	int m_nTopMost;
	CString m_ClosingTip;
	CString m_sTitle;
	CString m_sFaviconUrl;

	//CString m_sTrayIcon;
	//CString m_sTrayTip;
	//CString m_sTrayUrl;

	void LoadFaviconUrl(IHTMLDocument3 * pDocument3);
	//void LoadCCPSetting(IHTMLDocument3 * pDocument3);
	//void LoadExternalMethods(IHTMLDocument3 * pDocument3);
	//void LoadCallbacks(IHTMLDocument3 * pDocument3);
	//int GetElementAttribute(IHTMLElement* pElement, BSTR strAttributeName, int nDefault) const;
	CString GetElementAttribute(IHTMLElement* pElement, BSTR strAttributeName, LPCTSTR sDefault) const;

	void OnDocumentComplete(LPDISPATCH lpDisp);

	void SetCustomDoc(LPDISPATCH lpDisp)   
	{      
		if(lpDisp == NULL)         
			return;       
		IWebBrowser2* pWebBrowser2 = NULL; 
		HRESULT hr = lpDisp->QueryInterface(IID_IWebBrowser2, (void**)&pWebBrowser2);        
		if(SUCCEEDED(hr) && pWebBrowser2)   
		{      
			IDispatch* pDoc = NULL;   
			hr = pWebBrowser2->get_Document(&pDoc);       
			if(SUCCEEDED(hr) && pDoc)      
			{
				// SetEventHandler
				//IHTMLDocument3 * pDocument3;
				//pDoc->QueryInterface(IID_IHTMLDocument3,(void**)&pDocument3);
				//if (pDocument3 != NULL)
				//{
				//	VARIANT_BOOL pResult;
				//	CComBSTR bstrEventName(L"onclick");
				//	hr = pDocument3->attachEvent(bstrEventName, m_eventhandler, &pResult);
				//	pDocument3->Release();
				//}

				// SetUIHandler
				ICustomDoc* pCustDoc = NULL;    
				hr = pDoc->QueryInterface(IID_ICustomDoc, (void**)&pCustDoc); 
				if(SUCCEEDED(hr) && pCustDoc != NULL)    
				{                      
					pCustDoc->SetUIHandler(this);   
					pCustDoc->Release();       
				}              
				pDoc->Release();    
			}        
			pWebBrowser2->Release();  
		}   
	}
}; 
#endif // __WEBUICONTROLLER_H__