
// ebstesttoolDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ebstesttool.h"
#include "ebstesttoolDlg.h"

#include <Mmsystem.h>
#pragma comment(lib, "Winmm.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define TIMEID_CLEAR_LC		100
#define TIMEID_TEST_IO_MIN	101
#define TIMEID_TEST_IO_MAX	200


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CebstesttoolDlg dialog




CebstesttoolDlg::CebstesttoolDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CebstesttoolDlg::IDD, pParent)
	, m_sServer(_T("ip:18012"))
	//, m_sServer(_T("test-lc.entboost.com:18012"))
	, m_nCount(1000)
	, m_nThreadCount(10)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_nReturnCount = 0;
	m_nSendCount = 0;
	m_nDuration = 0;
	m_tTestStart = 0;
	m_nMinTime = 0;
	m_nMaxTime = 0;
	m_nNAckCount = 0;
	m_nUseTime = 0;
	m_bKilled = true;
	//for (int i=0;i<DEFAULT_MAX_THREAD_COUNT;i++)
	//	m_pProcessThread[i] = 0;
}

void CebstesttoolDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_SERVER, m_sServer);
	DDX_Text(pDX, IDC_EDIT_COUNT, m_nCount);
	DDV_MinMaxUInt(pDX, m_nCount, 1, 20000);
	DDX_Text(pDX, IDC_EDIT_THREAD_COUNT, m_nThreadCount);
	DDV_MinMaxInt(pDX, m_nThreadCount, 1, 100);
	DDX_Control(pDX, IDC_RICHEDIT21, m_pRichEdit);
}

BEGIN_MESSAGE_MAP(CebstesttoolDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_TEST_VISITOR, &CebstesttoolDlg::OnBnClickedButtonTestVisitor)
	ON_BN_CLICKED(IDC_BUTTON_TEST_IO, &CebstesttoolDlg::OnBnClickedButtonTestIo)
	ON_WM_DESTROY()
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CebstesttoolDlg message handlers

BOOL CebstesttoolDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	m_pRichEdit.LimitText(0);
	SetTimer(TIMEID_CLEAR_LC,1000,NULL);
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CebstesttoolDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CebstesttoolDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CebstesttoolDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CebstesttoolDlg::OnBnClickedButtonTestVisitor()
{
	// TODO: Add your control notification handler code here
	// 
}

