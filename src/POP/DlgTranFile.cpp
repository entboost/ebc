// DlgTranFile.cpp : implementation file
//

#include "stdafx.h"
#include "POP.h"
#include "DlgTranFile.h"


// CDlgTranFile dialog

IMPLEMENT_DYNAMIC(CDlgTranFile, CEbDialogBase)

CDlgTranFile::CDlgTranFile(CWnd* pParent /*=NULL*/)
	: CEbDialogBase(CDlgTranFile::IDD, pParent)
	, m_bIsSendingFile(false)
	//, m_sResourceId(0)
{

}

CDlgTranFile::~CDlgTranFile()
{
}

void CDlgTranFile::DoDataExchange(CDataExchange* pDX)
{
	CEbDialogBase::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROGRESS_PERCENT, m_progPercent);
	DDX_Control(pDX, IDC_BUTTON_SAVE2CLOUDDRIVE, m_btnSave2CloudDrive);
	DDX_Control(pDX, IDC_BUTTON_SAVEAS, m_btnSaveas);
	DDX_Control(pDX, IDC_BUTTON_SAVE, m_btnSave);
	DDX_Control(pDX, IDC_BUTTON_REJECT, m_btnReject);
	DDX_Control(pDX, IDC_BUTTON_OFFFILE, m_btnOffFile);
	DDX_Control(pDX, IDC_BUTTON_CANCEL, m_btnCancel);
}


BEGIN_MESSAGE_MAP(CDlgTranFile, CEbDialogBase)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BUTTON_SAVE2CLOUDDRIVE, &CDlgTranFile::OnBnClickedButtonSave2CloudDrive)
	ON_BN_CLICKED(IDC_BUTTON_SAVEAS, &CDlgTranFile::OnBnClickedButtonSaveas)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, &CDlgTranFile::OnBnClickedButtonSave)
	ON_BN_CLICKED(IDC_BUTTON_REJECT, &CDlgTranFile::OnBnClickedButtonReject)
	ON_BN_CLICKED(IDC_BUTTON_CANCEL, &CDlgTranFile::OnBnClickedButtonCancel)
	ON_WM_LBUTTONDOWN()
	ON_BN_CLICKED(IDC_BUTTON_OFFFILE, &CDlgTranFile::OnBnClickedButtonOfffile)
	ON_WM_PAINT()
END_MESSAGE_MAP()


// CDlgTranFile message handlers
//const double const_kb_size = 1024.0;
//const double const_mb_size = 1024*const_kb_size;
//const double const_gb_size = 1024*const_mb_size;

void CDlgTranFile::SetCtrlColor(void)
{
	m_btnSaveas.SetDrawPanel(true,-1,theApp.GetHotColor(),theApp.GetPreColor());
	m_btnSave.SetDrawPanel(true,-1,theApp.GetHotColor(),theApp.GetPreColor());
	m_btnReject.SetDrawPanel(true,-1,theApp.GetHotColor(),theApp.GetPreColor());
	m_btnOffFile.SetDrawPanel(true,-1,theApp.GetHotColor(),theApp.GetPreColor());
	m_btnCancel.SetDrawPanel(true,-1,theApp.GetHotColor(),theApp.GetPreColor());
}

