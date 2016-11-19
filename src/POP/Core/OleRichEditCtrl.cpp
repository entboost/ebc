// OleRichEditCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "OleRichEditCtrl.h"
#include "afxole.h"
#include <string>
#ifdef USES_IDB_BITMAP_MENU_TOOLBAR
#include <resource.h>
#endif

//#include "EBOleImage_i.h"
//#include "EBOleImage_i.c"
#include <Mmsystem.h>

#define TIMERID_DRAW_GIF 112
#define ID_RICH_UNDO		1101
#define ID_RICH_REDO		1102
#define ID_RICH_CUT			1103
#define ID_RICH_COPY		1104
#define ID_RICH_PASTE		1105
#define ID_RICH_SELALL		1106
#define ID_RICH_SAVEAS		1107

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COleRichEditCtrl

COleRichEditCtrl::COleRichEditCtrl()
{
	//m_pCallback = NULL;
	m_bCallbackSet = FALSE;
	m_pRichEditOle = NULL;
}

COleRichEditCtrl::~COleRichEditCtrl()
{
	// IExRichEditOleCallback class is a reference-counted class  
	// which deletes itself and for which delete should not be called

	// delete m_pIRichEditOleCallback;
}


BEGIN_MESSAGE_MAP(COleRichEditCtrl, CRichEditCtrl)
	//{{AFX_MSG_MAP(COleRichEditCtrl)
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	ON_WM_DESTROY()
	ON_WM_TIMER()
	//ON_MESSAGE(WM_DROP_FILE, OnDropFile)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


int COleRichEditCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
 	if (CRichEditCtrl::OnCreate(lpCreateStruct) == -1)
 		return -1;
	//ModifyStyleEx(0, WS_EX_TRANSPARENT);
 	
	// m_pIRichEditOleCallback should have been created in PreSubclassWindow

 	ASSERT( m_pIRichEditOleCallback != NULL );	

	// set the IExRichEditOleCallback pointer if it wasn't set 
	// successfully in PreSubclassWindow

	//m_uOwnOleClipboardFormat = RegisterClipboardFormat(_T("POP_CUSTOM_FORMAT"));   

	if ( !m_bCallbackSet )
	{
		SetOLECallback( m_pIRichEditOleCallback );
	}
	m_pRichEditOle = GetIRichEditOle();
 	ASSERT( m_pRichEditOle != NULL );	
	SetTimer(TIMERID_DRAW_GIF,50,NULL);
 	return 0;
}
//
//LRESULT COleRichEditCtrl::OnDropFile(WPARAM wParam, LPARAM lParam)
//{
//	const char * lpszFile = (const char*)wParam;
//	if (m_pCallback)
//		m_pCallback->OnDropFile(lpszFile);
//	return 1;
//}

void COleRichEditCtrl::PreSubclassWindow() 
{
	// base class first
	CRichEditCtrl::PreSubclassWindow();	

	m_pIRichEditOleCallback = NULL;
	m_pIRichEditOleCallback = new IExRichEditOleCallback(this);
	ASSERT( m_pIRichEditOleCallback != NULL );
	m_bCallbackSet = SetOLECallback( m_pIRichEditOleCallback );
}


long COleRichEditCtrl::StreamInFromResource(int iRes, LPCTSTR sType)
{
	HINSTANCE hInst = AfxGetInstanceHandle();
	HRSRC hRsrc = ::FindResource(hInst,
		MAKEINTRESOURCE(iRes), sType);
	
	DWORD len = SizeofResource(hInst, hRsrc); 
	BYTE* lpRsrc = (BYTE*)LoadResource(hInst, hRsrc); 
	ASSERT(lpRsrc); 
 
	CMemFile mfile;
	mfile.Attach(lpRsrc, len); 

	EDITSTREAM es;
	es.pfnCallback = readFunction;
	es.dwError = 0;
	es.dwCookie = (DWORD) &mfile;

	return StreamIn( SF_RTF, es );
}

