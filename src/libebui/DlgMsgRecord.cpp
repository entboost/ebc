// DlgMsgRecord.cpp : implementation file
//

#include "stdafx.h"
#include "EBUIApp.h"
#include "DlgMsgRecord.h"
#include "Core/SkinMemDC.h"

#define LOCATE_MSG_ITEMDATA			10000
#define TIMERID_LOAD_MSG_RECORD		100
#define TIMERID_DELETE_SELECT_MSG	101
#define TIMER_REFRESH_BROWSER		102
// CDlgMsgRecord dialog

const int const_one_page_count = 50;	// 一页显示多少条

IMPLEMENT_DYNAMIC(CDlgMsgRecord, CEbDialogBase)

CDlgMsgRecord::CDlgMsgRecord(CWnd* pParent /*=NULL*/)
	: CEbDialogBase(CDlgMsgRecord::IDD, pParent)
{
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

}

CDlgMsgRecord::~CDlgMsgRecord()
{
}

void CDlgMsgRecord::DoDataExchange(CDataExchange* pDX)
{
	CEbDialogBase::DoDataExchange(pDX);
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
	ON_WM_TIMER()
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BUTTON_MIN, &CDlgMsgRecord::OnBnClickedButtonMin)
	ON_BN_CLICKED(IDC_BUTTON_MAX, &CDlgMsgRecord::OnBnClickedButtonMax)
	ON_BN_CLICKED(IDC_BUTTON_CLOSE, &CDlgMsgRecord::OnBnClickedButtonClose)
	ON_WM_PAINT()
	ON_WM_CLOSE()
	ON_WM_ERASEBKGND()
	ON_BN_CLICKED(IDC_BUTTON_DELETEALL, &CDlgMsgRecord::OnBnClickedButtonDeleteall)
	ON_EN_CHANGE(IDC_EDIT_SEARCH, &CDlgMsgRecord::OnEnChangeEditSearch)
	ON_WM_LBUTTONDBLCLK()
	ON_STN_CLICKED(IDC_STATIC_LINE_CONVERSATION, &CDlgMsgRecord::OnStnClickedStaticLineConversation)
	ON_BN_CLICKED(IDC_BUTTON_MOVE_FIRST, &CDlgMsgRecord::OnBnClickedButtonMoveFirst)
	ON_BN_CLICKED(IDC_BUTTON_MOVE_PREV, &CDlgMsgRecord::OnBnClickedButtonMovePrev)
	ON_BN_CLICKED(IDC_BUTTON_MOVE_NEXT, &CDlgMsgRecord::OnBnClickedButtonMoveNext)
	ON_BN_CLICKED(IDC_BUTTON_MOVE_LAST, &CDlgMsgRecord::OnBnClickedButtonMoveLast)
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_DATETIMEPICKER1, &CDlgMsgRecord::OnDtnDatetimechangeDatetimepicker1)
END_MESSAGE_MAP()

BEGIN_EVENTSINK_MAP(CDlgMsgRecord, CEbDialogBase)
	//{{AFX_EVENTSINK_MAP(CDlgMsgRecord)
	ON_EVENT(CDlgMsgRecord,IDC_EXPLORER1,259,DocumentComplete,VTS_DISPATCH VTS_PVARIANT)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

// CDlgMsgRecord message handlers

