// DlgMsgRecord.cpp : implementation file
//

#include "stdafx.h"
#include "POP.h"
#include "DlgMsgRecord.h"
#include "Core/SkinMemDC.h"

#define LOCATE_MSG_ITEMDATA			10000
#define TIMERID_LOAD_MSG_RECORD		100
#define TIMERID_DELETE_SELECT_MSG	101
#define TIMER_REFRESH_BROWSER		102
#define TIMERID_MOVE_LEAVE			104
// CDlgMsgRecord dialog
#define TIMER_HIDE_STATUS_MESSAGE			0x104
#define TIMER_SHOW_STATUS_MESSAGE			0x105

const int const_one_page_count = 50;	// 一页显示多少条

IMPLEMENT_DYNAMIC(CDlgMsgRecord, CEbDialogBase)

CDlgMsgRecord::CDlgMsgRecord(CWnd* pParent /*=NULL*/)
	: CEbDialogBase(CDlgMsgRecord::IDD, pParent)
	, m_pCefBrowser(this)
{
	m_bChildMode = false;
	m_sAccount = 0;
	m_sGroupCode = 0;
	m_nLastMsgId = 0;
	m_nFromUid = 0;
	m_nToUid = 0;
	//m_tLastMsgTime = 0;
	m_nViewMsgId = 0;
	m_bViewSearchResult = false;
	m_bFindAllMsgId = false;
	m_bDocumentComplete = false;
	//m_bNavigateUrl = false;
	m_bSearchByDate = false;
	m_nRecordPages = 0;
	m_nCurrentPage = -1;

	m_nBrowserType = EB_BROWSER_TYPE_CEF;
	m_pPanelFind = NULL;
	m_pPanelStatus = NULL;
	m_fZoomLevel = 0.0;

	m_hCurrentSelect = NULL;
}

CDlgMsgRecord::~CDlgMsgRecord()
{
}

void CDlgMsgRecord::DoDataExchange(CDataExchange* pDX)
{
	CEbDialogBase::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON_ALL, m_labelViewAll);
	DDX_Control(pDX, IDC_BUTTON_IMAGE, m_labelViewImage);
	DDX_Control(pDX, IDC_BUTTON_FILE, m_labelViewFile);
	DDX_Control(pDX, IDC_BUTTON_AUDIO, m_labelViewAudio);
	DDX_Control(pDX, IDC_STATIC_LINE_CONVERSATION, m_labelLineConversation);
	DDX_Control(pDX, IDC_BUTTON_MIN, m_btnMin);
	DDX_Control(pDX, IDC_BUTTON_MAX, m_btnMax);
	DDX_Control(pDX, IDC_BUTTON_CLOSE, m_btnClose);
	DDX_Control(pDX, IDC_EDIT_SEARCH, m_editSearch);
	DDX_Control(pDX, IDC_BUTTON_DELETEALL, m_btnDeleteAll);
	DDX_Control(pDX, IDC_EXPLORER1, m_pExplorer);
	DDX_Control(pDX, IDC_BUTTON_MOVE_FIRST, m_btnMoveFirst);
	DDX_Control(pDX, IDC_BUTTON_MOVE_PREV, m_btnMovePrev);
	DDX_Control(pDX, IDC_BUTTON_MOVE_NEXT, m_btnMoveNext);
	DDX_Control(pDX, IDC_BUTTON_MOVE_LAST, m_btnMoveLast);
	DDX_Control(pDX, IDC_DATETIMEPICKER1, m_tDataTime);
}


BEGIN_MESSAGE_MAP(CDlgMsgRecord, CEbDialogBase)
	//ON_WM_CREATE()
	ON_WM_MOUSEWHEEL()
	ON_WM_TIMER()
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BUTTON_MIN, &CDlgMsgRecord::OnBnClickedButtonMin)
	ON_BN_CLICKED(IDC_BUTTON_MAX, &CDlgMsgRecord::OnBnClickedButtonMax)
	ON_BN_CLICKED(IDC_BUTTON_CLOSE, &CDlgMsgRecord::OnBnClickedButtonClose)
	ON_WM_PAINT()
	ON_WM_CLOSE()
	//ON_WM_ERASEBKGND()
	ON_BN_CLICKED(IDC_BUTTON_DELETEALL, &CDlgMsgRecord::OnBnClickedButtonDeleteall)
	ON_EN_CHANGE(IDC_EDIT_SEARCH, &CDlgMsgRecord::OnEnChangeEditSearch)
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONDOWN()
	ON_STN_CLICKED(IDC_STATIC_LINE_CONVERSATION, &CDlgMsgRecord::OnStnClickedStaticLineConversation)
	ON_BN_CLICKED(IDC_BUTTON_MOVE_FIRST, &CDlgMsgRecord::OnBnClickedButtonMoveFirst)
	ON_BN_CLICKED(IDC_BUTTON_MOVE_PREV, &CDlgMsgRecord::OnBnClickedButtonMovePrev)
	ON_BN_CLICKED(IDC_BUTTON_MOVE_NEXT, &CDlgMsgRecord::OnBnClickedButtonMoveNext)
	ON_BN_CLICKED(IDC_BUTTON_MOVE_LAST, &CDlgMsgRecord::OnBnClickedButtonMoveLast)
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_DATETIMEPICKER1, &CDlgMsgRecord::OnDtnDatetimechangeDatetimepicker1)
	ON_BN_CLICKED(IDC_BUTTON_ALL, &CDlgMsgRecord::OnBnClickedButtonViewAll)
	ON_BN_CLICKED(IDC_BUTTON_IMAGE, &CDlgMsgRecord::OnBnClickedButtonViewImage)
	ON_BN_CLICKED(IDC_BUTTON_FILE, &CDlgMsgRecord::OnBnClickedButtonViewFile)
	ON_BN_CLICKED(IDC_BUTTON_AUDIO, &CDlgMsgRecord::OnBnClickedButtonViewAudio)
	ON_MESSAGE(EB_COMMAND_OPEN_APP_URL, OnMsgOpenAppUrl)
END_MESSAGE_MAP()

BEGIN_EVENTSINK_MAP(CDlgMsgRecord, CEbDialogBase)
	//{{AFX_EVENTSINK_MAP(CDlgMsgRecord)
	ON_EVENT(CDlgMsgRecord,IDC_EXPLORER1,259,DocumentComplete,VTS_DISPATCH VTS_PVARIANT)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

// CDlgMsgRecord message handlers
void CDlgMsgRecord::SetCtrlColor(bool bInvalidate)
{
	if (m_pPanelFind!=NULL)
		m_pPanelFind->SetCtrlColor(bInvalidate);

	m_labelViewAll.SetDrawPanel(true,theDefaultFlatBgColor,theApp.GetNormalColor(),theApp.GetNormalColor());
	m_labelViewImage.SetDrawPanel(true,theDefaultFlatBgColor,theApp.GetNormalColor(),theApp.GetNormalColor());
	m_labelViewFile.SetDrawPanel(true,theDefaultFlatBgColor,theApp.GetNormalColor(),theApp.GetNormalColor());
	m_labelViewAudio.SetDrawPanel(true,theDefaultFlatBgColor,theApp.GetNormalColor(),theApp.GetNormalColor());
	
	m_editSearch.SetRectangleColor(theApp.GetHotColor(),theDefaultFlatLineColor);
	m_labelLineConversation.SetCheckBoxHoverBorder(TRUE,theApp.GetHotColor());
	m_labelLineConversation.SetTextColor(theDefaultFlatBlackTextColor,theApp.GetHotColor());

	m_btnDeleteAll.SetDrawPanel(true,theDefaultFlatBgColor,theApp.GetNormalColor(),theApp.GetPreColor());
	m_btnMoveFirst.SetDrawPanel(true,theDefaultFlatBgColor);
	m_btnMovePrev.SetDrawPanel(true,theDefaultFlatBgColor);
	m_btnMoveNext.SetDrawPanel(true,theDefaultFlatBgColor);
	m_btnMoveLast.SetDrawPanel(true,theDefaultFlatBgColor);

	const int const_arrow_width = 5;
	const COLORREF const_arrow_nor_color = RGB(64,64,64);
	const COLORREF const_arrow_hot_color = theApp.GetHotColor();	//RGB(250,250,250);
	const COLORREF const_arrow_dis_color = RGB(64,64,64);
	m_btnMoveFirst.SetDrawArrowPic(3,true,1,const_arrow_nor_color,const_arrow_hot_color,-1,const_arrow_dis_color,const_arrow_width);
	m_btnMovePrev.SetDrawArrowPic(3,false,1,const_arrow_nor_color,const_arrow_hot_color,-1,const_arrow_dis_color,const_arrow_width);
	m_btnMoveNext.SetDrawArrowPic(4,false,1,const_arrow_nor_color,const_arrow_hot_color,-1,const_arrow_dis_color,const_arrow_width);
	m_btnMoveLast.SetDrawArrowPic(4,true,1,const_arrow_nor_color,const_arrow_hot_color,-1,const_arrow_dis_color,const_arrow_width);

	m_btnMin.SetDrawPanel(true,theApp.GetMainColor(),theApp.GetHotColor(),theApp.GetPreColor());
	m_btnMax.SetDrawPanel(true,theApp.GetMainColor(),theApp.GetHotColor(),theApp.GetPreColor());
	m_btnClose.SetDrawPanel(true,theApp.GetMainColor(),theDefaultBtnCloseColor,theDefaultBtnCloseColor);

	if (m_pDlgToolbar.GetSafeHwnd()!=NULL)
		m_pDlgToolbar.SetCtrlColor(bInvalidate);

	if (bInvalidate)
		this->Invalidate();
}
//int CDlgMsgRecord::OnCreate(LPCREATESTRUCT lpCreateStruct) 
//{
//	if (m_bChildMode)
//	{
//		lpCreateStruct->dwExStyle = 0;
//		lpCreateStruct->style = (WS_CHILD|WS_VISIBLE);
//	}
// 	if (CEbDialogBase::OnCreate(lpCreateStruct) == -1)
// 		return -1;
// 	return 0;
//}
//BOOL CDlgMsgRecord::PreCreateWindow(CREATESTRUCT& cs)
//{
//	// TODO: Add your specialized code here and/or call the base class
//	if (m_bChildMode)
//	{
//		cs.style &= ~WS_POPUP;
//		cs.style |= WS_CHILD;
//	}
//	return CEbDialogBase::PreCreateWindow(cs);
//}
//BOOL CDlgMsgRecord::Create(LPCTSTR lpszClassName,
//		LPCTSTR lpszWindowName, DWORD dwStyle,
//		const RECT& rect,
//		CWnd* pParentWnd, UINT nID,
//		CCreateContext* pContext)
//{
//	return CEbDialogBase::Create(lpszClassName,);
//}