void COleRichEditCtrl::InsertBitmap(HBITMAP hBitmap, int nIndex)
{
	STGMEDIUM stgm;
	stgm.tymed = TYMED_GDI;    // Storage medium = HBITMAP handle
	stgm.hBitmap = hBitmap;
	stgm.pUnkForRelease = NULL; // Use ReleaseStgMedium

	FORMATETC fm;
	fm.cfFormat = CF_BITMAP;    // Clipboard format = CF_BITMAP
	fm.ptd = NULL;       // Target Device = Screen
	fm.dwAspect = DVASPECT_CONTENT;   // Level of detail = Full content
	fm.lindex = -1;       // Index = Not applicaple
	fm.tymed = TYMED_GDI;  

	IStoragePtr pStorage;
	ILockBytesPtr pLockBytes;
	SCODE sc = ::CreateILockBytesOnHGlobal(NULL, TRUE, &pLockBytes);
	if (sc != S_OK || pLockBytes == NULL)
	{
		//AfxThrowOleException(sc);
		return;
	}
	ASSERT(pLockBytes != NULL);
	sc = ::StgCreateDocfileOnILockBytes(pLockBytes,
		STGM_SHARE_EXCLUSIVE|STGM_CREATE|STGM_READWRITE, 0, &pStorage);
	if (sc != S_OK)
	{
		//AfxThrowOleException(sc);
		return;
	}
	ASSERT(pStorage != NULL);

	COleDataSource *pDataSource = new COleDataSource();
	pDataSource->CacheData(CF_BITMAP, &stgm);
	IDataObjectPtr lpDataObject = (LPDATAOBJECT)pDataSource->GetInterface(&IID_IDataObject);

	//获取RichEdit的OLEClientSite
	IOleClientSitePtr lpClientSite;
	CComPtr<IRichEditOle>	pRichEditOle;
	pRichEditOle = GetIRichEditOle();
	if(NULL == (IRichEditOle*)pRichEditOle)
	{
		lpDataObject.Release();
		delete pDataSource;
		return;
	}
	pRichEditOle->GetClientSite( &lpClientSite );
	if (lpClientSite == NULL)
	{
		lpDataObject.Release();
		delete pDataSource;
		return;
	}
	ASSERT(lpClientSite != NULL);

	//创建OLE对象
	IOleObjectPtr pOleObject;
	sc = ::OleCreateStaticFromData(lpDataObject,IID_IOleObject,OLERENDER_FORMAT,
		&fm,lpClientSite,pStorage,(void **)&pOleObject);
	if(sc!=S_OK)
	{
		//AfxThrowOleException(sc);
		lpDataObject.Release();
		delete pDataSource;
		return;
	}

	//插入OLE对象
	REOBJECT reobject;
	ZeroMemory(&reobject, sizeof(REOBJECT));
	reobject.cbStruct = sizeof(REOBJECT);

	CLSID clsid;
	sc = pOleObject->GetUserClassID(&clsid);
	if (sc != S_OK)
	{
		//AfxThrowOleException(sc);
		lpDataObject.Release();
		delete pDataSource;
		return;
	}
	reobject.clsid = clsid;
	reobject.cp = REO_CP_SELECTION;
	reobject.dvaspect = DVASPECT_CONTENT;
	reobject.poleobj = pOleObject;
	reobject.polesite = lpClientSite;
	reobject.pstg = pStorage;
	reobject.dwUser = nIndex; //save inner index
	HRESULT hr = pRichEditOle->InsertObject( &reobject );

	lpDataObject.Release();
	delete pDataSource;
	pDataSource = NULL;
	RedrawWindow();
}
HBITMAP LoadAnImage(const WCHAR* FileName)
{
	////////////////////////////////////
	IPicture *pPic = NULL;
	USES_CONVERSION;
	HRESULT hr = ::OleLoadPicturePath( const_cast<LPOLESTR>(W2OLE((LPWSTR)FileName)),
		NULL,
		0,
		0,
		IID_IPicture,
		reinterpret_cast<LPVOID*>(&pPic));
	if ( SUCCEEDED(hr) && pPic != NULL)
	{
		HBITMAP hbmp =NULL;
		pPic->get_Handle((UINT *)&hbmp);
		HBITMAP hBB = (HBITMAP) CopyImage( hbmp, IMAGE_BITMAP, 0, 0, LR_COPYRETURNORG);
		pPic->Release();
		return hBB;
	}
	return NULL;
}
bool COleRichEditCtrl::InsertImage(const WCHAR* pImageFile, DWORD dwUserData)
{
	IStorage*  lpStorage  = NULL;//存储接口
	IOleObject*  lpOleObject  = NULL;//OLE对象
	LPLOCKBYTES  lpLockBytes  = NULL;//LOCKBYTE
	IOleClientSite* lpOleClientSite = NULL;
	CComPtr<IEBImage> IPic;
	CLSID   clsid;
	REOBJECT  reobject;
	HRESULT   hr;

	if(m_pRichEditOle == NULL)
		return false;
	hr = ::CoCreateInstance(__uuidof(EBImage),NULL,CLSCTX_INPROC,__uuidof(IEBImage),(LPVOID*)&IPic);
	if( IPic == NULL )
	{
		return false;
	}

	bool bRet = true;
	IPic->LoadImageEx(_bstr_t(pImageFile),(LONG)this->GetSafeHwnd()); //装载要显示的图像

	try{
		hr = IPic->QueryInterface(IID_IOleObject, (void**)&lpOleObject);//获得数据对象接口
		if( hr != S_OK )
			AfxThrowOleException(hr);
		hr = lpOleObject->GetUserClassID(&clsid);
		if ( hr != S_OK)
			AfxThrowOleException(hr);

		hr = ::CreateILockBytesOnHGlobal(NULL, TRUE, &lpLockBytes);//创建LOCKBYTE对象
		if (hr != S_OK)
			AfxThrowOleException(hr);
		ASSERT(lpLockBytes != NULL);

		hr = ::StgCreateDocfileOnILockBytes(lpLockBytes,//创建复合文档
			STGM_SHARE_EXCLUSIVE|STGM_CREATE|STGM_READWRITE, 0, &lpStorage);
		if (hr != S_OK)
		{
			VERIFY(lpLockBytes->Release() == 0);
			lpLockBytes = NULL;
			AfxThrowOleException(hr);
		}

		m_pRichEditOle->GetClientSite(&lpOleClientSite);

		ZeroMemory(&reobject, sizeof(REOBJECT));//初始化一个对象 
		reobject.cbStruct = sizeof(REOBJECT);
		reobject.clsid  = clsid;
		reobject.cp   = REO_CP_SELECTION;
		reobject.dvaspect = DVASPECT_CONTENT;
		reobject.dwFlags = REO_BELOWBASELINE;
		reobject.poleobj = lpOleObject;
		reobject.polesite = lpOleClientSite;
		reobject.pstg  = lpStorage;
		reobject.dwUser = dwUserData;

		lpOleObject->SetClientSite(lpOleClientSite);//
		hr = m_pRichEditOle->InsertObject( &reobject );
		if (hr != S_OK)
			AfxThrowOleException(hr);
		IPic->put_ShowImage(VARIANT_TRUE);
		OleSetContainedObject(lpOleObject,TRUE);
		//HWND hWndRT = m_richInput.m_hWnd;
		//::SendMessage(hWndRT, EM_SCROLLCARET, (WPARAM)0, (LPARAM)0);
		//VARIANT_BOOL ret;
		////do frame changing
		////ret = m_lpAnimator->TriggerFrameChange();
		////show it
		//lpOleObject->DoVerb(OLEIVERB_UIACTIVATE, NULL, lpOleClientSite, 0, hWndRT, NULL);
		//lpOleObject->DoVerb(OLEIVERB_SHOW, NULL, lpOleClientSite, 0, hWndRT, NULL);

		//IPic->Load(_bstr_t(sFileName.c_str())); //装载要显示的图像
		//lpOleClientSite->RequestNewObjectLayout();
		//lpOleObject->DoVerb

		//SIZEL size;
		//lpOleObject->GetExtent(DVASPECT_CONTENT,&size);
		//int i=0;

		//IOleInPlaceSite * pOldInPlaceSite;
		//hr = lpOleObject->QueryInterface(IID_IOleInPlaceSite, (void**)&pOldInPlaceSite);//获得数据对象接口
		//if (hr == S_OK)
		//{
		//	RECT rect;
		//	pOldInPlaceSite->OnPosRectChange(&rect);
		//	IOleInPlaceObject * pOldInPlaceObject;
		//	hr = lpOleObject->QueryInterface(IID_IOleInPlaceObject, (void**)&pOldInPlaceObject);//获得数据对象接口
		//	if (hr == S_OK)
		//	{
		//		pOldInPlaceObject->SetObjectRects(&rect,&rect);
		//	}
		//}
	}

	catch( COleException* e )
	{
		TRACE(_T("OleException code:%d"),e->m_sc);
		e->Delete();
		bRet = false;
	}

	// release the interface
	//if( IPic     != NULL )  IPic->Release();
	if (lpLockBytes != NULL) lpLockBytes->Release();
	if( lpOleObject  != NULL )  lpOleObject->Release();
	if( lpOleClientSite != NULL ) lpOleClientSite->Release();
	if( lpStorage  != NULL ) lpStorage->Release();

	//HBITMAP hbmp = LoadAnImage(pImageFile);
	//if (hbmp == NULL)
	//	return;
	//InsertBitmap(hbmp, nIndex);
	//DeleteObject(hbmp);
	return bRet;
}


