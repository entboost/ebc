#pragma once

#include <mshtml.h>
#include <comdef.h>
#include <comdefsp.h>
#include "webbrowser2.h"
// CHTMLWnd

class CHTMLWnd : public CWnd
{
	DECLARE_DYNCREATE(CHTMLWnd)

public:
	CHTMLWnd();
	virtual ~CHTMLWnd();

public:
	int WriteBlock(const CString& str,BOOL bUnicode,BOOL bAppTimestamp= FALSE );
	void WriteBlocks(const CStringArray& strArray,BOOL bUnicode=FALSE,BOOL bAppTimestamp= FALSE );
	int WriteHTML(const CString& str,BOOL bUnicode);
	void WriteChatMsg(const CString& strSrcUserName,UINT nSrcUserID,const CString& strDest,UINT nDestUserID,
		const CString& strMsg);
	BOOL SetBkground(COLORREF crBk);
	BOOL SetBkground(const CString& strBk,BOOL bFixed=TRUE);
	void SetAutoScroll(bool bAuto=true) {m_bAutoScroll = bAuto;}
	bool GetAutoScroll(void) const {return m_bAutoScroll;}
	void SetBlockLimit(int nLimit);
    virtual void Reset();
	void ClearBlock(  );

	void SetBackupCount(int nCount) {m_nBackupCount = nCount;}
	void InvalidateWeb(void);
	void ScrollBy(void);
	void SetBrowserFocus(void);
protected:
	DECLARE_MESSAGE_MAP()

protected:
	//BSTR ToBSTR(const CString& str,BOOL bUnicode=FALSE);
	CString Convert(const CString &str, int sourceCodepage, int targetCodepage) ;
	void InitWeb();
	bool TestSetHTMLInDocument(void);

	virtual void OnBeforeCreateBody(void);
	//virtual void OnBeforeCreateBody() { WriteHTML(_T("<BODY></BODY>"));}
	virtual void OnAfterCreateBody() {};

	virtual void OnBeforeNavigate(VARIANT_BOOL* bCancel,LPCTSTR szURL) {} ;
	virtual void OnNavigateComplete2(LPCTSTR szURL) {};
	virtual void OnDocumentComplete(LPCTSTR szURL);

	void ShowContextMenu();
protected:
	int			m_nBackupCount;	// 0 ²»±¸·Ý
	CStringList m_pBackupList;
	BOOL                m_bReady;
	CWebBrowser2		m_Output;
	//IHTMLDocument2Ptr	m_pHTMLDocument;
	//IHTMLWindow2Ptr		m_spHTMLWnd;
	CComPtr<IHTMLDocument2> m_pHTMLDocument;
	CComPtr<IHTMLWindow2>		m_spHTMLWnd;
	bool				m_bAutoScroll;
	//IHTMLBodyElementPtr m_pHTMLBodyElement;
	CComQIPtr<IHTMLBodyElement> m_pHTMLBodyElement;
	int				m_nBlockLimit;
	UINT			m_nBlockWrited;
	CString			m_sLastHtml;
	BOOL			m_bLastUnicode;
	enum 
	{
		ID_HTML_WND	= 0x1114
	};
	//CMenu				m_ContextMenu;
	DECLARE_EVENTSINK_MAP()
	void BeforeNavigate2(LPDISPATCH pDisp, VARIANT* URL, VARIANT* Flags, VARIANT* TargetFrameName, VARIANT* PostData, VARIANT* Headers, VARIANT_BOOL* Cancel);
	void NavigateComplete2(LPDISPATCH pDisp, VARIANT* URL);
	void DocumentComplete(LPDISPATCH pDisp, VARIANT* URL);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnCopy();
	afx_msg void OnSelectall();
	afx_msg void OnSaveas();
	afx_msg void OnPaint();
	//virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	afx_msg void OnDestroy();
};

inline void CHTMLWnd::SetBlockLimit(int nLimit)
{
	m_nBlockLimit = nLimit;
}