BOOL CDlgMsgRecord::OnInitDialog()
{
	CEbDialogBase::OnInitDialog();
	ModifyStyle(0, WS_MINIMIZEBOX|WS_MAXIMIZEBOX|WS_SYSMENU);
	this->SetSplitterBorder();
	//this->SetTransparentType(CEbDialogBase::TT_STATIC);

	this->EnableToolTips();
	this->SetToolTipText(IDC_EDIT_SEARCH,_T("搜索本地聊天记录"));
	this->SetToolTipText(IDC_BUTTON_DELETEALL,_T("清空本地聊天记录"));

	m_labelLineConversation.LoadLabel(IDB_PNG_CHECKBOX,-1,0,2);
	m_labelLineConversation.SetCheckBoxHoverBorder(TRUE,RGB(0,128,255));
	m_labelLineConversation.SetToolTipText(_T("查看漫游消息（在线聊天记录）"));
	m_btnMin.SetAutoSize(false);
	m_btnMin.Load(IDB_PNG_MIN);
	m_btnMax.SetAutoSize(false);
	m_btnMax.Load(IDB_PNG_MAX);
	m_btnClose.SetAutoSize(false);
	m_btnClose.Load(IDB_PNG_CLOSE);
	if (theApp.GetSaveConversationServer())
	{
		m_pExplorer.ShowWindow(SW_SHOW);
		m_pExplorer.MoveWindow(0,0,0,0);
		m_pExplorer.ModifyStyle(0, WS_CLIPSIBLINGS|WS_CLIPCHILDREN);
		// 先显示空白页，用于刷新修改IE控件。
		m_webUICtrl.EnableContextMenu(FALSE);
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
	if (pUnknown==NULL)
		return FALSE;
	m_pMrFrameInterface->SetBackgroundColor(theApp.GetBgColor1());
	m_pMrFrameInterface->SetLineInterval(3);
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

	m_btnDeleteAll.SetTextHotMove(false);
	m_editSearch.SetPromptText(_T("搜索本地聊天记录"));
	this->ShowWindow(SW_SHOW);

	m_tDataTime.ModifyStyle(WS_BORDER,0);
	const int const_arrow_width = 5;
	const COLORREF const_arrow_nor_color = RGB(61,61,61);
	const COLORREF const_arrow_hot_color = RGB(250,250,250);
	const COLORREF const_arrow_dis_color = RGB(100,100,100);
	m_btnMoveFirst.SetDrawPanelRgn(true);
	m_btnMoveFirst.SetWindowText(_T(""));
	m_btnMoveFirst.SetToolTipText(_T("第一页"));
	m_btnMoveFirst.SetDrawArrowPic(3,true,1,const_arrow_nor_color,const_arrow_hot_color,-1,const_arrow_dis_color,const_arrow_width);
	m_btnMovePrev.SetDrawPanelRgn(true);
	m_btnMovePrev.SetWindowText(_T(""));
	m_btnMovePrev.SetToolTipText(_T("上一页"));
	m_btnMovePrev.SetDrawArrowPic(3,false,1,const_arrow_nor_color,const_arrow_hot_color,-1,const_arrow_dis_color,const_arrow_width);
	m_btnMoveNext.SetDrawPanelRgn(true);
	m_btnMoveNext.SetWindowText(_T(""));
	m_btnMoveNext.SetToolTipText(_T("下一页"));
	m_btnMoveNext.SetDrawArrowPic(4,false,1,const_arrow_nor_color,const_arrow_hot_color,-1,const_arrow_dis_color,const_arrow_width);
	m_btnMoveLast.SetDrawPanelRgn(true);
	m_btnMoveLast.SetWindowText(_T(""));
	m_btnMoveLast.SetToolTipText(_T("最后页"));
	m_btnMoveLast.SetDrawArrowPic(4,true,1,const_arrow_nor_color,const_arrow_hot_color,-1,const_arrow_dis_color,const_arrow_width);

	m_editSearch.SetRectangleColor(theApp.GetHotColor2(),theApp.GetHotColor1());
	m_btnDeleteAll.SetDrawPanel(true,theApp.GetFuncColor(),theApp.GetHotColor(),theApp.GetNormalColor());
	m_btnMoveFirst.SetDrawPanel(true,theApp.GetFuncColor());	// 0.7
	m_btnMovePrev.SetDrawPanel(true,theApp.GetFuncColor());	// 0.7
	m_btnMoveNext.SetDrawPanel(true,theApp.GetFuncColor());	// 0.7
	m_btnMoveLast.SetDrawPanel(true,theApp.GetFuncColor());	// 0.7

	this->GetDlgItem(IDC_EDIT_SEARCH)->MoveWindow(10,52,196,22);

	//m_pMrFrame.Create(CRect(0,0,0,0),this);
	//m_pMrFrame.SetScrollBarBmp(IDB_VERTICAL_SCROLLBAR_UPARROW, IDB_VERTICAL_SCROLLBAR_SPAN, IDB_VERTICAL_SCROLLBAR_DOWNARROW,
	//	IDB_VERTICAL_SCROLLBAR_THUMB, IDB_VERTICAL_SCROLLBAR_TOP, IDB_VERTICAL_SCROLLBAR_BOTTOM );
	//m_pMrFrame.SetLineAlignment(3);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgMsgRecord::OnTimer(UINT_PTR nIDEvent)
{
	if (TIMER_REFRESH_BROWSER==nIDEvent)
	{
		KillTimer(TIMER_REFRESH_BROWSER);
		if (m_labelLineConversation.GetCheck() && m_pExplorer.GetSafeHwnd())
		{
			const long nReadyState = m_pExplorer.get_ReadyState();
			if (nReadyState == READYSTATE_COMPLETE)
			{
				m_pExplorer.Refresh();
			}
		}
	}else if (nIDEvent == TIMERID_LOAD_MSG_RECORD)
	{
		KillTimer(nIDEvent);
		static bool theSetWindowText = false;
		if (!theSetWindowText)
		{
			theSetWindowText = true;
			CString sWindowText;
			sWindowText.Format(_T("%s聊天信息"),m_sName.c_str());
			this->SetWindowText(sWindowText);
		}

		CString sSearchSQLString;
		if (m_bViewSearchResult)
		{
			CString sSearchString;
			m_editSearch.GetWindowText(sSearchString);
			if (m_nViewMsgId==0 && !sSearchString.IsEmpty())
			{
				sSearchSQLString.Format(_T(" AND msg_text LIKE '%%%s%%'"),libEbc::ACP2UTF8(sSearchString).c_str());
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
			sSql.Format(_T("select count(msg_id) FROM msg_record_t WHERE dep_code=0 AND (from_uid=%lld OR to_uid=%lld) %s ORDER BY msg_time"),m_sAccount,m_sAccount,sSearchSQLString);
		}else if (m_sGroupCode>0)
		{
			sSql.Format(_T("select count(msg_id) FROM msg_record_t WHERE dep_code=%lld %s ORDER BY msg_time"),m_sGroupCode,sSearchSQLString);
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
					sSql.Format(_T("select count(msg_id) FROM msg_record_t WHERE dep_code=0 AND (from_uid=%lld OR to_uid=%lld) %s%s"),
						m_sAccount,m_sAccount,sDateSQLString,sSearchSQLString);
				}else if (m_sGroupCode>0)
				{
					sSql.Format(_T("select count(msg_id) FROM msg_record_t WHERE dep_code=%lld %s%s"),
						m_sGroupCode,sDateSQLString,sSearchSQLString);
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
					sSql.Format(_T("select Datetime(msg_time,'localtime'),msg_id,off_time,from_uid,from_name,to_uid,to_name,private,msg_type,msg_name,msg_text FROM msg_record_t ")\
						_T("WHERE dep_code=0 AND (from_uid=%lld OR to_uid=%lld) %s ORDER BY msg_time"),m_sAccount,m_sAccount,sSearchSQLString);
				}else
				{
					sSql.Format(_T("select Datetime(msg_time,'localtime'),msg_id,off_time,from_uid,from_name,to_uid,to_name,private,msg_type,msg_name,msg_text FROM msg_record_t ")\
						_T("WHERE dep_code=0 AND (from_uid=%lld OR to_uid=%lld) ORDER BY msg_time LIMIT %d OFFSET %d"),
						m_sAccount,m_sAccount,const_one_page_count,nOffset);
				}
				LoadMsgRecord(sSql);
			}else if (m_sGroupCode>0)
			{
				if (m_bViewSearchResult)
				{
					sSql.Format(_T("select Datetime(msg_time,'localtime'),msg_id,off_time,from_uid,from_name,to_uid,to_name,private,msg_type,msg_name,msg_text FROM msg_record_t ")\
						_T("WHERE dep_code=%lld %s ORDER BY msg_time"),m_sGroupCode,sSearchSQLString);
				}else
				{
					sSql.Format(_T("select Datetime(msg_time,'localtime'),msg_id,off_time,from_uid,from_name,to_uid,to_name,private,msg_type,msg_name,msg_text FROM msg_record_t ")\
						_T("WHERE dep_code=%lld ORDER BY msg_time LIMIT %d OFFSET %d"),m_sGroupCode,const_one_page_count,nOffset);
				}
				LoadMsgRecord(sSql);
			}
		}
	}else if (TIMERID_DELETE_SELECT_MSG==nIDEvent)
	{
		KillTimer(nIDEvent);
		if (!m_pDeletMsgId.empty())
		{
			CString sText;
			sText.Format(_T("你确定删除选中的[%d]条聊天记录吗？"), m_pDeletMsgId.size());
			if (CDlgMessageBox::EbDoModal(this,"删除聊天记录：",sText,CDlgMessageBox::IMAGE_QUESTION)==IDOK)
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
	}
	CEbDialogBase::OnTimer(nIDEvent);
}

void CDlgMsgRecord::OnSize(UINT nType, int cx, int cy)
{
	CEbDialogBase::OnSize(nType, cx, cy);
	const int const_minbtn_width = m_btnMin.GetImgWidth();
	const int const_minbtn_height = m_btnMin.GetImgHeight();
	int btnx = 0;
	btnx = cx-m_btnClose.GetImgWidth()-2;
	m_btnClose.MovePoint(btnx, 2);
	btnx -= const_minbtn_width;
	m_btnMax.MovePoint(btnx, 2);
	btnx -= const_minbtn_width;
	m_btnMin.MovePoint(btnx, 2);
	btnx -= const_minbtn_width;

	int y = 52;
	if (m_editSearch.GetSafeHwnd())
	{
		m_editSearch.MoveWindow(10,y,196,22);
	}
	const int const_btn_width = 81;//m_btnDeleteAll.GetImgWidth();
	m_btnDeleteAll.MovePoint(cx-const_btn_width-10,y-3,const_btn_width,28);
	
	const int const_interval = 1;
	if (m_labelLineConversation.GetCheck())
	{
		m_pExplorer.MoveWindow(const_interval,theDefaultBgColorTitleHeight,cx-const_interval*2,cy-theDefaultBgColorTitleHeight-28);
	}
	y = 84;
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
	int x = cx-const_arraw_btn_size-4;
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

}

void CDlgMsgRecord::OnDestroy()
{
	((CEBRichMessageEventsSink*)this)->DispEventUnadvise(m_pMrFrameInterface);
	m_pMrFrameInterface.Release();
	m_pMrFrameControl.DestroyWindow();
	CEbDialogBase::OnDestroy();
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
		m_btnMax.Load(IDB_PNG_MAX);
		MoveWindow(&theRestoreRect);
	}else
	{
		this->SetToolTipText(IDC_BUTTON_MAX,_T("向下还原"));
		m_btnMax.Load(IDB_PNG_RESTORE);
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
	else
	{
		CPaintDC dc(this); // 用于绘制的设备上下文
		CRect rectClient;
		this->GetClientRect(&rectClient);
		CSkinMemDC memDC(&dc, rectClient);
		this->ClearBgDrawInfo();
		if (theApp.GetColorGradient())
			this->AddBgDrawInfo(CEbBackDrawInfo(theDefaultBgColorTitleHeight,theDefaultBgColorTitleLight1,true));
		else
			this->AddBgDrawInfo(CEbBackDrawInfo(theDefaultBgColorTitleHeight,1.0,false));
		this->AddBgDrawInfo(CEbBackDrawInfo(42,theDefaultBgColorTitleLight1,false));
		this->AddBgDrawInfo(CEbBackDrawInfo(0,theDefaultBgColorTitleLight1,false));
		this->DrawPopBg(&memDC, theApp.GetMainColor());
		Gdiplus::Graphics graphics(memDC.m_hDC);

		CString sWindowText;
		sWindowText.Format(_T("%s聊天信息"),m_sName.c_str());
		USES_CONVERSION;
		const FontFamily fontFamily(theFontFamily.c_str());
		const Gdiplus::Font fontEbTitle(&fontFamily, 15, FontStyleBold, UnitPixel);
	Gdiplus::Color colorTitle;
	colorTitle.SetFromCOLORREF(theApp.GetTitleTextColor());
	Gdiplus::SolidBrush brushEbTitle(colorTitle);
		//Gdiplus::SolidBrush brushEbTitle(Gdiplus::Color(38,38,38));
		const Gdiplus::PointF pointTitle(13,13);
		graphics.DrawString(A2W_ACP(sWindowText),-1,&fontEbTitle,pointTitle,&brushEbTitle);
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
			sWindowText.Format(_T("%s#CTRL:1:%d:%lld#%s#CTRL:1:%d:%lld#%s"),sPrivateText,(int)EB_COMMAND_CALL_USER,m_nFromUid,sFromName.c_str(),(int)EB_COMMAND_CALL_USER,nToUid,sToText);
		else
			sWindowText.Format(_T("%s#CTRL:1:%d:%lld#%s%s"),sPrivateText,(int)EB_COMMAND_CALL_USER,m_nFromUid,sFromName.c_str(),sToText);
	}else
	{
		sWindowText.Format(_T("%s%s%s"),sPrivateText,sFromName.c_str(),sToText);
	}
	const COLORREF crTextColor = (nFromUid==theApp.GetLogonUserId())?theDefaultChatTitleColor1:theDefaultChatTitleColor2;
	m_pMrFrameInterface->WriteString((LPCTSTR)sWindowText,crTextColor);
	m_pMrFrameInterface->WriteSpace(1);
	m_pMrFrameInterface->WriteTime(tMsgTime,"%H:%M:%S");

	m_pMrFrameInterface->AddLine(nMsgId);	// *** new line
	m_pMrFrameInterface->SetFrameArc(theArcOffset,thePoloygonWidth,thePoloygonHeight);
	m_pMrFrameInterface->SetFrameBorderColor(RGB(128,128,128));
	if (nFromUid==theApp.GetLogonUserId())
	{
		m_pMrFrameInterface->SetAlignmentFormat(2);
		m_pMrFrameInterface->SetFrameBackGroundColor(theApp.GetBgColor3());
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
		const tstring sMsgTime = pRecord->getVector()[0]->getStrValue();
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

		if (sSearchString.IsEmpty())
		{
			const int nMsgIdOffset = (m_nCurrentPage==-1?(m_nRecordPages-1):m_nCurrentPage)*const_one_page_count+(nIndex++);
			m_pMsgIdOffsetList.insert(sMsgId,nMsgIdOffset);
		}
		const tstring soffTime = pRecord->getVector()[2]->getStrValue();
		const eb::bigint sFromAccount = pRecord->getVector()[3]->getBigIntValue();
		const tstring sFromName(libEbc::UTF82ACP(pRecord->getVector()[4]->getStrValue().c_str()));
		const eb::bigint sToAccount = pRecord->getVector()[5]->getBigIntValue();
		const tstring sToName(libEbc::UTF82ACP(pRecord->getVector()[6]->getStrValue().c_str()));
		const int nPrivate = pRecord->getVector()[7]->getIntValue();
		const int nMsgType = pRecord->getVector()[8]->getIntValue();
		const tstring sMsgName(libEbc::UTF82ACP(pRecord->getVector()[9]->getStrValue().c_str()));
		tstring sMsgText(libEbc::UTF82ACP(pRecord->getVector()[10]->getStrValue().c_str()));
		//const int nMsgSize = pResltSet->rsvalues[i]->fieldvalues[10]->v.varcharVal.size;

		//// 兼容旧版本
		//if (sFromName.empty())
		//	sFromName = sFromAccount;
		//if (sToName.empty())
		//	sToName = sToAccount;
		WriteTitle(sMsgId,nPrivate==1,sFromAccount,sFromName,sToAccount,sToName,nMsgTime);
		CString sWindowText;
		if (MRT_UNKNOWN==nMsgType)
		{
			pRecord = theApp.m_pBoUsers->next(nCookie);
			continue;
		}else if (MRT_TEXT==nMsgType)
		{
			CSqliteCdbc::escape_string_out(sMsgText);
			m_pMrFrameInterface->WriteString(sMsgText.c_str(),theDefaultChatTextColor);
		}else if (MRT_JPG==nMsgType)
		{
			CString sTemp;
			sTemp.Format(_T("#CTRL:%d::#%s"),EB_MR_CTRL_TYPE_LDBLCLICK_OPEN,sMsgName.c_str());
			m_pMrFrameInterface->WriteImage((LPCTSTR)sTemp,0,0);
		}else if (MRT_WAV==nMsgType)
		{
			//WriteFileHICON(sMsgName.c_str());
			m_pMrFrameInterface->WriteWav("播放语音",sMsgName.c_str(),EB_MR_CTRL_TYPE_LCLICK_OPEN);
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
				CString sTemp;
				sTemp.Format(_T("#CTRL:%d::#%s"),EB_MR_CTRL_TYPE_LDBLCLICK_OPEN,sMsgText.c_str());
				m_pMrFrameInterface->WriteImage((LPCTSTR)sTemp,0,0);
			}else
			{
				WriteFileHICON(sMsgText.c_str());
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
				m_pMrFrameInterface->WriteUrl((LPCTSTR)sFileText, sMsgText.c_str(), EB_MR_CTRL_TYPE_LDBLCLICK_OPEN,RGB(60,60,60));
				//m_pMrFrameInterface->WriteUrl(sFileName.c_str(), sMsgText.c_str(), EB_MR_CTRL_TYPE_LDBLCLICK_OPEN,RGB(60,60,60));
			}
			m_pMrFrameInterface->WriteLine();
			m_pMrFrameInterface->WriteSpace(11);
			if (nFileSize>=0)
			{
				m_pMrFrameInterface->WriteOpenFile(L"打开",sMsgText.c_str());
				m_pMrFrameInterface->WriteSpace(2);
			}
			m_pMrFrameInterface->WriteOpenDir(L"打开文件夹",sMsgText.c_str());
		}
		pRecord = theApp.m_pBoUsers->next(nCookie);
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
	//ShowWindow(SW_SHOW);
	SetForegroundWindow();
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
	if (m_sAccount>0)
		theApp.m_pMsgRecord.remove(m_sAccount);
	if (m_sGroupCode>0)
		theApp.m_pMsgRecord.remove(m_sGroupCode);
	theApp.m_pCloseWnd.add(this);
	this->ShowWindow(SW_HIDE);
	//CEbDialogBase::OnClose();
	//this->EndDialog(IDOK);
}

BOOL CDlgMsgRecord::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default
	return TRUE;
	return CEbDialogBase::OnEraseBkgnd(pDC);
}

void CDlgMsgRecord::OnBnClickedButtonDeleteall()
{
	bool bIsAccount = m_sAccount>0;
	eb::bigint sId = bIsAccount?m_sAccount:m_sGroupCode;
	CString sText;
	if (bIsAccount)
		sText.Format(_T("你确定清空[%s(%lld)]聊天记录吗？"), m_sName.c_str(),sId);
	else
		sText.Format(_T("你确定清空[%s]聊天记录吗？"), m_sName.c_str());	// 群组（部门）不需要显示group_code
	if (CDlgMessageBox::EbDoModal(this,"清空聊天记录：",sText,CDlgMessageBox::IMAGE_QUESTION)==IDOK)
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
		m_bViewSearchResult = sString.IsEmpty()?false:true;
		SetTimer(TIMERID_LOAD_MSG_RECORD,10,NULL);
	}
	//if (!sSearchString.IsEmpty())
	//{
	//	// ???

	//}
}

