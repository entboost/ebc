#pragma once

class CPanelFiles;
// CDlgFileManager dialog

/*
 * 文件管理类
 * 用于显示正在上传（或下载）的资源文件，和显示所有已经下载过的资源文件；
 * 
*//////////////////////////////////////////////////////

class CDlgFileManager : public CEbDialogBase
	, public CTreeCallback
{
	DECLARE_DYNAMIC(CDlgFileManager)

public:
	CDlgFileManager(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgFileManager();

#ifdef USES_EBCOM_TEST
	void OnSendingFile(IEB_ChatFileInfo* pCrFileInfo);
	void OnReceivingFile(IEB_ChatFileInfo* pCrFileInfo);
	void OnReceivedFile(IEB_ChatFileInfo* pCrFileInfo);
	void SetFilePercent(IEB_ChatFilePercent* pChatRoomFilePercent);
#else
	void OnSendingFile(const CCrFileInfo * pCrFileInfo);
	void OnReceivingFile(const CCrFileInfo * pCrFileInfo);
	void OnReceivedFile(const CCrFileInfo * pCrFileInfo);
	void SetFilePercent(const CChatRoomFilePercent * pChatRoomFilePercent);
#endif
	void DeleteDlgTranFile(eb::bigint nMsgId);
	bool IsEmpty(void) const;
	void SetCtrlColor(bool bInvalidate=true);

// Dialog Data
	enum { IDD = IDD_DLG_FILE_MANAGER };

protected:
	CLockMap<eb::bigint,CTreeItemInfo::pointer> m_pItemItemInfo;	// msgid->
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	CTraButton m_btnMin;
	CTraButton m_btnMax;
	CTraButton m_btnClose;
	CTraButton m_btnTraning;
	CTraButton m_btnTraned;
	CTraButton m_btnRefresh;
	CPanelFiles* m_pPanFiles;
	CTraButton m_btnHotDeleteFile;
	CTraButton m_btnHotOpenFile;
	CTraButton m_btnHotOpenDir;
	//CTraButton m_btnHotDelete;
	CHoverEdit m_editSearch;
	VividTree m_treeTraned;

	//virtual HICON GetItemIcon(const CTreeCtrl& pTreeCtrl,HTREEITEM hItem) const;
	LRESULT OnTreeItemDblclk(WPARAM wp, LPARAM lp);
	LRESULT OnTreeItemTrackHot(WPARAM wp, LPARAM lp);

	void OpenItemFile(HTREEITEM hItem);
	void DeleteItemFile(HTREEITEM hItem);

	DECLARE_MESSAGE_MAP()
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBnClickedButtonMin();
	afx_msg void OnBnClickedButtonMax();
	afx_msg void OnBnClickedButtonClose();
	afx_msg void OnPaint();
	afx_msg void OnClose();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnEnChangeEditSearch();
public: // ***
	afx_msg void OnBnClickedButtonTraned();
	afx_msg void OnBnClickedButtonTraning();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnBnClickedButtonRefresh();
};
