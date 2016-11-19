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

	CTraButton m_btn;

	bool Create(CWnd* pParentWnd,const RECT& rect,UINT nID,const EB_SubscribeFuncInfo& pFuncInfo,const tstring& sDefaultImage);
	const EB_SubscribeFuncInfo& GetFuncInfo(void) const {return m_pFuncInfo;}
	const tstring& GetResFile(void) const {return m_pFuncInfo.m_sResFile;}
	void UpdateResFile(const tstring& sResFile);

	CEBFuncButton(void);
	virtual ~CEBFuncButton(void);

private:
	EB_SubscribeFuncInfo m_pFuncInfo;
};

const int const_func_button_size = 24;
const int const_func_button_intever = 1;
