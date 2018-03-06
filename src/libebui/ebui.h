// ebui.h file here
// 版权所有：深圳恩布网络科技有限公司 2012－2015
// 版    本：1.0.0.0
// 版本历史：
// ** 2015-10-03: 创建项目；
// ** 2015-10-07: 增加callback事件机制；

#include <boost/shared_ptr.hpp>
#include "ebuihandle.h"

class EBUI_API Cebui
{
public:
	typedef boost::shared_ptr<Cebui> pointer;
	static Cebui::pointer create(void)
	{
		return Cebui::pointer(new Cebui());
	}
	Cebui(void);
	virtual ~Cebui(void);

	void SetHandler(CEBUIHandler* pHandler);
	BOOL IM_OAuth(const char* lpszParameter="");
	BOOL IM_Login(const char* lpszAccount, const char* lpszPasswd);
	BOOL IM_IsLogined(void) const;
	void IM_Logout(void);

	BOOL MainFrame_Create(HWND hParentHwnd, UINT nMsgId = EBUI_WM_DEFAULT_MSG_ID);
	HWND MainFrame_GetSafeHwnd(void) const;
	//void MainFrame_Destroy(void);
	void MainFrame_ShowWindow(int nShow = SW_SHOW);
	void MainFrame_MoveWindow(int x, int y, int w, int h, BOOL bRepaint = TRUE);
	void MainFrame_MoveWindow(LPCRECT lpRect, BOOL bRepaint = TRUE);
	void MainFrame_Invalidate(BOOL bErase = TRUE);

	void ChatWindow_SetTopMost(bool bTopMost);

	void SetMainColor(COLORREF nMainColor);
	COLORREF GetMainColor(void) const;
	void SetTreeListColor(COLORREF nSelColor, COLORREF nHotColor, COLORREF nBoderColor);
};
