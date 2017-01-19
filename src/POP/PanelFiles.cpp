// PanelFiles.cpp : implementation file
//

#include "stdafx.h"
#include "POP.h"
#include "PanelFiles.h"

#define TIMER_ID_OPEN_TRAN_FILE 1001
#define TIMER_ID_CLOSE_TRAN_FILE 1002

//const int const_TranFile_Width		= 250;
const int const_TranFile_Height		= 60;

// CPanelFiles dialog

IMPLEMENT_DYNAMIC(CPanelFiles, CEbDialogBase)

CPanelFiles::CPanelFiles(CWnd* pParent /*=NULL*/)
	: CEbDialogBase(CPanelFiles::IDD, pParent)
{

}

CPanelFiles::~CPanelFiles()
{
}

void CPanelFiles::DoDataExchange(CDataExchange* pDX)
{
	CEbDialogBase::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPanelFiles, CEbDialogBase)
	ON_WM_SIZE()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONDOWN()
	ON_WM_DESTROY()
	//ON_WM_PAINT()
	ON_WM_CTLCOLOR()
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CPanelFiles message handlers

BOOL CPanelFiles::OnInitDialog()
{
	CEbDialogBase::OnInitDialog();
	//SetTransparentType(CEbDialogBase::TT_DLG);
	this->SetMouseMove(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CPanelFiles::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class

	//CEbDialogBase::OnOK();
}

void CPanelFiles::OnCancel()
{
	// TODO: Add your specialized code here and/or call the base class

	//CEbDialogBase::OnCancel();
}

void CPanelFiles::OnSize(UINT nType, int cx, int cy)
{
	CEbDialogBase::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	MoveSize(cx, cy);
}
void CPanelFiles::MoveSize(int cx, int cy)
{
	int index = 0;
	BoostReadLock rdlock(m_pTranFiles.mutex());
	//boost::mutex::scoped_lock lock(m_pTranFiles.mutex());
	CLockMap<eb::bigint, CDlgTranFile::pointer>::const_iterator pIter = m_pTranFiles.begin();
	for (; pIter!=m_pTranFiles.end(); pIter++)
	{
		CDlgTranFile::pointer pDlgTranFile = pIter->second;
		pDlgTranFile->MoveWindow(0, index*const_TranFile_Height, cx, const_TranFile_Height);
		//pDlgTranFile->MoveWindow(0, index*const_TranFile_Height, const_TranFile_Width, const_TranFile_Height);
		index++;
	}
}

void CPanelFiles::SetCtrlColor(void)
{
	BoostReadLock rdlock(m_pTranFiles.mutex());
	CLockMap<eb::bigint, CDlgTranFile::pointer>::const_iterator pIter = m_pTranFiles.begin();
	for (; pIter!=m_pTranFiles.end(); pIter++)
	{
		CDlgTranFile::pointer pDlgTranFile = pIter->second;
		pDlgTranFile->SetCtrlColor();
	}
}
void CPanelFiles::ExitChat(bool bHangup)
{
	while (!m_pTranFiles.empty())
	{
		BoostWriteLock wtlock(m_pTranFiles.mutex());
		//boost::mutex::scoped_lock lock(m_pTranFiles.mutex());
		CLockMap<eb::bigint, CDlgTranFile::pointer>::const_iterator pIter = m_pTranFiles.begin();
		if (pIter!=m_pTranFiles.end())
		{
			CDlgTranFile::pointer pDlgTranFile = pIter->second;
			m_pTranFiles.erase(pIter);
			wtlock.unlock();	// 解锁，预防死锁
			pDlgTranFile->Cancel();
			pDlgTranFile->DestroyWindow();
		}
	}
}

#ifdef USES_EBCOM_TEST
void CPanelFiles::OnSendingFile(IEB_ChatFileInfo* pCrFileInfo)
{
	CRect rect;
	this->GetClientRect(&rect);
	int index = m_pTranFiles.size();
	CDlgTranFile::pointer pDlgTranFile = CDlgTranFile::create(this);
	pDlgTranFile->m_bIsSendingFile = true;
	pDlgTranFile->m_sResourceId = pCrFileInfo->ResId;
	pDlgTranFile->m_pCrFileInfo = pCrFileInfo;
	pDlgTranFile->Create(CDlgTranFile::IDD, this);
	pDlgTranFile->ShowWindow(SW_SHOW);
	pDlgTranFile->MoveWindow(0, index*const_TranFile_Height, rect.Width(), const_TranFile_Height);
	m_pTranFiles.insert(pCrFileInfo->MsgId, pDlgTranFile);
}
#else
void CPanelFiles::OnSendingFile(const CCrFileInfo * pCrFileInfo)
{
	if (pCrFileInfo->GetStateCode()==EB_STATE_FILE_ALREADY_EXIST)
	{
		CDlgTranFile::pointer pDlgTranFile;
		if (pCrFileInfo->GetParam()>0 && m_pTranFiles.find(pCrFileInfo->GetParam(),pDlgTranFile, true))
		{
			m_pDelTranFile.add(pDlgTranFile);
			SetTimer(TIMER_ID_CLOSE_TRAN_FILE,10,NULL);
		}
		return;
	}
	CDlgTranFile::pointer pDlgTranFile;
	if (pCrFileInfo->GetParam()>0 && m_pTranFiles.find(pCrFileInfo->GetParam(),pDlgTranFile,true))
	{
		// ** 找到前面等待预处理发送文件，更新界面即可
		pDlgTranFile->UpdateFileInfo(pCrFileInfo);
	}else
	{
		pDlgTranFile = CDlgTranFile::create(this);
		pDlgTranFile->m_bIsSendingFile = true;
		//pDlgTranFile->m_pCallInfo = m_pCallInfo;
		//pDlgTranFile->m_sResourceId = pCrFileInfo->m_sResId;
		pDlgTranFile->m_pCrFileInfo = pCrFileInfo;
		pDlgTranFile->Create(CDlgTranFile::IDD, this);
		pDlgTranFile->ShowWindow(SW_SHOW);

		CRect rect;
		this->GetClientRect(&rect);
		int index = m_pTranFiles.size();
		pDlgTranFile->MoveWindow(0, index*const_TranFile_Height, rect.Width(), const_TranFile_Height);
	}
	m_pTranFiles.insert(pCrFileInfo->m_nMsgId, pDlgTranFile);
}
#endif

#ifdef USES_EBCOM_TEST
void CPanelFiles::OnReceivingFile(IEB_ChatFileInfo* pCrFileInfo)
{
	if (m_pTranFiles.exist(pCrFileInfo->MsgId))
		return;
	m_pAddTranFile.add(CCrFileInfo(pCrFileInfo));
	SetTimer(TIMER_ID_OPEN_TRAN_FILE,10,NULL);

	//CRect rect;
	//this->GetClientRect(&rect);
	//int index = m_pTranFiles.size();
	//CDlgTranFile::pointer pDlgTranFile = CDlgTranFile::create(this);
	//pDlgTranFile->m_bIsSendingFile = false;
	//pDlgTranFile->m_sResourceId = pCrFileInfo->ResId;
	//pDlgTranFile->m_pCrFileInfo = pCrFileInfo;
	//pDlgTranFile->Create(CDlgTranFile::IDD, this);
	//pDlgTranFile->ShowWindow(SW_SHOW);
	//pDlgTranFile->MoveWindow(0, index*const_TranFile_Height, rect.Width(), const_TranFile_Height);
	//m_pTranFiles.insert(pCrFileInfo->MsgId, pDlgTranFile);
}
#else
void CPanelFiles::OnReceivingFile(const CCrFileInfo * pCrFileInfo)
{
	if (m_pTranFiles.exist(pCrFileInfo->m_nMsgId))
		return;
	m_pAddTranFile.add(*pCrFileInfo);
	SetTimer(TIMER_ID_OPEN_TRAN_FILE,10,NULL);

	//CRect rect;
	//this->GetClientRect(&rect);
	//int index = m_pTranFiles.size();
	//CDlgTranFile::pointer pDlgTranFile = CDlgTranFile::create(this);
	//pDlgTranFile->m_bIsSendingFile = false;
	//pDlgTranFile->m_sResourceId = pCrFileInfo->m_sResId;
	//pDlgTranFile->m_pCrFileInfo = pCrFileInfo;
	//pDlgTranFile->Create(CDlgTranFile::IDD, this);
	//pDlgTranFile->ShowWindow(SW_SHOW);
	//pDlgTranFile->MoveWindow(0, index*const_TranFile_Height, rect.Width(), const_TranFile_Height);
	//m_pTranFiles.insert(pCrFileInfo->m_nMsgId, pDlgTranFile);
}
#endif

void CPanelFiles::DeleteDlgTranFile(eb::bigint nMsgId)
{
	CDlgTranFile::pointer pDlgTranFile;
	if (m_pTranFiles.find(nMsgId, pDlgTranFile, true))
	{
		m_pDelTranFile.add(pDlgTranFile);
		SetTimer(TIMER_ID_CLOSE_TRAN_FILE,10,NULL);
	}
	//CDlgTranFile::pointer pDlgTranFile;
	//if (m_pTranFiles.find(nMsgId, pDlgTranFile, true))
	//{
	//	pDlgTranFile->DestroyWindow();

	//	CRect rect;
	//	this->GetClientRect(&rect);
	//	MoveSize(rect.Width(), rect.Height());
	//}
}
bool CPanelFiles::IsEmpty(void) const
{
	return m_pTranFiles.empty();
}

#ifdef USES_EBCOM_TEST
void CPanelFiles::SetFilePercent(IEB_ChatFilePercent* pChatRoomFilePercent)
{
	CDlgTranFile::pointer pDlgTranFile;
	if (m_pTranFiles.find(pChatRoomFilePercent->MsgId, pDlgTranFile))
	{
		pDlgTranFile->SetFilePercent(pChatRoomFilePercent);
	}
}
#else
void CPanelFiles::SetFilePercent(const CChatRoomFilePercent * pChatRoomFilePercent)
{
	CDlgTranFile::pointer pDlgTranFile;
	if (m_pTranFiles.find(pChatRoomFilePercent->m_nMsgId, pDlgTranFile))
	{
		pDlgTranFile->SetFilePercent(pChatRoomFilePercent);
	}
}
#endif

void CPanelFiles::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	this->GetParent()->PostMessage(EB_COMMAND_RAME_WND_MAX);
	CEbDialogBase::OnLButtonDblClk(nFlags, point);
}
void CPanelFiles::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	if (this->GetParent()->GetParent()!=NULL && this->GetParent()->GetParent()->GetParent()!=NULL)
		this->GetParent()->GetParent()->GetParent()->PostMessage(WM_NCLBUTTONDOWN, HTCAPTION, MAKELPARAM(point.x, point.y));
	else
		this->GetParent()->PostMessage(WM_NCLBUTTONDOWN, HTCAPTION, MAKELPARAM(point.x, point.y));	
	CEbDialogBase::OnLButtonDown(nFlags, point);
}

