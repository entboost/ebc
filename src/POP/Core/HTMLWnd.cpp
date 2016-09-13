//writed by Angus He (angushe@gmail.com)

// HTMLWnd.cpp : 实现文件
//

#include "stdafx.h"
//#include "resource.h"
#include "HTMLWnd.h"
#include <exdispid.h>
#include "webbrowser2.h"

//#include "ChineseCodeLib.h"

#pragma warning(disable:4100 4018)
// CHTMLWnd

IMPLEMENT_DYNCREATE(CHTMLWnd, CWnd)
CHTMLWnd::CHTMLWnd()
:m_pHTMLDocument(NULL)//,m_pHTMLBodyElement(NULL)
{
	m_bAutoScroll = false;
	m_nBlockLimit = 100;
	m_nBlockWrited = 0;
	m_pHTMLDocument = 0;
	m_spHTMLWnd = 0;
	//m_pHTMLBodyElement = 0;
	m_bReady = FALSE;
	m_nBackupCount = 0;
	m_bLastUnicode = FALSE;
}

CHTMLWnd::~CHTMLWnd()
{
	m_bReady = FALSE;
}


BEGIN_MESSAGE_MAP(CHTMLWnd, CWnd)
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
	ON_WM_SIZE()
	//ON_COMMAND(IDM_COPY, OnCopy)
	//ON_COMMAND(IDM_SELECTALL, OnSelectall)
	//ON_COMMAND(IDM_SAVEAS, OnSaveas)
	//ON_WM_PAINT()
	ON_WM_DESTROY()
END_MESSAGE_MAP()

BEGIN_EVENTSINK_MAP(CHTMLWnd,CWnd)
	ON_EVENT(CHTMLWnd,ID_HTML_WND,250,BeforeNavigate2,VTS_DISPATCH VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PBOOL)
	ON_EVENT(CHTMLWnd,ID_HTML_WND,252,NavigateComplete2,VTS_DISPATCH VTS_PVARIANT)
	ON_EVENT(CHTMLWnd,ID_HTML_WND,259,DocumentComplete,VTS_DISPATCH VTS_PVARIANT)
END_EVENTSINK_MAP()

// CHTMLWnd 消息处理程序


int CHTMLWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	m_bReady = FALSE;
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// CS_HREDRAW|CS_VREDRAW;

#ifdef USE_NEW_WEBBROWSER
	m_Output = new CWebBrowser2();
	if(!m_Output->Create(NULL,WS_VISIBLE|WS_CHILD|WS_CLIPSIBLINGS|WS_CLIPCHILDREN,CRect(0,0,0,0),this,ID_HTML_WND))
#else
	//if (!m_Output.Create(NULL,WS_VISIBLE|WS_CHILD,CRect(0,0,0,0),this,ID_HTML_WND))		// 龙帝测试，确实有灰
	//if(!m_Output.Create(NULL,WS_VISIBLE|WS_CHILD|WS_CLIPSIBLINGS,CRect(0,0,0,0),this,ID_HTML_WND)) // 176版本
	if(!m_Output.Create(NULL,WS_VISIBLE|WS_CHILD|WS_CLIPSIBLINGS|WS_CLIPCHILDREN,CRect(0,0,0,0),this,ID_HTML_WND)) // 178版本
#endif
	{
		return -1;
	}
	// 没用
	//m_Output.put_Offline(FALSE);
	//m_Output.put_RegisterAsBrowser(FALSE);
	//m_Output.put_RegisterAsDropTarget(FALSE);
	//m_Output.put_TheaterMode(FALSE);
	//m_Output.put_Silent(TRUE);
	//create an empty document
#ifdef USE_NEW_WEBBROWSER
	m_Output->Navigate(_T("About:Blank"),NULL,NULL,NULL,NULL);
#else
	m_Output.Navigate(_T("About:Blank"),NULL,NULL,NULL,NULL);
#endif
	//m_Output.Navigate(_T("http://cn.yahoo.com/"),NULL,NULL,NULL,NULL);
	
	//InitWeb();
	return 0;
}


void CHTMLWnd::OnSetFocus(CWnd* pOldWnd)
{
	CWnd::OnSetFocus(pOldWnd);
#ifdef USE_NEW_WEBBROWSER
	m_Output->SetFocus();
#else
	m_Output.SetFocus();
#endif
	// 没有执行
	//AfxMessageBox(_T("OnSetFocus"));
	//if (m_spHTMLWnd != NULL)
	//	m_spHTMLWnd->focus();

}

void CHTMLWnd::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);
	if(nType != SIZE_MINIMIZED)
	{
#ifdef USE_NEW_WEBBROWSER
		m_Output->MoveWindow(0,0,cx,cy);
#else
		m_Output.MoveWindow(0,0,cx,cy);
#endif
	}
}

