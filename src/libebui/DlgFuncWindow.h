#pragma once
#include "Core/Explorer.h"
#include "Core/WebUIController.h"
#include "BroadcastView.h"

// CDlgFuncWindow dialog

class CDlgFuncWindow : public CEbDialogBase
	, public CBroadcastViewCallback
{
	DECLARE_DYNAMIC(CDlgFuncWindow)

public:
	CDlgFuncWindow(CWnd* pParent = NULL,bool bDeleteThis=false);   // standard constructor
	virtual ~CDlgFuncWindow();

	CEBCCallInfo::pointer m_pEbCallInfo;

	int m_nMsgType;
	bool m_bBroadcastMsg;	// default false
	bool m_bOpenNewClose;	// default false
	std::string m_sTitle;
	std::string m_sFuncUrl;
	std::string m_sPostData;
	size_t m_nWidth;
	size_t m_nHeight;
	bool m_bDisableContextMenu;
	bool m_bDisableScrollBar;
	bool m_bClosePrompt;
	bool m_bWinResizable;

// Dialog Data
	enum { IDD = IDD_DLG_FUNC_WINDOW };

protected:
	bool m_bDeleteThis;
	//bool m_nShowFuncUrl;
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	CTraButton m_btnMin;
	CTraButton m_btnMax;
	CTraButton m_btnClose;
	CExplorer m_pExplorer;
	CWebUIController m_webUICtrl;
	bool m_bDocumentComplete;
	CBroadcastView* m_pBroadcastView;

	virtual void OnBeforeNavigate(VARIANT_BOOL* bCancel,LPCTSTR szURL);
	DECLARE_EVENTSINK_MAP()
	afx_msg void OnBeforeNavigate2(LPDISPATCH pDisp, VARIANT FAR* URL, VARIANT FAR* Flags, VARIANT FAR* TargetFrameName, VARIANT FAR* PostData, VARIANT FAR* Headers, VARIANT_BOOL FAR* Cancel);
	//afx_msg void NewWindow2(IDispatch **ppDisp, VARIANT_BOOL *Cancel); 
	afx_msg void NewWindow3(IDispatch **ppDisp, VARIANT_BOOL *Cancel, DWORD dwFlags, BSTR bstrUrlContext, BSTR bstrUrl); 
	afx_msg void DocumentComplete(LPDISPATCH pDisp, VARIANT* URL);
	//afx_msg void OnTitleChange(LPDISPATCH pDisp, VARIANT* sTitle);

	DECLARE_MESSAGE_MAP()
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnClose();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnPaint();
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedButtonMin();
	afx_msg void OnBnClickedButtonMax();
	afx_msg void OnBnClickedButtonClose();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
