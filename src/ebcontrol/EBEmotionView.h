// EBEmotionView.h : Declaration of the CEBEmotionView
#pragma once
#include "resource.h"       // main symbols
#include <atlctl.h>
#include "ebcontrol_i.h"
#include <string>
#include <vector>
#include "atltypes.h"
#include "SkinMemoryDC.h"
#include "EBSkinVBar.h"
#include "boost/shared_ptr.hpp"


#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

const int const_vbar_width = 11;
#define TIMERID_DRAW_FOCUS_IMAGE 111

//class CMyTip : public CWindowImpl<CMyTip>
//{
//public:
//	DECLARE_WND_CLASS_EX(NULL, 0, COLOR_3DFACE)
//
//	LPCTSTR m_lpstrInfo;
//
//	enum { m_nToolTipID = 1313 };
//	CToolTipCtrl m_tooltip;
//
//
//	CMyTip() : m_lpstrInfo(NULL)
//	{ }
//
//	void Init(HWND hWnd, LPCTSTR lpstrName)
//	{
//		ATLASSERT(::IsWindow(hWnd));
//		SubclassWindow(hWnd);
//
//		// Set tooltip
//		m_tooltip.Create(m_hWnd);
//		ATLASSERT(m_tooltip.IsWindow());
//		RECT rect;
//		GetClientRect(&rect);
//		CToolInfo ti(0, m_hWnd, m_nToolTipID, &rect, NULL);
//		m_tooltip.AddTool(&ti);
//
//		// set text
//		m_lpstrInfo = lpstrName;
//		if(m_lpstrInfo == NULL)
//			return;
//
//		// CClientDC dc(m_hWnd); // will not really paint
//		// HFONT hFontOld = dc.SelectFont(AtlGetStockFont(DEFAULT_GUI_FONT));
//
//		// RECT rcText = rect;
//		// dc.DrawText(m_lpstrInfo, -1, &rcText, DT_SINGLELINE | DT_LEFT | DT_VCENTER | DT_NOPREFIX | DT_CALCRECT);
//		// BOOL bTooLong = (rcText.right > rect.right);
//		// if(bTooLong)
//		m_tooltip.UpdateTipText(m_lpstrInfo, m_hWnd, m_nToolTipID);
//		m_tooltip.Activate(true);
//
//		// dc.SelectFont(hFontOld);
//
//		// Invalidate();
//		// UpdateWindow();
//	}
//
//	BEGIN_MSG_MAP(CMyTip)
//		MESSAGE_RANGE_HANDLER(WM_MOUSEFIRST, WM_MOUSELAST, OnMouseMessage)
//
//	END_MSG_MAP()
//
//	LRESULT OnMouseMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
//	{
//		if(m_tooltip.IsWindow())
//		{
//			MSG msg = { m_hWnd, uMsg, wParam, lParam };
//			m_tooltip.RelayEvent(&msg);
//		}
//		bHandled = FALSE;
//		return 1;
//	}
//
//
//};

class CEmotionInfo
{
public:
	typedef boost::shared_ptr<CEmotionInfo> pointer;
	static CEmotionInfo::pointer create(const std::wstring& sFile,const std::wstring& sDescription,const Gdiplus::Rect& rect,int nIndex,DWORD dwUserData)
	{
		return CEmotionInfo::pointer(new CEmotionInfo(sFile,sDescription,rect,nIndex,dwUserData));
	}
	std::wstring m_sEmotionFile;
	std::wstring m_sDescription;
	Gdiplus::Rect m_rect;
	int m_nIndex;
	DWORD m_dwUserData;
	Gdiplus::Image* m_pImage;
	void clear(void)
	{
		m_sEmotionFile.clear();
		m_rect = Gdiplus::Rect(0,0,0,0);
		m_nIndex = 0;
		m_dwUserData = 0;
		if (m_pImage)
		{
			delete m_pImage;
			m_pImage = NULL;
		}
	}
	int m_nDrawDescriptionCount;