CString CHTMLWnd::Convert(const CString & str, int sourceCodepage, int targetCodepage) 
{ 
	CString aa=_T("");
/*
	CChineseCodeLib lib;
	CString strRet;

	lib.GB2312ToUTF_8( strRet , (char*)str.GetBuffer(str.GetLength()) , str.GetLength() );
	return strRet;
*/
/*
	int len=str.GetLength(); 

	int unicodeLen=MultiByteToWideChar(sourceCodepage,0,str,-1,NULL,0); 

	wchar_t * pUnicode; 
	pUnicode=new wchar_t[unicodeLen+1]; 

	memset(pUnicode,0,(unicodeLen+1)*sizeof(wchar_t)); 


	MultiByteToWideChar(sourceCodepage,0,str,-1,(LPWSTR)pUnicode,unicodeLen); 

	BYTE * pTargetData=NULL; 
	int targetLen=WideCharToMultiByte(targetCodepage,0,(LPWSTR)pUnicode,-1,(char *)pTargetData,0,NULL,NULL); 

	pTargetData=new BYTE[targetLen+1]; 
	memset(pTargetData,0,targetLen+1); 

	WideCharToMultiByte(targetCodepage,0,(LPWSTR)pUnicode,-1,(char *)pTargetData,targetLen,NULL,NULL); 

	CString rt; 
	rt.Format(_T("%s"),pTargetData); 

	delete pUnicode; 
	delete pTargetData; 
	return rt; 
*/
	return aa;
} 


//BSTR CHTMLWnd::ToBSTR(const CString& str,BOOL bUnicode/* =FALSE */)
//{
//	USES_CONVERSION;
//	wchar_t* pw = NULL;
//	BSTR bstr;
//	if(!bUnicode)
//	{
//		//CString strTemp = Convert( str , CP_ACP ,CP_UTF8 );
//		//bstr = strTemp.AllocSysString();
//
//
///*
//		pw = new wchar_t[str.GetLength()*2];
//		int nRet = MultiByteToWideChar(CP_ACP,0,str,str.GetLength(),pw,str.GetLength()*2);
//		
//		pw[nRet] = 0;
//
//		bstr = OLE2BSTR(pw);
//		delete[] pw;
//		pw = NULL;
//*/		
//	}
//	else
//	{
//		pw = new wchar_t[str.GetLength()+1];
//		::wcscpy(pw,(const wchar_t*)(LPCTSTR)str);
//		bstr = OLE2BSTR(pw);
//		delete[] pw;
//		pw = NULL;
//	}
//
//	return bstr;
//}

int CHTMLWnd::WriteHTML(const CString& str,BOOL bUnicode)
{
	if (!m_bReady)
		return -1;
	TRY
	{
		ASSERT(m_pHTMLDocument != NULL);
		BOOL bReset = FALSE;
		HRESULT hr = S_OK;
		VARIANT * theParam = NULL;
		SAFEARRAY * thesfArray = NULL;
		if (theParam == NULL)
		{
			thesfArray = SafeArrayCreateVector(VT_VARIANT, 0, 1);
			if (thesfArray == NULL)
				return -1;

			hr = ::SafeArrayAccessData(thesfArray,(LPVOID*) &theParam);
			if (theParam == NULL)
			{
				SafeArrayDestroy(thesfArray);
				thesfArray = NULL;
				return -1;
			}
		}
		BSTR bstr = NULL;
		if (bUnicode)
		{
			USES_CONVERSION;
			std::wstring a = A2W_UTF8(str);
			bstr = ::SysAllocString(a.c_str());
		}else
		{
			bstr = str.AllocSysString();
		}
		theParam->vt = VT_BSTR;
		theParam->bstrVal = bstr;
		hr = m_pHTMLDocument->write(thesfArray);
		if (FAILED(hr))
		{
			bReset = TRUE;
		}

		::SysFreeString(bstr);
		//SafeArrayUnaccessData(thesfArray);
		//SafeArrayDestroy(thesfArray);
		if (bReset)
		{
			m_bReady = FALSE;
#ifdef USE_NEW_WEBBROWSER
			m_Output->Navigate(_T("About:Blank"),NULL,NULL,NULL,NULL);
#else
			m_Output.Navigate(_T("About:Blank"),NULL,NULL,NULL,NULL);
#endif
			//IHTMLElementPtr pElement(m_pHTMLBodyElement);		//这2行是有用的，把所有数据都清空
			//hr = pElement->put_innerHTML(NULL);
			//if (FAILED(hr))
			//{
			//	InitWeb();
			//}
			return -1;
		}

		if (m_bAutoScroll)
		{
			ScrollBy();
		}
		////没用
		//if (m_spHTMLWnd != NULL)
		//	m_spHTMLWnd->focus();
		// m_Output.SetFocus();
		// m_Output.Invalidate();

		////if(m_bAutoScroll && (m_pHTMLBodyElement != NULL))
		//{
		//	//IHTMLElement2 *pElement2 = NULL;
		//	//hr = m_pHTMLBodyElement.QueryInterface( IID_IHTMLElement2, (void **)&pElement2);
		//	//if (SUCCEEDED(hr) && pElement2)
		//	//{
		//	//	pElement2->put_scrollTop(20000000);
		//	//}
		//	/*IHTMLElementPtr pElem;
		//	m_pHTMLDocument->get_body(&pElem);
		//	IHTMLBodyElementPtr pBodyElem(pElem);
		//	IHTMLTxtRangePtr pTxtRange;
		//	pBodyElem->createTextRange(&pTxtRange);
		//	pTxtRange->scrollIntoView(VARIANT_FALSE);*/
		//	//m_spHTMLWnd->scrollTo(0,SHRT_MAX/*LONG_MAX*/);
		//}
	}
	CATCH(CResourceException, e);
	{
		e->Delete();
	}
	END_CATCH
	return 0;
}

