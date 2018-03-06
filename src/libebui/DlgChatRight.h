#pragma once

class CDlgUserList;
class CPanelVideos;
class CPanelRemoteDesktop;
class CDlgUserInfo;
class CPanelFiles;

// CDlgChatRight dialog
class CDlgChatRight : public CEbDialogBase
{
	DECLARE_DYNAMIC(CDlgChatRight)

public:
	typedef boost::shared_ptr<CDlgChatRight> pointer;
	CDlgChatRight(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgChatRight();
	static CDlgChatRight::pointer create(CWnd * pParent = NULL)
	{
		return CDlgChatRight::pointer(new CDlgChatRight(pParent));
	}
	void SetCallInfo(const EB_CallInfo& pCallInfo, const EB_AccountInfo& pFromAccountInfo);

	void SetCtrlColor(void);
	void LineStateChange(eb::bigint nUserId, EB_USER_LINE_STATE bLineState);
	void ExitRD(void);
	void OnMove(void);
	void GetProcessing(bool& pVideoProcessing, bool& pFileProcessing, bool& pDesktopProcessing) const;
	void ExitChat(bool bHangup);
	void OnExitUser(eb::bigint nUserId, bool bExitDep);
	void OnEnterUser(eb::bigint nUserId, const char * sFromInfo);
#ifdef USES_EBCOM_TEST
	void OnUserEmpInfo(IEB_MemberInfo* pMemberInfo);
	void RDRequestResponse(IEB_RemoteDesktopInfo* pVideoInfo,int nStateValue);
	void VRequestResponse(IEB_VideoInfo* pVideoInfo,int nStateValue);
	void VAckResponse(IEB_VideoInfo* pVideoInfo,int nStateValue);
	void VideoRequest(IEB_VideoInfo* pVideoInfo,IEB_UserVideoInfo* pUserVideoInfo);
	void VideoAccept(IEB_VideoInfo* pVideoInfo,IEB_UserVideoInfo* pUserVideoInfo);
	void VideoCancel(IEB_VideoInfo* pVideoInfo,IEB_UserVideoInfo* pUserVideoInfo);
	void VideoEnd(IEB_VideoInfo* pVideoInfo,IEB_UserVideoInfo* pUserVideoInfo);
#else
	void OnUserEmpInfo(const EB_MemberInfo* pMemberInfo);
	void RDRequestResponse(const EB_RemoteDesktopInfo* pVideoInfo,int nStateValue);
	void RDAckResponse(const EB_RemoteDesktopInfo* pVideoInfo,int nStateValue);
	void RDRequest(const EB_RemoteDesktopInfo* pVideoInfo);
	void RDAccept(const EB_RemoteDesktopInfo* pVideoInfo);
	void RDCancel(const EB_RemoteDesktopInfo* pVideoInfo);
	void RDEnd(const EB_RemoteDesktopInfo* pVideoInfo);
	void VRequestResponse(const EB_VideoInfo* pVideoInfo,int nStateValue);
	void VAckResponse(const EB_VideoInfo* pVideoInfo,int nStateValue);
	void VideoRequest(const EB_VideoInfo* pVideoInfo,const EB_UserVideoInfo* pUserVideoInfo);
	void VideoAccept(const EB_VideoInfo* pVideoInfo,const EB_UserVideoInfo* pUserVideoInfo);
	void VideoCancel(const EB_VideoInfo* pVideoInfo,const EB_UserVideoInfo* pUserVideoInfo);
	void VideoEnd(const EB_VideoInfo* pVideoInfo,const EB_UserVideoInfo* pUserVideoInfo);
#endif
	void DeleteDlgTranFile(eb::bigint nMsgId);
#ifdef USES_EBCOM_TEST
	void OnSendingFile(IEB_ChatFileInfo* pCrFileInfo);
	void OnReceivingFile(IEB_ChatFileInfo* pCrFileInfo);
	void OnReceivedFile(IEB_ChatFileInfo* pCrFileInfo);
	void SetFilePercent(IEB_ChatFilePercent* pChatRoomFilePercent);
#else
	void OnSendingFile(const CCrFileInfo * pCrFileInfo);
	void OnReceivingFile(const CCrFileInfo * pCrFileInfo);
	void OnReceivedFile(const CCrFileInfo * pCrFileInfo);
	void SetFilePercent(const CChatRoomFilePercent * pChatRoomFilePercent);
#endif

	void OpenVideoPan(void);
	void OpenRdPan(void);
	void OpenFilePan(void);

	EB_CallInfo m_pCallInfo;
	//Cchatroom::pointer m_pChatRoom;
	EB_AccountInfo m_pFromAccountInfo;


// Dialog Data
	enum { IDD = IDD_DLG_CHATRIGHT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	CTraButton m_labelUserList;
	CTraButton m_labelECard;
	CTraButton m_labelVideos;
	CTraButton m_labelRemoteDesktop;
	CTraButton m_labelFiles;
	CDlgUserList* m_pDlgUserList;
	CDlgUserInfo* m_pDlgUserInfo;
	CPanelVideos* m_pPanVideos;
	CPanelRemoteDesktop* m_pPanRemoteDesktop;
	CPanelFiles* m_pPanFiles;

	HWND m_hCurrentSelect;
	void Update(void);

	DECLARE_MESSAGE_MAP()
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnBnClickedButtonEcard();
	afx_msg void OnBnClickedButtonVideos();
public:
	afx_msg void OnPaint();
	afx_msg void OnBnClickedButtonUserlist();
	afx_msg void OnBnClickedButtonFiles();
	afx_msg void OnBnClickedButtonRd();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