/* static */
DWORD CALLBACK COleRichEditCtrl::readFunction(DWORD dwCookie,
		 LPBYTE lpBuf,			// the buffer to fill
		 LONG nCount,			// number of bytes to read
		 LONG* nRead)			// number of bytes actually read
{
	CFile* fp = (CFile *)dwCookie;
	*nRead = fp->Read(lpBuf,nCount);
	return 0;
}

/////////////////////////////////////////////////////////////////////////////

COleRichEditCtrl::IExRichEditOleCallback::IExRichEditOleCallback(CRichEditCtrl * pOwner)
: m_pOwner(pOwner)
{
	pStorage = NULL;
	m_iNumStorages = 0;
	m_dwRef = 0;

	// set up OLE storage

	HRESULT hResult = ::StgCreateDocfile(NULL,
		STGM_TRANSACTED | STGM_READWRITE | STGM_SHARE_EXCLUSIVE /*| STGM_DELETEONRELEASE */|STGM_CREATE ,
		0, &pStorage );

	if ( pStorage == NULL ||
		hResult != S_OK )
	{
		AfxThrowOleException( hResult );
	}
}

COleRichEditCtrl::IExRichEditOleCallback::~IExRichEditOleCallback()
{
	if (m_pMenu.GetSafeHmenu())
		m_pMenu.DestroyMenu();
}

HRESULT STDMETHODCALLTYPE 
COleRichEditCtrl::IExRichEditOleCallback::GetNewStorage(LPSTORAGE* lplpstg)
{
	// add by HD
	//COleClientItem item;
	//item.GetItemStorageFlat();
	//*lplpstg = item.m_lpStorage;
	//HRESULT hRes = E_OUTOFMEMORY;
	//if (item.m_lpStorage != NULL)
	//{
	//	item.m_lpStorage = NULL;
	//	hRes = S_OK;
	//}
	//return hRes;

	BOOL bText = IsClipboardFormatAvailable(CF_TEXT);
	BOOL bBitmap = IsClipboardFormatAvailable(CF_BITMAP);
	BOOL bFile = IsClipboardFormatAvailable(CF_HDROP);
	

	m_iNumStorages++;
	WCHAR tName[50];
	swprintf(tName, L"REOLEStorage%d", m_iNumStorages);

	HRESULT hResult = pStorage->CreateStorage(tName, 
		STGM_TRANSACTED | STGM_READWRITE | STGM_SHARE_EXCLUSIVE | STGM_CREATE ,
		0, 0, lplpstg );

	if (hResult != S_OK )
	{
		::AfxThrowOleException( hResult );
	}

	return hResult;
}

HRESULT STDMETHODCALLTYPE 
COleRichEditCtrl::IExRichEditOleCallback::QueryInterface(REFIID iid, void ** ppvObject)
{

	HRESULT hr = S_OK;
	*ppvObject = NULL;
	
	if ( iid == IID_IUnknown ||
		iid == IID_IRichEditOleCallback )
	{
		*ppvObject = this;
		AddRef();
		hr = NOERROR;
	}
	else
	{
		hr = E_NOINTERFACE;
	}

	return hr;
}



ULONG STDMETHODCALLTYPE 
COleRichEditCtrl::IExRichEditOleCallback::AddRef()
{
	return ++m_dwRef;
}



ULONG STDMETHODCALLTYPE 
COleRichEditCtrl::IExRichEditOleCallback::Release()
{
	if ( --m_dwRef == 0 )
	{
		delete this;
		return 0;
	}

	return m_dwRef;
}


HRESULT STDMETHODCALLTYPE 
COleRichEditCtrl::IExRichEditOleCallback::GetInPlaceContext(LPOLEINPLACEFRAME FAR *lplpFrame,
	LPOLEINPLACEUIWINDOW FAR *lplpDoc, LPOLEINPLACEFRAMEINFO lpFrameInfo)
{
	return S_OK;
}


HRESULT STDMETHODCALLTYPE 
COleRichEditCtrl::IExRichEditOleCallback::ShowContainerUI(BOOL fShow)
{
	return S_OK;
}



HRESULT STDMETHODCALLTYPE 
COleRichEditCtrl::IExRichEditOleCallback::QueryInsertObject(LPCLSID lpclsid, LPSTORAGE lpstg, LONG cp)
{
	//if (CLSID_DynamicGif == *lpclsid)
	//{
	//	return S_OK;
	//}
	return S_OK;
}


HRESULT STDMETHODCALLTYPE 
COleRichEditCtrl::IExRichEditOleCallback::DeleteObject(LPOLEOBJECT lpoleobj)
{
	return S_OK;
}



HRESULT STDMETHODCALLTYPE 
COleRichEditCtrl::IExRichEditOleCallback::QueryAcceptData(LPDATAOBJECT lpdataobj, CLIPFORMAT FAR *lpcfFormat,
	DWORD reco, BOOL fReally, HGLOBAL hMetaPict)
{
//	if (!bReally)   // just query  
//	{  
//		//return E_NOTIMPL;  
//
//		return  S_OK;  
//	}  
//
	//只处理粘贴  
	switch(reco)  
	{
	case RECO_DROP:
		//return E_NOTIMPL;	// *win8同样不行
		return S_FALSE;		// 外面主窗口接收文件（win8没有拖拉事件）
	case RECO_PASTE:  
		{
			if (fReally)	// 只查询，不处理
			{
				COleDataObject odo;  
				odo.Attach(lpdataobj);
				BOOL bFileAvailable = odo.IsDataAvailable(CF_HDROP);
				if (bFileAvailable)
				{
					HDROP hDrop = (HDROP)odo.GetGlobalData(CF_HDROP);
					if (hDrop==NULL) return S_FALSE;
					int nFileSum = min(12,DragQueryFile(hDrop,0xFFFFFFFF,NULL,0));   //得到文件个数
					for (int i=0; i<nFileSum; i++)
					{
						char lpszFile[MAX_PATH];
						memset(lpszFile,0,MAX_PATH);
						DragQueryFile(hDrop,i,lpszFile,MAX_PATH);         //得到文件名
						m_pOwner->GetParent()->SendMessage(WM_DROP_FILE,(WPARAM)lpszFile);
						//m_pOwner->SendMessage(WM_DROP_FILE,(WPARAM)lpszFile);
					}
					return S_FALSE;
				}
			}
			//CFile * pFile = odo.GetFileData(CF_HDROP);
			//STGMEDIUM stg;
			//memset(&stg,0,sizeof(STGMEDIUM));
			//FORMATETC f;
			//memset(&f,0,sizeof(FORMATETC));
			//BOOL ret3 = odo.GetData(CF_HDROP, &stg, &f);
			//odo.AttachClipboard

			//if (OpenClipboard())
			//{
			//	char lpszFormatName[250];
			//	GetClipboardFormatName(CF_HDROP,lpszFormatName,250);
			//	CloseClipboard();
			//}

			//mimeData
			//f.
			//if (odo.GetNextFormat(&f))
			//{
			//	//CF_ENHMETAFILE
			//	if (f.cfFormat == CF_TEXT)
			//	{

			//	}

			//}
		}break;
	default:
		break;
	}

	return S_OK;
}