void CHTMLWnd::ScrollBy(void)
{
	if (!m_bAutoScroll)
		return;
	//if(m_spHTMLWnd != NULL)
	//{
	//	m_spHTMLWnd->scrollBy(0, 10000);
	//	m_spHTMLWnd->scrollBy(0, 10000);
	//}

	if (m_pHTMLBodyElement)
	{
		IHTMLElement2 *pElement2 = NULL;
		HRESULT hr = m_pHTMLBodyElement->QueryInterface( IID_IHTMLElement2, (void **)&pElement2);
		if (SUCCEEDED(hr) && pElement2)
		{
			pElement2->put_scrollTop(20000000);
			pElement2->Release();
		}
	}
}
void CHTMLWnd::SetBrowserFocus(void)
{
#ifdef USE_NEW_WEBBROWSER
	if (m_Output!=NULL && m_Output->GetSafeHwnd()!=NULL)
		m_Output->SetFocus();
#else
	if (m_Output.GetSafeHwnd()!=NULL)
		m_Output.SetFocus();
#endif
}


void CHTMLWnd::ClearBlock(  )
{
	if ( !m_bReady)
		return;

	m_pBackupList.RemoveAll();

    HRESULT hr= E_FAIL;
	if (m_pHTMLBodyElement)
	{
		m_bReady = FALSE;
#ifdef USE_NEW_WEBBROWSER
		m_Output->Navigate(_T("About:Blank"),NULL,NULL,NULL,NULL);
#else
		m_Output.Navigate(_T("About:Blank"),NULL,NULL,NULL,NULL);
#endif
		//IHTMLElementPtr pElement(m_pHTMLBodyElement);		//这2行是有用的，把所有数据都清空
		//hr = pElement->put_innerHTML(NULL);
		//if (FAILED(hr))
		//{
		//	InitWeb();
		//}
		//CComQIPtr<IHTMLElement2> pElement2(m_pHTMLBodyElement);
		//BSTR bstr = ::SysAllocString(OLESTR("DIV"));
		//IHTMLElementCollectionPtr pCollect;
		//pElement2->getElementsByTagName(bstr,&pCollect);
		//::SysFreeString(bstr);
		//long len = 0;
		//pCollect->get_length(&len);
		//if (len > 0)
		//{
		//	/************************************************************************/
		//	/* 以下是一些测试，清空部分数据                                         */
		//	/************************************************************************/

		//	for( int i = 0 ; i < len ; i ++ )
		//	{
		//		IDispatchPtr pDispath;
		//		_variant_t index((long)i);
		//		if(SUCCEEDED(pCollect->item(index,index,&pDispath)))
		//		{
		//			IHTMLElementPtr pFirstElement(pDispath);
		//			if(pFirstElement)
		//			{
		//				pFirstElement->put_outerHTML(NULL);
		//			}
		//		}
		//	}
		//}
	}
}

