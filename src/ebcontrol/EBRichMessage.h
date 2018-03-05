// EBRichMessage.h : Declaration of the CEBRichMessage
#pragma once
#include "resource.h"       // main symbols
#include <atlctl.h>
#include "ebcontrol_i.h"

#include "core/MrLine.h"
#include <stl/locklist.h>
#include <stl/lockmap.h>
#include "_IEBRichMessageEvents_CP.H"


BOOL CALLBACK MyEnumFonts(CONST LOGFONTW* lplf, CONST TEXTMETRICW *lptm,DWORD dwType,LPARAM lparam)
{
	const wchar_t * lpszFine = (const wchar_t*)lparam;
	const std::wstring tempFontName(lplf->lfFaceName);
	if(tempFontName.find(lpszFine)!=std::wstring::npos)
	{
		theFontFamily = lpszFine;
		return false;
	}
	return true;
}

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

const int const_vbar_width = 11;
#define TIMERID_CLICK			101
#define TIMERID_SELECT			102
#define TIMERID_DRAW_GIF_IMAGE	103
#define TIMERID_CHECK_LEAVE		104

#define MENU_ID_COPY				1100
#define MENU_ID_DELETE				1101
#define MENU_ID_SELALL				1102
#define MENU_ID_CLEANALL			1103
#define MENU_ID_SAVEAS				1104
#define MENU_ID_USER_MENU_ITEM_F1	1110
#define MENU_ID_USER_MENU_ITEM_L1	1130
#define MENU_ID_USER_MENU_ITEM_F2	1150
#define MENU_ID_USER_MENU_ITEM_L2	1170
#define MENU_ID_MODE0				1200
#define MENU_ID_MODE1				1201
#define MENU_ID_FONT_SIZE0			1210	// 默认大小字体
#define MENU_ID_FONT_SIZE1			1211	// 调大显示字体
#define MENU_ID_FONT_SIZE2			1212	// 调小显示字体
#define MENU_ID_RECEIPT0				1220	// 显示未接收回执 0x0
#define MENU_ID_RECEIPT1				1221	// 显示未接收回执 0x1
#define MENU_ID_RECEIPT2				1222	// 显示已接收回执	0x2

#define RECEIPT_HIDE_ALL						0
#define RECEIPT_SHOW_UNRECEIVE_ONLY 1
#define RECEIPT_SHOW_ALL						2

//#define CONTEXTMENU_SELECT_ITEMDATA_CLEAN_ALL		10000

// CEBRichMessage
class ATL_NO_VTABLE CEBRichMessage :
	public CComObjectRootEx<CComSingleThreadModel>,
	public IDispatchImpl<IEBRichMessage, &IID_IEBRichMessage, &LIBID_ebcontrolLib, /*wMajor =*/ 1, /*wMinor =*/ 0>,
	public IPersistStreamInitImpl<CEBRichMessage>,
	public IOleControlImpl<CEBRichMessage>,
	public IOleObjectImpl<CEBRichMessage>,
	public IOleInPlaceActiveObjectImpl<CEBRichMessage>,
	public IViewObjectExImpl<CEBRichMessage>,
	public IOleInPlaceObjectWindowlessImpl<CEBRichMessage>,
	public ISupportErrorInfo,
	public IPersistStorageImpl<CEBRichMessage>,
	public ISpecifyPropertyPagesImpl<CEBRichMessage>,
	public IQuickActivateImpl<CEBRichMessage>,
#ifndef _WIN32_WCE
	public IDataObjectImpl<CEBRichMessage>,
#endif
	public IProvideClassInfo2Impl<&CLSID_EBRichMessage, NULL, &LIBID_ebcontrolLib>,
#ifdef _WIN32_WCE // IObjectSafety is required on Windows CE for the control to be loaded correctly
	public IObjectSafetyImpl<CEBRichMessage, INTERFACESAFE_FOR_UNTRUSTED_CALLER>,