HRESULT STDMETHODCALLTYPE 
COleRichEditCtrl::IExRichEditOleCallback::ContextSensitiveHelp(BOOL fEnterMode)
{
	return S_OK;
}



HRESULT STDMETHODCALLTYPE 
COleRichEditCtrl::IExRichEditOleCallback::GetClipboardData(CHARRANGE FAR *lpchrg, DWORD reco, LPDATAOBJECT FAR *lplpdataobj)
{
	if (reco==RECO_COPY || reco==RECO_CUT)  
	{
		//获得lpchrg对应的richedit的内容  
		m_pOwner->SetSel(lpchrg->cpMin,lpchrg->cpMax);
		const DWORD nSelectionType = m_pOwner->GetSelectionType();
		if ((nSelectionType&SEL_OBJECT)==SEL_OBJECT)
			return E_NOTIMPL;

		const CString strText(m_pOwner->GetSelText());

	//	//code  text,存入剪贴板的为string ，通过XML编码string  
	//	string   strCodedText=ToCodedString(* lpchrg,strText);  
		const tstring strCodedText(strText);


	//	//创建一个 DataSource  
		COleDataSource *pDataSource = new COleDataSource;  

		int  strBytes=  strCodedText.length();  
		HGLOBAL hG = GlobalAlloc(GMEM_DDESHARE, strBytes+1);  
		void* pBuffer = GlobalLock(hG);  
		{  
			memcpy(pBuffer, strCodedText.c_str(), strBytes);  
			GlobalUnlock(hG);  
		}  

		FORMATETC fmt;  
		fmt.cfFormat = CF_TEXT;//m_uOwnOleClipboardFormat;  
		fmt.dwAspect = DVASPECT_CONTENT;  
		fmt.lindex = -1;  
		fmt.ptd = NULL;  
		fmt.tymed = TYMED_HGLOBAL;  

		STGMEDIUM stg;  
		stg.tymed = TYMED_HGLOBAL;  
		stg.hGlobal = hG;  
		stg.pUnkForRelease = NULL;  

		pDataSource->CacheData(CF_TEXT/*m_uOwnOleClipboardFormat*/,&stg, &fmt);  
		//将 pDataSource的 IDataObject接口赋值给 lplpdataobj  
		*lplpdataobj= (IDataObject *)pDataSource->GetInterface(&IID_IDataObject);
		return  S_OK;  
	}  

	//return E_NOTIMPL; // add by HD
	return S_OK;
}


HRESULT STDMETHODCALLTYPE 
COleRichEditCtrl::IExRichEditOleCallback::GetDragDropEffect(BOOL fDrag, DWORD grfKeyState, LPDWORD pdwEffect)
{
	//if (!fDrag)
	//{
	//	//int nFileSum = DragQueryFile(hDropInfo,0xFFFFFFFF,NULL,0);   //得到文件个数
	//	//for (int i=0; i<nFileSum; i++)
	//	//{
	//	//	char lpszFile[MAX_PATH];
	//	//	memset(lpszFile,0,MAX_PATH);
	//	//	DragQueryFile(hDropInfo,i,lpszFile,MAX_PATH);         //得到文件名
	//	//}
	//	int i=0;
	//}
	return S_OK;
}

