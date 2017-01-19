#include "stdafx.h"
#include "ImageEx.h"

bool CImageEx::LoadFilename(LPCTSTR lpszFileName, UINT type)
{
	this->Destroy();
	HRESULT hr = Load(lpszFileName);
	return SUCCEEDED(hr)?true:false;
	//HBITMAP hBitmap = (HBITMAP)::LoadImage(0, lpszFileName, type, 0, 0, LR_LOADFROMFILE);
	//if (hBitmap != NULL)
	//{
	//	this->Attach(hBitmap);
	//	return true;
	//}
	//DWORD dwLastError = ::GetLastError();
	//return false;
}
bool CImageEx::LoadResource(UINT nIDResource)
{
	this->Destroy();
	CBitmap bitmap;
	if (bitmap.LoadBitmap(nIDResource))
	{
		this->Attach((HBITMAP)bitmap.Detach());
		return true;
	}
	DWORD dwLastError = ::GetLastError();
	return false;
}

bool CImageEx::Cut(int nCount, HGDIOBJ * pOutBitmaps)
{
	ASSERT (nCount > 0);
	int width = this->GetWidth()/nCount;
	int height = this->GetHeight();

	CDC*	pDC = CDC::FromHandle(this->GetDC());
	if (pDC == NULL) return false;
	CDC		dcMem;
	dcMem.CreateCompatibleDC(pDC);

	for (int i=0; i<nCount; i++)
	{
		CBitmap bitmap;
		if (!bitmap.CreateCompatibleBitmap(pDC, width, height))
			break;
		CBitmap * pOldBitmap = dcMem.SelectObject(&bitmap);
		this->BitBlt(dcMem.m_hDC, 0, 0, width, height, i*width, 0);
		pOutBitmaps[i] = bitmap.Detach();
		dcMem.SelectObject(pOldBitmap);
		bitmap.DeleteObject();
	}
	dcMem.DeleteDC();
	this->ReleaseDC();
	return true;
}
bool CImageEx::Cut2(int nCount, HGDIOBJ * pOutBitmaps)
{
	ASSERT (nCount > 0);
	int width = this->GetWidth();
	int height = this->GetHeight()/nCount;

	CDC*	pDC = CDC::FromHandle(this->GetDC());
	if (pDC == NULL) return false;
	CDC		dcMem;
	dcMem.CreateCompatibleDC(pDC);

	for (int i=0; i<nCount; i++)
	{
		CBitmap bitmap;
		if (!bitmap.CreateCompatibleBitmap(pDC, width, height))
			break;
		CBitmap * pOldBitmap = dcMem.SelectObject(&bitmap);
		this->BitBlt(dcMem.m_hDC, 0, 0, width, height, 0, i*height);
		pOutBitmaps[i] = bitmap.Detach();
		dcMem.SelectObject(pOldBitmap);
		bitmap.DeleteObject();
	}
	dcMem.DeleteDC();
	this->ReleaseDC();
	return true;
}
HGDIOBJ CImageEx::Copy(int x, int y, int w, int h)
{
	const int width = this->GetWidth();
	const int height = this->GetHeight();
	if (x >= width || y >= height)
		return NULL;
	if (w == -1)
		w = width - x;
	else if (x == -1)
		x = width - w;
	if (h == -1)
		h = height - y;
	else if (y == -1)
		y = height - h;

	CDC*	pDC = CDC::FromHandle(this->GetDC());
	if (pDC == NULL) return 0;
	CDC		dcMem;
	dcMem.CreateCompatibleDC(pDC);

	CBitmap bitmap;
	bitmap.CreateCompatibleBitmap(pDC, w, h);
	CBitmap * pOldBitmap = dcMem.SelectObject(&bitmap);
	this->BitBlt(dcMem.m_hDC, 0, 0, w, h, x, y);
	HGDIOBJ result = bitmap.Detach();
	dcMem.SelectObject(pOldBitmap);
	bitmap.DeleteObject();
	dcMem.DeleteDC();
	this->ReleaseDC();
	return result;
}
HGDIOBJ CImageEx::StrctchCopy(int x, int y, int w, int h)
{
	const int width = this->GetWidth();
	const int height = this->GetHeight();
	if (x >= width || y >= height)
		return NULL;
	if (w == -1)
		w = width - x;
	else if (x == -1)
		x = width - w;
	if (h == -1)
		h = height - y;
	else if (y == -1)
		y = height - h;

	CDC*	pDC = CDC::FromHandle(this->GetDC());
	if (pDC == NULL) return 0;
	CDC		dcMem;
	dcMem.CreateCompatibleDC(pDC);

	CBitmap bitmap;
	bitmap.CreateCompatibleBitmap(pDC, w, h);
	CBitmap * pOldBitmap = dcMem.SelectObject(&bitmap);
	::SetStretchBltMode(dcMem.m_hDC, HALFTONE);	// STRETCH_HALFTONE
	::SetBrushOrgEx(dcMem.m_hDC, 0, 0, NULL);
	this->StretchBlt(dcMem.m_hDC, 0, 0, w, h, x, y, width, height);
	HGDIOBJ result = bitmap.Detach();
	dcMem.SelectObject(pOldBitmap);
	bitmap.DeleteObject();
	dcMem.DeleteDC();
	this->ReleaseDC();
	return result;
}