	CEmotionInfo(void)
		: m_nIndex(0)
		, m_dwUserData(0)
		, m_pImage(NULL)
		, m_nDrawDescriptionCount(0)
	{
	}
	CEmotionInfo(const std::wstring& sFile,const std::wstring& sDescription,const Gdiplus::Rect& rect,int nIndex,DWORD dwUserData)
		: m_sEmotionFile(sFile), m_sDescription(sDescription)
		, m_rect(rect)
		, m_nIndex(nIndex)
		, m_dwUserData(dwUserData)
		, m_pImage(NULL)
		, m_nDrawDescriptionCount(0)
	{
	}
	virtual ~CEmotionInfo(void)
	{
		if (m_pImage)
		{
			delete m_pImage;
			m_pImage = NULL;
		}
	}
	const CEmotionInfo& operator =(const CEmotionInfo& pObj)
	{
		m_sEmotionFile = pObj.m_sEmotionFile;
		m_rect = pObj.m_rect;
		m_nIndex = pObj.m_nIndex;
		m_dwUserData = pObj.m_dwUserData;
		return *this;
	}
};

// CEBEmotionView
class ATL_NO_VTABLE CEBEmotionView :
	public CComObjectRootEx<CComSingleThreadModel>,
	public IDispatchImpl<IEBEmotionView, &IID_IEBEmotionView, &LIBID_ebcontrolLib, /*wMajor =*/ 1, /*wMinor =*/ 0>,
	public IPersistStreamInitImpl<CEBEmotionView>,
	public IOleControlImpl<CEBEmotionView>,
	public IOleObjectImpl<CEBEmotionView>,
	public IOleInPlaceActiveObjectImpl<CEBEmotionView>,
	public IViewObjectExImpl<CEBEmotionView>,
	public IOleInPlaceObjectWindowlessImpl<CEBEmotionView>,
	public ISupportErrorInfo,
	public IPersistStorageImpl<CEBEmotionView>,
	public ISpecifyPropertyPagesImpl<CEBEmotionView>,
	public IQuickActivateImpl<CEBEmotionView>,
#ifndef _WIN32_WCE
	public IDataObjectImpl<CEBEmotionView>,
#endif
	public IProvideClassInfo2Impl<&CLSID_EBEmotionView, NULL, &LIBID_ebcontrolLib>,
#ifdef _WIN32_WCE // IObjectSafety is required on Windows CE for the control to be loaded correctly
	public IObjectSafetyImpl<CEBEmotionView, INTERFACESAFE_FOR_UNTRUSTED_CALLER>,
