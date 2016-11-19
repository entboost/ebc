#pragma once

/*
 * 传输文件窗口管理面板
 * 用于显示当前所有传输文件列表窗口
 * 
*//////////////////////////////////////////////////////

#include "DlgTranFile.h"

// CPanelFiles dialog

class CPanelFiles : public CEbDialogBase
{
	DECLARE_DYNAMIC(CPanelFiles)

public:
	CPanelFiles(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPanelFiles();

	void ExitChat(bool bHangup);

#ifdef USES_EBCOM_TEST
	void OnSendingFile(IEB_ChatFileInfo* pCrFileInfo);
	void OnReceivingFile(IEB_ChatFileInfo* pCrFileInfo);
	void SetFilePercent(IEB_ChatFilePercent* pChatRoomFilePercent);
#else
	void OnSendingFile(const CCrFileInfo * pCrFileInfo);
	void OnReceivingFile(const CCrFileInfo * pCrFileInfo);
	void SetFilePercent(const CChatRoomFilePercent * pChatRoomFilePercent);
#endif
	void DeleteDlgTranFile(eb::bigint nMsgId);
	bool IsEmpty(void) const;

	//Cchatroom::pointer m_pChatRoom;

// Dialog Data
	enum { IDD = IDD_PAN_FILES };

private:
	CLockList<CCrFileInfo> m_pAddTranFile;
	CLockList<CDlgTranFile::pointer> m_pDelTranFile;
	CLockMap<eb::bigint, CDlgTranFile::pointer> m_pTranFiles;	// msgid->

protected:
	void MoveSize(int cx, int cy);

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnDestroy();
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
