// EBImage.h : Declaration of the CEBImage
#pragma once
#include "resource.h"       // main symbols
#include <atlctl.h>
#include <atlcom.h>
#include "EBOleImage_i.h"
#include <string>
#include "boost/thread.hpp"

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

//#define USES_THREAD_DRAW	// 内部启动线程

// CEBImage
class ATL_NO_VTABLE CEBImage :
	public CComObjectRootEx<CComSingleThreadModel>,
	public IDispatchImpl<IEBImage, &IID_IEBImage, &LIBID_EBOleImageLib, /*wMajor =*/ 1, /*wMinor =*/ 0>,
	public IPersistStreamInitImpl<CEBImage>,
	public IOleControlImpl<CEBImage>,
	public IOleObjectImpl<CEBImage>,
	public IOleInPlaceActiveObjectImpl<CEBImage>,
	public IViewObjectExImpl<CEBImage>,
	public IOleInPlaceObjectWindowlessImpl<CEBImage>,
	public ISupportErrorInfo,
	public IPersistStorageImpl<CEBImage>,
	public ISpecifyPropertyPagesImpl<CEBImage>,
	public IQuickActivateImpl<CEBImage>,
#ifndef _WIN32_WCE
	public IDataObjectImpl<CEBImage>,
#endif
	public IProvideClassInfo2Impl<&CLSID_EBImage, NULL, &LIBID_EBOleImageLib>,
#ifdef _WIN32_WCE // IObjectSafety is required on Windows CE for the control to be loaded correctly
	public IObjectSafetyImpl<CEBImage, INTERFACESAFE_FOR_UNTRUSTED_CALLER>,