#endif
	public CComCoClass<CEBEmotionView, &CLSID_EBEmotionView>,
	public CComControl<CEBEmotionView>,
	public IEBSkinVBarCallback
{
private:
	ULONG_PTR theGdiplusToken;
	USHORT m_nEmotionWidth;
	USHORT m_nEmotionHeight;
	USHORT m_nHCount;			// 水平数量
	USHORT m_nVCount;			// 垂直数量
	USHORT m_nTopInterval;		// 上边间隔
	USHORT m_nLeftInterval;		// 左边间隔
	USHORT m_nLineInterval;		// 线条间隔
	std::vector<CEmotionInfo::pointer> m_pImageDrawList;
	CEmotionInfo::pointer m_pMouseFocusInfo;		// 当前鼠标移动选项

	Gdiplus::Image * m_image;
	UINT m_nFrameCount;						// 图像多少帧
	UINT m_nCurrentFrame;					// 当前显示第几帧
	DWORD m_dwCurrentTime;					// 当前显示时间
	Gdiplus::PropertyItem* m_pFrameDelay;	// 每帧显示时间
	Gdiplus::Pen m_penBorder;				// 选中图像边框
	Gdiplus::SolidBrush m_brushBackground;	// 背景颜色
	void InvalidateFocusInfo(void);
	void DrawFucusInfo(HDC hDC,bool bResetImage);
	void SetImage(Gdiplus::Image * pImage);
	void DrawGif(Graphics& pGraphics, const Rect& layoutRect,DWORD dwNowTime);

	//CWindow m_wndToolTip;
	Gdiplus::Font * m_fontTt;
	Gdiplus::SolidBrush m_brushTtBg;
	Gdiplus::SolidBrush m_brushTt;
	Gdiplus::Pen m_penBorderTt;

public:
	// 实现滚动条
	CAxWindow m_pSkinVBar;
	CComPtr<IEBSkinVBar> m_pSkinVBarInterface;
	int m_nScrollMax;
	int m_nScrollPos;

#pragma warning(push)
#pragma warning(disable: 4355) // 'this' : used in base member initializer list


	CEBEmotionView()
		: m_penBorder(Gdiplus::Color(255,127,39))
		, m_brushBackground(Gdiplus::Color(0,162,232))
		, m_fontTt(NULL)
		, m_brushTtBg(Gdiplus::Color(255,255,200))
		, m_brushTt(Gdiplus::Color(28,28,28))
		, m_penBorderTt(Gdiplus::Color(128,128,128))
	{
		m_bWindowOnly = TRUE;
		theGdiplusToken = 0;

		m_nEmotionWidth = 32;
		m_nEmotionHeight = 32;
		m_nHCount = 12;
		m_nVCount = 6;
		m_nTopInterval = 1;
		m_nLeftInterval = 1;
		m_nLineInterval = 1;

		m_image = NULL;
		m_nFrameCount = 0;
		m_nCurrentFrame = 0;
		m_dwCurrentTime = 0;
		m_pFrameDelay = NULL;

		m_nScrollMax = 0;
		m_nScrollPos = 0;
	}

#pragma warning(pop)

DECLARE_OLEMISC_STATUS(OLEMISC_RECOMPOSEONRESIZE |
	OLEMISC_CANTLINKINSIDE |
	OLEMISC_INSIDEOUT |
	OLEMISC_ACTIVATEWHENVISIBLE |
	OLEMISC_SETCLIENTSITEFIRST
)

DECLARE_REGISTRY_RESOURCEID(IDR_EBEMOTIONVIEW)

BEGIN_CATEGORY_MAP(CEBEmotionView)
	IMPLEMENTED_CATEGORY(CATID_SafeForScripting)
	IMPLEMENTED_CATEGORY(CATID_SafeForInitializing)
END_CATEGORY_MAP()


BEGIN_COM_MAP(CEBEmotionView)
	COM_INTERFACE_ENTRY(IEBEmotionView)
	COM_INTERFACE_ENTRY(IEBSkinVBarCallback)
	COM_INTERFACE_ENTRY(IDispatch)
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
END_COM_MAP()

BEGIN_PROP_MAP(CEBEmotionView)
	PROP_DATA_ENTRY("_cx", m_sizeExtent.cx, VT_UI4)
	PROP_DATA_ENTRY("_cy", m_sizeExtent.cy, VT_UI4)
	// Example entries
	// PROP_ENTRY_TYPE("Property Name", dispid, clsid, vtType)
	// PROP_PAGE(CLSID_StockColorPage)
END_PROP_MAP()

void PositionScrollBars(void);

BEGIN_MSG_MAP(CEBEmotionView)
	MESSAGE_HANDLER(WM_CREATE, OnCreate)
	MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
	MESSAGE_HANDLER(WM_TIMER, OnTimer)
	MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkgnd)
	MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
	MESSAGE_HANDLER(WM_MOUSEWHEEL, OnMouseWheel)
	MESSAGE_HANDLER(WM_KILLFOCUS, OnKillFocus)
	//MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
	CHAIN_MSG_MAP(CComControl<CEBEmotionView>)
ALT_MSG_MAP(1)
	// Replace this with message map entries for superclassed Static
	 //CHAIN_MSG_MAP_MEMBER(m_pSkinVBar) 