void CPanelFiles::OnDestroy()
{
	ExitChat(false);
	CEbDialogBase::OnDestroy();
}

void CPanelFiles::OnPaint()
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
		//CPaintDC dc(this); // 用于绘制的设备上下文
		//CRect rectClient;
		//this->GetClientRect(&rectClient);
		//CSkinMemDC memDC(&dc, rectClient);
		//this->ClearBgDrawInfo();
		//this->AddBgDrawInfo(CEbBackDrawInfo(0,1.0,false,false,0,theDefaultFlatBgColor));
		//this->DrawPopBg(&memDC, theApp.GetMainColor(),0);
	}
}

void CPanelFiles::OnTimer(UINT_PTR nIDEvent)
{
	if (TIMER_ID_OPEN_TRAN_FILE==nIDEvent)
	{
		if (m_pAddTranFile.empty())
		{
			KillTimer(nIDEvent);
		}else
		{
			CRect rect;
			this->GetClientRect(&rect);
			CCrFileInfo pCrFileInfo;
			while (m_pAddTranFile.front(pCrFileInfo) && !m_pTranFiles.exist(pCrFileInfo.m_nMsgId))
			{
				const int index = m_pTranFiles.size();
				CDlgTranFile::pointer pDlgTranFile = CDlgTranFile::create(this);
				pDlgTranFile->m_bIsSendingFile = false;
				//pDlgTranFile->m_pCallInfo = m_pCallInfo;
				//pDlgTranFile->m_sResourceId = pCrFileInfo.m_sResId;
				pDlgTranFile->m_pCrFileInfo = pCrFileInfo;
				pDlgTranFile->Create(CDlgTranFile::IDD, this);
				pDlgTranFile->ShowWindow(SW_SHOW);
				pDlgTranFile->MoveWindow(0, index*const_TranFile_Height, rect.Width(), const_TranFile_Height);
				m_pTranFiles.insert(pCrFileInfo.m_nMsgId, pDlgTranFile);
			}
		}
	}else if (TIMER_ID_CLOSE_TRAN_FILE==nIDEvent)
	{
		KillTimer(nIDEvent);
		CDlgTranFile::pointer pDlgTranFile;
		while (m_pDelTranFile.front(pDlgTranFile))
		{
			pDlgTranFile->DestroyWindow();
		}
		CRect rect;
		this->GetClientRect(&rect);
		MoveSize(rect.Width(), rect.Height());
		this->GetParent()->Invalidate();	// * 刷新
	}

	CEbDialogBase::OnTimer(nIDEvent);
}
BOOL CPanelFiles::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN)
	{
		if (GetKeyState(VK_CONTROL)&0x80)
		{
			if (pMsg->wParam==VK_UP)
			{
				theApp.GetMainWnd()->PostMessage(EB_COMMAND_RAME_WND_MOVE_UP, 0, 0);
				return TRUE;
			}else if (pMsg->wParam==VK_DOWN)
			{
				theApp.GetMainWnd()->PostMessage(EB_COMMAND_RAME_WND_MOVE_DOWN, 0, 0);
				return TRUE;
			}
		}
	}
	return CEbDialogBase::PreTranslateMessage(pMsg);
}

HBRUSH CPanelFiles::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CEbDialogBase::OnCtlColor(pDC, pWnd, nCtlColor);
	pDC->SetBkMode(TRANSPARENT);  
	return hbr;
}