BOOL CDlgMsgRecord::OnInitDialog()
{
	CEbDialogBase::OnInitDialog();
	if (m_bChildMode)
	{
		ModifyStyle(0, WS_CLIPSIBLINGS|WS_CLIPCHILDREN);
		ModifyStyle(WS_POPUP, WS_CHILD);
		this->SetMouseMove(FALSE);
	}else
	{
		//ModifyStyle(0, WS_CLIPSIBLINGS|WS_CLIPCHILDREN);
		ModifyStyle(0, WS_MINIMIZEBOX|WS_MAXIMIZEBOX|WS_SYSMENU);
		this->SetSplitterBorder();
	}
	//this->SetTransparentType(CEbDialogBase::TT_STATIC);

	this->EnableToolTips();
	this->SetToolTipText(IDC_EDIT_SEARCH,_T("搜索本地聊天记录"));
	this->SetToolTipText(IDC_BUTTON_DELETEALL,_T("清空本地聊天记录"));

	m_pPanelFind = new CPanelFind(this);
	m_pPanelFind->Create(CPanelFind::IDD,this);
	m_pPanelFind->SetFindHandler(this);
	m_pPanelStatus = new CPanelText(this);
	m_pPanelStatus->Create(CPanelText::IDD,this);
	m_pPanelStatus->ShowWindow(SW_HIDE);
	m_pPanelStatus->SetBgColor(theDefaultFlatLineColor);

	m_nBrowserType = EB_BROWSER_TYPE_CEF;	// add by hd 2016-09-06
	//m_nBrowserType = EB_BROWSER_TYPE_IE;
	//m_nBrowserType = theApp.GetDefaultBrowserType();

	m_labelViewAll.SetTextTop(-1);
	m_labelViewAll.SetAutoSize(false);
	m_labelViewAll.SetTextHotMove(false);
	m_labelViewAll.SetWindowText(_T("全部"));
	m_labelViewAll.SetChecked(true);
	m_labelViewAll.SetPreTextColor(theDefaultBtnWhiteColor);
	m_labelViewImage.SetTextTop(-1);
	m_labelViewImage.SetAutoSize(false);
	m_labelViewImage.SetTextHotMove(false);
	m_labelViewImage.SetWindowText(_T("图片"));
	m_labelViewImage.SetPreTextColor(theDefaultBtnWhiteColor);
	m_labelViewFile.SetTextTop(-1);
	m_labelViewFile.SetAutoSize(false);
	m_labelViewFile.SetTextHotMove(false);
	m_labelViewFile.SetWindowText(_T("文件"));
	m_labelViewFile.SetPreTextColor(theDefaultBtnWhiteColor);
	m_labelViewAudio.SetTextTop(-1);
	m_labelViewAudio.SetAutoSize(false);
	m_labelViewAudio.SetTextHotMove(false);
	m_labelViewAudio.SetWindowText(_T("语音"));
	m_labelViewAudio.SetPreTextColor(theDefaultBtnWhiteColor);

	m_labelLineConversation.SetTextTop(-1);
	m_labelLineConversation.LoadLabel(IDB_PNG_CHECKBOX,-1,0,2);
	m_labelLineConversation.SetToolTipText(_T("查看漫游消息（在线聊天记录）"));
	m_btnMin.SetAutoSize(false);
	m_btnMin.SetDrawToolButtonPic(1,theDefaultBtnWhiteColor,-1,-1,-1,2);
	m_btnMin.SetDrawPanelRgn(false);
	m_btnMin.SetToolTipText(_T("最小化"));
	m_btnMax.SetAutoSize(false);
	m_btnMax.SetDrawToolButtonPic(2,theDefaultBtnWhiteColor,-1,-1,-1,2);
	m_btnMax.SetDrawPanelRgn(false);
	m_btnMax.SetToolTipText(_T("最大化"));
	m_btnClose.SetAutoSize(false);
	m_btnClose.SetDrawToolButtonPic(4,theDefaultBtnWhiteColor,-1,-1,-1,2);
	m_btnClose.SetDrawPanelRgn(false);
	m_btnClose.SetToolTipText(_T("关闭"));
	if (theApp.GetSaveConversationServer())
	{
#ifdef USES_LIBCEF
		if (m_nBrowserType==EB_BROWSER_TYPE_CEF)
		{
			CRect rect(0,0,0,0);
			m_pCefBrowser.Create("about:blank",rect,this->GetSafeHwnd(),0);
			m_pExplorer.MoveWindow(0,0,0,0);
		}else
		{
			CRect rect(0,0,0,0);
			m_pCefBrowser.Create("about:blank",rect,this->GetSafeHwnd(),0);
		}
#endif

		m_pExplorer.ShowWindow(SW_SHOW);
		m_pExplorer.MoveWindow(0,0,0,0);
		m_pExplorer.ModifyStyle(0, WS_CLIPSIBLINGS|WS_CLIPCHILDREN);
		// 先显示空白页，用于刷新修改IE控件。
		m_webUICtrl.EnableContextMenu(TRUE);
		//m_webUICtrl.EnableContextMenu(FALSE);
		m_webUICtrl.EnableScrollBar(TRUE);
		m_webUICtrl.SetWebBrowser(&m_pExplorer);
		m_pExplorer.Navigate(_T("About:Blank"),NULL,NULL,NULL,NULL);
	}else
	{
		m_pExplorer.ShowWindow(SW_HIDE);
		m_labelLineConversation.ShowWindow(SW_HIDE);
	}

	if (!m_pMrFrameControl.CreateControl(__uuidof(EBRichMessage),NULL,WS_CHILD|WS_VISIBLE,CRect(0,0,10,10),this,1111))
		return FALSE;
	LPUNKNOWN pUnknown = m_pMrFrameControl.GetControlUnknown();
	if (pUnknown == NULL)
		return FALSE;
	pUnknown->QueryInterface(__uuidof(IEBRichMessage),(void**)&m_pMrFrameInterface);
	if (m_pMrFrameInterface==NULL)
		return FALSE;
	m_pMrFrameInterface->SetSelectCbData1(EB_MR_CTRL_DATA_TYPE_SELECT_STRING);
	m_pMrFrameInterface->SetLineInterval(3);
	m_pMrFrameInterface->SetBackgroundColor(theDefaultFlatBgColor);
	((CEBRichMessageEventsSink*)this)->DispEventAdvise(m_pMrFrameInterface);
	// 定位消息
	m_pMrFrameInterface->AddContextMenuItem("显示前后消息",LOCATE_MSG_ITEMDATA,VARIANT_TRUE);
	// 右键菜单应用功能
	if (theApp.BuildChatMenuSubscribeFuncList()>0)
	{
		for (size_t i=0;i<theApp.m_pChatMenuSubscribeFuncList.size();i++)
		{
			const EB_SubscribeFuncInfo & pSubscribeFuncInfo = theApp.m_pChatMenuSubscribeFuncList[i];
			m_pMrFrameInterface->AddContextMenuItem(pSubscribeFuncInfo.m_sFunctionName.c_str(),i,VARIANT_FALSE);
		}
	}

	m_btnDeleteAll.SetTextTop(-1);
	m_btnDeleteAll.SetTextHotMove(false);
	m_editSearch.SetPromptText(_T("搜索本地聊天记录"));

	m_tDataTime.ModifyStyle(WS_BORDER,0);
	m_btnMoveFirst.SetDrawPanelRgn(true);
	m_btnMoveFirst.SetWindowText(_T(""));
	m_btnMoveFirst.SetToolTipText(_T("第一页"));
	m_btnMovePrev.SetDrawPanelRgn(true);
	m_btnMovePrev.SetWindowText(_T(""));
	m_btnMovePrev.SetToolTipText(_T("上一页"));
	m_btnMoveNext.SetDrawPanelRgn(true);
	m_btnMoveNext.SetWindowText(_T(""));
	m_btnMoveNext.SetToolTipText(_T("下一页"));
	m_btnMoveLast.SetDrawPanelRgn(true);
	m_btnMoveLast.SetWindowText(_T(""));
	m_btnMoveLast.SetToolTipText(_T("最后页"));

	SetCtrlColor(false);

	if (m_bChildMode)
	{
		m_btnMin.ShowWindow(SW_HIDE);
		m_btnMax.ShowWindow(SW_HIDE);
		m_btnClose.ShowWindow(SW_HIDE);
	}else
	{
		this->ShowWindow(SW_SHOW);
	}
	this->GetDlgItem(IDC_EDIT_SEARCH)->MoveWindow(10,52,196,22);
	//m_pMrFrame.Create(CRect(0,0,0,0),this);
	//m_pMrFrame.SetScrollBarBmp(IDB_VERTICAL_SCROLLBAR_UPARROW, IDB_VERTICAL_SCROLLBAR_SPAN, IDB_VERTICAL_SCROLLBAR_DOWNARROW,
	//	IDB_VERTICAL_SCROLLBAR_THUMB, IDB_VERTICAL_SCROLLBAR_TOP, IDB_VERTICAL_SCROLLBAR_BOTTOM );
	//m_pMrFrame.SetLineAlignment(3);

	SetDlgChildFont(theDefaultDialogFontSize,theFontFace.c_str());
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgMsgRecord::OnTimer(UINT_PTR nIDEvent)
{
	switch (nIDEvent)
	{
	case TIMERID_MOVE_LEAVE:
		{
			if (m_pDlgToolbar.GetSafeHwnd()!=NULL)
			{
				CPoint pos;
				::GetCursorPos(&pos);
				CRect rect;
				m_pDlgToolbar.GetWindowRect(&rect);
				if (!rect.PtInRect(pos))
				{
					KillTimer(nIDEvent);
					m_pDlgToolbar.HideReset();
				}
			}
		}break;
	case TIMER_REFRESH_BROWSER:
		{
			KillTimer(TIMER_REFRESH_BROWSER);
			if (m_labelLineConversation.GetCheck())
			{
				if (m_nBrowserType==EB_BROWSER_TYPE_CEF && m_pCefBrowser.GetSafeHwnd())
				{
					HWND hWnd = m_pCefBrowser.GetSafeHwnd();
					::ShowWindow(hWnd,SW_HIDE);
					::ShowWindow(hWnd,SW_SHOW);
				}else if (m_pExplorer.GetSafeHwnd())
				{
					const long nReadyState = m_pExplorer.get_ReadyState();
					if (nReadyState == READYSTATE_COMPLETE)
					{
						m_pExplorer.Refresh();
					}
				}
			}
		}break;
	case TIMERID_LOAD_MSG_RECORD:
		{
			KillTimer(nIDEvent);
			static bool theSetWindowText = false;
			if (!theSetWindowText)
			{
				theSetWindowText = true;
				CString sWindowText;
				sWindowText.Format(_T("%s聊天记录"),m_sName.c_str());
				this->SetWindowText(sWindowText);
			}

			CString sMsgTypeSQLString;
			if (m_labelViewImage.GetChecked())
			{
				sMsgTypeSQLString.Format(_T(" AND msg_type=%d"),(int)MRT_JPG);
			}else if (m_labelViewFile.GetChecked())
			{
				sMsgTypeSQLString.Format(_T(" AND msg_type=%d"),(int)MRT_FILE);
			}else if (m_labelViewAudio.GetChecked())
			{
				sMsgTypeSQLString.Format(_T(" AND msg_type=%d"),(int)MRT_WAV);
			}

			CString sSearchSQLString;
			if (m_bViewSearchResult)
			{
				CString sSearchString;
				m_editSearch.GetWindowText(sSearchString);
				if (m_nViewMsgId==0 && !sSearchString.IsEmpty())
				{
					tstring sSearchStringTemp(sSearchString);
					CSqliteCdbc::escape_string_in(sSearchStringTemp);
					sSearchSQLString.Format(_T(" AND msg_text LIKE '%%%s%%'"),libEbc::ACP2UTF8(sSearchStringTemp.c_str()).c_str());
					//sSearchSQLString.Format(_T(" AND msg_text LIKE '%%%s%%'"),libEbc::ACP2UTF8(sSearchString).c_str());
				}
			}
			CString sDateSQLString;
			if (m_bSearchByDate)
			{
				CTime pTime;
				m_tDataTime.GetTime(pTime);
				const int nYear = pTime.GetYear();
				const int nMonth = pTime.GetMonth();
				const int nDay = pTime.GetDay();
				sDateSQLString.Format(_T(" AND date(msg_time)>=date('%04d-%02d-%02d')"),nYear,nMonth,nDay);
			}

			CString sSql;
			if (m_sAccount>0)
			{
				sSql.Format(_T("select count(msg_id) FROM msg_record_t WHERE dep_code=0 AND (from_uid=%lld OR to_uid=%lld) %s%s ORDER BY msg_time"),m_sAccount,m_sAccount,sMsgTypeSQLString,sSearchSQLString);
			}else if (m_sGroupCode>0)
			{
				sSql.Format(_T("select count(msg_id) FROM msg_record_t WHERE dep_code=%lld %s%s ORDER BY msg_time"),m_sGroupCode,sMsgTypeSQLString,sSearchSQLString);
			}else
				return;
			int nRecordCount = 0;
			int nCookie = 0;
			theApp.m_pBoUsers->select(sSql, nCookie);
			cgcValueInfo::pointer pRecord = theApp.m_pBoUsers->first(nCookie);
			if (pRecord.get()!=NULL)
			{
				nRecordCount = pRecord->getVector()[0]->getIntValue();
			}
			theApp.m_pBoUsers->reset(nCookie);
			if (nRecordCount<=0)
			{
				m_pMrFrameInterface->Clear();
				m_nLastMsgId = 0;
				m_nFromUid = 0;
				m_nToUid = 0;
				m_nRecordPages = 0;
				m_nCurrentPage = -1;
				m_tDataTime.EnableWindow(FALSE);
				m_btnMoveFirst.EnableWindow(FALSE);
				m_btnMovePrev.EnableWindow(FALSE);
				m_btnMoveNext.EnableWindow(FALSE);
				m_btnMoveLast.EnableWindow(FALSE);

				//WriteTitle(0,false,-1,"阿猫",0,"",time(0));
				//m_pMrFrameInterface->WriteString(L"好像真的木有内容耶^_^",theDefaultChatTextColor2);
				//WriteTitle(0,false,theApp.GetLogonUserId(),"阿狗",0,"",time(0));
				//m_pMrFrameInterface->WriteString(L"我知道了 我正在找呢......",theDefaultChatTextColor2);
				//m_pMrFrameInterface->UpdateSize(VARIANT_TRUE);
				//m_pMrFrameInterface->ScrollToPos(-1);
			}else
			{
				m_nRecordPages = (nRecordCount+const_one_page_count-1)/const_one_page_count;	// 总共多少页
				if (m_nCurrentPage>m_nRecordPages)
					m_nCurrentPage = m_nRecordPages-1;
				int nOffsetPage = m_nCurrentPage==-1?(m_nRecordPages-1):m_nCurrentPage;
				int nOffset = nOffsetPage*const_one_page_count;
				if (m_bViewSearchResult)
				{
					m_nRecordPages = 1;
					nOffsetPage = 0;
					nOffset = 0;
				}
				if (m_nViewMsgId>0 && !m_bViewSearchResult)
				{
					if (!m_bFindAllMsgId)
					{
						m_bFindAllMsgId = true;
						if (m_sAccount>0)
						{
							sSql.Format(_T("select msg_id FROM msg_record_t WHERE dep_code=0 AND (from_uid=%lld OR to_uid=%lld) ORDER BY msg_time"),m_sAccount,m_sAccount);
						}else if (m_sGroupCode>0)
						{
							sSql.Format(_T("select msg_id FROM msg_record_t WHERE dep_code=%lld ORDER BY msg_time"),m_sGroupCode);
						}else
							return;
						nCookie = 0;
						theApp.m_pBoUsers->select(sSql, nCookie);
						pRecord = theApp.m_pBoUsers->first(nCookie);
						int nIndex = 0;
						while (pRecord.get()!=NULL)
						{
							const eb::bigint sMsgId = pRecord->getVector()[0]->getBigIntValue();
							m_pMsgIdOffsetList.insert(sMsgId,(nIndex++));
							pRecord = theApp.m_pBoUsers->next(nCookie);
						}
						theApp.m_pBoUsers->reset(nCookie);
					}
					int nMsgIdRecordIndex = 0;
					m_pMsgIdOffsetList.find(m_nViewMsgId,nMsgIdRecordIndex);
					nOffsetPage = nMsgIdRecordIndex/const_one_page_count;
					m_nCurrentPage = nOffsetPage;
					nOffset = m_nCurrentPage*const_one_page_count;
				}else if (m_bSearchByDate)
				{
					if (m_sAccount>0)
					{
						sSql.Format(_T("select count(msg_id) FROM msg_record_t WHERE dep_code=0 AND (from_uid=%lld OR to_uid=%lld) %s%s%s"),
							m_sAccount,m_sAccount,sMsgTypeSQLString,sDateSQLString,sSearchSQLString);
					}else if (m_sGroupCode>0)
					{
						sSql.Format(_T("select count(msg_id) FROM msg_record_t WHERE dep_code=%lld %s%s%s"),
							m_sGroupCode,sMsgTypeSQLString,sDateSQLString,sSearchSQLString);
					}else
						return;
					int nTimeRecordCount = 0;
					nCookie = 0;
					theApp.m_pBoUsers->select(sSql, nCookie);
					pRecord = theApp.m_pBoUsers->first(nCookie);
					if (pRecord.get()!=NULL)
					{
						nTimeRecordCount = pRecord->getVector()[0]->getIntValue();
					}
					theApp.m_pBoUsers->reset(nCookie);
					nOffset = max(0,(nRecordCount-nTimeRecordCount));
					nOffsetPage = nOffset/const_one_page_count;
					m_nCurrentPage = nOffsetPage;
				}

				m_tDataTime.EnableWindow(TRUE);
				if (m_nRecordPages==1 || m_bViewSearchResult)
				{
					m_btnMoveFirst.EnableWindow(FALSE);
					m_btnMovePrev.EnableWindow(FALSE);
					m_btnMoveNext.EnableWindow(FALSE);
					m_btnMoveLast.EnableWindow(FALSE);
				}else
				{
					CString sToolTipText;
					//bool bPrevDisable = m_btnMoveFirst.IsWindowEnabled()?false:true;
					m_btnMoveFirst.EnableWindow(nOffsetPage>0);
					//if (bPrevDisable)
					//{
					//	m_btnMoveFirst.ShowWindow(SW_HIDE);
					//	m_btnMoveFirst.ShowWindow(SW_SHOW);
					//	m_btnMoveFirst.RedrawWindow();
					//}
					sToolTipText.Format(_T("上一页[%d/%d]"),nOffsetPage,m_nRecordPages);
					m_btnMovePrev.SetToolTipText(sToolTipText);
					m_btnMovePrev.EnableWindow(nOffsetPage>0);
					m_btnMovePrev.Invalidate();
					m_btnMoveNext.EnableWindow((nOffsetPage+1)<m_nRecordPages);
					sToolTipText.Format(_T("下一页[%d/%d]"),nOffsetPage+2,m_nRecordPages);
					m_btnMoveNext.SetToolTipText(sToolTipText);
					m_btnMoveLast.EnableWindow((nOffsetPage+1)<m_nRecordPages);
				}

				if (m_sAccount>0)
				{
					if (m_bViewSearchResult)
					{
						sSql.Format(_T("select Datetime(msg_time,'localtime'),msg_id,off_time,from_uid,from_name,to_uid,to_name,private,msg_type,msg_name,msg_text,read_flag FROM msg_record_t ")\
							_T("WHERE dep_code=0 AND (from_uid=%lld OR to_uid=%lld) %s%s ORDER BY msg_time"),m_sAccount,m_sAccount,sMsgTypeSQLString,sSearchSQLString);
					}else
					{
						sSql.Format(_T("select Datetime(msg_time,'localtime'),msg_id,off_time,from_uid,from_name,to_uid,to_name,private,msg_type,msg_name,msg_text,read_flag FROM msg_record_t ")\
							_T("WHERE dep_code=0 AND (from_uid=%lld OR to_uid=%lld) %s ORDER BY msg_time LIMIT %d OFFSET %d"),
							m_sAccount,m_sAccount,sMsgTypeSQLString,const_one_page_count,nOffset);
					}
					LoadMsgRecord(sSql);
				}else if (m_sGroupCode>0)
				{
					if (m_bViewSearchResult)
					{
						sSql.Format(_T("select Datetime(msg_time,'localtime'),msg_id,off_time,from_uid,from_name,to_uid,to_name,private,msg_type,msg_name,msg_text,read_flag FROM msg_record_t ")\
							_T("WHERE dep_code=%lld %s%s ORDER BY msg_time"),m_sGroupCode,sMsgTypeSQLString,sSearchSQLString);
					}else
					{
						sSql.Format(_T("select Datetime(msg_time,'localtime'),msg_id,off_time,from_uid,from_name,to_uid,to_name,private,msg_type,msg_name,msg_text,read_flag FROM msg_record_t ")\
							_T("WHERE dep_code=%lld %s ORDER BY msg_time LIMIT %d OFFSET %d"),m_sGroupCode,sMsgTypeSQLString,const_one_page_count,nOffset);
					}
					LoadMsgRecord(sSql);
				}
			}
		}break;
	case TIMERID_DELETE_SELECT_MSG:
		{
			KillTimer(nIDEvent);
			if (!m_pDeletMsgId.empty())
			{
				CString sText;
				sText.Format(_T("确定删除选中的：%d 条聊天记录吗？"), m_pDeletMsgId.size());
				if (CDlgMessageBox::EbDoModal(this,"删除聊天记录",sText,CDlgMessageBox::IMAGE_QUESTION)==IDOK)
				{
					for (size_t i=0;i<m_pDeletMsgId.size();i++)
					{
						const eb::bigint nMsgId = m_pDeletMsgId[i];
						m_pMrFrameInterface->DeleteLine(nMsgId);
						theApp.DeleteDbRecord(nMsgId);
					}
					m_pMrFrameInterface->UpdateSize(VARIANT_TRUE);
					m_pMrFrameInterface->ScrollToPos(-1);
				}
				m_pDeletMsgId.clear();
			}
		}break;
	case TIMER_HIDE_STATUS_MESSAGE:
		{
			KillTimer(nIDEvent);
			if (m_pPanelStatus->IsWindowVisible())
				m_pPanelStatus->ShowWindow(SW_HIDE);
		}break;
	case TIMER_SHOW_STATUS_MESSAGE:
		{
			KillTimer(nIDEvent);
			if (m_sStatusMessage.empty())
			{
				if (m_pPanelStatus->IsWindowVisible())
					m_pPanelStatus->ShowWindow(SW_HIDE);
			}else
			{
				CRect rect;
				this->GetClientRect(&rect);
				const int nStringWidth = m_pPanelStatus->MeasureTextWidth(m_sStatusMessage.c_str(),rect.Width());
				const int const_status_height = 22;
				CRect rectStatus(0,rect.Height()-const_status_height,nStringWidth+2,rect.Height());
				CPoint pos;
				::GetCursorPos(&pos);
				this->ScreenToClient(&pos);
				if (rectStatus.PtInRect(pos))
				{
					rectStatus.bottom = pos.y-12;
					rectStatus.top = rectStatus.bottom-const_status_height;
				}
				m_pPanelStatus->MoveWindow(&rectStatus);
				m_pPanelStatus->SetDrawText(m_sStatusMessage.c_str());
			}
		}break;
	default:
		break;
	}

	CEbDialogBase::OnTimer(nIDEvent);
}

void CDlgMsgRecord::OnSize(UINT nType, int cx, int cy)
{
	CEbDialogBase::OnSize(nType, cx, cy);
	const int const_minbtn_width = 24;
	const int const_minbtn_height = 22;
	int btnx = 0;
	btnx = cx-const_minbtn_width-2;
	if (m_btnClose.GetSafeHwnd()!=NULL)
		m_btnClose.MoveWindow(btnx, 1,const_minbtn_width,const_minbtn_height);
	btnx -= const_minbtn_width;
	if (m_btnMax.GetSafeHwnd()!=NULL)
		m_btnMax.MoveWindow(btnx, 1,const_minbtn_width,const_minbtn_height);
	btnx -= const_minbtn_width;
	if (m_btnMin.GetSafeHwnd()!=NULL)
		m_btnMin.MoveWindow(btnx, 1,const_minbtn_width,const_minbtn_height);

	int y = m_bChildMode?10:52;
	int x = 10;
	const int const_viewbtn_width = 35;
	const int const_viewbtn_height = 20;
	m_labelViewAll.MovePoint(x, y+1,const_viewbtn_width,const_viewbtn_height);
	x += (const_viewbtn_width+2);
	m_labelViewImage.MovePoint(x, y+1,const_viewbtn_width,const_viewbtn_height);
	x += (const_viewbtn_width+2);
	m_labelViewFile.MovePoint(x, y+1,const_viewbtn_width,const_viewbtn_height);
	x += (const_viewbtn_width+2);
	m_labelViewAudio.MovePoint(x, y+1,const_viewbtn_width,const_viewbtn_height);
	x += (const_viewbtn_width+5);
	if (m_editSearch.GetSafeHwnd())
	{
		m_editSearch.MoveWindow(x,y,120,22);	// 196
	}
	const int const_btn_width = 60;//m_btnDeleteAll.GetImgWidth();
	m_btnDeleteAll.MovePoint(cx-const_btn_width-10,y+1,const_btn_width,const_viewbtn_height);
	
	const int const_interval = 1;
	if (m_labelLineConversation.GetCheck())
	{
		const int nExpolorerTop = m_bChildMode?0:theDefaultBgColorTitleHeight;
		if (m_nBrowserType==EB_BROWSER_TYPE_CEF)
		{
			m_pCefBrowser.MoveWindow(const_interval,nExpolorerTop,cx-const_interval*2,cy-nExpolorerTop-28);
			SetTimer(TIMER_REFRESH_BROWSER,10,NULL);
		}else
			m_pExplorer.MoveWindow(const_interval,nExpolorerTop,cx-const_interval*2,cy-nExpolorerTop-28);
	}
	y = m_bChildMode?42:84;
	if (m_pMrFrameControl.GetSafeHwnd())
	{
		m_pMrFrameControl.MoveWindow(const_interval,y,cx-const_interval*2,cy-y-28);
		if (m_pMrFrameInterface != NULL)
			m_pMrFrameInterface->ScrollToPos(-1);
	}
	if (m_labelLineConversation.GetSafeHwnd())
	{
		m_labelLineConversation.MoveWindow(5,cy-22,110,14);
	}

	y = cy-24;
	const int const_arraw_btn_size = 18;
	x = cx-const_arraw_btn_size-4;
	if (m_btnMoveLast.GetSafeHwnd()!=NULL)
		m_btnMoveLast.MoveWindow(x,y,const_arraw_btn_size,const_arraw_btn_size);
	x -= const_arraw_btn_size;
	if (m_btnMoveNext.GetSafeHwnd()!=NULL)
		m_btnMoveNext.MoveWindow(x,y,const_arraw_btn_size,const_arraw_btn_size);
	x -= (const_arraw_btn_size+10);	// ** 10
	if (m_btnMovePrev.GetSafeHwnd()!=NULL)
		m_btnMovePrev.MoveWindow(x,y,const_arraw_btn_size,const_arraw_btn_size);
	x -= const_arraw_btn_size;
	if (m_btnMoveFirst.GetSafeHwnd()!=NULL)
		m_btnMoveFirst.MoveWindow(x,y,const_arraw_btn_size,const_arraw_btn_size);
	x -= 100;
	if (m_tDataTime.GetSafeHwnd()!=NULL)
		m_tDataTime.MoveWindow(x,y-2,95,22);

	MovePanelFind(cx);
	if (m_pPanelStatus!=NULL && m_pPanelStatus->IsWindowVisible())
	{
		SetTimer(TIMER_SHOW_STATUS_MESSAGE,100,NULL);
	}
}

void CDlgMsgRecord::OnDestroy()
{
	((CEBRichMessageEventsSink*)this)->DispEventUnadvise(m_pMrFrameInterface);
	m_pMrFrameInterface.Release();
	m_pMrFrameControl.DestroyWindow();
	if (m_pDlgToolbar.GetSafeHwnd()!=NULL)
		m_pDlgToolbar.DestroyWindow();
	if (m_pDlgViewContact.GetSafeHwnd()!=NULL)
		m_pDlgViewContact.DestroyWindow();

	CEbDialogBase::OnDestroy();
	if (m_pPanelFind!=NULL)
	{
		m_pPanelFind->DestroyWindow();
		delete m_pPanelFind;
		m_pPanelFind = NULL;
	}
	if (m_pPanelStatus!=NULL)
	{
		m_pPanelStatus->DestroyWindow();
		delete m_pPanelStatus;
		m_pPanelStatus = NULL;
	}
	m_webUICtrl.ClearWebBrowser();
	delete this;
}

void CDlgMsgRecord::OnBnClickedButtonMin()
{
	PostMessage(WM_SYSCOMMAND, SC_MINIMIZE);
}

void CDlgMsgRecord::OnBnClickedButtonMax()
{
	int m_nScreenWidth = GetSystemMetrics(SM_CXSCREEN); //屏幕宽度   
	//int m_nScreenHeight = GetSystemMetrics(SM_CYSCREEN); //屏幕高度
	int m_nScreenHeight = GetSystemMetrics(SM_CYFULLSCREEN);
	m_nScreenHeight += GetSystemMetrics(SM_CYCAPTION);

	static CRect theRestoreRect;
	CRect rect;
	GetWindowRect(&rect);
	if (rect.Width() == m_nScreenWidth)
	{
		this->SetToolTipText(IDC_BUTTON_MAX,_T("最大化"));
		m_btnMax.SetDrawToolButtonPic(2,theDefaultBtnWhiteColor,-1,-1,-1,2);
		MoveWindow(&theRestoreRect);
	}else
	{
		this->SetToolTipText(IDC_BUTTON_MAX,_T("向下还原"));
		m_btnMax.SetDrawToolButtonPic(3,theDefaultBtnWhiteColor,-1,-1,-1,2);
		theRestoreRect = rect;
		rect.left = 0;
		rect.top = 0;
		rect.right = rect.left + m_nScreenWidth;
		rect.bottom = rect.top + m_nScreenHeight;
		MoveWindow(&rect);
	}
	if (m_labelLineConversation.GetCheck())
		SetTimer(TIMER_REFRESH_BROWSER,10,NULL);
	//m_pMrFrame.Scroll();
}

void CDlgMsgRecord::OnBnClickedButtonClose()
{
	if (!m_bChildMode)
		this->ShowWindow(SW_HIDE);
	this->SendMessage(WM_CLOSE, 0, 0);
}

void CDlgMsgRecord::OnPaint()
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
	//else if (!m_bChildMode)
	else if (!m_bChildMode || m_labelLineConversation.GetSafeHwnd()==NULL || !m_labelLineConversation.GetCheck())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文
		CRect rectClient;
		this->GetClientRect(&rectClient);
		CSkinMemDC memDC(&dc, rectClient);
		this->ClearBgDrawInfo();
		if (m_bChildMode)
		{
			this->AddBgDrawInfo(CEbBackDrawInfo(0,1.0,false,false,0,theDefaultFlatBgColor));
			this->DrawPopBg(&memDC, theApp.GetMainColor(), 0);
			return;
		}
		this->AddBgDrawInfo(CEbBackDrawInfo(theDefaultBgColorTitleHeight,1.0,false));
		this->AddBgDrawInfo(CEbBackDrawInfo(0,1.0,false,false,0,theDefaultFlatBgColor));
		this->DrawPopBg(&memDC, theApp.GetMainColor(), 1);

		//this->DrawPopBg(&memDC, theApp.GetMainColor(), 1,theDefaultBgColorTitleHeight,2);

		//if (theApp.GetColorGradient())
		//	this->AddBgDrawInfo(CEbBackDrawInfo(theDefaultBgColorTitleHeight,theDefaultBgColorTitleLight1,true));
		//else
		//	this->AddBgDrawInfo(CEbBackDrawInfo(theDefaultBgColorTitleHeight,1.0,false));
		//this->AddBgDrawInfo(CEbBackDrawInfo(41,theDefaultBgColorBgLight1,false));
		//this->AddBgDrawInfo(CEbBackDrawInfo(1,theDefaultBgColorBgLight3,false));
		//this->AddBgDrawInfo(CEbBackDrawInfo(0,theDefaultBgColorBgLight1,false));
		//this->DrawPopBg(&memDC, theApp.GetMainColor());
		Gdiplus::Graphics graphics(memDC.m_hDC);

		CString sWindowText;
		sWindowText.Format(_T("%s聊天记录"),m_sName.c_str());
		USES_CONVERSION;
		const FontFamily fontFamily(theFontFamily.c_str());
		const Gdiplus::Font fontEbTitle(&fontFamily, 14, FontStyleBold, UnitPixel);
		Gdiplus::SolidBrush brushEbTitle(Gdiplus::Color(255,255,255));
		const Gdiplus::PointF pointTitle(12,12);
		graphics.DrawString(A2W_ACP(sWindowText),-1,&fontEbTitle,pointTitle,&brushEbTitle);
	}else
	{
		CPaintDC dc(this); // 用于绘制的设备上下文
	}
}