#endif
	public CComCoClass<CEBImage, &CLSID_EBImage>,
	public CComControl<CEBImage>
{
public:

	CEBImage()
	{
		theGdiplusToken = 0;
#ifdef USES_THREAD_DRAW
		m_pDrawGifThread = NULL;
		m_bKilled = false;
#endif

		m_hParentHwnd = NULL;
		m_bShowImage = VARIANT_FALSE;
		m_image = NULL;
		memset(&m_sizePicture,0,sizeof(m_sizePicture));
		m_nFrameCount = 0;
		m_nCurrentFrame = 0;
		m_dwCurrentTime = 0;
		m_pFrameDelay = NULL;
		m_pImageAttributes = NULL;
	}
	STDMETHOD(DoVerb)(  
		_In_ LONG /* iVerb */,   
		_In_opt_ LPMSG /* pMsg */,   
		_Inout_ IOleClientSite* /* pActiveSite */,   
		_In_ LONG /* lindex */,  
		_In_ HWND /* hwndParent */,   
		_In_ LPCRECT /* lprcPosRect */)  
	{  
		return S_OK;  
	}  

DECLARE_OLEMISC_STATUS(OLEMISC_RECOMPOSEONRESIZE |
	OLEMISC_CANTLINKINSIDE |
	OLEMISC_INSIDEOUT |
	OLEMISC_ACTIVATEWHENVISIBLE |
	OLEMISC_SETCLIENTSITEFIRST
)

DECLARE_REGISTRY_RESOURCEID(IDR_EBIMAGE)


BEGIN_COM_MAP(CEBImage)
	COM_INTERFACE_ENTRY(IEBImage)
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

BEGIN_PROP_MAP(CEBImage)
	PROP_DATA_ENTRY("_cx", m_sizeExtent.cx, VT_UI4)
	PROP_DATA_ENTRY("_cy", m_sizeExtent.cy, VT_UI4)
	// Example entries
	// PROP_ENTRY_TYPE("Property Name", dispid, clsid, vtType)
	// PROP_PAGE(CLSID_StockColorPage)
END_PROP_MAP()


BEGIN_MSG_MAP(CEBImage)
	CHAIN_MSG_MAP(CComControl<CEBImage>)
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
			&IID_IEBImage,
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

// IEBImage
public:
	HRESULT OnDraw(ATL_DRAWINFO& di)
	{
		RECT& rc = *(RECT*)di.prcBounds;
		m_rcPos = rc;
		if (m_image)
		{
			HDC hDC = di.hdcDraw;
			Gdiplus::Graphics m_graphics(hDC);
			if (m_pImageAttributes == NULL)
			{
				m_graphics.DrawImage(m_image, rc.left,rc.top,m_sizePicture.cx,m_sizePicture.cy);
			}else
			{
				Gdiplus::Rect destRect(rc.left,rc.top,m_sizePicture.cx,m_sizePicture.cy);
				//Gdiplus::ImageAttributes pImageAttributes;
				//// 指定透明色
				//pImageAttributes.SetColorKey(Gdiplus::Color(255,255,255),Gdiplus::Color(255,255,255));
				// 80%透明
				//ColorMatrix colorMatrix = {1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.8f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f};
				//pImageAttributes.SetColorMatrix(&colorMatrix, ColorMatrixFlagsDefault, ColorAdjustTypeBitmap);
				m_graphics.DrawImage(m_image,destRect,0,0,m_sizePicture.cx,m_sizePicture.cy,Gdiplus::UnitPixel,m_pImageAttributes);
			}
		}
		return S_OK;
	}
private:
	ULONG_PTR theGdiplusToken;
#ifdef USES_THREAD_DRAW
	boost::thread * m_pDrawGifThread;
	bool m_bKilled;
#endif
	VARIANT_BOOL m_bShowImage;
	std::wstring m_sImageFile;
	HWND m_hParentHwnd;
	Gdiplus::Image * m_image;
	SIZE m_sizePicture;
	UINT m_nFrameCount;						// 图像多少帧
	UINT m_nCurrentFrame;					// 当前显示第几帧
	DWORD m_dwCurrentTime;					// 当前显示时间
	Gdiplus::PropertyItem* m_pFrameDelay;	// 每帧显示时间
	Gdiplus::ImageAttributes* m_pImageAttributes;	// 用于实现背景透明

public:
#ifdef USES_THREAD_DRAW
	static void draw_gif_thread_svr(void* lparam);
	void OnDrawGif(void);
#endif
	void DrawGif(DWORD dwNowTime);

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		Gdiplus::GdiplusStartupInput gdiplusStartupInput;
		Gdiplus::GdiplusStartup(&theGdiplusToken, &gdiplusStartupInput, NULL);
		return S_OK;
	}

	void FinalRelease()
	{
#ifdef USES_THREAD_DRAW
		m_bKilled = true;
		if (m_pDrawGifThread != NULL)
		{
			m_pDrawGifThread->join();
			delete m_pDrawGifThread;
			m_pDrawGifThread = NULL;
		}
#endif
		if (m_image)
		{
			delete m_image;
			m_image = NULL;
		}
		if (m_pFrameDelay)
		{
			free(m_pFrameDelay);
			m_pFrameDelay = NULL;
		}
		if (m_pImageAttributes)
		{
			delete m_pImageAttributes;
			m_pImageAttributes = NULL;
		}
		Gdiplus::GdiplusShutdown(theGdiplusToken);
	}
	STDMETHOD(LoadImageEx)(BSTR strImageFile,LONG hParentHwnd);
	STDMETHOD(get_ShowImage)(VARIANT_BOOL* pVal);
	STDMETHOD(put_ShowImage)(VARIANT_BOOL newVal);
	LRESULT OnMove(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	STDMETHOD(ShowNextFrame)(LONG nNowTime);
	STDMETHOD(GetImagePath)(BSTR* pOutImagePath);
	STDMETHOD(SetColorKey)(LONG colorLow, LONG colorHigh);
	STDMETHOD(ClearColorKey)(void);
};

OBJECT_ENTRY_AUTO(__uuidof(EBImage), CEBImage)
