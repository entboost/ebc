#ifndef __DlgEditVideo_h__
#define __DlgEditVideo_h__
#pragma once
//#include "videoroom.h"
#include "Core/DlgVoiceBar.h"
#include "DlgVideo.h"


// CDlgEditVideo dialog

class CDlgEditVideo : public CEbDialogBase
	, public CEBMMRoomEventsSink
{
	DECLARE_DYNAMIC(CDlgEditVideo)

public:
	CDlgEditVideo(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgEditVideo();

	void OnMove(void);
	void CloseVideo(void);
	void SetCtrlColor(void);
// Dialog Data
	enum { IDD = IDD_DLG_EDIT_VIDEO };

protected:
	CComPtr<IEBMMRoom> m_pVideoRoom;
	CDlgVideo * m_pMyVideo;

	virtual void Fire_onVideoCallback(LONGLONG nUserId, LONG nCallbackType, ULONG lParam, ULONG wParam, ULONG nUserData);

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	CBitComboBox m_comboVideos;
	CTraButton m_btnLocalVideo;

	bool CreateVideoRoom(void);

	DECLARE_MESSAGE_MAP()
	virtual void OnCancel();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnCbnSelchangeComboVideos();
	afx_msg void OnBnClickedButtonLocalVideo();
	afx_msg void OnDestroy();
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnMove(int x, int y);
};

#endif // __DlgEditVideo_h__
