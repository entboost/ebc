#pragma once

class CDlgSelectHead;

// CDlgChangeHead dialog
/*
 * 更换头像类
 * 用于实现修改部门员工（或群组成员）资料，里面的“更换头像”功能；
 * 
*//////////////////////////////////////////////////////

class CDlgChangeHead : public CEbDialogBase
	//, public CImageSelectCallback
{
	DECLARE_DYNAMIC(CDlgChangeHead)

public:
	CDlgChangeHead(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgChangeHead();

	void SetCallback(CImageSelectCallback* pCallback);
	eb::bigint m_sGroupCode;
	void SetHeadResorceFile(const tstring& sHeadResourceFile);

// Dialog Data
	enum { IDD = IDD_DLG_CHANGE_HEAD };

protected:
	tstring m_sHeadResourceFile;
	CRect m_rectHead;
	CRect m_rectBig;
	Gdiplus::Image * m_imageHead1;
	Gdiplus::Image * m_imageHead2;
	tstring m_sNewHeadFile;
	CDlgSelectHead* m_pDlgSelectHead;

	//virtual void OnSelectedImageInfo(const CEBImageDrawInfo& pSelectedImageInfo);

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	CTraButton m_btnClose;
	CTraButton m_btnOk;
	CTraButton m_btnCancel;
	CTraButton m_btnHead1;
	CTraButton m_btnHead2;
	CTraButton m_btnOpenFile;
	CTraButton m_btnMyVideo;

	void DrawInfo(void);

	DECLARE_MESSAGE_MAP()
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg void OnBnClickedButtonClose();
	afx_msg void OnBnClickedOk();
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedButtonOpenfile();
	afx_msg void OnBnClickedButtonMyVideo();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedButtonHead1();
	afx_msg void OnBnClickedButtonHead2();
};