HRESULT STDMETHODCALLTYPE 
COleRichEditCtrl::IExRichEditOleCallback::GetContextMenu(WORD seltyp, LPOLEOBJECT lpoleobj, CHARRANGE FAR *lpchrg,
														 HMENU FAR *lphmenu)
{
	//创建一个弹出式菜单  
	if (m_pMenu.GetSafeHmenu()==NULL)
	{
		m_pMenu.CreatePopupMenu();
#ifdef USES_IDB_BITMAP_MENU_TOOLBAR
		m_pImageList.Create(IDB_BITMAP_MENU_TOOLBAR,16,1,RGB(192,192,192));
		m_pMenu.SetIconSize (16,16);
		m_pMenu.AppendODMenu(_T("撒销(&U)"),MF_BYCOMMAND,ID_RICH_UNDO,&m_pImageList,3);
#else
		m_pMenu.AppendMenu(0, ID_RICH_UNDO, TEXT("撒销(&U)"));
#endif
		//m_pMenu.AppendMenu(0, ID_RICH_REDO, TEXT("重做(&O)\tCtrl+Y"));
		m_pMenu.AppendMenu(MF_SEPARATOR);
#ifdef USES_IDB_BITMAP_MENU_TOOLBAR
		m_pMenu.AppendODMenu(_T("剪切(&X)"),MF_BYCOMMAND,ID_RICH_CUT,&m_pImageList,0);
		m_pMenu.AppendODMenu(_T("复制(&C)"),MF_BYCOMMAND,ID_RICH_COPY,&m_pImageList,1);
		m_pMenu.AppendODMenu(_T("粘贴(&V)"),MF_BYCOMMAND,ID_RICH_PASTE,&m_pImageList,2);
#else
		m_pMenu.AppendMenu(0, ID_RICH_CUT, TEXT("剪切(&X)"));
		m_pMenu.AppendMenu(0, ID_RICH_COPY, TEXT("复制(&C)"));
		m_pMenu.AppendMenu(0, ID_RICH_PASTE, TEXT("粘贴(&V)"));
#endif
		m_pMenu.AppendMenu(MF_SEPARATOR);
		m_pMenu.AppendMenu(0, ID_RICH_SELALL, TEXT("全选(&A)"));
		m_pMenu.AppendMenu(MF_SEPARATOR);
		m_pMenu.AppendMenu(0, ID_RICH_SAVEAS, TEXT("图片另存为..."));
		
		//m_pMenu.AppendMenu(MF_SEPARATOR);
		//m_pMenu.AppendMenu(0, ID_RICH_SETFONT, TEXT("选择字体"));
	}
	const UINT nUndo = (m_pOwner->CanUndo() ? 0 : MF_GRAYED);
	//const UINT nRedo = (m_pOwner->CanRedo() ? 0 : MF_GRAYED);  
	const UINT nSel = ((m_pOwner->GetSelectionType() != SEL_EMPTY) ? 0 : MF_GRAYED);  
	const UINT nPaste = ((m_pOwner->CanPaste()||IsClipboardFormatAvailable(CF_BITMAP)|| IsClipboardFormatAvailable(CF_TEXT/*m_uOwnOleClipboardFormat*/)) ? 0 : MF_GRAYED);  
	//const UINT nSendFile = ((m_pOwner->CanPaste()||IsClipboardFormatAvailable(CF_HDROP)) ? 0 : MF_GRAYED);  
	const UINT nSaveAs = ((m_pOwner->GetSelectionType() == SEL_OBJECT) ? 0 : MF_GRAYED);  


	m_pMenu.EnableMenuItem(ID_RICH_UNDO, MF_BYCOMMAND|nUndo);  
	//m_pMenu.EnableMenuItem(ID_RICH_REDO, MF_BYCOMMAND|nRedo);  
	m_pMenu.EnableMenuItem(ID_RICH_CUT, MF_BYCOMMAND|nSel);  
	m_pMenu.EnableMenuItem(ID_RICH_COPY, MF_BYCOMMAND|nSel);      
	m_pMenu.EnableMenuItem(ID_RICH_PASTE, MF_BYCOMMAND|nPaste);  
	m_pMenu.EnableMenuItem(ID_RICH_SAVEAS, MF_BYCOMMAND|nSaveAs);  

	//if(seltype == SEL_OBJECT)  
	//{  
	//	m_pMenu.AppendMenu(MF_SEPARATOR);  
	//	m_pMenu.AppendMenu(MF_STRING, IDM_CHAT_DLG_SAVE_OLE_IMG, TEXT("另存为..."));  
	//}  

	//显示菜单  
	POINT pt;  
	GetCursorPos(&pt);  
	DWORD dwCmd = m_pMenu.TrackPopupMenu(TPM_LEFTALIGN|TPM_TOPALIGN|TPM_RETURNCMD, pt.x, pt.y, m_pOwner);  
	switch(dwCmd)  
	{
	case ID_RICH_UNDO:
		m_pOwner->Undo();
		break;
	//case ID_RICH_REDO:	// **有问题
	//	m_pOwner->Redo();
	//	break;
	case ID_RICH_COPY:
		m_pOwner->Copy();
		break;
	case ID_RICH_CUT:  
		m_pOwner->Cut();
		break;
	case ID_RICH_PASTE:  
		m_pOwner->Paste();
		break;
	case ID_RICH_SAVEAS:
		{
			m_pOwner->GetParent()->PostMessage(WM_RICK_SAVEAS,(WPARAM)m_pOwner->GetSafeHwnd(),0);
		}break;
	//case IDM_CHAT_DLG_SAVE_OLE_IMG:  
	//	{  
	//		CComPtr<IGGGifCtrl>  pGifCtrl;  
	//		HRESULT hr = lpoleobj->QueryInterface(&pGifCtrl);  
	//		if(SUCCEEDED(hr))  
	//		{  
	//			if(pGifCtrl)  
	//			{  
	//				BSTR bstrFile;  
	//				pGifCtrl->GetFilePath(&bstrFile);  
	//				// 保存文件到另外一个文件，这里控件根据控件中文件类型的不同设置  
	//				// 不同的扩展名，如果采用对话框的形式保存文件时注意分析文件的扩展名，来正确的保存文件类型。  
	//				if(_bstr_t(bstrFile).length())  
	//				{  
	//					CString strSrcFilePath = bstrFile;  
	//					SaveOleImgToFile(strSrcFilePath);  
	//				}  
	//			}  
	//		}  
	//		break;  
	//	}
	case ID_RICH_SELALL:
		m_pOwner->SetSel(0,-1);
		break;
	default:  
		break;  
	}  

	return S_OK;
}