#endif
	public CComCoClass<CEBRichMessage, &CLSID_EBRichMessage>,
	public CComControl<CEBRichMessage>,
	public IEBSkinVBarCallback,
	public IConnectionPointContainerImpl<CEBRichMessage>,
	public CProxy_IEBRichMessageEvents<CEBRichMessage>,
	public IDispatchImpl<_IEBRichMessageEvents, &__uuidof(_IEBRichMessageEvents), &LIBID_ebcontrolLib, /* wMajor = */ 1, /* wMinor = */ 0>
	//public IDispatchImpl<_IEBRichMessageEvents, &__uuidof(_IEBRichMessageEvents), &LIBID_ebcontrolLib, /* wMajor = */ 1, /* wMinor = */ 0>
{
public:
	ULONG_PTR theGdiplusToken;
	bool m_bMouseDown;
	Point m_pSelectBegin;
	Point m_pSelectEnd;

	SHORT m_nLineInterval;	// 行间隔
	Gdiplus::SolidBrush m_brushBackground;
	Gdiplus::Color m_colorSelected;

	CLockList<CMrLine::pointer> m_pList;
	CMrLine::pointer m_pCurrentLine;
	CMrLine::pointer m_pMoveEnterLine;
	CMrItem::pointer m_pMoveEnterItem;
	bool m_bEnterInFocus;
	ULONG m_nSelectStringCbData1;
	ULONG m_nDefaultUrlCbData1;
	bool m_bDisableReceipt;

	// 实现滚动条
	CAxWindow m_pSkinVBar;
	CComPtr<IEBSkinVBar> m_pSkinVBarInterface;
	int m_nScrollMax;
	int m_nScrollPos;

	//CMenu m_menuContext;
	HMENU m_hContextMenu;
	HMENU m_hMenu1;	// 聊天显示格式
	HMENU m_hMenu2;	// 消息回执标识
	int m_nMsgReceipt;
	class CContextMenuItemInfo
	{
	public:
		typedef boost::shared_ptr<CContextMenuItemInfo> pointer;
		static CContextMenuItemInfo::pointer create(const std::wstring& sItemName, ULONG nItemData)
		{
			return CContextMenuItemInfo::pointer(new CContextMenuItemInfo(sItemName,nItemData));
		}
		std::wstring m_sItemName;
		ULONG m_nItemData;

		CContextMenuItemInfo(void)
			: m_nItemData(0)
		{
		}
		CContextMenuItemInfo(const std::wstring& sItemName, ULONG nItemData)
			: m_sItemName(sItemName),m_nItemData(nItemData)
		{
		}
	};
	std::vector<CContextMenuItemInfo::pointer> m_pContextMenuItem1;	// single select
	std::vector<CContextMenuItemInfo::pointer> m_pContextMenuItem2;
	std::string m_sAppPath;			// 带'\\'
	std::string m_sRichSettingPath;
	int m_nFontSize;
	CEBRichMessage()
		: m_brushBackground(Gdiplus::Color(250,250,250))
		, m_colorSelected(255,128,0)
		, m_nFontSize(const_font_size)
		, m_nNextLineId(0)
	{
		//m_bIsTrackPopupMenu = false;
		m_bWindowOnly = TRUE;
		theGdiplusToken = 0;

		m_bEnterInFocus = false;
		m_nSelectStringCbData1 = 0;
		m_nDefaultUrlCbData1 = 0;
		m_bDisableReceipt = false;

		m_bMouseDown = false;
		m_nLineInterval = 1;

		m_nScrollMax = 0;
		m_nScrollPos = 0;
		m_hContextMenu = NULL;
		m_hMenu1 = NULL;
		m_hMenu2 = NULL;
		//m_nMsgReceipt = -1;
		m_bReOnSize = false;

		char lpszPath[MAX_PATH+1];
		GetModuleFileNameA( NULL, lpszPath, MAX_PATH);
		std::string sPath(lpszPath);
		const std::string::size_type find = sPath.rfind('\\');
		if (find!=std::string::npos)
		{
			m_sAppPath = sPath.substr(0,find+1);
		}
		m_sRichSettingPath = m_sAppPath+"datas\\setting.ini";
		if (!::PathFileExistsA(m_sRichSettingPath.c_str()))
		{
			m_sRichSettingPath = m_sAppPath+"setting.ini";
		}
		m_nFontSize = GetPrivateProfileIntA("ebrichmessage","font-size",const_font_size,m_sRichSettingPath.c_str());
		m_nMsgReceipt = GetPrivateProfileIntA("ebrichmessage","msg-receipt",(int)RECEIPT_SHOW_ALL,m_sRichSettingPath.c_str());
	}

	DECLARE_OLEMISC_STATUS(OLEMISC_RECOMPOSEONRESIZE |
	OLEMISC_CANTLINKINSIDE |
		OLEMISC_INSIDEOUT |
		OLEMISC_ACTIVATEWHENVISIBLE |
		OLEMISC_SETCLIENTSITEFIRST
		)

		DECLARE_REGISTRY_RESOURCEID(IDR_EBRICHMESSAGE)


	BEGIN_COM_MAP(CEBRichMessage)
		COM_INTERFACE_ENTRY(IEBRichMessage)
		COM_INTERFACE_ENTRY(IEBSkinVBarCallback)
		//COM_INTERFACE_ENTRY2(IDispatch, _IEBRichMessageEvents)
		COM_INTERFACE_ENTRY(IViewObjectEx)
		COM_INTERFACE_ENTRY(IViewObject2)
		COM_INTERFACE_ENTRY(IViewObject)
		COM_INTERFACE_ENTRY(IOleInPlaceObjectWindowless)
		COM_INTERFACE_ENTRY(IOleInPlaceObject)
		COM_INTERFACE_ENTRY2(IOleWindow, IOleInPlaceObjectWindowless)
		COM_INTERFACE_ENTRY(IOleInPlaceActiveObject)
		COM_INTERFACE_ENTRY(IOleControl)
		COM_INTERFACE_ENTRY(IOleObject)
		COM_INTERFACE_ENTRY(IPersistStreamInit)
		COM_INTERFACE_ENTRY2(IPersist, IPersistStreamInit)
		COM_INTERFACE_ENTRY(ISupportErrorInfo)
		COM_INTERFACE_ENTRY(ISpecifyPropertyPages)
		COM_INTERFACE_ENTRY(IQuickActivate)
		COM_INTERFACE_ENTRY(IPersistStorage)
#ifndef _WIN32_WCE
		COM_INTERFACE_ENTRY(IDataObject)
#endif
		COM_INTERFACE_ENTRY(IProvideClassInfo)
		COM_INTERFACE_ENTRY(IProvideClassInfo2)
#ifdef _WIN32_WCE // IObjectSafety is required on Windows CE for the control to be loaded correctly
		COM_INTERFACE_ENTRY_IID(IID_IObjectSafety, IObjectSafety)
#endif
		//COM_INTERFACE_ENTRY(_IEBRichMessageEvents)
		COM_INTERFACE_ENTRY(IConnectionPointContainer)
		COM_INTERFACE_ENTRY(_IEBRichMessageEvents)
	END_COM_MAP()

	BEGIN_PROP_MAP(CEBRichMessage)
		PROP_DATA_ENTRY("_cx", m_sizeExtent.cx, VT_UI4)
		PROP_DATA_ENTRY("_cy", m_sizeExtent.cy, VT_UI4)
		// Example entries
		// PROP_ENTRY_TYPE("Property Name", dispid, clsid, vtType)
		// PROP_PAGE(CLSID_StockColorPage)
	END_PROP_MAP()

	void PositionScrollBars(void);
	bool m_bReOnSize;
	void OnSize(int cx, int cy);

	BEGIN_MSG_MAP(CEBRichMessage)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(WM_TIMER, OnTimer)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkgnd)
		MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
		MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
		MESSAGE_HANDLER(WM_MOUSEWHEEL, OnMouseWheel)
		MESSAGE_HANDLER(WM_LBUTTONUP, OnLButtonUp)
		MESSAGE_HANDLER(WM_KEYDOWN, OnKeyDown)
		MESSAGE_HANDLER(WM_SETCURSOR, OnSetCursor)
		MESSAGE_HANDLER(WM_CONTEXTMENU, OnContextMenu)
		MESSAGE_HANDLER(WM_LBUTTONDBLCLK, OnLButtonDblClk)
		MESSAGE_HANDLER(WM_RBUTTONDOWN, OnRButtonDown)
		//NOTIFY_HANDLER(0,TTN_NEEDTEXT,OnTipNotify)
		//MESSAGE_HANDLER(WM_KILLFOCUS, OnKillFocus)
		CHAIN_MSG_MAP(CComControl<CEBRichMessage>)

		COMMAND_ID_HANDLER(MENU_ID_COPY,OnCopy)
		COMMAND_ID_HANDLER(MENU_ID_DELETE,OnDelete)
		COMMAND_ID_HANDLER(MENU_ID_SELALL,OnSelectAll)
		COMMAND_ID_HANDLER(MENU_ID_CLEANALL,OnCleanAll)
		COMMAND_ID_HANDLER(MENU_ID_SAVEAS,OnSaveAs)
		COMMAND_RANGE_HANDLER(MENU_ID_USER_MENU_ITEM_F1,MENU_ID_USER_MENU_ITEM_L1,OnUserMenuSelect1)
		COMMAND_RANGE_HANDLER(MENU_ID_USER_MENU_ITEM_F2,MENU_ID_USER_MENU_ITEM_L2,OnUserMenuSelect2)
		//ON_COMMAND(EB_COMMAND_STATE_ONLINE, &CPOPDlg::OnStateOnline)
		COMMAND_ID_HANDLER(MENU_ID_MODE0,OnMode0)
		COMMAND_ID_HANDLER(MENU_ID_MODE1,OnMode1)
		COMMAND_ID_HANDLER(MENU_ID_FONT_SIZE0,OnFontSize0)
		COMMAND_ID_HANDLER(MENU_ID_FONT_SIZE1,OnFontSize1)
		COMMAND_ID_HANDLER(MENU_ID_FONT_SIZE2,OnFontSize2)
		COMMAND_ID_HANDLER(MENU_ID_RECEIPT0,OnReceipt0)
		COMMAND_ID_HANDLER(MENU_ID_RECEIPT1,OnReceipt1)
		COMMAND_ID_HANDLER(MENU_ID_RECEIPT2,OnReceipt2)


		DEFAULT_REFLECTION_HANDLER()
	END_MSG_MAP()
	// Handler prototypes:
	//  LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	//  LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	//  LRESULT NotifyHandler(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);

	// ISupportsErrorInfo
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid)
	{
		static const IID* arr[] =
		{
			&IID_IEBRichMessage,
		};

		for (int i=0; i<sizeof(arr)/sizeof(arr[0]); i++)
		{
			if (InlineIsEqualGUID(*arr[i], riid))
				return S_OK;
		}
		return S_FALSE;
	}

	// IViewObjectEx
	DECLARE_VIEW_STATUS(VIEWSTATUS_SOLIDBKGND | VIEWSTATUS_OPAQUE)

	// IEBRichMessage