void CDlgMsgRecord::LoadAccountMsgRecord(eb::bigint sAccount, const tstring& sName)
{
	m_sAccount = sAccount;
	m_sGroupCode = 0;
	m_sName = sName;
	SetTimer(TIMERID_LOAD_MSG_RECORD,10,NULL);
}

void CDlgMsgRecord::LoadDepartmentMsgRecord(eb::bigint sDepCode, const tstring& sName)
{
	m_sAccount = 0;
	m_sGroupCode = sDepCode;
	m_sName = sName;
	SetTimer(TIMERID_LOAD_MSG_RECORD,10,NULL);
}
void CDlgMsgRecord::RedrawCefBrowser(void)
{
	if (m_labelLineConversation.GetCheck() && m_nBrowserType==EB_BROWSER_TYPE_CEF)
	{
		m_pCefBrowser.SetFocus();
		KillTimer(TIMER_REFRESH_BROWSER);
		SetTimer(TIMER_REFRESH_BROWSER,10,NULL);
	}
}
void CDlgMsgRecord::NotifyMoveOrResizeStarted(void)
{
#ifdef USES_LIBCEF
	if (m_nBrowserType==EB_BROWSER_TYPE_CEF && m_pCefBrowser.GetSafeHwnd()!=NULL)
	{
		m_pCefBrowser.NotifyMoveOrResizeStarted();
	}
#endif
}