//BOOL SaveToFile( IOleObject* pOleObject, LPCWSTR wszDstFile)
//{
//	// 要保存的目标文件名称
//	BOOL bRet = FALSE;
//	CComPtr<IDynamicGif> spDynGif;
//	HRESULT hr = pOleObject->QueryInterface( &spDynGif );
//	if( SUCCEEDED(hr) )
//	{
//		CComBSTR bstrFile;
//		// 注意在对象的生存期内，对应的文件不能删除，
//		// 否则在保存到文件或者拷贝时会失败
//		hr = spDynGif->GetFilePath( &bstrFile );
//		if( bstrFile.Length() > 0 )
//		{
//			bRet = CopyFileW( (BSTR)bstrFile,wszDstFile,FALSE);
//		}
//	}
//	retur bRet;
//}
void COleRichEditCtrl::SaveToBitMap(LPMETAFILEPICT pMFP, const CString& strPath)
{
	SIZE size;
	CDC* pDC = GetDC();
	if (pDC)
	{
		size.cx = pMFP->xExt;
		size.cy = pMFP->yExt;
		pDC->HIMETRICtoDP(&size);

		CBitmap bm;
		bm.CreateCompatibleBitmap(pDC, abs(size.cx), abs(size.cy));

		CDC memdc;
		memdc.CreateCompatibleDC(pDC);
		CBitmap* pOldBitmap = memdc.SelectObject(&bm);

		memdc.SetMapMode(pMFP->mm) ;
		memdc.SetViewportExt(abs(size.cx), abs(size.cy)) ;
		memdc.SetViewportOrg(0, 0);
		memdc.PlayMetaFile(pMFP->hMF);
		CImage img;
		img.Attach((HBITMAP)bm.GetSafeHandle());
		img.Save(strPath);
		memdc.SelectObject(pOldBitmap);
		memdc.DeleteDC();
		bm.DeleteObject();
		ReleaseDC(pDC);
	}
}
HANDLE COleRichEditCtrl::DDBToDIB(const CBitmap& bitmap, DWORD dwCompression, CPalette* pPal)
{
	BITMAP bm;
	BITMAPINFOHEADER bi;
	LPBITMAPINFOHEADER  lpbi;
	DWORD dwLen;
	HANDLE hDIB;
	HANDLE handle;
	HDC  hDC;
	HPALETTE hPal;


	ASSERT( bitmap.GetSafeHandle() );

	// The function has no arg for bitfields
	if( dwCompression == BI_BITFIELDS )
		return NULL;

	// If a palette has not been supplied use defaul palette
	hPal = pPal==NULL?NULL:(HPALETTE) pPal->GetSafeHandle();
	if (hPal==NULL)
		hPal = (HPALETTE) GetStockObject(DEFAULT_PALETTE);

	// Get bitmap information
	bitmap.GetObject(sizeof(bm),(LPSTR)&bm);

	// Initialize the bitmapinfoheader
	bi.biSize = sizeof(BITMAPINFOHEADER);
	bi.biWidth = bm.bmWidth;
	bi.biHeight  = bm.bmHeight;
	bi.biPlanes  = 1;
	bi.biBitCount = bm.bmPlanes * bm.bmBitsPixel;
	bi.biCompression = dwCompression;
	bi.biSizeImage = 0;
	bi.biXPelsPerMeter = 0;
	bi.biYPelsPerMeter = 0;
	bi.biClrUsed = 0;
	bi.biClrImportant = 0;

	// Compute the size of the  infoheader and the color table
	int nColors = (1 << bi.biBitCount);
	if( nColors > 256 )
		nColors = 0;
	dwLen  = bi.biSize + nColors * sizeof(RGBQUAD);

	// We need a device context to get the DIB from
	hDC = ::GetDC(NULL);
	hPal = ::SelectPalette(hDC,hPal,FALSE);
	::RealizePalette(hDC);

	// Allocate enough memory to hold bitmapinfoheader and color table
	hDIB = GlobalAlloc(GMEM_FIXED,dwLen);

	if (!hDIB){
		SelectPalette(hDC,hPal,FALSE);
		::ReleaseDC(NULL,hDC);
		return NULL;
	}

	lpbi = (LPBITMAPINFOHEADER)hDIB;

	*lpbi = bi;

	// Call GetDIBits with a NULL lpBits param, so the device driver
	// will calculate the biSizeImage field
	GetDIBits(hDC, (HBITMAP)bitmap.GetSafeHandle(), 0L, (DWORD)bi.biHeight,
		(LPBYTE)NULL, (LPBITMAPINFO)lpbi, (DWORD)DIB_RGB_COLORS);

	bi = *lpbi;

	// If the driver did not fill in the biSizeImage field, then compute it
	// Each scan line of the image is aligned on a DWORD (32bit) boundary
	if (bi.biSizeImage == 0){
		bi.biSizeImage = ((((bi.biWidth * bi.biBitCount) + 31) & ~31) / 8)
			* bi.biHeight;

		// If a compression scheme is used the result may infact be larger
		// Increase the size to account for this.
		if (dwCompression != BI_RGB)
			bi.biSizeImage = (bi.biSizeImage * 3) / 2;
	}

	// Realloc the buffer so that it can hold all the bits
	dwLen += bi.biSizeImage;
	if (handle = GlobalReAlloc(hDIB, dwLen, GMEM_MOVEABLE))
		hDIB = handle;
	else{
		GlobalFree(hDIB);

		// Reselect the original palette
		SelectPalette(hDC,hPal,FALSE);
		::ReleaseDC(NULL,hDC);
		return NULL;
	}

	// Get the bitmap bits
	lpbi = (LPBITMAPINFOHEADER)hDIB;

	// FINALLY get the DIB
	BOOL bGotBits = GetDIBits( hDC, (HBITMAP)bitmap.GetSafeHandle(),
		0L, // Start scan line
		(DWORD)bi.biHeight, // # of scan lines
		(LPBYTE)lpbi  // address for bitmap bits
		+ (bi.biSize + nColors * sizeof(RGBQUAD)),
		(LPBITMAPINFO)lpbi, // address of bitmapinfo
		(DWORD)DIB_RGB_COLORS); // Use RGB for color table

	if( !bGotBits )
	{
		GlobalFree(hDIB);

		SelectPalette(hDC,hPal,FALSE);
		::ReleaseDC(NULL,hDC);
		return NULL;
	}

	SelectPalette(hDC,hPal,FALSE);
	::ReleaseDC(NULL,hDC);
	return hDIB;
}