public:
	HRESULT OnDraw(ATL_DRAWINFO& di)
	{
		RECT& rc = *(RECT*)di.prcBounds;
		//int cx, cy;
		//cx = rc.right - rc.left;
		//cy = rc.bottom - rc.top;
		//OnSize(cx-const_vbar_width,cy);

		//enum TextRenderingHint{  
		//  TextRenderingHintSystemDefault = 0,//使用与系统相同的处理方式  
		//  TextRenderingHintSingleBitPerPixelGridFit,//不消除锯齿，使用网格匹配  
		//  TextRenderingHintSingleBitPerPixel,//不消除锯齿，不使用网格匹配  
		//  TextRenderingHintAntiAliasGridFit,//消除锯齿，使用网格匹配  
		//  TextRenderingHintAntiAlias,//消除锯齿，不使用网格匹配  
		//  TextRenderingHintClearTypeGridFit //在液晶显示器上使用ClearType技术增强字体清晰度  
		//};  
		HDC hDC = di.hdcDraw;
		{
			CSkinMemoryDC memDC(hDC,rc);
			Gdiplus::Graphics graphics(memDC.GetDC());
			//graphics.SetSmoothingMode( SmoothingModeAntiAlias );  
			//graphics.SetTextRenderingHint(TextRenderingHintAntiAlias);	// 使用这个基本计算会准，但字体有点粗
			graphics.SetTextRenderingHint(TextRenderingHintAntiAliasGridFit);
			graphics.FillRectangle(&m_brushBackground,memDC.GetLeft(),memDC.GetTop(),memDC.Width(),memDC.Height());
			RectF layoutRect(0,m_nScrollPos,memDC.Width(),memDC.Height());
			//graphics.SetClip(Rect(0,0,memDC.Width(),memDC.Height()));

			BoostReadLock rdlock(m_pList.mutex());
			CLockList<CMrLine::pointer>::iterator pIter = m_pList.begin();
			for (;pIter!=m_pList.end();pIter++)
			{
				const CMrLine::pointer& pLine = *pIter;
				pLine->OnDraw(graphics, layoutRect);
			}
		}
		if (m_nScrollMax > 0 && m_pSkinVBarInterface != NULL)
			m_pSkinVBarInterface->UpdatePosition();

		return S_OK;
	}

	bool IsSingleSelected(mr::bigint* pOutMsgId) const;
	bool IsImgClicked(CMrItem::pointer& pOutImgItem) const;
	int IsSelected(void) const;	// 0:没有选择；1单选；2：多选
	bool IsSelected(bool bCheckMsgId) const;
	CMrItem::pointer m_pClickImgItem;
	void Copy(void);
	void Delete(void);
	void SelectAll(void);
	void SaveAs(void);
	//bool m_bIsTrackPopupMenu;

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		PAINTSTRUCT ps;
		HDC hDC = BeginPaint(&ps);
		::EnumFontsW(hDC,NULL,(FONTENUMPROCW)MyEnumFonts,(LPARAM)theFontFamily1.c_str());	// 
		::EnumFontsW(hDC,NULL,(FONTENUMPROCW)MyEnumFonts,(LPARAM)theFontFamily0.c_str());
		EndPaint(&ps);

		RECT rc;
		GetWindowRect(&rc);
		rc.right -= rc.left;
		rc.bottom -= rc.top;
		rc.top = rc.left = 0;
		m_pSkinVBar.Create(m_hWnd,CRect(0,0,0,0),L"ebcontrol.EBSkinVBar",WS_CHILD);
		m_pSkinVBar.QueryControl(IID_IEBSkinVBar,(void**)&m_pSkinVBarInterface);
		if (m_pSkinVBarInterface!=NULL)
		{
			IEBSkinVBarCallback * pCallback = this;
			m_pSkinVBarInterface->SetCallback(pCallback);
		}
		SetTimer(TIMERID_DRAW_GIF_IMAGE,50,NULL);
		return 0;
	}
	STDMETHOD(SetObjectRects)(LPCRECT prcPos,LPCRECT prcClip)
	{
		IOleInPlaceObjectWindowlessImpl<CEBRichMessage>::SetObjectRects(prcPos, prcClip);
		int cx, cy;
		cx = prcPos->right - prcPos->left;
		cy = prcPos->bottom - prcPos->top;
		OnSize(cx, cy);
		if (cx > 0 && cy > 0)
			PositionScrollBars();
		return S_OK;
	}

	HRESULT FinalConstruct()
	{
		Gdiplus::GdiplusStartupInput gdiplusStartupInput;
		Gdiplus::GdiplusStartup(&theGdiplusToken, &gdiplusStartupInput, NULL);
		return S_OK;
	}

	void FinalRelease()
	{
		if (m_hContextMenu!=NULL)
		{
			DestroyMenu(m_hContextMenu);
			m_hContextMenu = NULL;
		}
		if (m_hMenu1!=NULL)
		{
			DestroyMenu(m_hMenu1);
			m_hMenu1 = NULL;
		}
		if (m_hMenu2!=NULL)
		{
			DestroyMenu(m_hMenu2);
			m_hMenu2 = NULL;
		}
		Clear();
		Gdiplus::GdiplusShutdown(theGdiplusToken);
	}
	// IEBSkinVBarCallback
	STDMETHOD(GetScrollMax)(LONG* pOutMax);
	STDMETHOD(GetScrollMin)(LONG* pOutMin);
	STDMETHOD(GetScrollPos)(LONG* pOutPos);
	STDMETHOD(ScrollPageUp)(void);
	STDMETHOD(ScrollPageDown)(void);
	STDMETHOD(ScrollLineUp)(void);
	STDMETHOD(ScrollLineDown)(void);
	STDMETHOD(ScrollBy)(LONG nScrollTimes);
	HRESULT ScrollBy(LONG nScrollTimes, LONG& pOutScrollTimes);
	STDMETHOD(ScrollTo)(LONG nScrollPos);
	STDMETHOD(OnVBarKillFocus)(void);

	STDMETHOD(SetLineInterval)(SHORT nLineInterval);
	STDMETHOD(SetBackgroundColor)(ULONG colorRef);
	STDMETHOD(SetSelectedColor)(ULONG colorRef);
	STDMETHOD(SetAlignmentFormat)(SHORT nAlignmentFormat);
	STDMETHOD(SetAlignmentValue)(SHORT nAlignment1, SHORT nAlignment2);
	STDMETHOD(UpdateSize)(VARIANT_BOOL bRepaint);
	STDMETHOD(ScrollToPos)(LONG nScrollPos);
	STDMETHOD(IsScrollBegin)(VARIANT_BOOL * pVal);
	STDMETHOD(IsScrollEnd)(VARIANT_BOOL * pVal);
	STDMETHOD(Clear)(void);
	int m_nNextLineId;
	int GetNextLineId(void) {return ++m_nNextLineId;}
	STDMETHOD(AddLine)(LONGLONG nMsgId);
	STDMETHOD(DeleteLine)(LONGLONG nMsgId);
	STDMETHOD(DeleteLineItem)(LONG nLineId, LONG nItemId);
	STDMETHOD(ViewLine)(LONGLONG nMsgId);
	STDMETHOD(GetLineCount)(LONG * pVal);
	STDMETHOD(SetFrameArc)(SHORT nArcOffset, SHORT nPoloygonWidth, SHORT nPoloygonHeight);
	STDMETHOD(SetFrameBorderColor)(ULONG colorRef);
	STDMETHOD(SetFrameBackGroundColor)(ULONG colorRef);
	STDMETHOD(WriteString)(BSTR sString, ULONG colorRef);
	void WriteText(const CMrLine::pointer& pCurrentLine, BSTR sText, ULONG colorRef);
	STDMETHOD(WriteText)(BSTR sText, ULONG colorRef);
	STDMETHOD(WriteImage)(BSTR sImageFilePath, LONG nLimitWidth, LONG nLimitHeight);
	STDMETHOD(WriteWav)(BSTR sTitle, BSTR sWavFilePath, LONG nCtrlType);
	STDMETHOD(WriteHICON)(ULONG* hIcon, BSTR sUrl, LONG nCtrlType);
	STDMETHOD(WriteTime)(ULONG nTimeValue, BSTR sTimeFormat);
	STDMETHOD(WriteUrl)(BSTR sTitle, BSTR sUrl, LONG nCtrlType, ULONG colorRef);
	STDMETHOD(WriteLine)(void);
	STDMETHOD(WriteSpace)(ULONG nSpaceNumber);
	STDMETHOD(WriteOpenFile)(BSTR sTitle, BSTR sFilePath);
	STDMETHOD(WriteOpenDir)(BSTR sTitle, BSTR sFilePath);
	STDMETHOD(WritePic)(LONG nPicType, LONG x, LONG y, LONG w, LONG h, ULONG colorRef, LONG nCtrlType, ULONG nData1);
	STDMETHOD(AddContextMenuItem)(BSTR sName, ULONG nItemData, VARIANT_BOOL bSingleSelect);
	STDMETHOD(ClearContextMenuItem)(void);
	STDMETHOD(GetSelectString)(BSTR* pVal);
	STDMETHOD(SetSelectCbData1)(ULONG nData1);
	STDMETHOD(SetDefaultUrlCbData1)(ULONG nData1);
	STDMETHOD(SetParameter)(LONG nParameter, LONG nValue);
	STDMETHOD(GetParameter)(LONG nParameter, LONG* pValue);
	STDMETHOD(FreeParameter)(LONG nParameter, LONG nValue);
	//STDMETHOD(AddLine2)(LONGLONG nMsgId, LONG nReceiptFlag);
	STDMETHOD(SetReceiptFlag)(LONG nReceiptFlag);
	STDMETHOD(SetMsgReceiptFlag)(LONGLONG nMsgId, LONG nReceiptFlag);
	STDMETHOD(UpdateMsgText)(LONGLONG nMsgId, LONG nLineOffset, BSTR sNewText, ULONG colorRef);
	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnTimer(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnEraseBkgnd(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnLButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnMouseMove(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnMouseWheel(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnLButtonUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnKeyDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnSetCursor(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnContextMenu(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnCopy(WORD ,WORD ,HWND , BOOL& /*bHandled*/);
	LRESULT OnDelete(WORD ,WORD ,HWND , BOOL& /*bHandled*/);
	LRESULT OnSelectAll(WORD ,WORD ,HWND , BOOL& /*bHandled*/);
	LRESULT OnCleanAll(WORD ,WORD ,HWND , BOOL& /*bHandled*/);
	LRESULT OnSaveAs(WORD ,WORD ,HWND , BOOL& /*bHandled*/);
	LRESULT OnUserMenuSelect1(WORD ,WORD ,HWND , BOOL& /*bHandled*/);
	LRESULT OnUserMenuSelect2(WORD ,WORD ,HWND , BOOL& /*bHandled*/);
	LRESULT OnMode0(WORD ,WORD ,HWND , BOOL& /*bHandled*/);
	LRESULT OnMode1(WORD ,WORD ,HWND , BOOL& /*bHandled*/);
	LRESULT OnFontSize0(WORD ,WORD ,HWND , BOOL& /*bHandled*/);
	LRESULT OnFontSize1(WORD ,WORD ,HWND , BOOL& /*bHandled*/);
	LRESULT OnFontSize2(WORD ,WORD ,HWND , BOOL& /*bHandled*/);
	LRESULT OnReceipt0(WORD ,WORD ,HWND , BOOL& /*bHandled*/);
	LRESULT OnReceipt1(WORD ,WORD ,HWND , BOOL& /*bHandled*/);
	LRESULT OnReceipt2(WORD ,WORD ,HWND , BOOL& /*bHandled*/);
	void UpdateReceiptData(int nMsgReceipt);
	void UpdateReceiptData(const CMrLine::pointer& pLine, int nMsgReceipt);
	LRESULT OnLButtonDblClk(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnRButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	//LRESULT OnKillFocus(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

	//BOOL OnTipNotify(int /*wParam*/, LPNMHDR /*lParam*/, BOOL& /*bHandled*/);

	// _IEBRichMessageEvents Methods
public:
	//STDMETHOD_(void, onContextMenuSelect)( ULONG nData,  BSTR sSelectString)
	//{
	//	// Add your function implementation here.
	//	Fire_onContextMenuSelect(1,"");
	//}
	BEGIN_CONNECTION_POINT_MAP(CEBRichMessage)
		CONNECTION_POINT_ENTRY(__uuidof(_IEBRichMessageEvents))
	END_CONNECTION_POINT_MAP()

	// _IEBRichMessageEvents Methods
public:
	STDMETHOD_(void, onContextMenuCopy)( BSTR sCopyString)
	{
		// Add your function implementation here.

	}
	STDMETHOD_(void, onContextMenuDelete)( LONGLONG nMsgId,  LONG nIndex)
	{
		// Add your function implementation here.

	}
	STDMETHOD_(void, onContextMenuSelect)( ULONG nItemData,  BSTR sSelectString,  LONGLONG nSelectMsgId)
	{
		// Add your function implementation here.

	}
	STDMETHOD_(void, onItemClick)( BSTR sParamString)
	{
		// Add your function implementation here.

	}
	STDMETHOD_(void, onItemDblClick)( BSTR sParamString)
	{
		// Add your function implementation here.

	}
};

OBJECT_ENTRY_AUTO(__uuidof(EBRichMessage), CEBRichMessage)