//void CDlgMsgRecord::RefreshWeb(void)
//{
//	if (m_nBrowserType==EB_BROWSER_TYPE_CEF && m_pCefBrowser.GetSafeHwnd()!=NULL && m_labelLineConversation.GetCheck())
//	{
//		KillTimer(TIMER_REFRESH_BROWSER);
//		SetTimer(TIMER_REFRESH_BROWSER,20,NULL);
//	}
//}

void CDlgMsgRecord::OnMove(void)
{
	if (m_nBrowserType==EB_BROWSER_TYPE_CEF && m_pCefBrowser.GetSafeHwnd()!=NULL && m_labelLineConversation.GetCheck())
	{
		KillTimer(TIMER_REFRESH_BROWSER);
		SetTimer(TIMER_REFRESH_BROWSER,20,NULL);
	}
}
void CDlgMsgRecord::WriteTitle(eb::bigint nMsgId,bool bPrivate,eb::bigint nFromUid,const tstring& sFromName,eb::bigint nToUid,const tstring& sToName,time_t tMsgTime)
{
	if (m_nLastMsgId==nMsgId && m_nFromUid==nFromUid && m_nToUid==nToUid)
		return;

	m_nLastMsgId = nMsgId;
	m_nFromUid = nFromUid;
	m_nToUid = nToUid;
	
	const CTime pMsgTime(tMsgTime);
	if (pMsgTime.GetYear() != m_tLastMsgDayTime.GetYear() ||
		pMsgTime.GetMonth() != m_tLastMsgDayTime.GetMonth() ||
		pMsgTime.GetDay() != m_tLastMsgDayTime.GetDay())
	{
		m_tLastMsgDayTime = pMsgTime;
		const CString sDayOfWeek = theDayOfWeek[m_tLastMsgDayTime.GetDayOfWeek()-1];
		CString sText;
		sText.Format(_T("--------  %04d-%02d-%02d %s  --------"),m_tLastMsgDayTime.GetYear(),m_tLastMsgDayTime.GetMonth(),m_tLastMsgDayTime.GetDay(),sDayOfWeek);
		m_pMrFrameInterface->AddLine(nMsgId);
		m_pMrFrameInterface->SetAlignmentFormat(1);
		m_pMrFrameInterface->WriteString((LPCTSTR)sText,theDefaultChatSystemColor);
		//const bool hIsScrollEnd = (m_pMrFrameInterface->IsScrollEnd()==VARIANT_TRUE)?true:false;
		//m_pMrFrameInterface->UpdateSize(VARIANT_TRUE);
		//if (hIsScrollEnd)
		//	m_pMrFrameInterface->ScrollToPos(-1);
	}

	m_pMrFrameInterface->AddLine(nMsgId);	// *** new line
	if (nFromUid==theApp.GetLogonUserId())
	{
		m_pMrFrameInterface->SetAlignmentFormat(2);
	}
	CString sPrivateText;
	CString sToText;
	if (m_sGroupCode>0)
	{
		if (bPrivate)
			sPrivateText = _T("[私聊]");
		if (nToUid == theApp.GetLogonUserId())
			sToText.Format(_T("对你说"));
		else if (nToUid>0)
			sToText.Format(_T("对%s说"),sToName.c_str());
	}
	CString sWindowText;
	if (m_nFromUid!=theApp.GetLogonUserId())// && m_sGroupCode>0)
	{
		if (nToUid>0)
		{
			sWindowText.Format(_T("%s#CTRL:%d:%d:%lld#%s#CTRL:%d:%d:%lld#%s"),sPrivateText,(int)(EB_MR_CTRL_TYPE_LCLICK_CB|EB_MR_CTRL_TYPE_MOVE_ENTER_CB|EB_MR_CTRL_TYPE_MOVE_LEAVE_CB),(int)EB_MR_CTRL_DATA_TYPE_UID,m_nFromUid,sFromName.c_str(),
				(int)(EB_MR_CTRL_TYPE_LCLICK_CB|EB_MR_CTRL_TYPE_MOVE_ENTER_CB|EB_MR_CTRL_TYPE_MOVE_LEAVE_CB),(int)EB_MR_CTRL_DATA_TYPE_UID,nToUid,sToText);
		}else if (m_nFromUid>0)
			sWindowText.Format(_T("%s#CTRL:%d:%d:%lld#%s%s"),sPrivateText,(int)(EB_MR_CTRL_TYPE_LCLICK_CB|EB_MR_CTRL_TYPE_MOVE_ENTER_CB|EB_MR_CTRL_TYPE_MOVE_LEAVE_CB),(int)EB_MR_CTRL_DATA_TYPE_UID,m_nFromUid,sFromName.c_str(),sToText);
		else
			sWindowText.Format(_T("%s%s%s"),sPrivateText,sFromName.c_str(),sToText);
	}else
	{
		sWindowText.Format(_T("%s%s%s"),sPrivateText,sFromName.c_str(),sToText);
	}
	const COLORREF crTextColor = (nFromUid==theApp.GetLogonUserId())?theDefaultChatTitleColor1:theDefaultChatTitleColor2;
	m_pMrFrameInterface->WriteString((LPCTSTR)sWindowText,crTextColor);
	m_pMrFrameInterface->WriteSpace(1);
	m_pMrFrameInterface->WriteTime(tMsgTime,"%H:%M");

	m_pMrFrameInterface->AddLine(nMsgId);	// *** new line
	m_pMrFrameInterface->SetFrameArc(theArcOffset,thePoloygonWidth,thePoloygonHeight);
	m_pMrFrameInterface->SetFrameBorderColor(RGB(128,128,128));
	if (nFromUid==theApp.GetLogonUserId())
	{
		m_pMrFrameInterface->SetAlignmentFormat(2);
		m_pMrFrameInterface->SetFrameBackGroundColor(theDefaultChatBackGroundColor1);
	}else
	{
		m_pMrFrameInterface->SetFrameBackGroundColor(theDefaultChatBackGroundColor2);
	}
}