bool COleRichEditCtrl::GetObjectData(long nIndex, char ** pOutData, DWORD & dwDataSize, DWORD& pOutUserData)
{
	// REO_IOB_SELECTION
	LONG nNumber = m_pRichEditOle->GetObjectCount();  //Your Images' Number
	if (nIndex >= nNumber)
		return false;

	REOBJECT reobject;
	memset( &reobject, 0 , sizeof(REOBJECT) );
	reobject.cbStruct = sizeof(REOBJECT);
	HRESULT hr = m_pRichEditOle->GetObject(nIndex, &reobject, REO_GETOBJ_ALL_INTERFACES);
	if(FAILED(hr)) return false;

	pOutUserData = reobject.dwUser;
	if (reobject.dwUser != 0)
	{
		// 这里是资源ID，后面不处理，在外面处理
		return true;
	}

	IDataObject* lpDataObject;
	hr = (reobject.poleobj)->QueryInterface(IID_IDataObject, (void **)&lpDataObject);
	if(FAILED(hr)) return false;

	dwDataSize = 0;
	IEnumFORMATETC *pEnumFmt = NULL;
	if (S_OK != lpDataObject->EnumFormatEtc(DATADIR_GET,&pEnumFmt))
	{
		//const DWORD dwError = GetLastError();
		STGMEDIUM sm;  //out
		FORMATETC fm;    //in
		fm.cfFormat = CF_DIB;  // CF_BITMAP // Clipboard format
		fm.ptd = NULL;         // Target Device = Screen
		fm.dwAspect = DVASPECT_CONTENT;
		// Level of detail = Full content
		fm.lindex = -1;        // Index = Not applicaple
		fm.tymed = TYMED_HGLOBAL;	// TYMED_GDI;//
		//fm.tymed = TYMED_FILE|TYMED_GDI|TYMED_HGLOBAL;	// TYMED_GDI;//
		if (lpDataObject->GetData(&fm, &sm)==S_OK)
		{
			dwDataSize = ::GlobalSize(sm.hGlobal);
			*pOutData = new char[dwDataSize];
			::CopyMemory(*pOutData, (LPVOID)::GlobalLock(sm.hGlobal), dwDataSize);
			::GlobalUnlock(sm.hGlobal);
			if (sm.pUnkForRelease == NULL)
			{
				GlobalFree(sm.hGlobal);
			}
			::ReleaseStgMedium(&sm);
		}
		lpDataObject->Release();
		return dwDataSize==0?false:true;
	}

	FORMATETC fm;  
	STGMEDIUM sm;   

	HRESULT Ret = S_OK;
	ULONG Fetched;
	while(Ret == S_OK)
	{
		HRESULT ret=pEnumFmt->Next(1,&fm,&Fetched);
		if(SUCCEEDED(ret))
		{
			if(fm.cfFormat == CF_METAFILEPICT //媒体
				||fm.cfFormat == CF_BITMAP //位图
				|| fm.cfFormat == CF_DIB) //DIB
			{
				//if(fm.cfFormat == CF_DIB)
				//	fm.cfFormat = CF_BITMAP;

				if (S_OK != lpDataObject->GetData(&fm, &sm))
				{
					break;
				}
				if (sm.pUnkForRelease != NULL)
				{
					break;
				}
				if (fm.cfFormat == CF_BITMAP)
				{
					CBitmap bm;
					if (!bm.Attach(sm.hBitmap))
					{
						break;
					}
					HGLOBAL hGlobal = DDBToDIB(bm,BI_RGB);
					bm.Detach();
					if (hGlobal==NULL)
					{
						break;
					}
					dwDataSize = ::GlobalSize(hGlobal);
					*pOutData = new char[dwDataSize];
					::CopyMemory(*pOutData, (LPVOID)::GlobalLock(hGlobal), dwDataSize);
					::GlobalUnlock(hGlobal);
					GlobalFree(hGlobal);
					::ReleaseStgMedium(&sm);
				}else if (fm.cfFormat == CF_DIB)
				{
					dwDataSize = ::GlobalSize(sm.hGlobal);
					*pOutData = new char[dwDataSize];
					::CopyMemory(*pOutData, (LPVOID)::GlobalLock(sm.hGlobal), dwDataSize);
					::GlobalUnlock(sm.hGlobal);
					if (sm.pUnkForRelease == NULL)
					{
						GlobalFree(sm.hGlobal);
					}

					::ReleaseStgMedium(&sm);
					break;
				}else
				{
					LPMETAFILEPICT pMFP = (LPMETAFILEPICT) GlobalLock (sm.hMetaFilePict);
					//SaveToBitMap(pMFP, strPath);
					CDC* pDC = GetDC();
					if (pDC!=NULL)
					{
						SIZE size;
						size.cx = pMFP->xExt;
						size.cy = pMFP->yExt;
						pDC->HIMETRICtoDP(&size);

						CBitmap bm;
						bm.CreateCompatibleBitmap(pDC, abs(size.cx), abs(size.cy));

						CDC memdc;
						memdc.CreateCompatibleDC(pDC);
						CBitmap* pOldBitmap = memdc.SelectObject(&bm);

						memdc.SetMapMode(pMFP->mm) ;
						memdc.SetViewportExt(abs(size.cx), abs(size.cy)) ;
						memdc.SetViewportOrg(0, 0);
						memdc.PlayMetaFile(pMFP->hMF);

						HGLOBAL hGlobal = DDBToDIB(bm,BI_RGB);
						if (hGlobal!=NULL)
						{
							dwDataSize = ::GlobalSize(hGlobal);
							*pOutData = new char[dwDataSize];
							::CopyMemory(*pOutData, (LPVOID)::GlobalLock(hGlobal), dwDataSize);
							::GlobalUnlock(hGlobal);
							GlobalFree(hGlobal);
						}
						//CImage img;
						//img.Attach((HBITMAP)bm.GetSafeHandle());
						//img.Save(strPath);
						memdc.SelectObject(pOldBitmap);
						memdc.DeleteDC();
						bm.DeleteObject();
						ReleaseDC(pDC);
					}
					GlobalUnlock(sm.hMetaFilePict);
					::ReleaseStgMedium(&sm);
					break;
				}
				//dRet = reobject.dwUser = SetImagePath(strPath);
				//::ReleaseStgMedium(&sm);
				//Ret = S_FALSE;
			}
		}
		else
		{
			Ret = S_FALSE;
		}
	}
	lpDataObject->Release();
	return dwDataSize==0?false:true;

	//// FORMATETC fm={ 0xC007/*FileNameW*/, 0, DVASPECT_CONTENT, -1, TYMED_HGLOBAL }; 
	//STGMEDIUM stgm;  //out
	//FORMATETC fm;    //in
	//fm.cfFormat = CF_DIB;  // CF_BITMAP // Clipboard format
	//fm.ptd = NULL;         // Target Device = Screen
	//fm.dwAspect = DVASPECT_CONTENT;
	//// Level of detail = Full content
	//fm.lindex = -1;        // Index = Not applicaple
	//fm.tymed = TYMED_HGLOBAL;	// TYMED_GDI;//
	////fm.tymed = TYMED_FILE|TYMED_GDI|TYMED_HGLOBAL;	// TYMED_GDI;//
	//hr = lpDataObject->GetData(&fm, &stgm);
	//if(FAILED(hr))
	//{
	//	lpDataObject->Release();
	//	return false;
	//}
	//ASSERT(::GlobalSize(stgm.hGlobal));
	//dwDataSize = ::GlobalSize(stgm.hGlobal);
	//*pOutData = new char[dwDataSize];
	//::CopyMemory(*pOutData, (LPVOID)::GlobalLock(stgm.hGlobal), dwDataSize);
	//::GlobalUnlock(stgm.hGlobal);
	//if (stgm.pUnkForRelease == NULL)
	//{
	//	GlobalFree(stgm.hGlobal);
	//}

	//lpDataObject->Release();
	//return true;
}

