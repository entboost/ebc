// DlgDiaRecord.cpp : implementation file
//

#include "stdafx.h"
#include "POP.h"
#include "DlgDiaRecord.h"


// CDlgDiaRecord dialog

IMPLEMENT_DYNAMIC(CDlgDiaRecord, CDialog)

CDlgDiaRecord::CDlgDiaRecord(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgDiaRecord::IDD, pParent)
	, m_sCallNumber(_T(""))
{

}

CDlgDiaRecord::~CDlgDiaRecord()
{
}

void CDlgDiaRecord::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_NUMBER, m_sCallNumber);
	DDX_Control(pDX, IDC_LIST_NUMBERS, m_listNumbers);
	DDX_Control(pDX, IDOK, m_btnOk);
}


BEGIN_MESSAGE_MAP(CDlgDiaRecord, CDialog)
	ON_BN_CLICKED(IDOK, &CDlgDiaRecord::OnBnClickedOk)
	ON_LBN_DBLCLK(IDC_LIST_NUMBERS, &CDlgDiaRecord::OnLbnDblclkListNumbers)
	ON_LBN_SELCHANGE(IDC_LIST_NUMBERS, &CDlgDiaRecord::OnLbnSelchangeListNumbers)
	ON_EN_CHANGE(IDC_EDIT_NUMBER, &CDlgDiaRecord::OnEnChangeEditNumber)
END_MESSAGE_MAP()


// CDlgDiaRecord message handlers

void CDlgDiaRecord::OnBnClickedOk()
{
	UpdateData(TRUE);
	if (m_sCallNumber.IsEmpty())
	{
		this->GetDlgItem(IDC_EDIT_NUMBER)->SetFocus();
		return;
	}
	OnOK();
}

void CDlgDiaRecord::OnLbnDblclkListNumbers()
{
	// TODO: Add your control notification handler code here
	int ncursel = m_listNumbers.GetCurSel();
	if (ncursel >= 0)
	{
		m_listNumbers.GetText(ncursel, m_sCallNumber);
		OnOK();
	}
}

void CDlgDiaRecord::OnLbnSelchangeListNumbers()
{
	int ncursel = m_listNumbers.GetCurSel();
	if (ncursel >= 0)
	{
		m_listNumbers.GetText(ncursel, m_sCallNumber);
		UpdateData(FALSE);
	}
}

BOOL CDlgDiaRecord::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_btnOk.Load(IDB_PNG_BTN);
	m_btnOk.SetAutoSize(false);

	//m_btnOk.SetToolTipText(_T(""));
/*
	bo::PRESULTSET pResltSet = NULL;
	theApp.m_pBoUsers->execsql("select begintime,endtime,type,status,seconds,fromaccount,dep_code,text from user_ses_record_t order by endtime desc", &pResltSet);
	if (pResltSet != NULL)
	{
		for (int i=0; i<pResltSet->rscount; i++)
		{
			time_t tBeginTime = pResltSet->rsvalues[i]->fieldvalues[0]->v.timestampVal.time;
			time_t tEndTime = pResltSet->rsvalues[i]->fieldvalues[1]->v.timestampVal.time;
			short nType = pResltSet->rsvalues[i]->fieldvalues[2]->v.tinyintVal;
			short nStatus = pResltSet->rsvalues[i]->fieldvalues[3]->v.tinyintVal;
			unsigned int nSeconds = pResltSet->rsvalues[i]->fieldvalues[4]->v.uintegerVal;
			tstring sAccount = pResltSet->rsvalues[i]->fieldvalues[5]->v.varcharVal.buffer;
			tstring sDepCode = pResltSet->rsvalues[i]->fieldvalues[6]->v.varcharVal.buffer;
			tstring sText = pResltSet->rsvalues[i]->fieldvalues[7]->v.varcharVal.size>0?pResltSet->rsvalues[i]->fieldvalues[7]->v.varcharVal.buffer:"";
			CUserDiaInfo pUserDiaInfo(tBeginTime, tEndTime, (CUserDiaInfo::DiaType)nType,(CUserDiaInfo::DiaStatus)nStatus, nSeconds, sAccount);
			pUserDiaInfo.m_sText = sText;
			m_pDiaList.push_back(pUserDiaInfo);
			m_listNumbers.AddString(sAccount.c_str());
		}
		bo::bodb_free(pResltSet);
	}
	*/
	if (m_listNumbers.GetCount() == 0)
	{
		m_listNumbers.AddString(_T("popuser2@192.168.0.208"));
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgDiaRecord::OnEnChangeEditNumber()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	//int i=0;
}