void CDlgMsgRecord::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	OnBnClickedButtonMax();
	CEbDialogBase::OnLButtonDblClk(nFlags, point);
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
		theApp.OpenSubscribeFuncWindow(pSubscribeFuncInfo,libEbc::ACP2UTF8(sSelectString.c_str()));
	}
}
void CDlgMsgRecord::Fire_onItemLBtnClick(LONG nLineId, LONG nItemId, ULONG nItemData, const CEBString& sParamString, LONGLONG nSelectMsgId)
{
	switch (nItemData)
	{
	case EB_COMMAND_CALL_USER:
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
//	case EB_COMMAND_CALL_USER:
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
void CDlgMsgRecord::OnCancel()
{
	// TODO: Add your specialized code here and/or call the base class
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
		m_pExplorer.MoveWindow(const_interval,theDefaultBgColorTitleHeight,rect.Width()-const_interval*2,rect.Height()-theDefaultBgColorTitleHeight-24);
	}else
	{
		m_editSearch.ShowWindow(SW_SHOW);
		m_btnDeleteAll.ShowWindow(SW_SHOW);
		m_pExplorer.MoveWindow(0,0,0,0);
		m_tDataTime.ShowWindow(SW_SHOW);
		m_btnMoveFirst.ShowWindow(SW_SHOW);
		m_btnMovePrev.ShowWindow(SW_SHOW);
		m_btnMoveNext.ShowWindow(SW_SHOW);
		m_btnMoveLast.ShowWindow(SW_SHOW);
	}
}

void CDlgMsgRecord::DocumentComplete(LPDISPATCH pDisp, VARIANT* URL)
{
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