int CHTMLWnd::WriteBlock(const CString& str,BOOL bUnicode/* =FALSE */,BOOL bAppTimestamp)
{
	//if ( !m_bReady)
	//{
	//	m_sLastHtml = str;
	//	return -1;
	//}

	bool bClearHtml = false;
    HRESULT hr= E_FAIL;
//	if (m_pHTMLBodyElement)
//	{
//		static int staticPutOuterNullIndex = 0;
//		if ((staticPutOuterNullIndex++) % m_nBlockLimit == (m_nBlockLimit-1))
//		{
//			bClearHtml = true;
//			m_sLastHtml = str;
//			m_bReady = FALSE;
//#ifdef USE_NEW_WEBBROWSER
//			m_Output->Navigate(_T("About:Blank"),NULL,NULL,NULL,NULL);
//#else
//			m_Output.Navigate(_T("About:Blank"),NULL,NULL,NULL,NULL);
//#endif
//			//IHTMLElementPtr pElement(m_pHTMLBodyElement);		//这2行是有用的，把所有数据都清空
//			//hr = pElement->put_innerHTML(NULL);
//			return 1;
//		}
//		//if ((staticPutOuterNullIndex++) % 50 == 1)
//		//{
//		//	// 不用每次做判断，用于性能提升
//		//	CComQIPtr<IHTMLElement2> pElement2(m_pHTMLBodyElement);
//		//	BSTR bstr = ::SysAllocString(OLESTR("DIV"));
//		//	IHTMLElementCollectionPtr pCollect;
//		//	pElement2->getElementsByTagName(bstr,&pCollect);
//		//	::SysFreeString(bstr);
//		//	long len = 0;
//		//	pCollect->get_length(&len);
//		//	if (len >= m_nBlockLimit)
//		//	{
//		//		IHTMLElementPtr pElement(m_pHTMLBodyElement);		//这2行是有用的，把所有数据都清空
//		//		hr = pElement->put_innerHTML(NULL);
//		//		/************************************************************************/
//		//		/* 以下是一些测试，清空部分数据                                         */
//		//		/************************************************************************/
//
//		//		//for( int i = 0 ; i < m_nBlockLimit/2 ; i ++ )
//		//		//{
//		//		//	IDispatchPtr pDispath;
//		//		//	_variant_t index((long)0); //修改by yuxin
//		//		//	if(SUCCEEDED(pCollect->item(index,index,&pDispath)))
//		//		//	{
//		//		//		IHTMLElementPtr pFirstElement(pDispath);
//		//		//		if(pFirstElement)
//		//		//		{
//		//		//			pFirstElement->put_outerHTML(NULL);
//		//		//		}
//		//		//	}
//		//		//}
//		//	}
//		//}
//	}
	

    /*
	if(m_nBlockLimit>0 && m_nBlockWrited>=m_nBlockLimit)
	{
		ASSERT(m_pHTMLBodyElement!=NULL);			
		IHTMLElementPtr pElement;
		m_pHTMLBodyElement->QueryInterface(IID_IHTMLElement,(void**)&pElement);
		CComQIPtr<IHTMLElement2> pElement2(m_pHTMLBodyElement);
		BSTR bstr = ::SysAllocString(L"DIV");
		IHTMLElementCollectionPtr pCollect;
		pElement2->getElementsByTagName(bstr,&pCollect);
		::SysFreeString(bstr);
		long len = 0;
		pCollect->get_length(&len);
		if(len > 0)
		{
			_variant_t index((long)0);
			IDispatchPtr pDispath;
			if(SUCCEEDED(pCollect->item(index,index,&pDispath)))
			{
				IHTMLElementPtr pFirstElement(pDispath);
				if(pFirstElement)
				{
					pFirstElement->put_outerHTML(NULL);
					m_nBlockWrited--;
				}
			}
		}
	}
	*/

	CString strOut = _T("<DIV style=\"WIDTH: 100%; WORD-WRAP: break-word\" align=\"left\">") + str;
	if(bAppTimestamp)
	{
		//  这里是添加聊天信息的时间
		CTime time(CTime::GetCurrentTime());
		strOut += time.Format(_T("<font color=#C0C0C0 style=\"FONT-SIZE:10pt\" > %H:%M:%S</font>"));
	}
	strOut += _T("</DIV>");
	if ( !m_bReady)
	{
		m_bLastUnicode = bUnicode;
		m_sLastHtml = strOut;
		return -1;
	}

	if (m_nBackupCount > 0)
	{
		m_pBackupList.AddTail(strOut);
		if (m_pBackupList.GetCount() > m_nBackupCount)
		{
			m_pBackupList.RemoveHead();
		}
	}
	if (m_pHTMLBodyElement)
	{
		static int staticPutOuterNullIndex = 0;
		if (((staticPutOuterNullIndex++) % m_nBlockLimit) == (m_nBlockLimit-1))
		{
			bClearHtml = true;
			//m_sLastHtml = str;
			m_bReady = FALSE;
#ifdef USE_NEW_WEBBROWSER
			m_Output->Navigate(_T("About:Blank"),NULL,NULL,NULL,NULL);
#else
			m_Output.Navigate(_T("About:Blank"),NULL,NULL,NULL,NULL);
#endif
			//IHTMLElementPtr pElement(m_pHTMLBodyElement);		//这2行是有用的，把所有数据都清空
			//hr = pElement->put_innerHTML(NULL);
			//if (FAILED(hr))
			//{
			//	InitWeb();
			//}
			return 1;
		}
	}

	//if (m_nBackupCount > 0 && bClearHtml)
	//{
	//	POSITION pos = m_pBackupList.GetHeadPosition();
	//	while (NULL != pos)
	//	{
	//		WriteHTML(m_pBackupList.GetNext(pos), FALSE);
	//	}
	//}else if (!bClearHtml)
	//{
	//	WriteHTML(strOut,bUnicode);
	//}
	WriteHTML(strOut,bUnicode);
	m_nBlockWrited++;
	return 0;
}