void CDlgMsgRecord::LoadMsgRecord(const CString& sSql)
{
	if (m_pMrFrameInterface==NULL) return;
	m_pMrFrameInterface->Clear();
	m_nLastMsgId = 0;
	m_nFromUid = 0;
	m_nToUid = 0;
	CString sSearchString;
	m_editSearch.GetWindowText(sSearchString);
	const tstring sSearchStringTemp(sSearchString);

	eb::bigint nSearchByDateMsgId = 0;
	CTime pSearchByDateTime;
	if (m_bSearchByDate)
	{
		m_tDataTime.GetTime(pSearchByDateTime);
	}

	//CString sMsgTime;
	//time_t nLocalMsgTime = 0;
	USES_CONVERSION;
	int nCookie = 0;
	const mycp::bigint nRet = theApp.m_pBoUsers->select(sSql, nCookie);
	cgcValueInfo::pointer pRecord = theApp.m_pBoUsers->first(nCookie);
	int nIndex = 0;
	while (pRecord.get()!=NULL)
	{
		const tstring sMsgTime(pRecord->getVector()[0]->getStrValue());
		time_t nMsgTime = 0;
		libEbc::ChangeTime(sMsgTime.c_str(),nMsgTime);
		const eb::bigint sMsgId = pRecord->getVector()[1]->getBigIntValue();
		if (m_bSearchByDate && nSearchByDateMsgId==0)
		{
			CTime pMsgTime(nMsgTime);
			if (pMsgTime.GetYear()>=pSearchByDateTime.GetYear() &&
				pMsgTime.GetMonth()>=pSearchByDateTime.GetMonth() &&
				pMsgTime.GetDay()>=pSearchByDateTime.GetDay())
			{
				nSearchByDateMsgId = sMsgId;
			}
		}

		if (sSearchStringTemp.empty())
		{
			const int nMsgIdOffset = (m_nCurrentPage==-1?(m_nRecordPages-1):m_nCurrentPage)*const_one_page_count+(nIndex++);
			m_pMsgIdOffsetList.insert(sMsgId,nMsgIdOffset);
		}
		const tstring soffTime(pRecord->getVector()[2]->getStrValue());
		const eb::bigint sFromAccount = pRecord->getVector()[3]->getBigIntValue();
		const tstring sFromName(libEbc::UTF82ACP(pRecord->getVector()[4]->getStrValue().c_str()));
		const eb::bigint sToAccount = pRecord->getVector()[5]->getBigIntValue();
		const tstring sToName(libEbc::UTF82ACP(pRecord->getVector()[6]->getStrValue().c_str()));
		const int nPrivate = pRecord->getVector()[7]->getIntValue();
		const int nMsgType = pRecord->getVector()[8]->getIntValue();
		const tstring sMsgName(libEbc::UTF82ACP(pRecord->getVector()[9]->getStrValue().c_str()));
		tstring sMsgText(libEbc::UTF82ACP(pRecord->getVector()[10]->getStrValue().c_str()));
		//const int nMsgSize = pResltSet->rsvalues[i]->fieldvalues[10]->v.varcharVal.size;
		//const int nReadFlag = pRecord->getVector()[11]->getIntValue();
		pRecord = theApp.m_pBoUsers->next(nCookie);

		//// 兼容旧版本
		//if (sFromName.empty())
		//	sFromName = sFromAccount;
		//if (sToName.empty())
		//	sToName = sToAccount;
		WriteTitle(sMsgId,nPrivate==1,sFromAccount,sFromName,sToAccount,sToName,nMsgTime);
		CString sWindowText;
		if (MRT_UNKNOWN==nMsgType)
		{
			//pRecord = theApp.m_pBoUsers->next(nCookie);
			continue;
		}else if (MRT_TEXT==nMsgType)
		{
			const bool bReceive = sFromAccount!=theApp.GetLogonUserId();
			CSqliteCdbc::escape_string_out(sMsgText);
			const COLORREF colorText = m_nViewMsgId>0&&m_nViewMsgId==sMsgId?RGB(255,0,0):(bReceive?theDefaultChatTextColor2:theDefaultChatTextColor1);
			bool bSearchStringExistUrl = false;
			if (!sSearchStringTemp.empty())
			{
				CString sStringLower(sMsgText.c_str());
				sStringLower.MakeLower();
				int nfind1 = sStringLower.Find("http://");
				if (nfind1 < 0)
					nfind1 = sStringLower.Find("https://");
				if (nfind1 < 0)
					nfind1 = sStringLower.Find("www.");
				bSearchStringExistUrl = (nfind1>=0)?true:false;
			}
			if (sSearchStringTemp.empty() || bSearchStringExistUrl)
				m_pMrFrameInterface->WriteString(sMsgText.c_str(),colorText);
			else
			{
				std::vector<tstring> pList;
				const int nSize = libEbc::ParseString(sMsgText.c_str(),sSearchStringTemp.c_str(),pList);
				const bool bLastTextIsSearchText = sMsgText.size()<sSearchStringTemp.size()?false:(sMsgText.substr(sMsgText.size()-sSearchStringTemp.size())==sSearchStringTemp);
				for (int i=0;i<nSize; i++)
				{
					const tstring sTextTemp(pList[i]);
					if (!sTextTemp.empty())
						m_pMrFrameInterface->WriteString(sTextTemp.c_str(),colorText);
					if ((i+1)!=nSize || bLastTextIsSearchText)
					{
						m_pMrFrameInterface->WriteString(sSearchStringTemp.c_str(),RGB(255,128,0));
					}
				}
			}
		}else if (MRT_JPG==nMsgType)
		{
			const tstring sFilePath(libEbc::URLEncode(sMsgName.c_str()));
			CString sTemp;
			sTemp.Format(_T("#CTRL:%d:%d:%s#%s"),(int)(EB_MR_CTRL_TYPE_LDBLCLICK_OPEN|EB_MR_CTRL_TYPE_MOVE_ENTER_CB|EB_MR_CTRL_TYPE_MOVE_LEAVE_CB),(int)EB_MR_CTRL_DATA_TYPE_IMAGE,sFilePath.c_str(),sMsgName.c_str());
			m_pMrFrameInterface->WriteImage((LPCTSTR)sTemp,0,0);
		}else if (MRT_WAV==nMsgType)
		{
			//WriteFileHICON(sMsgName.c_str());
			const int nWavTimeLength = libEbc::GetWaveTimeLength(sMsgName.c_str());
			CString sText;
			if (nWavTimeLength>=0)
				sText.Format(_T("语音消息 %d\""),nWavTimeLength);
			else if (nWavTimeLength==-1)
				sText.Format(_T("语音消息不存在\""),nWavTimeLength);
			else
				sText.Format(_T("语音消息格式错误\""),nWavTimeLength);
			m_pMrFrameInterface->WriteWav((LPCTSTR)sText,sMsgName.c_str(),EB_MR_CTRL_TYPE_LCLICK_OPEN);
			//m_pMrFrameInterface->WriteWav("播放语音",sMsgName.c_str(),EB_MR_CTRL_TYPE_LCLICK_OPEN);
		}else if (MRT_FILE==nMsgType)
		{
			//const bool bReceive = sFromAccount!=theApp.GetLogonUserId();

			const std::wstring sImagePath = A2W_ACP(sMsgText.c_str());
			bool bIsImage = false;
			eb::bigint nFileSize = -1;
			FILE * f = fopen(sMsgText.c_str(), "rb");
			if (f != NULL)
			{
				_fseeki64(f, 0, SEEK_END);
				nFileSize = _ftelli64(f);
				fclose(f);
				Gdiplus::Image * image = Gdiplus::Image::FromFile(sImagePath.c_str());
				bIsImage = (bool)(image->GetType()!= Gdiplus::ImageTypeUnknown);
				delete image;
			}
			if (bIsImage)
			{
				const tstring sFilePath(libEbc::URLEncode(sMsgText.c_str()));
				CString sTemp;
				sTemp.Format(_T("#CTRL:%d:%d:%s#%s"),(int)(EB_MR_CTRL_TYPE_LDBLCLICK_OPEN|EB_MR_CTRL_TYPE_MOVE_ENTER_CB|EB_MR_CTRL_TYPE_MOVE_LEAVE_CB),(int)EB_MR_CTRL_DATA_TYPE_IMAGE,sFilePath.c_str(),sMsgText.c_str());
				m_pMrFrameInterface->WriteImage((LPCTSTR)sTemp,0,0);
				m_pMrFrameInterface->WriteLine();
			}else
			{
				WriteFileHICON(sMsgText.c_str());
			}
			{
				const tstring sFileName = libEbc::GetFileName(sMsgText);
				CString sFileText;
				if (nFileSize == -1)
					sFileText.Format(_T("%s (文件不存在)"),sFileName.c_str());
				else if (nFileSize >= const_gb_size)
					sFileText.Format(_T("%s (%.02fGB)"),sFileName.c_str(),(double)nFileSize/const_gb_size);
				else if (nFileSize >= const_mb_size)
					sFileText.Format(_T("%s (%.02fMB)"),sFileName.c_str(),(double)nFileSize/const_mb_size);
				else if (nFileSize >= const_kb_size)
					sFileText.Format(_T("%s (%.02fKB)"),sFileName.c_str(),(double)nFileSize/const_kb_size);
				else
					sFileText.Format(_T("%s (%lldByte)"),sFileName.c_str(),nFileSize);
				m_pMrFrameInterface->WriteSpace(1);
				CString sTemp;
				sTemp.Format(_T("#CTRL:0:%d:#%s"),(int)EB_MR_CTRL_DATA_TYPE_FILE,sFileText);
				m_pMrFrameInterface->WriteUrl((LPCTSTR)sTemp, sMsgText.c_str(),EB_MR_CTRL_TYPE_LDBLCLICK_OPEN|EB_MR_CTRL_TYPE_MOVE_ENTER_CB|EB_MR_CTRL_TYPE_MOVE_LEAVE_CB,theDefaultFlatBlackTextColor);
				//m_pMrFrameInterface->WriteUrl((LPCTSTR)sFileText, sMsgText.c_str(), EB_MR_CTRL_TYPE_LDBLCLICK_OPEN,theDefaultFlatBlackTextColor);
			}
			//m_pMrFrameInterface->WriteLine();
			//m_pMrFrameInterface->WriteSpace(11);
			//if (nFileSize>=0)
			//{
			//	m_pMrFrameInterface->WriteOpenFile(L"打开",sMsgText.c_str());
			//	m_pMrFrameInterface->WriteSpace(2);
			//}
			//m_pMrFrameInterface->WriteOpenDir(L"打开文件夹",sMsgText.c_str());
		}
		//pRecord = theApp.m_pBoUsers->next(nCookie);
	}
	theApp.m_pBoUsers->reset(nCookie);

	if (nRet>0)
	{
		m_pMrFrameInterface->UpdateSize(VARIANT_TRUE);
		if (m_nViewMsgId>0)
		{
			m_pMrFrameInterface->ViewLine(m_nViewMsgId);
			m_nViewMsgId = 0;
		}else
		{
			if (m_bSearchByDate)
			{
				m_bSearchByDate = false;
				if (nSearchByDateMsgId>0)
					m_pMrFrameInterface->ViewLine(nSearchByDateMsgId);
				else
					m_pMrFrameInterface->ScrollToPos(-1);
			}else if (m_bViewSearchResult)
			{
				//m_bViewSearchResult = false;
				m_pMrFrameInterface->ScrollToPos(0);
			}else
			{
				m_pMrFrameInterface->ScrollToPos(-1);
			}
		}
	}
	if (!m_bChildMode)
	{
		//ShowWindow(SW_SHOW);
		SetForegroundWindow();
	}
}
void CDlgMsgRecord::WriteFileHICON(const char* lpszFilePath)
{
	SHFILEINFO sfi; 
	ZeroMemory(&sfi,sizeof(sfi)); 
	DWORD ret = SHGetFileInfo(lpszFilePath,
		FILE_ATTRIBUTE_NORMAL, 
		&sfi, 
		sizeof(sfi), 
		SHGFI_USEFILEATTRIBUTES|SHGFI_ICON);
	if (ret == 1)
	{
		m_pMrFrameInterface->WriteHICON((ULONG*)sfi.hIcon,lpszFilePath,EB_MR_CTRL_TYPE_LCLICK_OPEN);
	}
}

