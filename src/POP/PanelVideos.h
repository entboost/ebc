#pragma once
//#include "videoroom.h"
#include "afxwin.h"
#include "Core/DlgVoiceBar.h"
//#include "Epvolume.h"
//#include "Core/Mixer.h"
//#include "Core/AVDeviceManager.h"
//#include "Core/CAVDevice.h"
#include "DlgVideo.h"

// CPanelVideos dialog
#define MAX_VIDEO_COUNT 12
#define FFT_LEN  1024

class CPanelVideos : public CEbDialogBase
#ifdef USES_EBCOM_TEST
	, public CEBVideoDataEventsSink
	//, public CEBVRoomEventsSink
#endif
{
	DECLARE_DYNAMIC(CPanelVideos)

public:
	CPanelVideos(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPanelVideos();

	EB_CallInfo m_pCallInfo;
	void SetCtrlColor(bool bInvalidate = true);

	void OnMove(void);
	bool IsInFVideoRequest(void) const {return m_bInFVideoRequest;}
	int GetVideoCount(void) const;
	void ExitChat(bool bHangup);

// Dialog Data
	enum { IDD = IDD_PAN_VIDEOS };

#ifdef USES_EBCOM_TEST
	// CEBVRoomEventsSink
	//virtual void Fire_onVideoStream(ULONG nUserId, BYTE* pVideoData, ULONG nVideoSize, ULONG nUserData);
	virtual void Fire_onVideoData(LONGLONG nUserVideoId, const BYTE* pData, DWORD nSize, DWORD dwParam);
	virtual void Fire_onAudioData(LONGLONG nUserId,const BYTE* pData, DWORD nSize, DWORD dwParam);

	void VRequestResponse(IEB_VideoInfo* pVideoInfo,int nStateValue);
	void VAckResponse(IEB_VideoInfo* pVideoInfo,int nStateValue);
	void VideoRequest(IEB_VideoInfo* pVideoInfo,IEB_UserVideoInfo* pUserVideoInfo);
	void VideoAccept(IEB_VideoInfo* pVideoInfo,IEB_UserVideoInfo* pUserVideoInfo);
	void VideoCancel(IEB_VideoInfo* pVideoInfo,IEB_UserVideoInfo* pUserVideoInfo);
	void VideoEnd(IEB_VideoInfo* pVideoInfo,IEB_UserVideoInfo* pUserVideoInfo);
#else
	void VRequestResponse(const EB_VideoInfo* pVideoInfo,int nStateValue);
	void VAckResponse(const EB_VideoInfo* pVideoInfo,int nStateValue);
	void VideoRequest(const EB_VideoInfo* pVideoInfo, const EB_UserVideoInfo* pUserVideoInfo);
	void VideoAccept(const EB_VideoInfo* pVideoInfo, const EB_UserVideoInfo* pUserVideoInfo);
	void VideoCancel(const EB_VideoInfo* pVideoInfo, const EB_UserVideoInfo* pUserVideoInfo);
	void VideoEnd(const EB_VideoInfo* pVideoInfo, const EB_UserVideoInfo* pUserVideoInfo);
#endif
	//static void ProcessVideoStream(DWORD nId, VIDEOFRAME *pFrame, void *lParam, DWORD udata);
	//static void FFTdataProcess(char *pdata, int length, void* udata);
	//static void VideoDataCallBack(eb::bigint nUserVideoId, BYTE* pData, int dwSize, DWORD dwParam);
	static void EBVideoDataCallBack(eb::bigint nUserId, int nCallBackType, unsigned int lParam, unsigned int wParam, void* pUserData);
	//virtual void DrawVideoData(INT64 nuserid, void * pdata, int len);
protected:
	bool m_bInFVideoRequest;
	bool m_bInVideoConference;
	std::vector<tstring> m_pVideoDevices;
	//int m_nLocalUserId;
	boost::shared_mutex m_mutex; 
	CDlgVideo * m_pUserVideo[MAX_VIDEO_COUNT];
	INT64 m_nVideoUserId[MAX_VIDEO_COUNT];
	CRect m_rectUser[MAX_VIDEO_COUNT];
	CLockList<CDlgVideo*> m_pCloseWnd;
	void CloseAllVideo(void);

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	CTraButton m_btnVideoOn;
	CTraButton m_btnVideoAccept;
	CTraButton m_btnVideoEnd;
	CTraButton m_btnVideoCancel;

	bool IsMyOnVideo(void) const;
	int GetVideoIndex(INT64 nVideoUserId) const;
	int CloseUserVideo(INT64 nVideoUserId);
	int GetNullVideoIndex(void);
	void OpenUserVideo(INT64 nVideoUserId);
	void DoVideoDisonnecte(bool bHideOnly=false, INT64 nOnlyHideUid=0);
	void DoVideoConnected(void);

	// video
//#ifndef USES_EBCOM1
//	LRESULT OnMessageVRequestResponse(WPARAM wParam, LPARAM lParam);
//	LRESULT OnMessageVAckResponse(WPARAM wParam, LPARAM lParam);
//	LRESULT OnMessageVideoRequest(WPARAM wParam, LPARAM lParam);
//	LRESULT OnMessageVideoAccept(WPARAM wParam, LPARAM lParam);
//	LRESULT OnMessageVideoCancel(WPARAM wParam, LPARAM lParam);
//	LRESULT OnMessageVideoEnd(WPARAM wParam, LPARAM lParam);
//#endif
	//LRESULT OnMsgBuildUserVideo(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedButtonVideoEnd();
	afx_msg void OnBnClickedButtonVideoAccept();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnBnClickedButtonOn();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedButtonVideoCancel();
};