void CHTMLWnd::WriteBlocks(const CStringArray& strArray,BOOL bUnicode/* =FALSE */,BOOL bAppTimestamp/* = FALSE  */)
{
	if (!m_bReady)
		return;

     HRESULT hr = E_FAIL;
	 CString strOut;
	 int i = 0;

	INT_PTR size = strArray.GetSize();
	int capacity = m_nBlockWrited + size - m_nBlockLimit;
	if (m_pHTMLBodyElement)
	{
		CComQIPtr<IHTMLElement2> pElement2(m_pHTMLBodyElement);
		BSTR bstr = ::SysAllocString(OLESTR("DIV"));
		CComPtr<IHTMLElementCollection> pCollect;
		pElement2->getElementsByTagName(bstr,&pCollect);
		::SysFreeString(bstr);
		long len = 0;
		pCollect->get_length(&len);
		if (len + size >= m_nBlockLimit)
		{
			m_Output.Navigate(_T("About:Blank"),NULL,NULL,NULL,NULL);
			//CComPtr<IHTMLElement> pElement(m_pHTMLBodyElement);
			//hr = pElement->put_innerHTML(NULL);
		}
	}
    /*
	if(m_nBlockLimit>0 && capacity > 0)
	{
		ASSERT(m_pHTMLBodyElement!=NULL);			
		//IHTMLElementPtr pElement;
		//m_pHTMLBodyElement->QueryInterface(IID_IHTMLElement,(void**)&pElement);
		CComQIPtr<IHTMLElement2> pElement2(m_pHTMLBodyElement);
		BSTR bstr = ::SysAllocString(L"DIV");
		IHTMLElementCollectionPtr pCollect;
		pElement2->getElementsByTagName(bstr,&pCollect);
		::SysFreeString(bstr);
		long len = 0;
		pCollect->get_length(&len);
		if(len > 0)
		{
			if(size > m_nBlockLimit)
			{
				IHTMLElementPtr element(m_pHTMLBodyElement);
				element->put_innerHTML(NULL);
			}	
			else
			{
				for(int i=0;i<capacity; i++)
				{
					_variant_t index((long)0);
					IDispatchPtr pDispath;
					if(SUCCEEDED(pCollect->item(index,index,&pDispath)))
					{
						IHTMLElementPtr pFirstElement(pDispath);
						if(pFirstElement)
						{
							pFirstElement->put_outerHTML(NULL);
							m_nBlockWrited--;
						}
					}
				}
			}
		}

	}
	if(size > m_nBlockLimit)
	{
		m_nBlockWrited = m_nBlockLimit;
		i = size - m_nBlockLimit;
	}
	else
	{
		m_nBlockWrited += size;
		i = 0;
	}
	*/

	for(i=0;i<size; i++)
	{
		strOut += _T("<DIV align=\"left\">") + strArray[i];
		if(bAppTimestamp)
		{
			CTime time(CTime::GetCurrentTime());
			/*strOut += time.Format(_T("<font color=#FF00FF>[%Y-%m-%d %H-%M-%S]</font>"));*/
			strOut += time.Format(_T("<font color=#FF00FF style=\"FONT-SIZE:12px\">[%H:%M:%S]</font>"));
		}
		strOut += _T("</DIV>");
	}
	WriteHTML(strOut,bUnicode);
}


void CHTMLWnd::WriteChatMsg(const CString& strSrcUserName,UINT nSrcUserID,
							const CString& strDest,UINT nDestUserID,const CString& strMsg)
{
	CString str;
	str.Format(_T("<a href=%u>%s</a>对<a href=%u>%s</a>说:%s"),nSrcUserID,strSrcUserName,
		nDestUserID,strDest,strMsg);
	BOOL bUnicode = FALSE;
#if (defined(_UNICODE) || defined(UNICODE))
	bUnicode = TRUE;
#endif
	WriteBlock(str,bUnicode);
}

