#pragma once
#include "DlgVideo.h"

// CDlgVideoPhoto dialog
/*
 * 视频图像显示窗口
 * 用于更换头像，自拍头像。
 * 
*//////////////////////////////////////////////////////

class CDlgVideoPhoto : public CEbDialogBase
	, public CEBMMRoomEventsSink
{
	DECLARE_DYNAMIC(CDlgVideoPhoto)

public:
	CDlgVideoPhoto(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgVideoPhoto();

	tstring m_sSavePhotoFile;
// Dialog Data
	enum { IDD = IDD_DLG_VIDEO_PHOTO };

protected:
	CRect m_rectHead;
	CRect m_rectBig;
	Gdiplus::Image * m_imageHead;

	//int m_nLocalId;
	CComPtr<IEBMMRoom> m_pVideoRoom;
	CDlgVideo * m_pMyVideo;

	virtual void Fire_onVideoCallback(LONGLONG nUserId, LONG nCallbackType, ULONG lParam, ULONG wParam, ULONG nUserData);
	void OpenLocalVideo(void);
	bool CreateVideoRoom(void);
	void CloseVideo(void);

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	CTraButton m_btnClose;
	CTraButton m_btnOk;
	CTraButton m_btnCancel;
	CTraButton m_btnPhoto;

	void DrawInfo(void);

	DECLARE_MESSAGE_MAP()
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedButtonClose();
public:
	afx_msg void OnBnClickedButtonPhoto();
	afx_msg void OnMove(int x, int y);
};