//void CebstesttoolDlg::OnSessionOpen(int nResultCode,const tstring& sSessionId,const CPOPCLogonCenter* pLCOwner)
//{
//	BoostWriteLock wtlock(m_pRichMutex);
//	const DWORD tCurrentTime = timeGetTime();
//	const int nUserTime = tCurrentTime-pLCOwner->GetIntParameter();	// 使用时，单位ms
//	//const int nUserTime = tCurrentTime-m_tTestStart;	// 使用时，单位ms
//	if (m_nMinTime==0)
//		m_nMinTime = nUserTime;
//	else if (nUserTime<m_nMinTime)
//		m_nMinTime = nUserTime;
//	if (m_nMaxTime==0)
//		m_nMaxTime = nUserTime;
//	else if (nUserTime>m_nMaxTime)
//		m_nMaxTime = nUserTime;
//	m_nUseTime += nUserTime;
//	m_nReturnCount++;
//	CString sText;
//	sText.Format(_T("%d %dS/%03dms\n"), m_nReturnCount, (nUserTime/1000), (nUserTime%1000));
//	//sText.Format(_T("%d %s %dS/%03dms\n"), m_nReturnCount, sSessionId.c_str(), (nUserTime/1000), (nUserTime%1000));
//	m_pRichEdit.SetSel(-1, -1);
//	m_pRichEdit.ReplaceSel(sText);
//	if (m_nReturnCount==m_nCount)
//	{
//		const int nCountTime = tCurrentTime-m_tTestStart;	// 使用时，单位ms
//		const int nUseTimeAve = m_nUseTime/m_nReturnCount;	// 平均响应时间ms
//		sText.Format(_T("测试结果：\n并发数量：%d\n测试用时：%dS/%dms\n最短响应时间：%dS/%dms\n最长响应时间：%dS/%dms\n平均响应时间：%dS/%dms\n"),
//			m_nCount,(nCountTime/1000),(nCountTime%1000),(m_nMinTime/1000),(m_nMinTime%1000),(m_nMaxTime/1000),(m_nMaxTime%1000),(nUseTimeAve/1000),(nUseTimeAve%1000));
//		m_pRichEdit.SetSel(-1, -1);
//		m_pRichEdit.ReplaceSel(sText);
//	}
//
//	m_pResultList.add((void*)pLCOwner);
//	//CPOPCLogonCenter::pointer pClearLC;
//	//if (m_pList.find((void*)pLCOwner,pClearLC,true))
//	//{
//	//	m_pClearList.add(pClearLC);
//	//}
//}
void CebstesttoolDlg::OnLCULQInfoResponse(const CPOPSotpRequestInfo::pointer & pSotpRequestInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CPOPCLogonCenter* pLCOwner)
{
	const int nIndex = pSotpRequestInfo->m_pRequestList.getParameterValue("index",(int)0);
	const DWORD tReqTime = pSotpRequestInfo->m_pRequestList.getParameterValue("ts",(int)0);

	BoostWriteLock wtlock(m_pRichMutex);
	const DWORD tCurrentTime = timeGetTime();
	const int nUserTime = tCurrentTime-tReqTime;	// 使用时，单位ms
	//const int nUserTime = tCurrentTime-m_tTestStart;	// 使用时，单位ms
	if (m_nMinTime==0)
		m_nMinTime = nUserTime;
	else if (nUserTime<m_nMinTime)
		m_nMinTime = nUserTime;
	if (m_nMaxTime==0)
		m_nMaxTime = nUserTime;
	else if (nUserTime>m_nMaxTime)
	{
		m_nMaxTime = nUserTime;
	}
	if (nUserTime>2000)
		m_nNAckCount++;
	m_nUseTime += nUserTime;
	m_nReturnCount++;
	CString sText;
	sText.Format(_T("%d(%d) %dS/%03dms\n"), m_nReturnCount,nIndex, (nUserTime/1000), (nUserTime%1000));
	m_pRichEdit.SetSel(-1, -1);
	m_pRichEdit.ReplaceSel(sText);
	if (m_nReturnCount==m_nCount)
	{
		const int nCountTime = tCurrentTime-m_tTestStart;	// 使用时间，单位ms
		const int nUseTimeAve = m_nUseTime/m_nReturnCount;	// 平均响应时间ms
		const int nSendAve = (m_nReturnCount*1000)/m_nDuration;	// 每秒发送数量
		const int nRecvAve = (m_nReturnCount*1000)/nCountTime;	// 并发数量
		sText.Format(_T("测试结果：\n发送数量：%d\n发送用时：%dS/%dms 发送并发每秒 %d 个\n响应用时：%dS/%dms 响应并发每秒 %d 个\n丢包补偿：%d 个，丢包率 %.02f%%\n最短响应时间：%dS/%dms\n最长响应时间：%dS/%dms\n平均响应时间：%dS/%dms\n"),
			m_nCount,(m_nDuration/1000),(m_nDuration%1000),nSendAve,(nCountTime/1000),(nCountTime%1000),nRecvAve,m_nNAckCount,((float)(m_nNAckCount*100)/m_nReturnCount),(m_nMinTime/1000),(m_nMinTime%1000),(m_nMaxTime/1000),(m_nMaxTime%1000),(nUseTimeAve/1000),(nUseTimeAve%1000));
		m_pRichEdit.SetSel(-1, -1);
		m_pRichEdit.ReplaceSel(sText);
	}

}

//void CebstesttoolDlg::process_thread_svr(void* lparam)
//{
//	CebstesttoolDlg * pSvr = (CebstesttoolDlg*)lparam;
//	try
//	{
//		pSvr->DoProcess();
//	}catch(std::exception&)
//	{}catch(...)
//	{}
//}
void CebstesttoolDlg::DoProcess(void)
{
	const DWORD tStartTime = timeGetTime();
	int nDuration = 0;
	CPOPCLogonCenter::pointer pLC = CPOPCLogonCenter::create(this);
	pLC->Start(CCgcAddress((LPCTSTR)m_sServer),0,0,10,true);
	while (!m_bKilled)
	{
		int nIndex = 0;
		{
			BoostWriteLock wtlock(m_pSendMutex);
			if (m_nSendCount>m_nCount)
			{
				//if (nDuration==0)
				//{
				//	nDuration = timeGetTime()-tStartTime;
				//	if (m_nDuration<nDuration)
				//		m_nDuration = nDuration;
				//}
				Sleep(10);
				continue;
			}
			nIndex = (++m_nSendCount);
		}
		if (nIndex<=m_nCount)
		{
			if (nIndex==m_nCount && nDuration==0)
			{
				m_nDuration = timeGetTime()-tStartTime;
			}

			CPOPSotpRequestInfo::pointer pSotpRequestInfo = CPOPSotpRequestInfo::create(0);
			pSotpRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("index", nIndex));
			pSotpRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("ts", (int)timeGetTime()));
			pLC->SendLCQueryInfo("1",0,0,pSotpRequestInfo);
		}
		Sleep(20);
	}
	pLC->Stop();
	pLC.reset();
	m_nSendCount = 0;
}