BOOL CDlgTranFile::OnInitDialog()
{
	CEbDialogBase::OnInitDialog();
	SetTransparentType(CEbDialogBase::TT_STATIC);
	this->SetMouseMove(FALSE);
	//this->SetBgColor(theApp.GetMainColor());

	if (!m_bIsSendingFile && m_pCrFileInfo.m_bOffFile)
	{
		m_btnSave2CloudDrive.SetToolTipText(_T("保存到我的云盘"));
		m_btnSave2CloudDrive.SetAutoSize(false);
		m_btnSave2CloudDrive.SetNorTextColor(RGB(0,128,255));
		m_btnSave2CloudDrive.SetHotTextColor(RGB(0,128,255));
		m_btnSave2CloudDrive.SetPreTextColor(RGB(0,128,255));
		m_btnSave2CloudDrive.ShowWindow(SW_SHOW);
	}else
	{
		m_btnSave2CloudDrive.ShowWindow(SW_HIDE);
	}

	m_btnSaveas.SetTextHotMove(false);
	m_btnSaveas.SetToolTipText(_T("保存文件到指定目录"));
	m_btnSaveas.SetAutoSize(false);
	m_btnSaveas.SetNorTextColor(RGB(0,128,255));
	m_btnSaveas.SetHotTextColor(RGB(255,255,255));
	m_btnSaveas.SetPreTextColor(RGB(255,255,255));
	//m_btnSaveas.Load(IDB_PNG_58X24);
	m_btnSave.SetTextHotMove(false);
	m_btnSave.SetToolTipText(_T("保存文件到默认目录"));
	m_btnSave.SetAutoSize(false);
	m_btnSave.SetNorTextColor(RGB(0,128,255));
	m_btnSave.SetHotTextColor(RGB(255,255,255));
	m_btnSave.SetPreTextColor(RGB(255,255,255));
	//m_btnSave.Load(IDB_PNG_58X24);
	m_btnReject.SetTextHotMove(false);
	m_btnReject.SetToolTipText(_T("拒绝接收文件"));
	m_btnReject.SetAutoSize(false);
	m_btnReject.SetNorTextColor(RGB(0,128,255));
	m_btnReject.SetHotTextColor(RGB(255,255,255));
	m_btnReject.SetPreTextColor(RGB(255,255,255));
	//m_btnReject.Load(IDB_PNG_58X24);
	m_btnOffFile.SetTextHotMove(false);
	m_btnOffFile.SetToolTipText(_T("发送离线文件"));
	m_btnOffFile.SetAutoSize(false);
	m_btnOffFile.SetNorTextColor(RGB(0,128,255));
	m_btnOffFile.SetHotTextColor(RGB(255,255,255));
	m_btnOffFile.SetPreTextColor(RGB(255,255,255));
	//m_btnOffFile.Load(IDB_PNG_58X24);
	m_btnCancel.SetTextHotMove(false);
	m_btnCancel.SetToolTipText(_T("取消发送文件"));
	m_btnCancel.SetAutoSize(false);
	m_btnCancel.SetNorTextColor(RGB(0,128,255));
	m_btnCancel.SetHotTextColor(RGB(255,255,255));
	m_btnCancel.SetPreTextColor(RGB(255,255,255));
	//m_btnCancel.Load(IDB_PNG_58X24);
	SetCtrlColor();

	const tstring sFileName = libEbc::GetFileName(m_pCrFileInfo.m_sFileName);
	CString sFileText;
	if (m_pCrFileInfo.m_nFileSize >= const_gb_size)
		sFileText.Format(_T("%s(%.02fGB)"),sFileName.c_str(),(double)m_pCrFileInfo.m_nFileSize/const_gb_size);
	else if (m_pCrFileInfo.m_nFileSize >= const_mb_size)
		sFileText.Format(_T("%s(%.02fMB)"),sFileName.c_str(),(double)m_pCrFileInfo.m_nFileSize/const_mb_size);
	else if (m_pCrFileInfo.m_nFileSize >= const_kb_size)
		sFileText.Format(_T("%s(%.02fKB)"),sFileName.c_str(),(double)m_pCrFileInfo.m_nFileSize/const_kb_size);
	else if (m_pCrFileInfo.m_nFileSize>0)
		sFileText.Format(_T("%s(%lldByte)"),sFileName.c_str(),m_pCrFileInfo.m_nFileSize);
	else
		sFileText = sFileName.c_str();
	this->GetDlgItem(IDC_STATIC_FILENAME)->SetWindowText(sFileText);
	//if (m_sResourceId>0)
	//{
	//	this->GetDlgItem(IDC_BUTTON_SAVEAS)->ShowWindow(SW_HIDE);
	//	this->GetDlgItem(IDC_BUTTON_SAVE)->ShowWindow(SW_HIDE);
	//	this->GetDlgItem(IDC_BUTTON_REJECT)->ShowWindow(SW_HIDE);
	//	this->GetDlgItem(IDC_BUTTON_OFFFILE)->ShowWindow(SW_HIDE);
	//	this->GetDlgItem(IDC_BUTTON_CANCEL)->ShowWindow(SW_SHOW);
	//}else
	{
		if (m_bIsSendingFile)
		{
			this->GetDlgItem(IDC_BUTTON_SAVEAS)->ShowWindow(SW_HIDE);
			this->GetDlgItem(IDC_BUTTON_SAVE)->ShowWindow(SW_HIDE);
			this->GetDlgItem(IDC_BUTTON_REJECT)->ShowWindow(SW_HIDE);
			if (m_pCrFileInfo.GetStateCode()==EB_STATE_WAITING_PROCESS)
			{
				this->GetDlgItem(IDC_BUTTON_OFFFILE)->ShowWindow(SW_HIDE);
				this->GetDlgItem(IDC_BUTTON_CANCEL)->ShowWindow(SW_HIDE);
			}else
			{
				if (m_pCrFileInfo.m_sResId>0 || m_pCrFileInfo.m_bOffFile)
					this->GetDlgItem(IDC_BUTTON_OFFFILE)->ShowWindow(SW_HIDE);
				else
					this->GetDlgItem(IDC_BUTTON_OFFFILE)->ShowWindow(SW_SHOW);
				this->GetDlgItem(IDC_BUTTON_CANCEL)->ShowWindow(SW_SHOW);
			}
		//}else if (m_pCrFileInfo.m_sResId>0 && (!m_pCrFileInfo.m_bOffFile || m_pCrFileInfo.m_sResId==m_pCrFileInfo.m_nMsgId))
		}else if (m_pCrFileInfo.m_sResId>0)// && m_pCrFileInfo.m_sResId==m_pCrFileInfo.m_nMsgId)
		{
			this->GetDlgItem(IDC_BUTTON_SAVEAS)->ShowWindow(SW_HIDE);
			this->GetDlgItem(IDC_BUTTON_SAVE)->ShowWindow(SW_HIDE);
			this->GetDlgItem(IDC_BUTTON_REJECT)->ShowWindow(SW_HIDE);
			this->GetDlgItem(IDC_BUTTON_OFFFILE)->ShowWindow(SW_HIDE);
			this->GetDlgItem(IDC_BUTTON_CANCEL)->ShowWindow(SW_SHOW);
			m_btnCancel.SetToolTipText(_T("取消下载文件"));
		}else
		{
			//if (this->m_pCallInfo.m_sGroupCode>0)
			//{
			//	m_btnReject.SetWindowText(_T("取消"));
			//	m_btnSave.SetWindowText(_T("下载"));
			//}
			this->GetDlgItem(IDC_BUTTON_SAVEAS)->ShowWindow(SW_SHOW);
			this->GetDlgItem(IDC_BUTTON_SAVE)->ShowWindow(SW_SHOW);
			this->GetDlgItem(IDC_BUTTON_REJECT)->ShowWindow(SW_SHOW);
			this->GetDlgItem(IDC_BUTTON_OFFFILE)->ShowWindow(SW_HIDE);
			this->GetDlgItem(IDC_BUTTON_CANCEL)->ShowWindow(SW_HIDE);
		}
	}
	m_progPercent.SetBitmap(IDB_BITMAP_PROGRESS);
	m_progPercent.SetRange(0, 100*100);
	this->GetDlgItem(IDC_STATIC_SHEECHSTATUS)->SetWindowText(_T(""));

	SetDlgChildFont(theDefaultDialogFontSize,theFontFace.c_str());
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgTranFile::OnSize(UINT nType, int cx, int cy)
{
	CEbDialogBase::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	if (GetDlgItem(IDC_STATIC_FILENAME)->GetSafeHwnd())
		this->GetDlgItem(IDC_STATIC_FILENAME)->MoveWindow(2, 2, cx-2*2, 20);
	if (m_progPercent.GetSafeHwnd())
		m_progPercent.MoveWindow(2, 22, cx-4, 15);
	if (GetDlgItem(IDC_STATIC_SHEECHSTATUS)->GetSafeHwnd())
		this->GetDlgItem(IDC_STATIC_SHEECHSTATUS)->MoveWindow(2, 40, cx-4, 14);
	const int const_Button_Top		= 40;
	const int const_Button_Width1	= 30;//m_btnSaveas.GetImgWidth();
	const int const_Button_Width2	= 55;//m_btnSaveas.GetImgWidth();
	const int const_Button_Height	= 17;//m_btnSaveas.GetImgHeight();
	const int const_Button_Interval = 2;	// 按钮间隔
	int x = cx-const_Button_Width1-3;			// 3是最右边间隔
		
	if (m_pCrFileInfo.m_sResId>0 || m_bIsSendingFile)
	{
		m_btnCancel.MovePoint(x, const_Button_Top, const_Button_Width1,const_Button_Height);
		if (m_pCrFileInfo.m_sResId==0)
		{
			x -= (const_Button_Width2+const_Button_Interval);
			m_btnOffFile.MovePoint(x, const_Button_Top,const_Button_Width2,const_Button_Height);
		}
	}else
	{
		m_btnSaveas.MovePoint(x, const_Button_Top,const_Button_Width1,const_Button_Height);
		x -= (const_Button_Width1+const_Button_Interval);
		m_btnSave.MovePoint(x, const_Button_Top,const_Button_Width1,const_Button_Height);
		x -= (const_Button_Width1+const_Button_Interval);
		m_btnReject.MovePoint(x, const_Button_Top,const_Button_Width1,const_Button_Height);
	}
	if (!m_bIsSendingFile && m_pCrFileInfo.m_bOffFile)
	{
		x -= (const_Button_Width2+const_Button_Interval);
		m_btnSave2CloudDrive.MovePoint(x, const_Button_Top,const_Button_Width2,const_Button_Height);
	}
}

void CDlgTranFile::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class

	//CEbDialogBase::OnOK();
}

