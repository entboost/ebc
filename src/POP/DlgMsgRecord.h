#pragma once
#include "Core/Explorer.h"
#include "Core/WebUIController.h"
#include "afxdtctl.h"
#include "DlgToolbar.h"
#include "DlgViewContactInfo.h"
#ifdef USES_LIBCEF
#include "cef/cefbrowser.h"
#endif
#include "PanelFind.h"

// CDlgMsgRecord dialog
/*
 * 聊天记录显示类
 * 用于显示聊天记录，包括个人聊天记录，或群组（部门）聊天记录，
 * 支持单独弹出显示和集成聊天窗口右边；
 * 聊天漫游消息也显示在这里显示。
 * 
*//////////////////////////////////////////////////////

class CDlgMsgRecord : public CEbDialogBase
	, public CEBRichMessageEventsSink
	, public CToolbarCallback
#ifdef USES_LIBCEF
	, public CCefBrowserHandler
	, public CFindHandler
#endif
{
	DECLARE_DYNAMIC(CDlgMsgRecord)

public:
	CDlgMsgRecord(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgMsgRecord();

	void SetChildMode(bool bChildMode = true) {m_bChildMode = bChildMode;}
	void SetCtrlColor(bool bInvalidate = true);
	void LoadAccountMsgRecord(eb::bigint sAccount, const tstring& sName);
	void LoadDepartmentMsgRecord(eb::bigint sDepCode, const tstring& sName);
	void RedrawCefBrowser(void);
	void NotifyMoveOrResizeStarted(void);
	//void RefreshWeb(void);
	void OnMove(void);

// Dialog Data
	enum { IDD = IDD_DLG_MSGRECORD, IDD_CHILD = IDD_DLG_MSGRECORD_CHILD };

protected:
	bool m_bChildMode;
	eb::bigint m_sAccount;
	eb::bigint m_sGroupCode;
	tstring m_sName;
	eb::bigint m_nLastMsgId;
	eb::bigint m_nFromUid;
	eb::bigint m_nToUid;
	CTime m_tLastMsgDayTime;
	bool m_bFindAllMsgId;
	CLockMap<eb::bigint,int> m_pMsgIdOffsetList;
	void WriteTitle(eb::bigint nMsgId,bool bPrivate,eb::bigint nFromUid,const tstring& sFromname,eb::bigint nToUid,const tstring& sToName,time_t tMsgTime, int nReadFlag);
	void LoadMsgRecord(const CString& sSql);
	void WriteFileHICON(const char* lpszFilePath);

	CPanelFind* m_pPanelFind;
	CPanelText * m_pPanelStatus;
	std::wstring m_sStatusMessage;
	double m_fZoomLevel;

	EB_BROWSER_TYPE m_nBrowserType;
	CCefBrowser m_pCefBrowser;
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	CTraButton m_labelViewAll;
	CTraButton m_labelViewImage;
	CTraButton m_labelViewFile;
	CTraButton m_labelViewAudio;
	CLabelEx m_labelLineConversation;
	CTraButton m_btnMin;
	CTraButton m_btnMax;
	CTraButton m_btnClose;
	//CMrFrame m_pMrFrame;
	CWnd m_pMrFrameControl;
	CComPtr<IEBRichMessage> m_pMrFrameInterface;
	CHoverEdit m_editSearch;
	CTraButton m_btnDeleteAll;
	CExplorer m_pExplorer;
	CWebUIController m_webUICtrl;
	bool m_bDocumentComplete;
	CDateTimeCtrl m_tDataTime;
	bool m_bSearchByDate;
	CTraButton m_btnMoveFirst;
	CTraButton m_btnMovePrev;
	CTraButton m_btnMoveNext;
	CTraButton m_btnMoveLast;
	//bool m_bNavigateUrl;
	int m_nRecordPages;	// 总共多少页
	int m_nCurrentPage;	// 当前显示第几页	0=第一页 -1=最后页

	CDlgToolbar m_pDlgToolbar;
	CDlgViewContactInfo m_pDlgViewContact;
	bool m_bViewSearchResult;
	eb::bigint m_nViewMsgId;
	std::vector<eb::bigint> m_pDeletMsgId;
	virtual void Fire_onContextMenuDelete(LONGLONG nMsgId,LONG nIndex);
	virtual void Fire_onContextMenuSelect(ULONG nItemData, const CEBString& sSelectString,LONGLONG nSelectMsgId);
	virtual void Fire_onItemLBtnClick(LONG nLineId, LONG nItemId, ULONG nItemData, const CEBString& sParamString, LONGLONG nSelectMsgId);
	//virtual void Fire_onItemLBtnDblClick(LONG nLineId, LONG nItemId, ULONG nItemData, const CEBString& sParamString, LONGLONG nSelectMsgId);
	virtual void Fire_onItemMoveEnter(LONG nLineId, LONG nItemId, ULONG nItemData, const CEBString& sParamString, LONGLONG nSelectMsgId, ULONG nParam);
	virtual void Fire_onItemMoveLeave(LONG nLineId, LONG nItemId, ULONG nItemData, const CEBString& sParamString, LONGLONG nSelectMsgId);

	virtual tstring GetSelectString(void) const;

	HWND m_hCurrentSelect;
	void Update(void);

	void ShowZoomLevel(void);
#ifdef USES_LIBCEF
	virtual bool OnDisableBackForward(void) const {return true;}
	void MovePanelFind(int cx);
	virtual void OnLoadingStateChange(const wchar_t* sUrl, bool bIsLoading, bool bCanGoBack, bool bCanGoForward);
	virtual bool OnKeyEvent(const wchar_t* sUrl, CefEventHandle os_event);
	virtual void OnFindResult(int identifier,int count,const CefRect& selectionRect,int activeMatchOrdinal,bool finalUpdate);
	virtual void OnFindClose(void);
	virtual void OnFindText(const char* lpszFindText, bool bFindUp, bool bMatchCase, bool bFindNext);
	virtual bool OnExecuteMenu(int nCommandId, const wchar_t* lpszUrl);
	virtual void OnStatusMessage(const wchar_t* sValue);
#endif
	DECLARE_EVENTSINK_MAP()
	afx_msg void DocumentComplete(LPDISPATCH pDisp, VARIANT* URL);

	DECLARE_MESSAGE_MAP()
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedButtonMin();
	afx_msg void OnBnClickedButtonMax();
	afx_msg void OnBnClickedButtonClose();
	afx_msg void OnPaint();
	afx_msg void OnClose();
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnBnClickedButtonDeleteall();
	afx_msg void OnEnChangeEditSearch();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
protected:
	virtual void OnCancel();
	virtual void OnOK();
	//virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
public:
	afx_msg void OnStnClickedStaticLineConversation();
	afx_msg void OnBnClickedButtonMoveFirst();
	afx_msg void OnBnClickedButtonMovePrev();
	afx_msg void OnBnClickedButtonMoveNext();
	afx_msg void OnBnClickedButtonMoveLast();
	afx_msg void OnDtnDatetimechangeDatetimepicker1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButtonViewAll();
	afx_msg void OnBnClickedButtonViewImage();
	afx_msg void OnBnClickedButtonViewFile();
	afx_msg void OnBnClickedButtonViewAudio();
	LRESULT OnMsgOpenAppUrl(WPARAM wParam, LPARAM lParam);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
};