void CHTMLWnd::BeforeNavigate2(LPDISPATCH pDisp, VARIANT* URL, VARIANT* Flags, VARIANT* TargetFrameName, VARIANT* PostData, VARIANT* Headers, VARIANT_BOOL* Cancel)
{
	ASSERT(URL->vt == VT_BSTR);
	CString str(URL->bstrVal);
	OnBeforeNavigate(Cancel,str);
}

void CHTMLWnd::NavigateComplete2(LPDISPATCH pDisp, VARIANT* URL)
{
	ASSERT(URL->vt == VT_BSTR);
	CString str(URL->bstrVal);
	OnNavigateComplete2(str);
}

void CHTMLWnd::DocumentComplete(LPDISPATCH pDisp, VARIANT* URL)
{
	ASSERT(URL->vt == VT_BSTR);
	CString str(URL->bstrVal);
	OnDocumentComplete(str);
}

BOOL CHTMLWnd::SetBkground(COLORREF crBk)
{
	ASSERT(m_pHTMLBodyElement!=NULL);
	long lColor = GetBValue(crBk)| GetGValue(crBk)<<8 | GetRValue(crBk)<<16;
	CComVariant value(lColor,VT_I4);
	return m_pHTMLBodyElement->put_bgColor(value)==S_OK;
}

BOOL CHTMLWnd::SetBkground(const CString& strBk,BOOL bFixed)
{
	ASSERT(m_pHTMLBodyElement!=NULL);
	BSTR bstr = strBk.AllocSysString();
	BOOL bRet = m_pHTMLBodyElement->put_background(bstr)==S_OK;
	::SysFreeString(bstr);
	if(bRet)
	{
		if(bFixed)
		{
			bstr = ::SysAllocString(OLESTR("fixed"));
		}
		else
		{
			bstr = NULL;
		}
		bRet = m_pHTMLBodyElement->put_bgProperties(bstr)==S_OK;
		::SysFreeString(bstr);
	}

	return bRet;
}

/*
void CHTMLWnd::ShowContextMenu()
{
	if(m_ContextMenu.GetSafeHmenu())
	{
		POINT pt;
		GetCursorPos(&pt);
		m_ContextMenu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN|TPM_LEFTBUTTON,pt.x,pt.y,this,NULL);
	}
}
*/

void CHTMLWnd::OnCopy()
{
	CComPtr<IDispatch> spDisp = m_pHTMLDocument;
	if (spDisp != NULL)
	{
		CComQIPtr<IOleCommandTarget> spTarget = spDisp;
		if (spTarget != NULL)
			spTarget->Exec(NULL, OLECMDID_COPY, 0, NULL, NULL);
	}

	//m_Output.ExecWB(OLECMDID_COPY,OLECMDEXECOPT_DONTPROMPTUSER,NULL,NULL); //not work
}

void CHTMLWnd::OnSelectall()
{
	//m_Output.ExecWB(OLECMDID_SELECTALL,OLECMDEXECOPT_DONTPROMPTUSER,NULL,NULL);
	CComPtr<IDispatch> spDisp = m_pHTMLDocument;
	if (spDisp != NULL)
	{
		CComQIPtr<IOleCommandTarget> spTarget = spDisp;
		if (spTarget != NULL)
			spTarget->Exec(NULL, OLECMDID_SELECTALL, 0, NULL, NULL);
	}
}

void CHTMLWnd::OnSaveas()
{
#ifdef USE_NEW_WEBBROWSER
	m_Output->ExecWB(OLECMDID_SAVEAS,OLECMDEXECOPT_DONTPROMPTUSER,NULL,NULL);
#else
	m_Output.ExecWB(OLECMDID_SAVEAS,OLECMDEXECOPT_DONTPROMPTUSER,NULL,NULL);
#endif
}


void CHTMLWnd::Reset()
{
	m_pBackupList.RemoveAll();
	m_nBackupCount = 0;

	//test code
	m_bReady = FALSE;
#ifdef USE_NEW_WEBBROWSER
	m_Output->Navigate(_T("About:Blank"),NULL,NULL,NULL,NULL);
#else
	m_Output.Navigate(_T("About:Blank"),NULL,NULL,NULL,NULL);
#endif
	//if (m_pHTMLBodyElement)
	//{
	//	IHTMLElementPtr pElement(m_pHTMLBodyElement);		//这2行是有用的，把所有数据都清空
	//	HRESULT hr = pElement->put_innerHTML(NULL);
	//	if (FAILED(hr))
	//	{
	//		InitWeb();
	//	}
	//}
	return;
   
	////
	//HRESULT hr;
	//m_nBlockWrited = 0;
	//if (m_pHTMLBodyElement) {
	//	ASSERT(m_pHTMLBodyElement!=NULL);
	//	IHTMLElementPtr pElement(m_pHTMLBodyElement);
	//	hr = pElement->put_innerHTML(NULL);
	//	if (FAILED(hr))
	//	{
	//		InitWeb();
	//	}
	//}
}