END_MSG_MAP()
// Handler prototypes:
//  LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
//  LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
//  LRESULT NotifyHandler(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);
	HRESULT OnDraw(ATL_DRAWINFO& di)
	{
		RECT& rc = *(RECT*)di.prcBounds;
		HDC hDC = di.hdcDraw;

		{
			CSkinMemoryDC memDC(hDC,rc);
			Gdiplus::Graphics graphics(memDC.GetDC());
			graphics.FillRectangle(&m_brushBackground,memDC.GetLeft(),memDC.GetTop(),memDC.Width(),memDC.Height());
			const size_t nSize = m_pImageDrawList.size();
			for (size_t i=0; i<nSize; i++)
			{
				int nCurrentLine = i/m_nHCount;
				if ((nCurrentLine-m_nScrollPos)==m_nVCount) break;	// 已经画多少行
				if (nCurrentLine<m_nScrollPos) continue;							// 前面几行不画
				CEmotionInfo::pointer pImageDrawInfo = m_pImageDrawList[i];
				Gdiplus::Rect rect(pImageDrawInfo->m_rect);
				rect.Y -= (m_nScrollPos*(m_nLineInterval+m_nEmotionHeight));
				Gdiplus::Image pImage(pImageDrawInfo->m_sEmotionFile.c_str());
				graphics.DrawImage(&pImage, rect);
			}
		}
		if (m_pSkinVBarInterface != NULL)
			m_pSkinVBarInterface->UpdatePosition();

		return S_OK;
	}
static BOOL CALLBACK MyEnumFonts(CONST LOGFONTW* lplf, CONST TEXTMETRICW *lptm,DWORD dwType,LPARAM lparam)
{
	const wchar_t * lpszFine = (const wchar_t*)lparam;
	const std::wstring tempFontName(lplf->lfFaceName);
	if(tempFontName.find(lpszFine)!=std::wstring::npos)
	{
		theFontFamily = lpszFine;
		return FALSE;
	}
	return TRUE;
}

	//std::wstring m_sTooltip;
	//HWND m_hTheWnd;
	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		PAINTSTRUCT ps;
		HDC hDC = BeginPaint(&ps);
		::EnumFontsW(hDC,NULL,(FONTENUMPROCW)MyEnumFonts,(LPARAM)theFontFamily1.c_str());	// 
		::EnumFontsW(hDC,NULL,(FONTENUMPROCW)MyEnumFonts,(LPARAM)theFontFamily0.c_str());
		EndPaint(&ps);
		Gdiplus::FontFamily fontFamily(theFontFamily.c_str());
		m_fontTt = new Gdiplus::Font(&fontFamily, 15, FontStyleBold, UnitPixel);

		RECT rc;
		GetWindowRect(&rc);
		rc.right -= rc.left;
		rc.bottom -= rc.top;
		rc.top = rc.left = 0;
		m_pSkinVBar.Create(m_hWnd,CRect(0,0,0,0),_T("ebcontrol.EBSkinVBar"),WS_CHILD|WS_VISIBLE);
		m_pSkinVBar.QueryControl(IID_IEBSkinVBar,(void**)&m_pSkinVBarInterface);
		if (m_pSkinVBarInterface!=NULL)
		{
			IEBSkinVBarCallback * pCallback = this;
			m_pSkinVBarInterface->SetCallback(pCallback);
		}
		SetTimer(TIMERID_DRAW_FOCUS_IMAGE,50,NULL);

		//if (m_wndToolTip.m_hWnd == NULL)
		//{
		//	// If we're windowless we still need an HWND 
		//	if (m_bWndLess)
		//	{
		//		HDC hDC;
		//		// Get the HDC from the client
		//		m_spInPlaceSite->GetDC(NULL, OLEDC_NODRAW, &hDC);
		//		m_hTheWnd = WindowFromDC(hDC);
		//		m_spInPlaceSite->ReleaseDC(hDC);
		//	}
		//	else
		//		m_hTheWnd = m_hWnd;

		//	//Load the Common control library and create tooltip control
		//	InitCommonControls();
		//	m_wndToolTip.m_hWnd =
		//		::CreateWindow(TOOLTIPS_CLASS, (LPTSTR)NULL, TTS_ALWAYSTIP,
		//		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		//		NULL, (HMENU) NULL, NULL, NULL);
		//		//NULL, (HMENU) NULL, _Module.GetModuleInstance(), NULL);

		//	// Ensure the tooltip always appears above our window
		//	::SetWindowPos(m_wndToolTip.m_hWnd, HWND_TOPMOST,0,0,0,0,
		//		SWP_NOACTIVATE|SWP_NOMOVE|SWP_NOREDRAW|SWP_NOSIZE);

		//	m_sTooltip = m_bWndLess ? L"My Windowless Tip" : L"My Windowed Tip";
		//	TOOLINFO    ti;
		//	ti.cbSize = sizeof(TOOLINFO);
		//	ti.uFlags = 0;
		//	ti.hwnd   = m_hTheWnd;
		//	ti.uId    = 1;
		//	ti.hinst  = NULL;
		//	ti.lpszText = (LPWSTR)m_sTooltip.c_str();
		//	m_wndToolTip.SendMessage(TTM_ADDTOOL, 0, (LPARAM)&ti);
		//	//char lpszBuffer[60];
		//	//sprintf(lpszBuffer,"%x,%x,%d",ret,(int)m_hTheWnd,GetLastError());
		//	//MessageBoxA(NULL,lpszBuffer,"",MB_OK);
		//}
		return 0;
	}

	//void RelayEvent(UINT message, WPARAM wParam, LPARAM lParam)
	//{
	//	if (NULL != m_wndToolTip.m_hWnd)
	//	{
	//		MSG msg;

	//		msg.hwnd= m_hTheWnd;
	//		msg.message= message;
	//		msg.wParam= wParam;
	//		msg.lParam= lParam;
	//		msg.time= 0;
	//		msg.pt.x= LOWORD (lParam);
	//		msg.pt.y= HIWORD (lParam);

	//		::SendMessage(m_wndToolTip.m_hWnd, TTM_RELAYEVENT, 0, (LPARAM)&msg);	
	//	}
	//} 

	STDMETHOD(SetObjectRects)(LPCRECT prcPos,LPCRECT prcClip)
	{
		IOleInPlaceObjectWindowlessImpl<CEBEmotionView>::SetObjectRects(prcPos, prcClip);
		int cx, cy;
		cx = prcPos->right - prcPos->left;
		cy = prcPos->bottom - prcPos->top;
		if (cx > 0 && cy > 0)
			PositionScrollBars();
		return S_OK;
	}

