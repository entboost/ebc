#pragma once


// CDlgMessageBox dialog

class CDlgMessageBox : public CEbDialogBase
{
	DECLARE_DYNAMIC(CDlgMessageBox)

public:
	CDlgMessageBox(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgMessageBox();
	typedef enum
	{
		IMAGE_NULL
		, IMAGE_INFORMATION
		, IMAGE_QUESTION
		, IMAGE_WARNING
		, IMAGE_ERROR
	}IMAGE_TYPE;

	static CDlgMessageBox* EbMessageBox(CWnd * pParent,LPCTSTR lpszTitle,LPCTSTR lpszText, IMAGE_TYPE nImageType, int nAutoClose=0);
	static int EbDoModal(CWnd * pParent,LPCTSTR lpszTitle,LPCTSTR lpszText, IMAGE_TYPE nImageType, bool bCloseOnlyMode = false, int nAutoClose=0);

	static void GetScreenSize(int & pWidth, int & pHeight);

	BOOL m_bDeleteThis;	// default true
	CString m_sTitle;
	CString m_sDescription;
	IMAGE_TYPE m_nImageType;
	int m_nAutoClose;
	bool m_bCloseOnlyMode;

// Dialog Data
	enum { IDD = IDD_DLG_MESSAGE_BOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	CTraButton m_btnClose;
	CTraButton m_btnOk;
	CTraButton m_btnCancel;

	void DrawInfo(void);

	DECLARE_MESSAGE_MAP()
	afx_msg void OnDestroy();
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	//afx_msg void OnBnClickedOk();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnSize(UINT nType, int cx, int cy);
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedButtonClose();
};