void CDlgTranFile::OnCancel()
{
	// TODO: Add your specialized code here and/or call the base class

	//CEbDialogBase::OnCancel();
}

void CDlgTranFile::OnBnClickedButtonSave2CloudDrive()
{
#ifdef USES_EBCOM_TEST
	theEBClientCore->EB_Save2CloudDrive(m_pCrFileInfo.GetCallId(),m_pCrFileInfo.m_nMsgId);
#else
	theEBAppClient.EB_Save2CloudDrive(m_pCrFileInfo.GetCallId(),m_pCrFileInfo.m_nMsgId);
#endif
}

void CDlgTranFile::OnBnClickedButtonSaveas()
{
	tstring sFileName;
	tstring sFileExt;
	libEbc::GetFileExt(m_pCrFileInfo.m_sFileName,sFileName,sFileExt);
	CFileDialog dlg(FALSE, sFileExt.c_str(), m_pCrFileInfo.m_sFileName.c_str(), OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, "All Files (*.*)|*.*||", this);
	if (dlg.DoModal() == IDOK)
	{
		const CString sPathName(dlg.GetPathName());
		if (::PathFileExists(sPathName))
		{
			// * 判断文件是否打开
			FILE * f = fopen(sPathName,"ab");
			if (f==NULL)
			{
				CDlgMessageBox::EbMessageBox(this,"",_T("目标文件被占用，不能接收文件：\r\n请关闭文件后重试！"),CDlgMessageBox::IMAGE_ERROR,3);
				return;
			}
			fclose(f);
		}
		//m_pChatRoom->ReceiveFile(m_pCrFileInfo.m_nMsgId, sPathName);
#ifdef USES_EBCOM_TEST
		theEBClientCore->EB_AcceptFile(m_pCrFileInfo.GetCallId(),m_pCrFileInfo.m_nMsgId, (LPCTSTR)sPathName);
#else
		theEBAppClient.EB_AcceptFile(m_pCrFileInfo.GetCallId(),m_pCrFileInfo.m_nMsgId, sPathName);
#endif
		// ? P2P等开始传统文件再hide
		m_btnSave2CloudDrive.ShowWindow(SW_HIDE);
		m_btnSaveas.EnableWindow(FALSE);
		m_btnSave.EnableWindow(FALSE);
		m_btnReject.ShowWindow(SW_HIDE);
		m_btnReject.SetWindowText(_T("取消"));
		m_btnReject.ShowWindow(SW_SHOW);
		m_btnReject.SetToolTipText(_T("停止接收文件"));
	}
}
void CDlgTranFile::OnBnClickedButtonSave()
{
	// TODO: Add your control notification handler code here
	const tstring sFileName = m_pCrFileInfo.m_sFileName;
	tstring sFileSimpleName;
	tstring sFileExt;
	libEbc::GetFileExt(sFileName, sFileSimpleName, sFileExt);
	//size_t nFileSize = m_pCrFileInfo.m_nFileSize;
	CString sSaveTo;
	sSaveTo.Format(_T("%s\\%s"), theApp.GetUserFilePath(), sFileName.c_str());
	int index = 0;
	while (true)
	{
		if (!PathFileExists(sSaveTo))
			break;
		if (sFileExt.empty())
			sSaveTo.Format(_T("%s\\%s(%d)"), theApp.GetUserFilePath(), sFileName.c_str(), ++index);
		else
			sSaveTo.Format(_T("%s\\%s(%d).%s"), theApp.GetUserFilePath(), sFileSimpleName.c_str(), ++index, sFileExt.c_str());
	}
#ifdef USES_EBCOM_TEST
	theEBClientCore->EB_AcceptFile(m_pCrFileInfo.GetCallId(),m_pCrFileInfo.m_nMsgId, (LPCTSTR)sSaveTo);
#else
	theEBAppClient.EB_AcceptFile(m_pCrFileInfo.GetCallId(),m_pCrFileInfo.m_nMsgId, sSaveTo);
#endif
	//m_pChatRoom->ReceiveFile(m_pCrFileInfo.m_nMsgId, sSaveTo);
	m_btnSave2CloudDrive.ShowWindow(SW_HIDE);
	m_btnSaveas.EnableWindow(FALSE);
	m_btnSave.EnableWindow(FALSE);
	m_btnReject.ShowWindow(SW_HIDE);
	m_btnReject.SetWindowText(_T("取消"));
	m_btnReject.ShowWindow(SW_SHOW);
	m_btnReject.SetToolTipText(_T("停止接收文件"));
}
void CDlgTranFile::OnBnClickedButtonReject()
{
#ifdef USES_EBCOM_TEST
	theEBClientCore->EB_CancelFile(m_pCrFileInfo.GetCallId(),m_pCrFileInfo.m_nMsgId);
#else
	theEBAppClient.EB_CancelFile(m_pCrFileInfo.GetCallId(),m_pCrFileInfo.m_nMsgId);
#endif
}
void CDlgTranFile::Cancel(void)
{
	OnBnClickedButtonCancel();
}
void CDlgTranFile::UpdateFileInfo(const CCrFileInfo * pCrFileInfo)
{
	m_pCrFileInfo = pCrFileInfo;
	// 更新 界面
	if (m_bIsSendingFile)
	{
		if (m_pCrFileInfo.GetStateCode()==EB_STATE_WAITING_PROCESS)
		{
			this->GetDlgItem(IDC_BUTTON_OFFFILE)->ShowWindow(SW_HIDE);
			this->GetDlgItem(IDC_BUTTON_CANCEL)->ShowWindow(SW_HIDE);
		}else
		{
			if (m_pCrFileInfo.m_sResId>0 || m_pCrFileInfo.m_bOffFile)
				this->GetDlgItem(IDC_BUTTON_OFFFILE)->ShowWindow(SW_HIDE);
			else
				this->GetDlgItem(IDC_BUTTON_OFFFILE)->ShowWindow(SW_SHOW);
			this->GetDlgItem(IDC_BUTTON_CANCEL)->ShowWindow(SW_SHOW);
		}
	}
}