// ISupportsErrorInfo
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid)
	{
		static const IID* arr[] =
		{
			&IID_IEBEmotionView,
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

// IEBEmotionView

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		Gdiplus::GdiplusStartupInput gdiplusStartupInput;
		Gdiplus::GdiplusStartup(&theGdiplusToken, &gdiplusStartupInput, NULL);
		return S_OK;
	}

	void FinalRelease()
	{
		//if (m_image)
		//{
		//	delete m_image;
		//	m_image = NULL;
		//}
		if (m_pFrameDelay)
		{
			free(m_pFrameDelay);
			m_pFrameDelay = NULL;
		}
		if (m_fontTt)
		{
			delete m_fontTt;
			m_fontTt = NULL;
		}

		Gdiplus::GdiplusShutdown(theGdiplusToken);
	}

	//HWND m_hTheWnd;
	//HRESULT InPlaceActivate(LONG iVerb, const RECT* prcPosRect=NULL)
	//{
	//	HRESULT hr;

	//	hr = CComControlBase::InPlaceActivate(iVerb, prcPosRect);
	//	if (m_wndToolTip.m_hWnd == NULL)
	//	{
	//		// If we're windowless we still need an HWND 
	//		if (m_bWndLess)
	//		{
	//			HDC hDC;
	//			// Get the HDC from the client
	//			m_spInPlaceSite->GetDC(NULL, OLEDC_NODRAW, &hDC);
	//			m_hTheWnd = WindowFromDC(hDC);
	//			m_spInPlaceSite->ReleaseDC(hDC);
	//		}
	//		else
	//			m_hTheWnd = m_hWnd;

	//		//Load the Common control library and create tooltip control
	//		InitCommonControls();
	//		m_wndToolTip.m_hWnd =
	//			::CreateWindow(TOOLTIPS_CLASS, (LPTSTR)NULL, TTS_ALWAYSTIP,
	//			CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
	//			NULL, (HMENU) NULL, NULL, NULL);
	//			//NULL, (HMENU) NULL, _Module.GetModuleInstance(), NULL);

	//		// Ensure the tooltip always appears above our window
	//		::SetWindowPos(m_wndToolTip.m_hWnd, HWND_TOPMOST,0,0,0,0,
	//			SWP_NOACTIVATE|SWP_NOMOVE|SWP_NOREDRAW|SWP_NOSIZE);

	//		TOOLINFO    ti;
	//		ti.cbSize = sizeof(TOOLINFO);
	//		ti.uFlags = 0;
	//		ti.hwnd   = m_hTheWnd;
	//		ti.uId    = 1;
	//		ti.hinst  = NULL;
	//		ti.lpszText = m_bWndLess ? ti.lpszText = _T("My Windowless Tip") : ti.lpszText = _T("My Windowed Tip");
	//		m_wndToolTip.SendMessage(TTM_ADDTOOL, 0, (LPARAM)&ti);
	//	}
	//	return hr;
	//}

	STDMETHOD(AddEmotion)(BSTR sEmotionFilePath, BSTR sDescription, ULONG nUserData);
	STDMETHOD(MoveWindow)(LONG x, LONG y, LONG width, LONG height, VARIANT_BOOL bRepaint);
	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnTimer(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	STDMETHOD(SetBorderColor)(BYTE r, BYTE g, BYTE b);
	STDMETHOD(SetBorderColor2)(BYTE a, BYTE r, BYTE g, BYTE b);
	STDMETHOD(SetBackGroundColor)(BYTE r, BYTE g, BYTE b);
	STDMETHOD(SetBackGroundColor2)(BYTE a, BYTE r, BYTE g, BYTE b);
	// IEBSkinVBarCallback
	STDMETHOD(GetScrollMax)(LONG* pOutMax);
	STDMETHOD(GetScrollMin)(LONG* pOutMin);
	STDMETHOD(GetScrollPos)(LONG* pOutPos);
	STDMETHOD(ScrollPageUp)(void);
	STDMETHOD(ScrollPageDown)(void);
	STDMETHOD(ScrollLineUp)(void);
	STDMETHOD(ScrollLineDown)(void);
	STDMETHOD(ScrollBy)(LONG nScrollTimes);
	STDMETHOD(ScrollTo)(LONG nScrollPos);
	STDMETHOD(OnVBarKillFocus)(void);
	LRESULT OnEraseBkgnd(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnLButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnMouseWheel(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	STDMETHOD(get_EmotionWidth)(USHORT* pVal);
	STDMETHOD(put_EmotionWidth)(USHORT newVal);
	STDMETHOD(get_EmotionHeight)(USHORT* pVal);
	STDMETHOD(put_EmotionHeight)(USHORT newVal);
	STDMETHOD(get_TopInterval)(USHORT* pVal);
	STDMETHOD(put_TopInterval)(USHORT newVal);
	STDMETHOD(get_LeftInterval)(USHORT* pVal);
	STDMETHOD(put_LeftInterval)(USHORT newVal);
	STDMETHOD(get_LineInterval)(USHORT* pVal);
	STDMETHOD(put_LineInterval)(USHORT newVal);
	STDMETHOD(get_HCount)(USHORT* pVal);
	STDMETHOD(put_HCount)(USHORT newVal);
	STDMETHOD(get_VCount)(USHORT* pVal);
	STDMETHOD(put_VCount)(USHORT newVal);
	LRESULT OnKillFocus(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	STDMETHOD(get_EmotionCount)(ULONG* pVal);
	//LRESULT OnMouseMove(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
};

OBJECT_ENTRY_AUTO(__uuidof(EBEmotionView), CEBEmotionView)