void CDlgMsgRecord::OnClose()
{
	if (m_bChildMode)
	{
		this->ShowWindow(SW_HIDE);
		//CEbDialogBase::OnClose();
	}else
	{
		if (m_sAccount>0)
			theApp.m_pMsgRecord.remove(m_sAccount);
		if (m_sGroupCode>0)
			theApp.m_pMsgRecord.remove(m_sGroupCode);
		this->ShowWindow(SW_HIDE);
		theApp.m_pCloseWnd.add(this);
	}
	//CEbDialogBase::OnClose();
	//this->EndDialog(IDOK);
}

//BOOL CDlgMsgRecord::OnEraseBkgnd(CDC* pDC)
//{
//	// TODO: Add your message handler code here and/or call default
//	return TRUE;
//	return CEbDialogBase::OnEraseBkgnd(pDC);
//}

void CDlgMsgRecord::OnBnClickedButtonDeleteall()
{
	if (m_pMrFrameInterface->GetLineCount()==0) return;
	const bool bIsAccount = m_sAccount>0;
	eb::bigint sId = bIsAccount?m_sAccount:m_sGroupCode;
	CString sText;
	if (bIsAccount)
		sText.Format(_T("确定清空：\r\n%s(%lld) 本地聊天记录吗？"), m_sName.c_str(),sId);
	else
		sText.Format(_T("确定清空：\r\n%s 本地聊天记录吗？"), m_sName.c_str());	// 群组（部门）不需要显示group_code
	if (CDlgMessageBox::EbDoModal(this,"清空聊天记录",sText,CDlgMessageBox::IMAGE_QUESTION)==IDOK)
	{
		theApp.DeleteDbRecord(sId,bIsAccount);
		m_pMrFrameInterface->Clear();
		//m_pMrFrameControl.Invalidate();	// 没用
	}
}

void CDlgMsgRecord::OnEnChangeEditSearch()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CEbDialogBase::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	static CString theSearchString;
	CString sString;
	m_editSearch.GetWindowText(sString);
	if (sString==theSearchString)
		return;
	theSearchString = sString;
	if (this->IsWindowVisible())	// 避免关闭窗口后，重新打开；
	{
		KillTimer(TIMERID_LOAD_MSG_RECORD);
		m_bViewSearchResult = sString.IsEmpty()?false:true;
		SetTimer(TIMERID_LOAD_MSG_RECORD,100,NULL);
	}
	//if (!sSearchString.IsEmpty())
	//{
	//	// ???

	//}
}

void CDlgMsgRecord::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	if (this->m_bChildMode)
		this->GetParent()->PostMessage(EB_COMMAND_RAME_WND_MAX);
	else
		OnBnClickedButtonMax();
	CEbDialogBase::OnLButtonDblClk(nFlags, point);
}
void CDlgMsgRecord::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (this->m_bChildMode && this->GetParent()->GetParent()!=NULL && this->GetParent()->GetParent()->GetParent()!=NULL)
		this->GetParent()->GetParent()->GetParent()->PostMessage(WM_NCLBUTTONDOWN, HTCAPTION, MAKELPARAM(point.x, point.y));	
	CEbDialogBase::OnLButtonDown(nFlags, point);
}

void CDlgMsgRecord::Fire_onContextMenuDelete(LONGLONG nMsgId,LONG nIndex)
{
	if (nMsgId>0)
	{
		m_pDeletMsgId.push_back(nMsgId);
	}else if (!m_pDeletMsgId.empty())
	{
		SetTimer(TIMERID_DELETE_SELECT_MSG,10,NULL);
	}
}

void CDlgMsgRecord::Fire_onContextMenuSelect(ULONG nItemData, const CEBString& sSelectString,LONGLONG nSelectMsgId)
{
	if (LOCATE_MSG_ITEMDATA==nItemData && nSelectMsgId>0)
	{
		CString sSearchString;
		m_editSearch.GetWindowText(sSearchString);
		if (!sSearchString.IsEmpty())
		{
			m_bViewSearchResult = false;
			m_nViewMsgId = nSelectMsgId;
			SetTimer(TIMERID_LOAD_MSG_RECORD,10,NULL);
		}
	}else if (nItemData>=0 && nItemData<theApp.m_pChatMenuSubscribeFuncList.size())
	{
		const EB_SubscribeFuncInfo& pSubscribeFuncInfo = theApp.m_pChatMenuSubscribeFuncList[nItemData];
		theApp.OpenSubscribeFuncWindow(pSubscribeFuncInfo,libEbc::ACP2UTF8(sSelectString.c_str()),"",NULL);
	}
}
void CDlgMsgRecord::Fire_onItemLBtnClick(LONG nLineId, LONG nItemId, ULONG nItemData, const CEBString& sParamString, LONGLONG nSelectMsgId)
{
	switch (nItemData)
	{
	case EB_MR_CTRL_DATA_TYPE_UID:
		{
#ifdef USES_EBCOM_TEST
			theEBClientCore->EB_CallAccount(sParamString.c_str(),0);
#else
			theEBAppClient.EB_CallAccount(sParamString.c_str());
#endif
		}break;
	default:
		break;
	}
}
//void CDlgMsgRecord::Fire_onItemLBtnDblClick(LONG nLineId, LONG nItemId, ULONG nItemData, const CEBString& sParamString, LONGLONG nSelectMsgId)
//{
//	switch (nItemData)
//	{
//	case EB_MR_CTRL_DATA_TYPE_UID:
//		{
//#ifdef USES_EBCOM_TEST
//			theEBClientCore->EB_CallAccount(sParamString.c_str(),0);
//#else
//			theEBAppClient.EB_CallAccount(sParamString.c_str());
//#endif
//		}break;
//	default:
//		break;
//	}
//}
void CDlgMsgRecord::Fire_onItemMoveEnter(LONG nLineId, LONG nItemId, ULONG nItemData, const CEBString& sParamString, LONGLONG nSelectMsgId, ULONG nParam)
{
	KillTimer(TIMERID_MOVE_LEAVE);

	switch (nItemData)
	{
	case EB_MR_CTRL_DATA_TYPE_UID:
		{
			const eb::bigint nUserId = eb_atoi64(sParamString.c_str());
			if (m_sGroupCode==0 || nUserId==0)
				break;
			if (m_pDlgToolbar.GetSafeHwnd()!=NULL && m_pDlgToolbar.IsWindowVisible())
				m_pDlgToolbar.HideReset();

			if (m_pDlgViewContact.GetSafeHwnd()==NULL)
			{
				m_pDlgViewContact.Create(CDlgViewContactInfo::IDD,this);
			}
			
			EB_GroupInfo pGroupInfo;
			if (!theEBAppClient.EB_GetGroupInfo(m_sGroupCode,&pGroupInfo))
				break;
			EB_MemberInfo pMemberInfo;
			if (!theEBAppClient.EB_GetMemberInfoByUserId(&pMemberInfo,m_sGroupCode,nUserId))
				break;
			m_pDlgViewContact.SetEmployeeInfo(&pMemberInfo,&pGroupInfo);
			const int const_dlg_width = 380;
			const int const_dlg_height = 188;
			CPoint pos;
			pos.x = LOWORD(nParam);
			pos.y = HIWORD(nParam);
			//::GetCursorPos(&pos);
			int x = pos.x+15 + (pos.x%15);
			const int y = pos.y-35 - (pos.y%35);
			const int nScreenWidth = theApp.GetScreenWidth();
			if (x+const_dlg_width>nScreenWidth)
				x = pos.x-15 - (pos.x%15) - const_dlg_width;
			m_pDlgViewContact.MoveWindow(x,y,const_dlg_width,const_dlg_height);
			m_pDlgViewContact.SetCircle();
			m_pDlgViewContact.SetMoveEnter();
			//m_pDlgViewContact.ShowWindow(SW_SHOW);
			//m_pDlgViewContact.SetCheckLeave();
		}break;
	case EB_MR_CTRL_DATA_TYPE_URL:
	case EB_MR_CTRL_DATA_TYPE_IMAGE:
	case EB_MR_CTRL_DATA_TYPE_FILE:
	case EB_MR_CTRL_DATA_TYPE_SELECT_STRING:
		{
			if (m_pDlgViewContact.GetSafeHwnd()!=NULL)// && m_pDlgViewContact.IsWindowVisible())
				m_pDlgViewContact.HideReset();
			if (m_pDlgToolbar.GetSafeHwnd()==NULL)
			{
				m_pDlgToolbar.Create(CDlgToolbar::IDD,this);
				m_pDlgToolbar.SetMsgHwnd(this->GetSafeHwnd(),this);
				m_pDlgToolbar.SetChildMode(m_bChildMode);
			}
			bool bChangeData = false;
			const int nCount = m_pDlgToolbar.SetMoveEnterData((EB_MR_CTRL_DATA_TYPE)nItemData,libEbc::URLDecode(sParamString.c_str(),false),bChangeData);
			if (bChangeData || !m_pDlgToolbar.IsWindowVisible())
			{
				CPoint pos;
				pos.x = LOWORD(nParam);
				pos.y = HIWORD(nParam);
				//::GetCursorPos(&pos);
				int x = pos.x-35 - (pos.x%35);
				const int y = pos.y+10 + (pos.y%10);
				const int nToolBarWidth = 56*nCount+11;
				const int nScreenWidth = theApp.GetScreenWidth();
				if (x+nToolBarWidth>nScreenWidth)
					x = nScreenWidth-nToolBarWidth;
				m_pDlgToolbar.MoveWindow(x,y,nToolBarWidth,36);
				m_pDlgToolbar.ShowWindow(SW_SHOW);
			}
		}break;
	default:
		break;
	}

}
void CDlgMsgRecord::Fire_onItemMoveLeave(LONG nLineId, LONG nItemId, ULONG nItemData, const CEBString& sParamString, LONGLONG nSelectMsgId)
{
	switch (nItemData)
	{
	case EB_MR_CTRL_DATA_TYPE_UID:
		{
			if (m_pDlgViewContact.GetSafeHwnd()!=NULL)// && m_pDlgViewContact.IsWindowVisible())
			{
				m_pDlgViewContact.SetMoveLeave();
			}
		}break;
	default:
		{
			KillTimer(TIMERID_MOVE_LEAVE);
			SetTimer(TIMERID_MOVE_LEAVE,400,NULL);
		}break;
	}
}
tstring CDlgMsgRecord::GetSelectString(void) const
{
	if (m_pMrFrameInterface==NULL) return "";
	const CEBString sString(m_pMrFrameInterface->GetSelectString().GetBSTR());
	return sString;
}

void CDlgMsgRecord::OnCancel()
{
	// TODO: Add your specialized code here and/or call the base class
	if (m_bChildMode)
		this->GetParent()->PostMessage(EB_COMMAND_RAME_WND_CLOSE, (WPARAM)this->GetSafeHwnd(), 0);
	OnBnClickedButtonClose();
	//__super::OnCancel();
}

void CDlgMsgRecord::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class
	//OnBnClickedButtonClose();
	//__super::OnOK();
}