void COleRichEditCtrl::save()
{
	////IRichEditOle* pReo = m_richInput.GetIRichEditOle();
	//If you use Platform SDK directly
	IRichEditOle* pReo;
	::SendMessage(this->GetSafeHwnd(), EM_GETOLEINTERFACE, 0, (LPARAM)(LPVOID*)&pReo);
	//Note: in both cases, inside pReo's AddRef got called, so
	//remember to release it later

	LONG nNumber = pReo->GetObjectCount();  //Your Images' Number
	//Handle Error yourself, code simplified for space limitation
	for(int i = 0; i < nNumber; i++)
	{
		REOBJECT reobject;
		memset( &reobject, 0 , sizeof(REOBJECT) );
		reobject.cbStruct = sizeof(REOBJECT);
		HRESULT hr = pReo->GetObject(i, &reobject, REO_GETOBJ_ALL_INTERFACES);
		if(FAILED(hr)) continue;

		// 是否当前选中对象
		if((reobject.dwFlags & REO_SELECTED)  == REO_SELECTED)
		{
			int i=0;
			// 找到当前的图片对象
			//reobject.dwUser // 插入图片时指定的数值
		}

		//caller should released the inner object
		IDataObject* lpDataObject;
		hr = (reobject.poleobj)->QueryInterface(IID_IDataObject,
			(void **)&lpDataObject);
		if(FAILED(hr)) continue;

		// FORMATETC fm={ 0xC007/*FileNameW*/, 0, DVASPECT_CONTENT, -1, TYMED_HGLOBAL }; 
		STGMEDIUM stgm;  //out
		FORMATETC fm;    //in
		fm.cfFormat = CF_DIB;  // CF_BITMAP // Clipboard format
		fm.ptd = NULL;         // Target Device = Screen
		fm.dwAspect = DVASPECT_CONTENT;
		// Level of detail = Full content
		fm.lindex = -1;        // Index = Not applicaple
		fm.tymed = TYMED_HGLOBAL;	// TYMED_GDI;//
		hr = lpDataObject->GetData(&fm, &stgm);
		if(FAILED(hr)) continue;

		ASSERT(::GlobalSize(stgm.hGlobal));

		HANDLE hFile = ::CreateFile(_T("c:\\img.bmp"),
			GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
			FILE_ATTRIBUTE_NORMAL, NULL);
		if(hFile == INVALID_HANDLE_VALUE)
		{
			continue;
		}

		DWORD dwWritten;
		//Writing Bitmap File header
		BITMAPFILEHEADER bmfh;
		bmfh.bfType = 0x4d42;    //'BM'
		int nColorTableEntries = 0;
		int nSizeHdr = sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) *
			nColorTableEntries;
		bmfh.bfSize = 0;
		bmfh.bfReserved1 = bmfh.bfReserved2 = 0;
		bmfh.bfOffBits = sizeof(BITMAPFILEHEADER) +
			sizeof(BITMAPINFOHEADER) +
			sizeof(RGBQUAD) * nColorTableEntries;
		::WriteFile(hFile, (LPVOID)&bmfh, sizeof(BITMAPFILEHEADER),
			&dwWritten, NULL);

		DWORD dwGlobalSize = ::GlobalSize(stgm.hGlobal);
		LPVOID lpMem = ::VirtualAlloc(NULL, dwGlobalSize, MEM_COMMIT, PAGE_READWRITE);
		::CopyMemory(lpMem, (LPVOID)::GlobalLock(stgm.hGlobal), dwGlobalSize);
		BITMAPINFOHEADER* pInfoHead = (BITMAPINFOHEADER*)lpMem;
		pInfoHead->biXPelsPerMeter = pInfoHead->biYPelsPerMeter = 0;

		//Special Careful Here!!! Discard the Color Mask,
		//We need TRUE COLOR image
		//I bellieve you guys running Win2X/Xp use true color
		//screen setting
		if(pInfoHead->biCompression == BI_BITFIELDS)
		{
			pInfoHead->biCompression = BI_RGB;
			dwGlobalSize -= 3 * sizeof(RGBQUAD);  //delete the 3 DWORD
			//color mask
			LPBYTE pSrc, pDst;
			pSrc = (LPBYTE)lpMem;
			pSrc += sizeof(BITMAPINFOHEADER);
			pDst = pSrc;
			pSrc += 3 * sizeof(RGBQUAD);
			::MoveMemory(pDst, pSrc, dwGlobalSize - sizeof(BITMAPINFOHEADER));
		}

		//Write Image Data
		::WriteFile(hFile, lpMem, dwGlobalSize, &dwWritten, NULL);
		::GlobalUnlock(stgm.hGlobal);

		//You may find good if all image are same size, keeping the
		//memory for performance
		::VirtualFree(lpMem, 0, MEM_RELEASE);
		::CloseHandle(hFile);

		lpDataObject->Release();
	}
	pReo->Release();
}

BOOL COleRichEditCtrl::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default
	CRect rc;
    GetClientRect(rc);  
    pDC->FillSolidRect(rc, RGB(255, 255, 255));  
	return TRUE;
	return CRichEditCtrl::OnEraseBkgnd(pDC);
}

void COleRichEditCtrl::OnDestroy()
{
	KillTimer(TIMERID_DRAW_GIF);
	SetWindowText(_T(""));  
    EmptyUndoBuffer();  
	CRichEditCtrl::OnDestroy();

	// TODO: Add your message handler code here
	if (m_pIRichEditOleCallback)
	{
		delete m_pIRichEditOleCallback;
		m_pIRichEditOleCallback = NULL;
	}
}

int COleRichEditCtrl::FindFirstObject(int cpMin, int nObjectCount)  
{  
    // 标准的二分查找算法，不用解释了  
    int low = 0;  
    int high = nObjectCount - 1;  
    REOBJECT reoMid = {0};  
    reoMid.cbStruct = sizeof(REOBJECT);  
    while (low <= high) {  
        int mid = (low + high) >> 1;  
        if (m_pRichEditOle->GetObject(mid, &reoMid, REO_GETOBJ_POLEOBJ) != S_OK) {  
            return -1;  
        }  
        reoMid.poleobj->Release();  
        if (reoMid.cp == cpMin) {  
            return mid;  
        } else if (reoMid.cp < cpMin) {  
            low = mid + 1;  
        } else {  
            high = mid - 1;  
        }  
    }  
  
    // 只不过最后没找到时不是返回-1，而是返回low，此时low必然大于high  
    // 刚好满足条件  
    return low;  
}
void COleRichEditCtrl::OnTimer(UINT_PTR nIDEvent)
{
	if (TIMERID_DRAW_GIF==nIDEvent)
	{
		// 得到对象总数，大于0时才需要刷新  
		int nObjectCount = m_pRichEditOle->GetObjectCount();  
		if (nObjectCount > 0) {  
			CRect rc;  
			GetRect(rc); // 得到可视区文本的矩形区域  
			// 分别使用左上角和右下角的点得到最小和最大字符索引  
			// 即可见区域的最小和最大字符索引  
			int cpMin = CharFromPos(rc.TopLeft());  
			int cpMax = CharFromPos(rc.BottomRight());  

			// 使用二分查找算法找到第一个字符索引大于或等于cpMin的对象索引  
			int iFirst = FindFirstObject(cpMin, nObjectCount);  
			REOBJECT reo = {0};  
			reo.cbStruct = sizeof(REOBJECT);  

			DWORD dwNowTime = timeGetTime();
			// 从第一个索引开始遍历对象更换帧  
			for (int i = iFirst; i < nObjectCount; i++) {  
				if (m_pRichEditOle->GetObject(i, &reo, REO_GETOBJ_POLEOBJ) == S_OK) {  
					reo.poleobj->Release();  
					// 当前对象的字符索引大于最大字符索引，说明对象不在可见区域，停止遍历  
					if (reo.cp > cpMax) {  
						break;  
					}  

					// 是COleImage对象时才能更新  
					if (InlineIsEqualGUID(reo.clsid, __uuidof(EBImage))) {  
						// 更换帧 
						CComPtr<IEBImage> IPic;
						HRESULT hr = reo.poleobj->QueryInterface(__uuidof(IEBImage), (void**)&IPic);//获得数据对象接口
						if( hr == S_OK )
							IPic->ShowNextFrame(dwNowTime);
					}  
				}  
			}  
		}  
	}

	CRichEditCtrl::OnTimer(nIDEvent);
}