void CHTMLWnd::InitWeb()
{
	//m_nBlockWrited = 0;
	//m_pHTMLBodyElement = 0;
	m_pHTMLBodyElement.Release();
	m_pHTMLDocument.Release();
	m_pHTMLBodyElement.Release();
	m_spHTMLWnd.Release();
	//if (m_pHTMLDocument) {
	//	m_pHTMLDocument->Release();
	//	m_pHTMLDocument = NULL;
	//}
	//if (m_pHTMLBodyElement) {
	//	m_pHTMLBodyElement->Release();
	//	m_pHTMLBodyElement = NULL;
	//}
	//if (m_spHTMLWnd) {
	//	m_spHTMLWnd.Release();
	//	m_spHTMLWnd = NULL;
	//}

#ifdef USE_NEW_WEBBROWSER
	m_pHTMLDocument = m_Output->get_Document();
#else
	m_pHTMLDocument = (IHTMLDocument2*)m_Output.get_Document();
#endif
	ASSERT(m_pHTMLDocument != NULL);

	//CString str = _T("UTF-8");
	CString str = _T("_autodetect");
	BSTR bstr = str.AllocSysString( );
	m_pHTMLDocument->put_charset( bstr );
	::SysFreeString(bstr);

	//BSTR * pbstr;
	//m_pHTMLDocument->get_charset( &bstr );

	m_pHTMLDocument->get_parentWindow(&m_spHTMLWnd);

    m_bReady = TRUE;
	OnBeforeCreateBody();
	m_pHTMLDocument->get_body((IHTMLElement**)&m_pHTMLBodyElement);

	//if(!m_pHTMLBodyElement)
	//{
	//	OnBeforeCreateBody();
	//	m_pHTMLDocument->get_body((IHTMLElement**)&m_pHTMLBodyElement);
	//}
	//m_pHTMLBodyElement = pElement;
	ASSERT(m_pHTMLBodyElement != NULL);
	//m_ContextMenu.LoadMenu(IDR_MENU_CHAT_VIEW);

	POSITION pos = m_pBackupList.GetHeadPosition();
	while (NULL != pos)
	{
		WriteHTML(m_pBackupList.GetNext(pos), FALSE);
	}
	m_pBackupList.RemoveAll();
	if (!m_sLastHtml.IsEmpty())
	{
		const BOOL bUnicode = m_bLastUnicode;
		const CString strHtml = m_sLastHtml;
		m_sLastHtml = _T("");
		m_bLastUnicode = FALSE;
		//WriteBlock(strHtml, FALSE, FALSE);
		WriteHTML(strHtml, bUnicode);
	}
}

void CHTMLWnd::OnDocumentComplete(LPCTSTR szURL)
{
	//TestSetHTMLInDocument();
	TRY
	{
		InitWeb();
	}
	CATCH(CResourceException, e);
	{
		e->Delete();
	}
	END_CATCH
}

