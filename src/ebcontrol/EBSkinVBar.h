// EBSkinVBar.h : Declaration of the CEBSkinVBar
#pragma once
#include "resource.h"       // main symbols
#include <atlctl.h>
#include "ebcontrol_i.h"

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif


// CEBSkinVBar
class ATL_NO_VTABLE CEBSkinVBar :
	public CComObjectRootEx<CComSingleThreadModel>,
	public IDispatchImpl<IEBSkinVBar, &IID_IEBSkinVBar, &LIBID_ebcontrolLib, /*wMajor =*/ 1, /*wMinor =*/ 0>,
	public IPersistStreamInitImpl<CEBSkinVBar>,
	public IOleControlImpl<CEBSkinVBar>,
	public IOleObjectImpl<CEBSkinVBar>,
	public IOleInPlaceActiveObjectImpl<CEBSkinVBar>,
	public IViewObjectExImpl<CEBSkinVBar>,
	public IOleInPlaceObjectWindowlessImpl<CEBSkinVBar>,
	public ISupportErrorInfo,
	public IPersistStorageImpl<CEBSkinVBar>,
	public ISpecifyPropertyPagesImpl<CEBSkinVBar>,
	public IQuickActivateImpl<CEBSkinVBar>,
#ifndef _WIN32_WCE
	public IDataObjectImpl<CEBSkinVBar>,
#endif
	public IProvideClassInfo2Impl<&CLSID_EBSkinVBar, NULL, &LIBID_ebcontrolLib>,
#ifdef _WIN32_WCE // IObjectSafety is required on Windows CE for the control to be loaded correctly
	public IObjectSafetyImpl<CEBSkinVBar, INTERFACESAFE_FOR_UNTRUSTED_CALLER>,
#endif
	public CComCoClass<CEBSkinVBar, &CLSID_EBSkinVBar>,
	public CComControl<CEBSkinVBar>
{
public:
	Gdiplus::SolidBrush m_brushBackground;
	HWND m_hMsgHwnd;
	CComPtr<IEBSkinVBarCallback> m_pCallback;
	int m_nTopInterval;		// ÉÏÏÂ±ß¿ò
	int m_nLeftInterval;	// ×óÓÒ±ß¿ò
	CSize m_sizeTopArrow;
	CSize m_sizeUpArrow;
	CSize m_sizeDownArrow;
	CSize m_sizeThumb;
	CSize m_sizeBottomArrow;
	int m_nThumbTop;
	bool m_bMouseDownArrowTop;
	bool m_bMouseDownArrowUp;
	bool m_bMouseDownArrowDown;
	bool m_bMouseDownArrowBottom;
	bool m_bMouseDown;

	bool m_bMoveInTopArrow;
	bool m_bMoveInUpArrow;
	bool m_bMoveInThumb;
	bool m_bMoveInDownArrow;
	bool m_bMoveInBottomArrow;

#pragma warning(push)
#pragma warning(disable: 4355) // 'this' : used in base member initializer list


	CEBSkinVBar()
		: m_brushBackground(Gdiplus::Color(208,217,222))
	{
		m_bWindowOnly = TRUE;
		m_hMsgHwnd = NULL;

		const int const_default_cx = 10;
		const int const_default_cy = 8;
		m_sizeTopArrow.SetSize(const_default_cx,const_default_cy);
		m_sizeUpArrow.SetSize(const_default_cx,const_default_cy);
		m_sizeDownArrow.SetSize(const_default_cx,const_default_cy);
		m_nTopInterval = 0;
		m_nLeftInterval = 0;
		m_sizeThumb.SetSize(const_default_cx,50);
		m_sizeBottomArrow.SetSize(const_default_cx,const_default_cy);

		m_nThumbTop = 0;
		m_bMouseDownArrowTop = false;
		m_bMouseDownArrowUp = false;
		m_bMouseDownArrowDown = false;
		m_bMouseDownArrowBottom = false;
		m_bMouseDown = false;

		m_bMoveInTopArrow = false;
		m_bMoveInUpArrow = false;
		m_bMoveInThumb = false;
		m_bMoveInDownArrow = false;
		m_bMoveInBottomArrow = false;
	}

#pragma warning(pop)

DECLARE_OLEMISC_STATUS(OLEMISC_RECOMPOSEONRESIZE |
	OLEMISC_CANTLINKINSIDE |
	OLEMISC_INSIDEOUT |
	OLEMISC_ACTIVATEWHENVISIBLE |
	OLEMISC_SETCLIENTSITEFIRST
)

DECLARE_REGISTRY_RESOURCEID(IDR_EBSKINVBAR)


BEGIN_COM_MAP(CEBSkinVBar)
	COM_INTERFACE_ENTRY(IEBSkinVBar)
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

BEGIN_PROP_MAP(CEBSkinVBar)
	PROP_DATA_ENTRY("_cx", m_sizeExtent.cx, VT_UI4)
	PROP_DATA_ENTRY("_cy", m_sizeExtent.cy, VT_UI4)
	// Example entries
	// PROP_ENTRY_TYPE("Property Name", dispid, clsid, vtType)
	// PROP_PAGE(CLSID_StockColorPage)
END_PROP_MAP()

void LimitThumbPosition(void);
void ScrollUp();
void ScrollDown();
void PageUp();
void PageDown();

BEGIN_MSG_MAP(CEBSkinVBar)
	MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
	MESSAGE_HANDLER(WM_MOUSEWHEEL, OnMouseWheel)
	MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
	MESSAGE_HANDLER(WM_LBUTTONUP, OnLButtonUp)
	MESSAGE_HANDLER(WM_TIMER, OnTimer)
	MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkgnd)
	MESSAGE_HANDLER(WM_KILLFOCUS, OnKillFocus)
	CHAIN_MSG_MAP(CComControl<CEBSkinVBar>)
ALT_MSG_MAP(1)
	// Replace this with message map entries for superclassed Static
END_MSG_MAP()
// Handler prototypes:
//  LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
//  LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
//  LRESULT NotifyHandler(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);
	HRESULT OnDraw(ATL_DRAWINFO& di);
	void DrawTopArrow(Gdiplus::Graphics& graphics,const Rect& rectTopArrow);
	void DrawUpArrow(Gdiplus::Graphics& graphics,const Rect& rectUpArrow);
	void DrawDownArrow(Gdiplus::Graphics& graphics,const Rect& rectDownArrow);
	void DrawBottomArrow(Gdiplus::Graphics& graphics,const Rect& rectBottomArrow);

// ISupportsErrorInfo
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid)
	{
		static const IID* arr[] =
		{
			&IID_IEBSkinVBar,
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

// IEBSkinVBar

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalRelease()
	{
	}
	STDMETHOD(UpdatePosition)(void);
	STDMETHOD(SetMsgHwnd)(ULONG hWnd);
	STDMETHOD(SetCallback)(IUnknown* pCallback);
	STDMETHOD(SetBackGroundColor)(BYTE r, BYTE g, BYTE b);
	STDMETHOD(SetBackGroundColor2)(BYTE a, BYTE r, BYTE g, BYTE b);
	LRESULT OnMouseMove(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnMouseWheel(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnLButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnLButtonUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnTimer(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnEraseBkgnd(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnKillFocus(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
};

OBJECT_ENTRY_AUTO(__uuidof(EBSkinVBar), CEBSkinVBar)
