#pragma once

/*
 * 集成应用按钮类
 * 用于管理集成应用按钮基本信息。
 * 
*//////////////////////////////////////////////////////

class CEBFuncButton
{
public:
	typedef boost::shared_ptr<CEBFuncButton> pointer;
	static CEBFuncButton::pointer create(void)
	{
		return CEBFuncButton::pointer(new CEBFuncButton());
	}


	bool Create(CWnd* pParentWnd,const RECT& rect,UINT nID,const EB_SubscribeFuncInfo& pFuncInfo,const tstring& sDefaultImage);
	const EB_SubscribeFuncInfo& GetFuncInfo(void) const {return m_pFuncInfo;}
	const tstring& GetResFile(void) const {return m_pFuncInfo.m_sResFile;}
	void UpdateResFile(const tstring& sResFile);
	bool MoveWindow(LPCRECT lpRect, BOOL bRepaint = TRUE);
	void ShowWindow(bool bShow);
	void AddUnreadMsg(void);
	void SetUnreadMsg(size_t nUnreadMsgCount);
	int ClearUnreadMsg(void);

	CEBFuncButton(void);
	virtual ~CEBFuncButton(void);

private:
	EB_SubscribeFuncInfo m_pFuncInfo;
	CTraButton m_btn;
	CTraButton m_msg;
	CRect m_rectMsg;
	int m_nUnreadMsgCount;
};

const int const_func_button_size = 24;
const int const_func_button_intever = 1;
