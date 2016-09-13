#ifndef __DlgEmotionSelect_h__
#define __DlgEmotionSelect_h__

//#include "Core/MrSkinVBar.h"


class CEBImageDrawInfo : public EB_EmotionInfo
{
public:
	CEBImageDrawInfo(void)
		: EB_EmotionInfo()
		, m_dwUserData(0)
	{
	}
	CEBImageDrawInfo(const EB_EmotionInfo& pObj)
		: EB_EmotionInfo(pObj)
		, m_dwUserData(0)
	{
	}
	const CEBImageDrawInfo& operator =(const CEBImageDrawInfo& pObj)
	{
		EB_EmotionInfo::operator =(&pObj);
		m_dwUserData = pObj.m_dwUserData;
		return *this;
	}
	DWORD m_dwUserData;
};

// CDlgEmotionSelect dialog
class CImageSelectCallback
{
public:
	virtual void OnSelectedImageInfo(const CEBImageDrawInfo& pSelectedImageInfo) = 0;
};

class CDlgEmotionSelect : public CEbDialogBase
{
	DECLARE_DYNAMIC(CDlgEmotionSelect)

public:
	CDlgEmotionSelect(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgEmotionSelect();

	bool GetImageResourceId(DWORD dwUserData,eb::bigint& pOutResourceId);
	bool GetImageDrawInfo(DWORD dwUserData,CEBImageDrawInfo& pOutImageInfo);
	void SetCallback(CImageSelectCallback* pCallback) {m_pCallback=pCallback;}
	void ShowImage(bool bShowWindow);
	void SetCtrlColor(void);
// Dialog Data
	enum { IDD = IDD_DLG_EMOTION_SELECT };

protected:
	bool m_bHasEmptyFile;
	CWnd m_pEmotionViewControl;
	CComPtr<IEBEmotionView> m_pEbEmotionView;

	CLockMap<DWORD,CEBImageDrawInfo> m_pImageDrawList;
	//std::vector<CEBImageDrawInfo> m_pImageDrawList;
	CEBImageDrawInfo m_pMouseFocusInfo;		// 当前鼠标移动选项
	CImageSelectCallback* m_pCallback;


	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	LRESULT OnWmEVClickEmotion(WPARAM wParam, LPARAM lParam);
	LRESULT OnWmEVKillFocus(WPARAM wParam, LPARAM lParam);
};

#endif // __DlgEmotionSelect_h__