bool CHTMLWnd::TestSetHTMLInDocument()
{
	static int iCounter=0;
	iCounter++;
	CString csHTML;

	// Preparing the HTML page to be shown...
	csHTML.Format(_T("<html><body bgcolor=\"7AA1E6\" leftmargin=\"0\" topmargin=\"0\" rightmargin=\"0\" bottommargin=\"0\" oncontextmenu=\"return false\"><table width=\"100%%\" bgcolor=\"7AA1E6\" align=\"left\" cellspacing=\"6\"><tr><td bgcolor=\"D6DFF7\" bordercolor=\"#FFFFFF\"><table width=\"100%%\"><tr><td bgcolor=\"#FFFFFF\"><img src=\"rang.png\" width=\"16\" height=\"16\"> Actual value</td></tr><tr><td><font size=\"2\">Now the value is: <b>%i</b>.</font></td></tr></table></td></tr></table></body></html>"),
		iCounter);

	SAFEARRAY *pSafeArray = NULL;
	VARIANT *pElement = NULL;
	CComPtr<IHTMLDocument2> pHTMLDoc2;
	LPDISPATCH lpDispatch = NULL;
	HRESULT hr;
	bool bRetorn = false;


	//pHTMLDoc2->elementFromPoint(

	// Processat del document HTML, assignaci?de punters
	// i escriptura del contingut de la cadena de text.
	//test code
#ifdef USE_NEW_WEBBROWSER
	lpDispatch = m_Output->get_Document();
#else
	lpDispatch = m_Output.get_Document();
#endif
	if (lpDispatch != NULL)
	{
		hr = lpDispatch->QueryInterface(IID_IHTMLDocument2, (LPVOID*)&pHTMLDoc2);
		if (!SUCCEEDED(hr))
		{
			bRetorn = false;
		}
		else
		{
			pSafeArray = ::SafeArrayCreateVector(VT_VARIANT, 0, 1);
			if (pSafeArray)
			{
				hr = ::SafeArrayAccessData(pSafeArray,(LPVOID*) &pElement);
				if (!SUCCEEDED(hr))
				{
					bRetorn = false;
				}
				else
				{
					pElement->vt				= VT_BSTR;
					pElement->bstrVal		= csHTML.AllocSysString();
					hr = SafeArrayUnaccessData(pSafeArray);
					if (!SUCCEEDED(hr))
					{
						bRetorn = false;
					}
					else
					{
						//pHTMLDoc2->clear();
						//pHTMLDoc2->close();
						hr = pHTMLDoc2->write(pSafeArray);
						if (!SUCCEEDED(hr))
						{
							bRetorn = false;
						}
					}
					SysFreeString(pElement->bstrVal);
				}
				if (pSafeArray) SafeArrayDestroy(pSafeArray);
			}
			if (pHTMLDoc2)
			{
				pHTMLDoc2->close();
				pHTMLDoc2.Release();
			}
		}

		lpDispatch->Release();
	}

	return bRetorn;
}

void CHTMLWnd::OnBeforeCreateBody(void)
{
	WriteHTML(_T("<BODY></BODY>"),FALSE);
	//WriteHTML(_T("<head><meta http-equiv=\"Content-Type\" content=\"text/html; charset=gb2312\" /></head><BODY></BODY>"));
}

#pragma warning(default:4100 4018)

void CHTMLWnd::InvalidateWeb(void)
{
	//if (m_pHTMLDocument)
	//{
	//	IHTMLWindow2Ptr pHTMLWindow;
	//	m_pHTMLDocument->get_parentWindow(&pHTMLWindow);
	//	pHTMLWindow->scrollBy(0, 100);
	//	pHTMLWindow.Release();
	//}
	//m_pHTMLDocument
	//m_Output.Refresh();
#ifdef USE_NEW_WEBBROWSER
	m_Output->Invalidate();
#else
	m_Output.Invalidate();
#endif
	//m_Output.UpdateWindow();
}

void CHTMLWnd::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CWnd::OnPaint() for painting messages
	
#ifdef USE_NEW_WEBBROWSER
	m_Output->Invalidate();
#else
	m_Output.Invalidate();		// 176
#endif
	//m_Output.RedrawWindow();
	//m_Output.UpdateWindow();
}

//BOOL CHTMLWnd::PreCreateWindow(CREATESTRUCT& cs)
//{
//	// TODO: Add your specialized code here and/or call the base class
//	//cs.hMenu = NULL;
//	//cs.dwExStyle |= WS_EX_TOOLWINDOW;
//	//cs.style &= ~WS_OVERLAPPEDWINDOW;
//	//WNDCLASS wc;
//	//::memset(&wc,0,sizeof(wc));
//	//wc.lpszClassName = _T("HTMLWnd1.0");
//	//wc.hbrBackground = (HBRUSH)NULL_BRUSH;
//	//wc.style = CS_DBLCLKS;
//	////wc.style = CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS;
//	//wc.hInstance = AfxGetInstanceHandle();
//	//wc.lpfnWndProc = AfxWndProc;
//	//wc.hCursor = AfxGetApp()->LoadStandardCursor(IDC_ARROW);
//	//wc.hIcon = AfxGetApp()->LoadStandardIcon(IDI_APPLICATION);
//	//AfxRegisterClass(&wc);
//	//cs.lpszClass = wc.lpszClassName;
//
//	return CWnd::PreCreateWindow(cs);
//}

void CHTMLWnd::OnDestroy()
{
	m_pHTMLDocument.Release();
	m_spHTMLWnd.Release();
	m_pHTMLBodyElement.Release();

	//m_Output.Stop();
	//m_Output.Navigate(_T("About:Blank"),NULL,NULL,NULL,NULL);
#ifdef USE_NEW_WEBBROWSER
	m_Output->DestroyWindow();
	delete m_Output;
	m_Output = NULL;
#else
	m_Output.DestroyWindow();
#endif
	CWnd::OnDestroy();
}
