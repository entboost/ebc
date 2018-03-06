#pragma once
#include "Core/Explorer.h"
#include "Core/WebUIController.h"
#include "afxdtctl.h"


// CDlgMsgRecord dialog

class CDlgMsgRecord : public CEbDialogBase
	, public CEBRichMessageEventsSink
{
	DECLARE_DYNAMIC(CDlgMsgRecord)

public:
	CDlgMsgRecord(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgMsgRecord();

	void LoadAccountMsgRecord(eb::bigint sAccount, const tstring& sName);
	void LoadDepartmentMsgRecord(eb::bigint sDepCode, const tstring& sName);

// Dialog Data
	enum { IDD = IDD_DLG_MSGRECORD };

protected:
	eb::bigint m_sAccount;
	eb::bigint m_sGroupCode;
	tstring m_sName;
	eb::bigint m_nLastMsgId;
	eb::bigint m_nFromUid;
	eb::bigint m_nToUid;
	CTime m_tLastMsgDayTime;
	bool m_bFindAllMsgId;
	CLockMap<eb::bigint,int> m_pMsgIdOffsetList;
	void WriteTitle(eb::bigint nMsgId,bool bPrivate,eb::bigint nFromUid,const tstring& sFromname,eb::bigint nToUid,const tstring& sToName,time_t tMsgTime);
	void LoadMsgRecord(const CString& sSql);
	void WriteFileHICON(const char* lpszFilePath);

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
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

	bool m_bViewSearchResult;
	eb::bigint m_nViewMsgId;
	std::vector<eb::bigint> m_pDeletMsgId;
	virtual void Fire_onContextMenuDelete(LONGLONG nMsgId,LONG nIndex);
	virtual void Fire_onContextMenuSelect(ULONG nItemData, const CEBString& sSelectString,LONGLONG nSelectMsgId);
	virtual void Fire_onItemLBtnClick(ULONG nItemData, const CEBString& sParamString, LONGLONG nSelectMsgId);
	//virtual void Fire_onItemLBtnDblClick(ULONG nItemData, const CEBString& sParamString, LONGLONG nSelectMsgId);

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
protected:
	virtual void OnCancel();
	virtual void OnOK();
public:
	afx_msg void OnStnClickedStaticLineConversation();
	afx_msg void OnBnClickedButtonMoveFirst();
	afx_msg void OnBnClickedButtonMovePrev();
	afx_msg void OnBnClickedButtonMoveNext();
	afx_msg void OnBnClickedButtonMoveLast();
	afx_msg void OnDtnDatetimechangeDatetimepicker1(NMHDR *pNMHDR, LRESULT *pResult);
};
