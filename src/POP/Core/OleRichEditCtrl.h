#if !defined(AFX_OLERICHEDITCTRL_H__3DFF15EE_7336_4297_9620_7F00B611DAA1__INCLUDED_)
#define AFX_OLERICHEDITCTRL_H__3DFF15EE_7336_4297_9620_7F00B611DAA1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OleRichEditCtrl.h : header file
//

#define USES_IDB_BITMAP_MENU_TOOLBAR

#include <richole.h>
#include "NewMenu.h"

#define WM_DROP_FILE	(WM_USER+210)
#define WM_RICK_SAVEAS	(WM_USER+211)

//class COleRichEditCtrlCallback
//{
//public:
//	virtual void OnDropFile(const char* lpszFile) = 0;
//};

//UINT m_uOwnOleClipboardFormat = 50;

/////////////////////////////////////////////////////////////////////////////
// COleRichEditCtrl window

class COleRichEditCtrl : public CRichEditCtrl
{
// Construction
public:
	COleRichEditCtrl();
	virtual ~COleRichEditCtrl();

	//void SetCallback(COleRichEditCtrlCallback* callback) {m_pCallback = callback;}
	void InsertBitmap(HBITMAP hBitmap, int nIndex);
	bool InsertImage(const WCHAR* pImageFile, DWORD dwUserData);

	long StreamInFromResource(int iRes, LPCTSTR sType);
	void SaveToBitMap(LPMETAFILEPICT pMFP, const CString& strPath );
	static HANDLE DDBToDIB(const CBitmap& bitmap, DWORD dwCompression, CPalette* pPal=NULL);
	bool GetObjectData(long nIndex, char ** pOutData, DWORD & dwDataSize, DWORD& pOutUserData);
	void save();

protected:
	static DWORD CALLBACK readFunction(DWORD dwCookie,
		 LPBYTE lpBuf,			// the buffer to fill
		 LONG nCount,			// number of bytes to read
		 LONG* nRead);			// number of bytes actually read

	CComPtr<IRichEditOle> m_pRichEditOle;
	interface IExRichEditOleCallback;	// forward declaration (see below in this header file)

	//COleRichEditCtrlCallback* m_pCallback;
	IExRichEditOleCallback* m_pIRichEditOleCallback;
	BOOL m_bCallbackSet;
	
	int FindFirstObject(int cpMin, int nObjectCount);
	
	interface IExRichEditOleCallback : public IRichEditOleCallback
	{
	public:
		IExRichEditOleCallback(CRichEditCtrl * pOwner);
		virtual ~IExRichEditOleCallback();
		int m_iNumStorages;
		IStorage* pStorage;
		DWORD m_dwRef;

		virtual HRESULT STDMETHODCALLTYPE GetNewStorage(LPSTORAGE* lplpstg);
		virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, void ** ppvObject);
		virtual ULONG STDMETHODCALLTYPE AddRef();
		virtual ULONG STDMETHODCALLTYPE Release();
		virtual HRESULT STDMETHODCALLTYPE GetInPlaceContext(LPOLEINPLACEFRAME FAR *lplpFrame,
			LPOLEINPLACEUIWINDOW FAR *lplpDoc, LPOLEINPLACEFRAMEINFO lpFrameInfo);
 		virtual HRESULT STDMETHODCALLTYPE ShowContainerUI(BOOL fShow);
 		virtual HRESULT STDMETHODCALLTYPE QueryInsertObject(LPCLSID lpclsid, LPSTORAGE lpstg, LONG cp);
 		virtual HRESULT STDMETHODCALLTYPE DeleteObject(LPOLEOBJECT lpoleobj);
 		virtual HRESULT STDMETHODCALLTYPE QueryAcceptData(LPDATAOBJECT lpdataobj, CLIPFORMAT FAR *lpcfFormat,
			DWORD reco, BOOL fReally, HGLOBAL hMetaPict);
 		virtual HRESULT STDMETHODCALLTYPE ContextSensitiveHelp(BOOL fEnterMode);
 		virtual HRESULT STDMETHODCALLTYPE GetClipboardData(CHARRANGE FAR *lpchrg, DWORD reco, LPDATAOBJECT FAR *lplpdataobj);
 		virtual HRESULT STDMETHODCALLTYPE GetDragDropEffect(BOOL fDrag, DWORD grfKeyState, LPDWORD pdwEffect);
 		virtual HRESULT STDMETHODCALLTYPE GetContextMenu(WORD seltyp, LPOLEOBJECT lpoleobj, CHARRANGE FAR *lpchrg,
			HMENU FAR *lphmenu);
	private:
		CRichEditCtrl * m_pOwner;
		CNewMenu m_pMenu;
#ifdef USES_IDB_BITMAP_MENU_TOOLBAR
		CImageList m_pImageList;
#endif
	};
 

public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COleRichEditCtrl)
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:


	// Generated message map functions
protected:
	//{{AFX_MSG(COleRichEditCtrl)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
	//LRESULT OnDropFile(WPARAM wParam, LPARAM lParam);
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OLERICHEDITCTRL_H__3DFF15EE_7336_4297_9620_7F00B611DAA1__INCLUDED_)
