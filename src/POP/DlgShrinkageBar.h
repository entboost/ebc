#pragma once


// CDlgShrinkageBar dialog
/*
 * 靠边隐藏工作条窗口类
 * 用于主界面，或聊天会话列表窗口，靠边自动隐藏时，显示一条颜色工具条，鼠标移动时会自动隐藏，并重新显示主界面，或聊天会话列表窗口。
 * 
*//////////////////////////////////////////////////////

class CDlgShrinkageBar : public CEbDialogBase
{
	DECLARE_DYNAMIC(CDlgShrinkageBar)

public:
	CDlgShrinkageBar(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgShrinkageBar();

	void SetBgColor(COLORREF colorBg=-1);
// Dialog Data
	enum { IDD = IDD_DLG_SHRINKAGEBAR };

protected:
	COLORREF m_colorBg;
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
	virtual BOOL OnInitDialog();
};
