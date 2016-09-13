// BroadcastView.cpp : implementation file
//

#include "stdafx.h"
#include "POP.h"
#include "BroadcastView.h"


// CBroadcastView

IMPLEMENT_DYNAMIC(CBroadcastView, CHTMLWnd)
CBroadcastView::CBroadcastView(CBroadcastViewCallback* pCallback)
: m_pCallback(pCallback)
{
}

CBroadcastView::~CBroadcastView()
{
}


BEGIN_MESSAGE_MAP(CBroadcastView, CHTMLWnd)
	//ON_COMMAND(ID_MENU_COPY_COPY, OnMenuCopyCopy)
END_MESSAGE_MAP()


// CBroadcastView message handlers
void CBroadcastView::OnBeforeNavigate(VARIANT_BOOL* bCancel,LPCTSTR szURL)
{
	if (m_pCallback)
		m_pCallback->OnBeforeNavigate(bCancel,szURL);
	else
		*bCancel = VARIANT_FALSE;
}

//void CBroadcastView::SetOverflowHidden(bool bHidden)
//{
//	m_bOverflowHidden = bHidden;
//}
//
//void CBroadcastView::SetScallBarZoom(bool bZoom)
//{
//	m_bAddScallBarZoom = bZoom;
//}

void CBroadcastView::OnBeforeCreateBody()
{
	//AfxMessageBox(_T("CBroadcastView::OnBeforeCreateBody"));

	//CString strPath;
	CString str;
	{
		// 
		//str = _T("<HEAD>\r\n")\
		//	_T("<meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\">\r\n")\
		//	_T("<style type=\"text/css\"> \r\n")\
		//	_T("body{ \r\n");
		//str += _T("border-style: none;  \r\n");
		////str += _T("BORDER-LEFT-STYLE: none;  \r\n")\
		////	_T("BORDER-RIGHT-STYLE: none; \r\n")\
		////	_T("BORDER-TOP-STYLE: none;  \r\n")\
		////	_T("BORDER-BOTTOM-STYLE: none;  \r\n")\
		////	_T("BORDER-STYLE: none;  \r\n");
		////str += _T("MARGIN: 0px;  \r\n");
		//////if (m_bOverflowHidden)
		////	str += _T("overflow:hidden;");
		//str += _T("} \r\n") \
		//	_T("</style> \r\n")\
		//	_T("</HEAD><BODY></BODY>\r\n");
		//	AfxMessageBox(str);

//			_T("<META content=\"MSHTML 6.00.3790.2440\" name=GENERATOR>\r\n") 

		str = _T("<HEAD>\r\n")\
			_T("<meta http-equiv=\"content-type\" content=\"text/html; charset=utf-8\">\r\n")\
			_T("<style> \r\n")\
			_T("<!-- \r\n")\
			_T(".popups { \r\n")\
			_T("	BORDER-RIGHT: black 1px solid; PADDING-RIGHT: 5px; BORDER-TOP: black 1px solid; DISPLAY: none; PADDING-LEFT: 5px; BACKGROUND: #eee; PADDING-BOTTOM: 5px; BORDER-LEFT: black 1px solid; WIDTH: 110px; PADDING-TOP: 5px; BORDER-BOTTOM: black 1px solid; POSITION: absolute; HEIGHT: 90px \r\n")\
			_T("} \r\n")\
			_T("input, td, body, p { \r\n")\
			_T("FONT-SIZE: 14px; \r\n")\
			_T("line-height:20px;font-family: \"Arial\", \"Verdana\", \"Helvetica\", \"sans-serif\";\r\n")\
			_T("} \r\n")\
			_T("Body{ \r\n")\
			_T("Scrollbar-Face-Color:#F4F7FA; \r\n");
		str += _T("BORDER-LEFT-STYLE: none;  \r\n")\
			_T("BORDER-RIGHT-STYLE: none; \r\n")\
			_T("BORDER-TOP-STYLE: none;  \r\n")\
			_T("BORDER-BOTTOM-STYLE: none;  \r\n");
		//str += _T("MARGIN: 0px;  \r\n")\
		//	_T("Scrollbar-Highlight-Color:#F4F7FA; \r\n")\
		//	_T("Scrollbar-Shadow-Color:#BAC7DB; \r\n")\
		//	_T("Scrollbar-3Dlight-Color:#BAC7DB; \r\n")\
		//	_T("Scrollbar-Darkshadow-Color:#F4F7FA; \r\n")\
		//	_T("Scrollbar-Arrow-Color:#BAC7DB; \r\n")\
		//	_T("Scrollbar-Track-Color:#FFFFFF;\r\n")\
		//	_T("background-color:#FFFFFF; \r\n");
		//if (m_bOverflowHidden)
			str += _T("overflow:hidden;");
		str += _T("} \r\n") \
			_T("a:link { \r\n")\
			_T("text-decoration: none; \r\n")\
			_T("color: #0066CC; \r\n")\
			_T("} \r\n")\
			_T("a:visited { \r\n")\
			_T("text-decoration: none;\r\n")\
			_T("color: #0066CC;\r\n")\
			_T("}\r\n")\
			_T("-->\r\n")\
			_T("</style> \r\n") \
			_T("\r\n</HEAD> \r\n");
		//if (m_bAddScallBarZoom)
		//	str += _T("<body style=\"zoom:0.02\">");
	}
	WriteHTML(str,FALSE);

	//body
	//str.Format(_T("<BODY background=\"%s\" bgproperties=\"fixed\"></BODY>"), strPath);
	if (!m_strBackground.IsEmpty())
	{
		str.Format(_T("<BODY background=\"%s\" ></BODY>"), m_strBackground);
		WriteHTML(str,FALSE);
	}else if (!m_strBgColor.IsEmpty())
	{
		str.Format(_T("<BODY bgcolor=\"%s\" ></BODY>"), m_strBgColor);
		//str.Format(_T("<BODY bgcolor=\"%s\" >%s,%s</BODY></HTML>"), m_strBgColor,"≤‚ ‘",libEbc::ACP2UTF8("≤‚ ‘").c_str());
		WriteHTML(str,FALSE);
	}else
		WriteHTML(_T("<BODY></BODY>"),FALSE);
}