void CDlgTranFile::OnBnClickedButtonCancel()
{
	if (m_pCrFileInfo.m_sResId>0 && m_pCrFileInfo.m_sResId==m_pCrFileInfo.m_nMsgId)
	{
		theApp.m_pCancelFileList.insert(m_pCrFileInfo.m_nMsgId,true,false);
		return;
	}

#ifdef USES_EBCOM_TEST
	if (m_pCrFileInfo.m_sResId>0)
		theEBClientCore->EB_CancelFileRes(m_pCrFileInfo.m_sResId,m_pCrFileInfo.m_nMsgId);
	else
		theEBClientCore->EB_CancelFile(m_pCrFileInfo.GetCallId(),m_pCrFileInfo.m_nMsgId);
#else
	if (m_pCrFileInfo.m_sResId>0)
		theEBAppClient.EB_CancelFileRes(m_pCrFileInfo.m_sResId,m_pCrFileInfo.m_nMsgId);
	else
		theEBAppClient.EB_CancelFile(m_pCrFileInfo.GetCallId(),m_pCrFileInfo.m_nMsgId);
#endif
}

#ifdef USES_EBCOM_TEST
void CDlgTranFile::SetFilePercent(IEB_ChatFilePercent* pChatRoomFilePercent)
{
	m_btnOffFile.EnableWindow(FALSE);
	m_progPercent.SetPos(pChatRoomFilePercent->Percent*100);
	CString sSheechStatus;
	if (pChatRoomFilePercent->CurSpeed>1024000.0)
		sSheechStatus.Format(_T("%.2f%% %.2fMB/S"), pChatRoomFilePercent->Percent,pChatRoomFilePercent->CurSpeed/(1024.0*1024.0));
	else
		sSheechStatus.Format(_T("%.2f%% %.2fKB/S"), pChatRoomFilePercent->Percent,pChatRoomFilePercent->CurSpeed/1024.0);
	//sSheechStatus.Format(_T("%.2f%% %.2fKB/%.2fKB %ds %ds"), pChatRoomFilePercent->Percent,
	//	pChatRoomFilePercent->CurSpeed/1024.0,pChatRoomFilePercent->AvrSpeed/1024.0,pChatRoomFilePercent->TranSeconds,pChatRoomFilePercent->LeftSeconds);
	CWnd * pWnd = GetDlgItem(IDC_STATIC_SHEECHSTATUS);
	pWnd->SetWindowText(sSheechStatus);
	CRect rect;
	pWnd->GetWindowRect(&rect);
	this->GetParent()->GetParent()->ScreenToClient(&rect);
	this->GetParent()->GetParent()->InvalidateRect(&rect);
}
#else
void CDlgTranFile::SetFilePercent(const CChatRoomFilePercent * pChatRoomFilePercent)
{
	m_btnOffFile.EnableWindow(FALSE);
	m_progPercent.SetPos(pChatRoomFilePercent->m_percent*100);
	CString sSheechStatus;
	if (pChatRoomFilePercent->m_percent>100)
	{
		// ** 这是传输字节
		const double fReceivedBytes = pChatRoomFilePercent->m_percent;
		if (fReceivedBytes >= const_gb_size)
			sSheechStatus.Format(_T("%.02fGB"),(double)fReceivedBytes/const_gb_size);
		else if (fReceivedBytes >= const_mb_size)
			sSheechStatus.Format(_T("%.02fMB"),(double)fReceivedBytes/const_mb_size);
		else if (fReceivedBytes >= const_kb_size)
			sSheechStatus.Format(_T("%.02fKB"),(double)fReceivedBytes/const_kb_size);
		else
			sSheechStatus.Format(_T("%dByte"),(int)fReceivedBytes);
	}else if (pChatRoomFilePercent->m_nCurSpeed>1024000.0)
		sSheechStatus.Format(_T("%.2f%% %.2fMB/S"), pChatRoomFilePercent->m_percent,pChatRoomFilePercent->m_nCurSpeed/(1024.0*1024.0));
	else
		sSheechStatus.Format(_T("%.2f%% %.2fKB/S"), pChatRoomFilePercent->m_percent,pChatRoomFilePercent->m_nCurSpeed/1024.0);
	//sSheechStatus.Format(_T("%.2f%% %.2fKB/%.2fKB %ds %ds"), pChatRoomFilePercent->m_percent,
	//	pChatRoomFilePercent->m_nCurSpeed/1024.0,pChatRoomFilePercent->m_nAvrSpeed/1024.0,pChatRoomFilePercent->m_nTranSeconds,pChatRoomFilePercent->m_nLeftSeconds);
	CWnd * pWnd = GetDlgItem(IDC_STATIC_SHEECHSTATUS);
	pWnd->SetWindowText(sSheechStatus);
	CRect rect;
	pWnd->GetWindowRect(&rect);
	this->GetParent()->GetParent()->ScreenToClient(&rect);
	this->GetParent()->GetParent()->InvalidateRect(&rect);
}
#endif

