#pragma once


// CDlgSelectHead dialog

class CDlgSelectHead : public CEbDialogBase
{
	DECLARE_DYNAMIC(CDlgSelectHead)

public:
	CDlgSelectHead(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgSelectHead();

	void SetCallback(CImageSelectCallback* pCallback) {m_pCallback=pCallback;}
// Dialog Data
	enum { IDD = IDD_DLG_SELECT_HEAD };

protected:
	CWnd m_pEmotionViewControl;
	CComPtr<IEBEmotionView> m_pEbEmotionView;
	std::vector<CEBImageDrawInfo> m_pImageDrawList;
	CImageSelectCallback* m_pCallback;

	void ShowImage(void);
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	LRESULT OnWmEVClickEmotion(WPARAM wParam, LPARAM lParam);
};