void CebstesttoolDlg::OnBnClickedButtonTestIo()
{
	if (m_tTestStart>0)
	{
		if (MessageBox(_T("是否清空测试数据重新开始？"),_T("并发测试"),MB_YESNO|MB_ICONQUESTION)!=IDYES)
			return;
		m_bKilled = true;
		for (size_t i=0;i<m_pProcessThread.size();i++)
		{
			boost::shared_ptr<boost::thread> pThread = m_pProcessThread[i];
			pThread->join();
		}
		m_pProcessThread.clear();
		//for (int i=0;i<DEFAULT_MAX_THREAD_COUNT;i++)
		//{
		//	if (m_pProcessThread[i] != NULL)
		//	{
		//		m_pProcessThread[i]->join();
		//		delete m_pProcessThread[i];
		//		m_pProcessThread[i] = NULL;
		//	}
		//}
		m_pRichEdit.SetSel(0,-1);	// select all
		m_pRichEdit.ReplaceSel("");
		m_nReturnCount = 0;
		m_nSendCount = 0;
		m_nDuration = 0;
		m_nMinTime = 0;
		m_nMaxTime = 0;
		m_nNAckCount = 0;
		m_nUseTime = 0;
	}

	UpdateData();
	if (m_sServer.IsEmpty() || m_sServer==_T("ip:port"))
	{
		this->GetDlgItem(IDC_EDIT_SERVER)->SetFocus();
		AfxMessageBox(_T("请输入要测试的恩布IM服务器EBS地址！"));
		return;
	}
	m_tTestStart = timeGetTime();
	CString sText;
	sText.Format(_T("并发测试...\n"));
	m_pRichEdit.SetSel(0,-1);
	m_pRichEdit.ReplaceSel(sText);
	m_pRichEdit.SetFocus();
	boost::thread_attributes attrs;
	attrs.set_stack_size(CGC_THREAD_STACK_MIN);
	m_bKilled = false;
	for (int i=0;i<m_nThreadCount;i++)
	{
		m_pProcessThread.push_back(boost::shared_ptr<boost::thread>(new boost::thread(attrs,boost::bind(&CebstesttoolDlg::DoProcess, this))));
		//m_pProcessThread[i] = new boost::thread(attrs,boost::bind(&CebstesttoolDlg::DoProcess, this));
		//m_pProcessThread[i] = new boost::thread(attrs,boost::bind(process_thread_svr, (void*)this));
	}
}

void CebstesttoolDlg::OnDestroy()
{
	m_bKilled = true;
	for (size_t i=0;i<m_pProcessThread.size();i++)
	{
		boost::shared_ptr<boost::thread> pThread = m_pProcessThread[i];
		pThread->join();
	}
	m_pProcessThread.clear();
	//for (int i=0;i<DEFAULT_MAX_THREAD_COUNT;i++)
	//{
	//	if (m_pProcessThread[i] != NULL)
	//	{
	//		m_pProcessThread[i]->join();
	//		delete m_pProcessThread[i];
	//		m_pProcessThread[i] = NULL;
	//	}
	//}
	__super::OnDestroy();

	// TODO: Add your message handler code here
}

void CebstesttoolDlg::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class

	//__super::OnOK();
}

void CebstesttoolDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent>=TIMEID_TEST_IO_MIN && nIDEvent<=TIMEID_TEST_IO_MAX)
	{
		//{
		//	BoostWriteLock wtlock(m_pSendMutex);
		//	m_nSendCount++;
		//	if (m_nSendCount>m_nCount)
		//	{
		//		for (int nTimerID=TIMEID_TEST_IO_MIN;nTimerID<=TIMEID_TEST_IO_MAX;nTimerID++)
		//			KillTimer(nTimerID);
		//	}
		//}
		//if (m_nSendCount<=m_nCount)
		//{
		//	CPOPCLogonCenter::pointer pLC = CPOPCLogonCenter::create(this);
		//	m_pList.insert(pLC.get(), pLC);
		//	pLC->SetIntParameter(timeGetTime());
		//	pLC->Start(CCgcAddress((LPCTSTR)m_sServer),0,0,0,false);
		//}
	}else if (TIMEID_CLEAR_LC==nIDEvent)
	{
		//do
		//{
		//	CPOPCLogonCenter::pointer pClearLC;
		//	m_pClearList.front(pClearLC);
		//}while (m_pClearList.size()>5);
	}

	__super::OnTimer(nIDEvent);
}
