#pragma once

// CDlgResourceMgr dialog

class CDlgResourceMgr : public CEbDialogBase
	//, public CTreeCallback
{
	DECLARE_DYNAMIC(CDlgResourceMgr)

public:
	CDlgResourceMgr(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgResourceMgr();

	enum RES_MANAGER_TYPE
	{
		RES_MANAGER_GROUP
		, RES_MANAGER_MY
	};

	enum RES_SELECT_TYPE
	{
		RES_SELECT_ALLRES
		, RES_SELECT_ALLFILE
		, RES_SELECT_ALLNOTE
		, RES_SELECT_ALLNOTDIR
		, RES_SELECT_DIR
	};
	void SetChildMode(bool bChildMode = true) {m_bChildMode = bChildMode;}
	void LoadGroupResource(eb::bigint sGroupCode);
	void LoadMyResource(void);
#ifdef USES_EBCOM_TEST
	void OnResourceMove(IEB_ResourceInfo* pResourceInfo,const std::string& sOldParentResId);
	void OnResourceInfo(IEB_ResourceInfo* pResourceInfo);
	void OnResourceDelete(IEB_ResourceInfo* pResourceInfo);
#else
	void OnResourceMove(const EB_ResourceInfo& pResourceInfo,eb::bigint nOldParentResId);
	void OnResourceInfo(const EB_ResourceInfo& pResourceInfo);
	void OnResourceDelete(const EB_ResourceInfo& pResourceInfo);
#endif
	void SetCtrlColor(bool bInvalidate = true);

// Dialog Data
	enum { IDD = IDD_DLG_RESOURCEMGR, IDD_CHILD = IDD_DLG_RESOURCEMGR_CHILD};

private:
	bool m_bChildMode;
	bool m_bPostCloseMsg;	// ?有时关闭太慢，导致处理二次，会发生内存异常（所有需要处理）
protected:
	eb::bigint m_sAccount;
	eb::bigint m_sGroupCode;
	tstring m_sName;
	//CTreeItemInfo::pointer m_pAllFileItemInfo;
	//CTreeItemInfo::pointer m_pAllNoteItemInfo;
	RES_MANAGER_TYPE m_nManagerType;
	RES_SELECT_TYPE m_nSelectType;
	HTREEITEM m_hNewDirTempItem;
	CTreeItemInfo::pointer m_pSelectDirItemInfo;
	eb::bigint m_sMoveResId;
	std::vector<eb::bigint> m_sMove2DirResId;
	CLockMap<eb::bigint,CTreeItemInfo::pointer> m_pDirItemInfo;		// resid->
	CLockMap<eb::bigint,CTreeItemInfo::pointer> m_pItemItemInfo;	// resid->
	CTreeItemInfo::pointer m_pAllResParent;							// 所有资源

#ifdef USES_EBCOM_TEST
	void NewItem(IEB_ResourceInfo* pResourceInfo);
#else
	void NewItem(const EB_ResourceInfo& pResourceInfo);
#endif
	void DownloadFile(HTREEITEM hItem);
	void DeleteItemRes(HTREEITEM hItem);
	void NewSubDirRes(HTREEITEM hItem);

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	CTraButton m_btnHotNewSubDir;
	CTraButton m_btnHotDeleteItem;
	CTraButton m_btnHotDownload;
	CTraButton m_btnMin;
	CTraButton m_btnMax;
	CTraButton m_btnClose;
	CTraButton m_btnAllRes;
	CTraButton m_btnAllFile;
	CTraButton m_btnAllNote;
	CTraButton m_btnAllNotParent;
	CTraButton m_btnNewDir;
	CTraButton m_btnNewFile;
	CTraButton m_btnRefresh;
	VividTree m_treeDir;
	VividTree m_treeItem;

	//virtual HICON GetItemIcon(const CTreeCtrl& pTreeCtrl,HTREEITEM hItem) const;
	//virtual int GetItemState(const CTreeCtrl& pTreeCtrl,HTREEITEM hItem, HICON& pOutIcon) const;
	//virtual bool GetItemDrawOpenClose(const CTreeCtrl& pTreeCtrl,HTREEITEM hItem) const;
	LRESULT OnTreeItemSelChanged(WPARAM wp, LPARAM lp);
	LRESULT OnTreeItemDblclk(WPARAM wp, LPARAM lp);
	LRESULT OnTreeItemTrackHot(WPARAM wp, LPARAM lp);
	BOOL IsTreeCtrlEditMessage(WPARAM KeyCode);

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
	//afx_msg void OnTvnSelchangedTreeDir(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMRClickTreeDir(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMRClickTreeItem(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNewDirRes(void);
	afx_msg void OnNewSubDirRes(void);
	afx_msg void OnNewFileRes(void);
	afx_msg void OnNewNoteRes(void);
	afx_msg void OnDownloadFile(void);
	afx_msg void OnOnlineView(void);
	afx_msg void OnDeleteDirRes(void);
	afx_msg void OnEditDirRes(void);
	afx_msg void OnDeleteItemRes(void);
	afx_msg LRESULT OnMsgDeleteItemRes(WPARAM wParam, LPARAM lParam);
	afx_msg void OnMove2Dir(UINT nID);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedButtonRefresh();
	afx_msg void OnBnClickedButtonAllfile();
	afx_msg void OnBnClickedButtonAllnote();
	afx_msg void OnBnClickedButtonAllres();
	virtual void OnOK();
	afx_msg void OnTvnEndlabeleditTreeDir(NMHDR *pNMHDR, LRESULT *pResult);
	virtual void OnCancel();
	afx_msg void OnBnClickedButtonAllNotparent();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnBnClickedButtonNewdir();
	afx_msg void OnBnClickedButtonNewfile();
	afx_msg void OnDropFiles(HDROP hDropInfo);
};
