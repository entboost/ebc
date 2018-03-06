// EBVWindow.h : Declaration of the CEBVWindow
#pragma once
#include "resource.h"       // main symbols
#include <atlctl.h>
#include "ebvideoroom_i.h"
#include "videoroom.h"

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif
//
//
//// CEBVWindow
//class ATL_NO_VTABLE CEBVWindow :
//	public CComObjectRootEx<CComSingleThreadModel>,
//	public IDispatchImpl<IEBVWindow, &IID_IEBVWindow, &LIBID_ebvideoroomLib, /*wMajor =*/ 1, /*wMinor =*/ 0>,
//	public IPersistStreamInitImpl<CEBVWindow>,
//	public IOleControlImpl<CEBVWindow>,
//	public IOleObjectImpl<CEBVWindow>,
//	public IOleInPlaceActiveObjectImpl<CEBVWindow>,
//	public IViewObjectExImpl<CEBVWindow>,
//	public IOleInPlaceObjectWindowlessImpl<CEBVWindow>,
//	public ISupportErrorInfo,
//	public IPersistStorageImpl<CEBVWindow>,
//	public ISpecifyPropertyPagesImpl<CEBVWindow>,
//	public IQuickActivateImpl<CEBVWindow>,
//#ifndef _WIN32_WCE
//	public IDataObjectImpl<CEBVWindow>,
//#endif
//	public IProvideClassInfo2Impl<&CLSID_EBVWindow, NULL, &LIBID_ebvideoroomLib>,
//#ifdef _WIN32_WCE // IObjectSafety is required on Windows CE for the control to be loaded correctly
//	public IObjectSafetyImpl<CEBVWindow, INTERFACESAFE_FOR_UNTRUSTED_CALLER>,
//#endif
//	public CComCoClass<CEBVWindow, &CLSID_EBVWindow>,
//	public CComControl<CEBVWindow>
//{
//public:
//
//	Cvideowindow* m_pVideoWindow;
//	CEBVWindow()
//	{
//		m_bWindowOnly = TRUE;
//		m_pVideoWindow = NULL;
//	}
//
//DECLARE_OLEMISC_STATUS(OLEMISC_RECOMPOSEONRESIZE |
//	OLEMISC_CANTLINKINSIDE |
//	OLEMISC_INSIDEOUT |
//	OLEMISC_ACTIVATEWHENVISIBLE |
//	OLEMISC_SETCLIENTSITEFIRST
//)
//
//DECLARE_REGISTRY_RESOURCEID(IDR_EBVWINDOW)
//
//
//BEGIN_COM_MAP(CEBVWindow)
//	COM_INTERFACE_ENTRY(IEBVWindow)
//	COM_INTERFACE_ENTRY(IDispatch)
//	COM_INTERFACE_ENTRY(IViewObjectEx)
//	COM_INTERFACE_ENTRY(IViewObject2)
//	COM_INTERFACE_ENTRY(IViewObject)
//	COM_INTERFACE_ENTRY(IOleInPlaceObjectWindowless)
//	COM_INTERFACE_ENTRY(IOleInPlaceObject)
//	COM_INTERFACE_ENTRY2(IOleWindow, IOleInPlaceObjectWindowless)
//	COM_INTERFACE_ENTRY(IOleInPlaceActiveObject)
//	COM_INTERFACE_ENTRY(IOleControl)
//	COM_INTERFACE_ENTRY(IOleObject)
//	COM_INTERFACE_ENTRY(IPersistStreamInit)
//	COM_INTERFACE_ENTRY2(IPersist, IPersistStreamInit)
//	COM_INTERFACE_ENTRY(ISupportErrorInfo)
//	COM_INTERFACE_ENTRY(ISpecifyPropertyPages)
//	COM_INTERFACE_ENTRY(IQuickActivate)
//	COM_INTERFACE_ENTRY(IPersistStorage)
//#ifndef _WIN32_WCE
//	COM_INTERFACE_ENTRY(IDataObject)
//#endif
//	COM_INTERFACE_ENTRY(IProvideClassInfo)
//	COM_INTERFACE_ENTRY(IProvideClassInfo2)
//#ifdef _WIN32_WCE // IObjectSafety is required on Windows CE for the control to be loaded correctly
//	COM_INTERFACE_ENTRY_IID(IID_IObjectSafety, IObjectSafety)
//#endif
//END_COM_MAP()
//
//BEGIN_PROP_MAP(CEBVWindow)
//	PROP_DATA_ENTRY("_cx", m_sizeExtent.cx, VT_UI4)
//	PROP_DATA_ENTRY("_cy", m_sizeExtent.cy, VT_UI4)
//	// Example entries
//	// PROP_ENTRY_TYPE("Property Name", dispid, clsid, vtType)
//	// PROP_PAGE(CLSID_StockColorPage)
//END_PROP_MAP()
//
//
//BEGIN_MSG_MAP(CEBVWindow)
//	MESSAGE_HANDLER(WM_SHOWWINDOW, OnShowWindow)
//	CHAIN_MSG_MAP(CComControl<CEBVWindow>)
//	DEFAULT_REFLECTION_HANDLER()
//END_MSG_MAP()
//// Handler prototypes:
////  LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
////  LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
////  LRESULT NotifyHandler(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);
//
//// ISupportsErrorInfo
//	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid)
//	{
//		static const IID* arr[] =
//		{
//			&IID_IEBVWindow,
//		};
//
//		for (int i=0; i<sizeof(arr)/sizeof(arr[0]); i++)
//		{
//			if (InlineIsEqualGUID(*arr[i], riid))
//				return S_OK;
//		}
//		return S_FALSE;
//	}
//
//// IViewObjectEx
//	DECLARE_VIEW_STATUS(0)
//
//// IEBVWindow
//public:
////		HRESULT OnDrawAdvanced(ATL_DRAWINFO& di)
////		{
////		RECT& rc = *(RECT*)di.prcBounds;
////		// Set Clip region to the rectangle specified by di.prcBounds
////		HRGN hRgnOld = NULL;
////		if (GetClipRgn(di.hdcDraw, hRgnOld) != 1)
////			hRgnOld = NULL;
////		bool bSelectOldRgn = false;
////
////		HRGN hRgnNew = CreateRectRgn(rc.left, rc.top, rc.right, rc.bottom);
////
////		if (hRgnNew != NULL)
////		{
////			bSelectOldRgn = (SelectClipRgn(di.hdcDraw, hRgnNew) != ERROR);
////		}
////
////		Rectangle(di.hdcDraw, rc.left, rc.top, rc.right, rc.bottom);
////		SetTextAlign(di.hdcDraw, TA_CENTER|TA_BASELINE);
////		LPCTSTR pszText = _T("ATL 8.0 : EBVWindow");
////#ifndef _WIN32_WCE
////		TextOut(di.hdcDraw,
////			(rc.left + rc.right) / 2,
////			(rc.top + rc.bottom) / 2,
////			pszText,
////			lstrlen(pszText));
////#else
////		ExtTextOut(di.hdcDraw,
////			(rc.left + rc.right) / 2,
////			(rc.top + rc.bottom) / 2,
////			ETO_OPAQUE,
////			NULL,
////			pszText,
////			ATL::lstrlen(pszText),
////			NULL);
////#endif
////
////		if (bSelectOldRgn)
////			SelectClipRgn(di.hdcDraw, hRgnOld);
////
////		return S_OK;
////	}
//
//
//	DECLARE_PROTECT_FINAL_CONSTRUCT()
//	STDMETHOD(SetObjectRects)(LPCRECT prcPos,LPCRECT prcClip)
//	{
//		IOleInPlaceObjectWindowlessImpl<CEBVWindow>::SetObjectRects(prcPos, prcClip);
//		int cx, cy;
//		cx = prcPos->right - prcPos->left;
//		cy = prcPos->bottom - prcPos->top;
//		if (m_pVideoWindow != NULL)
//		{
//			m_pVideoWindow->VW_MoveWindow(prcPos->left,prcPos->top,prcPos->right,prcPos->bottom);
//		}
//		return S_OK;
//	}
//	HRESULT FinalConstruct()
//	{
//		return S_OK;
//	}
//
//	void FinalRelease()
//	{
//		if (m_pVideoWindow)
//		{
//			m_pVideoWindow->VW_StopGraph();
//			m_pVideoWindow->VW_ReleaseGraph();
//			delete m_pVideoWindow;
//			m_pVideoWindow = NULL;
//		}
//	}
//	STDMETHOD(VW_BuildVideoWindow)(SHORT nVideoType, SHORT nVideoWidth, SHORT nVideoHeight);
//	STDMETHOD(VW_BuildGraph)(VARIANT_BOOL bDefaultRender);
//	STDMETHOD(VW_StartGraph)(void);
//	STDMETHOD(VW_StopGraph)(void);
//	STDMETHOD(VW_ReleaseGraph)(void);
//	STDMETHOD(VW_DrawVideo)(BYTE* pVideoData, ULONG nVideoSize);
//	STDMETHOD(VW_DrawVideo2)(VARIANT* pVideoData, ULONG nVideoSize);
//	LRESULT OnShowWindow(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
//	STDMETHOD(VW_SaveBitmapFile)(BSTR sSaveImageTo);
//	STDMETHOD(VW_ShowVideoWindow)(VARIANT_BOOL newVal);
//};
//
//OBJECT_ENTRY_AUTO(__uuidof(EBVWindow), CEBVWindow)
