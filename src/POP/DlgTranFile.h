#pragma once
#include "afxcmn.h"

// CDlgTranFile dialog
/*
 * 文件传输（上传、下载）信息窗口类
 * 用于显示传输文件信息，支持接受、拒绝、取消、另存等操作功能；
 * 支持显示文件名称，文件大小，传输进度等。
 * 
*//////////////////////////////////////////////////////

class CDlgTranFile : public CEbDialogBase
{
	DECLARE_DYNAMIC(CDlgTranFile)

public:
	typedef boost::shared_ptr<CDlgTranFile> pointer;
	CDlgTranFile(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgTranFile();
	static CDlgTranFile::pointer create(CWnd* pParent)
	{
		return CDlgTranFile::pointer(new CDlgTranFile(pParent));
	}

	void SetCtrlColor(void);
	void Cancel(void);
	void UpdateFileInfo(const CCrFileInfo * pCrFileInfo);
#ifdef USES_EBCOM_TEST
	void SetFilePercent(IEB_ChatFilePercent* pChatRoomFilePercent);
#else
	void SetFilePercent(const CChatRoomFilePercent * pChatRoomFilePercent);
#endif
	//Cchatroom::pointer m_pChatRoom;
	bool m_bIsSendingFile;					// default false;
	//eb::bigint m_sResourceId;
	CCrFileInfo m_pCrFileInfo;
	//EB_CallInfo m_pCallInfo;
// Dialog Data
	enum { IDD = IDD_DLG_TRANFILE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	CTraButton m_btnSave2CloudDrive;
	CTraButton m_btnSaveas;
	CTraButton m_btnSave;
	CTraButton m_btnReject;
	CTraButton m_btnOffFile;
	CTraButton m_btnCancel;
	CProgressST m_progPercent;

	DECLARE_MESSAGE_MAP()
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnBnClickedButtonSave2CloudDrive();
	afx_msg void OnBnClickedButtonSaveas();
	afx_msg void OnBnClickedButtonSave();
	afx_msg void OnBnClickedButtonReject();
	afx_msg void OnBnClickedButtonCancel();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnBnClickedButtonOfffile();
	afx_msg void OnPaint();
};