void CDlgTranFile::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	//this->GetParent()->GetParent()->PostMessage(WM_NCLBUTTONDOWN, HTCAPTION, MAKELPARAM(point.x, point.y));	
	CEbDialogBase::OnLButtonDown(nFlags, point);
}

void CDlgTranFile::OnBnClickedButtonOfffile()
{
	// ???这里是否用私聊
	const eb::bigint sCallId(m_pCrFileInfo.GetCallId());
	const CString sFileName(m_pCrFileInfo.m_sFileName.c_str());
#ifdef USES_EBCOM_TEST
	theEBClientCore->EB_SendFile(sCallId,(LPCTSTR)sFileName,0,VARIANT_FALSE,VARIANT_TRUE);	// 必须先发送，后面取消
	theEBClientCore->EB_CancelFile(m_pCrFileInfo.GetCallId(),m_pCrFileInfo.m_nMsgId);
#else
	theEBAppClient.EB_SendFile(sCallId,sFileName,0,false,true);	// 必须先发送，后面取消
	theEBAppClient.EB_CancelFile(m_pCrFileInfo.GetCallId(),m_pCrFileInfo.m_nMsgId);
#endif
}

void CDlgTranFile::OnPaint()
{
	if (IsIconic())
	{
		//CPaintDC dc(this); // 用于绘制的设备上下文

		//SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		//// 使图标在工作区矩形中居中
		//int cxIcon = GetSystemMetrics(SM_CXICON);
		//int cyIcon = GetSystemMetrics(SM_CYICON);
		//CRect rect;
		//GetClientRect(&rect);
		//int x = (rect.Width() - cxIcon + 1) / 2;
		//int y = (rect.Height() - cyIcon + 1) / 2;

		//// 绘制图标
		//dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CPaintDC dc(this); // 用于绘制的设备上下文
		CRect rectClient;
		this->GetClientRect(&rectClient);
		CSkinMemDC memDC(&dc, rectClient);
		this->ClearBgDrawInfo();
		this->AddBgDrawInfo(CEbBackDrawInfo(0,1.0,false,false,0,theDefaultFlatBgColor));
		//this->AddBgDrawInfo(CEbBackDrawInfo(0,theDefaultBgColorBgLight2,false));
		this->DrawPopBg(&memDC, theApp.GetMainColor(),0);
	}

}