void CDlgMsgRecord::OnStnClickedStaticLineConversation()
{
	if (m_labelLineConversation.GetCheck())
	{
//		if (!m_bNavigateUrl)
//		{
//#ifdef USES_EBCOM_TEST
//			const CEBString sFuncUrl = theEBClientCore->EB_GetConversationsUrl(m_sAccount,m_sGroupCode).GetBSTR();
//#else
//			const CEBString sFuncUrl = "www.entboost.com";
//			//const CEBString sFuncUrl = theEBAppClient.EB_GetConversationsUrl(m_sAccount,m_sGroupCode);
//#endif
//			if (!sFuncUrl.empty())
//			{
//				m_bNavigateUrl = true;
//				m_pExplorer.Navigate(sFuncUrl.c_str(),NULL,NULL,NULL,NULL);
//			}else
//			{
//				m_labelLineConversation.SetCheck(FALSE);
//				m_labelLineConversation.Invalidate();
//				return;
//			}
//		}
		m_labelViewAll.ShowWindow(SW_HIDE);
		m_labelViewImage.ShowWindow(SW_HIDE);
		m_labelViewFile.ShowWindow(SW_HIDE);
		m_labelViewAudio.ShowWindow(SW_HIDE);
		m_editSearch.ShowWindow(SW_HIDE);
		m_btnDeleteAll.ShowWindow(SW_HIDE);
		m_tDataTime.ShowWindow(SW_HIDE);
		m_btnMoveFirst.ShowWindow(SW_HIDE);
		m_btnMovePrev.ShowWindow(SW_HIDE);
		m_btnMoveNext.ShowWindow(SW_HIDE);
		m_btnMoveLast.ShowWindow(SW_HIDE);
		CRect rect;
		this->GetClientRect(&rect);
		const int const_interval = 1;
		const int nExpolorerTop = m_bChildMode?0:theDefaultBgColorTitleHeight;
		if (m_nBrowserType==EB_BROWSER_TYPE_CEF)
		{
			m_pCefBrowser.MoveWindow(const_interval,nExpolorerTop,rect.Width()-const_interval*2,rect.Height()-nExpolorerTop-28);
			m_pCefBrowser.ShowWindow(SW_SHOW);
		}else
			m_pExplorer.MoveWindow(const_interval,nExpolorerTop,rect.Width()-const_interval*2,rect.Height()-nExpolorerTop-28);
		//m_pExplorer.MoveWindow(const_interval,theDefaultBgColorTitleHeight,rect.Width()-const_interval*2,rect.Height()-theDefaultBgColorTitleHeight-24);
		m_pMrFrameControl.ShowWindow(SW_HIDE);
	}else
	{
		m_labelViewAll.ShowWindow(SW_SHOW);
		m_labelViewImage.ShowWindow(SW_SHOW);
		m_labelViewFile.ShowWindow(SW_SHOW);
		m_labelViewAudio.ShowWindow(SW_SHOW);
		m_editSearch.ShowWindow(SW_SHOW);
		m_btnDeleteAll.ShowWindow(SW_SHOW);
		if (m_nBrowserType==EB_BROWSER_TYPE_CEF)
			m_pCefBrowser.MoveWindow(0,0,0,0);
		else
			m_pExplorer.MoveWindow(0,0,0,0);
		m_tDataTime.ShowWindow(SW_SHOW);
		m_btnMoveFirst.ShowWindow(SW_SHOW);
		m_btnMovePrev.ShowWindow(SW_SHOW);
		m_btnMoveNext.ShowWindow(SW_SHOW);
		m_btnMoveLast.ShowWindow(SW_SHOW);
		m_pMrFrameControl.ShowWindow(SW_SHOW);
	}
}
void CDlgMsgRecord::ShowZoomLevel(void)
{
	int nZoomLevel = 100.0 + m_fZoomLevel*20.0;
	if (m_fZoomLevel>CEF_DEFAULT_ZOOM_OFFSET*2.0)
		nZoomLevel += (m_fZoomLevel-CEF_DEFAULT_ZOOM_OFFSET*2.0)*m_fZoomLevel*5.0;
	//const int nZoomLevel = 100.0 + m_fZoomLevel*20.0;
	wchar_t lpszBuffer[32];
	swprintf(lpszBuffer,L"缩放：%d%%",nZoomLevel);
	this->OnStatusMessage(lpszBuffer);
	SetTimer(TIMER_HIDE_STATUS_MESSAGE,2000,NULL);
}
BOOL CDlgMsgRecord::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	if (m_nBrowserType==EB_BROWSER_TYPE_CEF && m_pCefBrowser.GetSafeHwnd()!=NULL)
	{
		if (GetKeyState(VK_CONTROL)&0x80)
		{
			if (zDelta>=120)
			{
				// 向上滚动
				m_fZoomLevel = min(CEF_DEFAULT_MAX_ZOOM_IN,m_fZoomLevel+((m_fZoomLevel>CEF_DEFAULT_ZOOM_OFFSET)?(CEF_DEFAULT_ZOOM_OFFSET*2.0):CEF_DEFAULT_ZOOM_OFFSET));
				m_pCefBrowser.SetZoomLevel(m_fZoomLevel);
				ShowZoomLevel();
			}else if (zDelta<=-120)
			{
				// 向下滚动
				m_fZoomLevel = max(CEF_DEFAULT_MAX_ZOOM_OUT,m_fZoomLevel+(((m_fZoomLevel>CEF_DEFAULT_ZOOM_OFFSET*2.0)?(CEF_DEFAULT_ZOOM_OFFSET*2.0):CEF_DEFAULT_ZOOM_OFFSET)*-1.0));
				m_pCefBrowser.SetZoomLevel(m_fZoomLevel);
				ShowZoomLevel();
				return TRUE;
			}
		}
	}

	return __super::OnMouseWheel(nFlags, zDelta, pt);
}
void CDlgMsgRecord::MovePanelFind(int cx)
{
	if (m_pPanelFind != NULL && m_pPanelFind->GetSafeHwnd()!=NULL)
	{
		//int x = 0;
		//if (m_pPanelFind->GetUserData()>0)
		//{
		//	x = LOWORD(m_pPanelFind->GetUserData());
		//	const int w = HIWORD(m_pPanelFind->GetUserData());
		//	if (cx-x-w>DEFAULT_PANEL_FIND_WIDTH+DEFAULT_PANEL_FIND_RIGHT)
		//	{
		//		m_pPanelFind->SetUserData(0);
		//	}
		//}
		CRect rect;
		if (m_bChildMode)
		{
			rect.left = 20;
			rect.top = -2;
		}else
		{
			rect.left = 120;
			rect.top = 2;
		}
		rect.right = rect.left+DEFAULT_PANEL_FIND_WIDTH;
		//if (m_pPanelFind->GetUserData()==0)
		//{
		//	rect.right = cx-DEFAULT_PANEL_FIND_RIGHT;
		//	rect.left = rect.right - DEFAULT_PANEL_FIND_WIDTH;
		//}else
		//{
		//	rect.right = x-5;
		//	rect.left = rect.right - DEFAULT_PANEL_FIND_WIDTH;
		//}
		//rect.top = -2;
		rect.bottom = rect.top + DEFAULT_PANEL_FIND_HEIGHT;
		m_pPanelFind->MoveWindow(&rect);
	}
}
void CDlgMsgRecord::OnLoadingStateChange(const wchar_t* sUrl, bool bIsLoading, bool bCanGoBack, bool bCanGoForward)
{
	if (m_nBrowserType!=EB_BROWSER_TYPE_CEF) return;
	if (!bIsLoading)
	{
		if (!m_bDocumentComplete)
		{
			if (m_pCefBrowser.GetSafeHwnd()==NULL) return;
			m_bDocumentComplete = true;
#ifdef USES_EBCOM_TEST
			const CEBString sFuncUrl = theEBClientCore->EB_GetConversationsUrl(m_sAccount,m_sGroupCode).GetBSTR();
#else
			//const CEBString sFuncUrl = "www.entboost.com";
			const CEBString sFuncUrl = theEBAppClient.EB_GetConversationsUrl(m_sAccount,m_sGroupCode);
#endif
			if (!sFuncUrl.empty())
			{
				m_pCefBrowser.LoadURL(libEbc::ACP2UTF8(sFuncUrl.c_str()).string());
				//m_pCefBrowser.LoadURL(sFuncUrl.c_str());
			}else
			{
				m_labelLineConversation.EnableWindow(FALSE);
			}
		}
	}
}
bool CDlgMsgRecord::OnKeyEvent(const wchar_t* sUrl, CefEventHandle os_event)
{
	switch (os_event->message)
	{
	case WM_KEYDOWN:
		{
			if (os_event->wParam==VK_BACK) return true;	// cancel
			if (GetKeyState(VK_CONTROL)&0x80)
			{
				switch (os_event->wParam)
				{
				case VK_ADD:
					{
						m_fZoomLevel = min(CEF_DEFAULT_MAX_ZOOM_IN,m_fZoomLevel+((m_fZoomLevel>CEF_DEFAULT_ZOOM_OFFSET)?(CEF_DEFAULT_ZOOM_OFFSET*2.0):CEF_DEFAULT_ZOOM_OFFSET));
						m_pCefBrowser.SetZoomLevel(m_fZoomLevel);
						ShowZoomLevel();
						return TRUE;
					}
				case VK_SUBTRACT:
					{
						m_fZoomLevel = max(CEF_DEFAULT_MAX_ZOOM_OUT,m_fZoomLevel+(((m_fZoomLevel>CEF_DEFAULT_ZOOM_OFFSET*2.0)?(CEF_DEFAULT_ZOOM_OFFSET*2.0):CEF_DEFAULT_ZOOM_OFFSET)*-1.0));
						m_pCefBrowser.SetZoomLevel(m_fZoomLevel);
						ShowZoomLevel();
						return TRUE;
					}
				case 'P':
					m_pCefBrowser.Print();
					break;
				case 'F':
					OnExecuteMenu(CEF_CLIENT_MENU_COMMAND_ID_FIND,NULL);
					break;
				//case VK_UP:
				//	theApp.GetMainWnd()->PostMessage(EB_COMMAND_RAME_WND_MOVE_UP, 0, 0);
				//	return TRUE;
				//case VK_DOWN:
				//	theApp.GetMainWnd()->PostMessage(EB_COMMAND_RAME_WND_MOVE_DOWN, 0, 0);
				//	return TRUE;
				//case VK_NUMPAD0:
				//case VK_NUMPAD1:
				//case VK_NUMPAD2:
				//case VK_NUMPAD3:
				//case VK_NUMPAD4:
				//case VK_NUMPAD5:
				//case VK_NUMPAD6:
				//case VK_NUMPAD7:
				//case VK_NUMPAD8:
				//case VK_NUMPAD9:
				//	theApp.GetMainWnd()->PostMessage(EB_COMMAND_RAME_WND_MOVE_OFFSET, (WPARAM)(os_event->wParam-VK_NUMPAD0), 0);
				//	return TRUE;
				//case VK_LEFT:
				//	this->GetParent()->PostMessage(EB_COMMAND_RAME_WND_MOVE_UP,0,0);
				//	return TRUE;
				//case VK_RIGHT:
				//	this->GetParent()->PostMessage(EB_COMMAND_RAME_WND_MOVE_DOWN,0,0);
				//	return TRUE;
				//case VK_TAB:
				//	if (GetKeyState(VK_SHIFT)&0x80)
				//		this->GetParent()->PostMessage(EB_COMMAND_RAME_WND_MOVE_UP,0,0);
				//	else
				//		this->GetParent()->PostMessage(EB_COMMAND_RAME_WND_MOVE_DOWN,0,0);
				//	return TRUE;
				default:
					break;
				}
			}
		}break;
	default:
		break;
	}
	return false;
}
void CDlgMsgRecord::OnFindResult(int identifier,int count,const CefRect& selectionRect,int activeMatchOrdinal,bool finalUpdate)
{
	if (m_pPanelFind!=NULL && m_pPanelFind->IsWindowVisible())
	{
		if (activeMatchOrdinal>0)
		{
			CPoint posSelection1(selectionRect.x+1,selectionRect.y+1);
			::ClientToScreen(m_pCefBrowser.GetSafeHwnd(),&posSelection1);
			CPoint posSelection2(posSelection1.x+selectionRect.width-1,posSelection1.y);
			CRect rectPanelFind;
			m_pPanelFind->GetWindowRect(&rectPanelFind);
			if (rectPanelFind.PtInRect(posSelection1) || rectPanelFind.PtInRect(posSelection2))
			{
				m_pPanelFind->SetUserData(MAKELONG(selectionRect.x,selectionRect.width));
				CRect rect;
				this->GetClientRect(&rect);
				MovePanelFind(rect.Width());
				m_pPanelFind->Invalidate();
			}
		}
		m_pPanelFind->Cef_OnFindResult(count,activeMatchOrdinal,finalUpdate);
	}
}
void CDlgMsgRecord::OnFindClose(void)
{
	if (m_pPanelFind->GetUserData()!=0)// && !m_pPanelFind->IsWindowVisible())
	{
		m_pPanelFind->SetUserData(0);
		CRect rect;
		this->GetClientRect(&rect);
		MovePanelFind(rect.Width());
		if (m_pPanelFind->IsWindowVisible())
			m_pPanelFind->Invalidate();
	}
	m_pCefBrowser.StopFinding(true);
	if (!m_pPanelFind->IsWindowVisible())
		m_pCefBrowser.SetFocus();
}
void CDlgMsgRecord::OnFindText(const char* lpszFindText, bool bFindUp, bool bMatchCase, bool bFindNext)
{
	m_pCefBrowser.Find(0,libEbc::ACP2UTF8(lpszFindText).c_str(),!bFindUp,bMatchCase,bFindNext);
}
bool CDlgMsgRecord::OnExecuteMenu(int nCommandId, const wchar_t* lpszUrl)
{
	switch (nCommandId)
	{
	case CEF_CLIENT_MENU_COMMAND_ID_IMAGE_SAVEAS:
		{
			if (lpszUrl==NULL || wcslen(lpszUrl)==0) return false;

			USES_CONVERSION;
			const std::string sCopyImageUrl = W2A(lpszUrl);
			std::string::size_type find = sCopyImageUrl.rfind("/");
			if (find==std::string::npos) return false;
			std::string sFileName = sCopyImageUrl.substr(find+1);
			find = sFileName.rfind(".");
			if (find!=std::string::npos)
				sFileName = sFileName.substr(0,find);

			char szFileName[MAX_PATH*2] = {0} ; 
			sprintf(szFileName,"%s",sFileName.c_str());

			OPENFILENAME ofn={0};
			ofn.lStructSize = sizeof(OPENFILENAME);
			ofn.hwndOwner = m_hWnd;
			ofn.lpstrFile = szFileName;
			ofn.nMaxFile = sizeof(szFileName);
			ofn.lpstrFilter = "PNG Image (*.png)\0*.png\0JPEG Image (*.jpg)\0*.jpg\0BMP Image (*.bmp)\0*.bmp\0\0";
			ofn.lpstrDefExt = "png";
			ofn.Flags = OFN_OVERWRITEPROMPT|OFN_PATHMUSTEXIST;
			if (GetSaveFileName(&ofn)==TRUE ) 
			{
				CString sTempFile;
				sTempFile.Format(_T("%s\\%lld_%s"),theApp.GetAppImgTempPath(),(mycp::bigint)time(0),sFileName.c_str());
				CString sOldLastModified;
				CString sNewLastModified;
				if (!DownloadHttpFile(sCopyImageUrl.c_str(),sTempFile,sOldLastModified,sNewLastModified))
				{
					CDlgMessageBox::EbDoModal(this,"","图片另存失败：\r\n请重试！",CDlgMessageBox::IMAGE_WARNING,true,5);
					return false;
				}

				std::string sExt(ofn.lpstrFile);
				sExt = sExt.substr(sExt.size()-3);
				std::transform(sExt.begin(), sExt.end(), sExt.begin(), tolower);
				CLSID                           codecClsid;
				if (sExt=="png")
					libEbc::GetCodecClsid(L"image/png",   &codecClsid);
				else if (sExt=="bmp")
					libEbc::GetCodecClsid(L"image/bmp",   &codecClsid);
				else
					libEbc::GetCodecClsid(L"image/jpeg",   &codecClsid);

				{
					USES_CONVERSION;
					Gdiplus::Image   image(A2W(sTempFile));
					image.Save(A2W(ofn.lpstrFile), &codecClsid);
				}
				DeleteFile(sTempFile);
				return true;
			}
		}break;
	case CEF_CLIENT_MENU_COMMAND_ID_COPY_IMAGE:
		{
			if (lpszUrl==NULL || wcslen(lpszUrl)==0) return false;

			USES_CONVERSION;
			const std::string sCopyImageUrl = W2A(lpszUrl);
			//const std::string::size_type find = sCopyImageUrl.rfind(".");
			//if (find==std::string::npos) return false;
			//const std::string sExt = sCopyImageUrl.substr(find+1);
			CString sTempFile;
			sTempFile.Format(_T("%s\\%lld.ebtemp"),theApp.GetAppImgTempPath(),(mycp::bigint)time(0));
			//sTempFile.Format(_T("%s\\%lld.%s"),theApp.GetAppImgTempPath(),(mycp::bigint)time(0),sExt.c_str());

			CString sOldLastModified;
			CString sNewLastModified;
			if (DownloadHttpFile(sCopyImageUrl.c_str(),sTempFile,sOldLastModified,sNewLastModified))
			{
				if ( OpenClipboard() )
				{
					Gdiplus::Bitmap * pImage = Gdiplus::Bitmap::FromFile(A2W(sTempFile),FALSE);
					if (pImage!=NULL)
					{
						EmptyClipboard();
						// **下面支持复制 WORD QQ 等其他程序；
						PAINTSTRUCT ps;
						HDC hDC = ::BeginPaint(this->GetSafeHwnd(),&ps);
						HDC dcMem = ::CreateCompatibleDC(hDC);
						HBITMAP bitmap = ::CreateCompatibleBitmap(hDC,pImage->GetWidth(),pImage->GetHeight());
						SelectObject(dcMem,(HGDIOBJ)bitmap);
						Gdiplus::Graphics graphics(dcMem);
						graphics.DrawImage(pImage,0,0,pImage->GetWidth(),pImage->GetHeight());
						SetClipboardData(CF_BITMAP,bitmap);
						CloseClipboard();
						EndPaint(&ps);
						DeleteObject(bitmap);
						DeleteDC(dcMem);
						delete pImage;
					}else
					{
						CloseClipboard();
						CDlgMessageBox::EbDoModal(this,"","复制图片失败：\r\n请重试！",CDlgMessageBox::IMAGE_WARNING,true,5);
					}
				}
				DeleteFile(sTempFile);
			}
		}break;
	case CEF_CLIENT_MENU_COMMAND_ID_FIND:
		{
			if (m_pPanelFind==NULL) break;
			if (!m_pPanelFind->IsWindowVisible())
			{
				m_pPanelFind->ShowWindow(SW_SHOW);
			}
			m_pPanelFind->SetFindFocus();
			return true;
		}break;
	default:
		break;
	}
	return false;
}
void CDlgMsgRecord::OnStatusMessage(const wchar_t* sValue)
{
	if (sValue==NULL || wcslen(sValue)==0)
	{
		if (m_pPanelStatus->IsWindowVisible())
			m_pPanelStatus->ShowWindow(SW_HIDE);
		m_sStatusMessage.clear();
	}else
	{
		m_sStatusMessage = sValue;
		KillTimer(TIMER_HIDE_STATUS_MESSAGE);
		KillTimer(TIMER_SHOW_STATUS_MESSAGE);
		SetTimer(TIMER_SHOW_STATUS_MESSAGE,100,NULL);
	}
}

void CDlgMsgRecord::DocumentComplete(LPDISPATCH pDisp, VARIANT* URL)
{
	if (m_nBrowserType!=EB_BROWSER_TYPE_IE) return;
	if (!m_bDocumentComplete)
	{
		m_bDocumentComplete = true;
#ifdef USES_EBCOM_TEST
		const CEBString sFuncUrl = theEBClientCore->EB_GetConversationsUrl(m_sAccount,m_sGroupCode).GetBSTR();
#else
		//const CEBString sFuncUrl = "www.entboost.com";
		const CEBString sFuncUrl = theEBAppClient.EB_GetConversationsUrl(m_sAccount,m_sGroupCode);
#endif
		if (!sFuncUrl.empty())
		{
			//AfxMessageBox(sFuncUrl.c_str());
			//ShellExecute(NULL, "open", sFuncUrl.c_str(), NULL, NULL, SW_SHOW);
			m_pExplorer.Navigate(sFuncUrl.c_str(),NULL,NULL,NULL,NULL);
		}else
		{
			m_labelLineConversation.EnableWindow(FALSE);
		}
	}
}

void CDlgMsgRecord::OnBnClickedButtonMoveFirst()
{
	if (m_nRecordPages<=1)
		return;
	if (m_nCurrentPage!=0)
	{
		m_nCurrentPage = 0;
		SetTimer(TIMERID_LOAD_MSG_RECORD,10,NULL);
	}
}

void CDlgMsgRecord::OnBnClickedButtonMovePrev()
{
	if (m_nRecordPages<=1)
		return;
	if (m_nCurrentPage==-1)
		m_nCurrentPage = m_nRecordPages-2;
	else if (m_nCurrentPage>0)
		m_nCurrentPage -= 1;
	SetTimer(TIMERID_LOAD_MSG_RECORD,10,NULL);
}

void CDlgMsgRecord::OnBnClickedButtonMoveNext()
{
	if (m_nRecordPages<=1)
		return;
	if (m_nCurrentPage==-1)
		return;
	else if ((m_nCurrentPage+1)<m_nRecordPages)
	{
		m_nCurrentPage += 1;
		SetTimer(TIMERID_LOAD_MSG_RECORD,10,NULL);
	}
}

void CDlgMsgRecord::OnBnClickedButtonMoveLast()
{
	if (m_nRecordPages<=1)
		return;
	if (m_nCurrentPage==-1)
		return;
	m_nCurrentPage = -1;
	SetTimer(TIMERID_LOAD_MSG_RECORD,10,NULL);
}

void CDlgMsgRecord::OnDtnDatetimechangeDatetimepicker1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMDATETIMECHANGE pDTChange = reinterpret_cast<LPNMDATETIMECHANGE>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
	m_bSearchByDate = true;
	//if (m_nRecordPages<=1)
	//{
	//	CString sString;
	//	m_editSearch.GetWindowText(sString);
	//	m_bViewSearchResult = sString.IsEmpty()?false:true;
	//}
	SetTimer(TIMERID_LOAD_MSG_RECORD,10,NULL);
}

void CDlgMsgRecord::Update(void)
{
	m_labelViewAll.SetChecked(m_hCurrentSelect==m_labelViewAll.GetSafeHwnd());
	theApp.InvalidateParentRect(&m_labelViewAll);
	m_labelViewImage.SetChecked(m_hCurrentSelect==m_labelViewImage.GetSafeHwnd());
	theApp.InvalidateParentRect(&m_labelViewImage);
	m_labelViewFile.SetChecked(m_hCurrentSelect==m_labelViewFile.GetSafeHwnd());
	theApp.InvalidateParentRect(&m_labelViewFile);
	m_labelViewAudio.SetChecked(m_hCurrentSelect==m_labelViewAudio.GetSafeHwnd());
	theApp.InvalidateParentRect(&m_labelViewAudio);

}
void CDlgMsgRecord::OnBnClickedButtonViewAll(void)
{
	if (m_labelViewAll.GetChecked())
		return;
	m_hCurrentSelect = m_labelViewAll.GetSafeHwnd();
	Update();
	SetTimer(TIMERID_LOAD_MSG_RECORD,10,NULL);
}
void CDlgMsgRecord::OnBnClickedButtonViewImage(void)
{
	if (m_labelViewImage.GetChecked())
		return;
	m_hCurrentSelect = m_labelViewImage.GetSafeHwnd();
	Update();
	SetTimer(TIMERID_LOAD_MSG_RECORD,10,NULL);
}
void CDlgMsgRecord::OnBnClickedButtonViewFile(void)
{
	if (m_labelViewFile.GetChecked())
		return;
	m_hCurrentSelect = m_labelViewFile.GetSafeHwnd();
	Update();
	SetTimer(TIMERID_LOAD_MSG_RECORD,10,NULL);
}
void CDlgMsgRecord::OnBnClickedButtonViewAudio(void)
{
	if (m_labelViewAudio.GetChecked())
		return;
	m_hCurrentSelect = m_labelViewAudio.GetSafeHwnd();
	Update();
	SetTimer(TIMERID_LOAD_MSG_RECORD,10,NULL);
}

LRESULT CDlgMsgRecord::OnMsgOpenAppUrl(WPARAM wParam, LPARAM lParam)
{
	if (m_bChildMode)
		this->GetParent()->PostMessage(EB_COMMAND_OPEN_APP_URL, wParam, lParam);
	else
	{
		COpenAppUrlInfo * pOpenAppUrlInfo = (COpenAppUrlInfo*)wParam;
		if (pOpenAppUrlInfo==0) return 0;
		delete pOpenAppUrlInfo;
	}
	return 0;
}
LRESULT CDlgMsgRecord::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_MOVING:
	case WM_MOVE:
		{
			if (!m_bChildMode)
				NotifyMoveOrResizeStarted();
		}break;
	//case WM_KEYDOWN:
	//	{
	//		const bool bShift = (GetKeyState(VK_SHIFT)&0x80)==0x80?true:false;
	//		const bool bControl = (GetKeyState(VK_CONTROL)&0x80)==0x80?true:false;
	//		if (bShift && bControl)
	//		{
	//			if (wParam=='S')
	//			{
	//				theApp.GetMainWnd()->PostMessage(EB_COMMAND_SEARCH_SET_FOCUS, 1, 1);
	//				return TRUE;
	//			}
	//		}
	//	}break;
	case WM_ERASEBKGND:
		{
			if (m_nBrowserType==EB_BROWSER_TYPE_CEF && m_pCefBrowser.GetSafeHwnd()!=NULL)
				return 0;
			else if (m_nBrowserType==EB_BROWSER_TYPE_IE && m_pExplorer.GetSafeHwnd()!=NULL)
				return 0;
		}break;
	default:
		break;
	}
	return __super::WindowProc(message, wParam, lParam);
}
