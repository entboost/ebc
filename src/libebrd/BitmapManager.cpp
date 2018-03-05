
#include "stdafx.h"
#include "BitmapManager.h"

CBitmapManager::CBitmapManager(void)
: m_hMemDC(NULL)
, m_hBitmap(NULL)
, m_pBits(NULL)

{
	memset(&m_bmpinfo, 0, sizeof(BITMAPINFO));
}

CBitmapManager::~CBitmapManager(void)
{
	Release();
}

bool CBitmapManager::Create(int bmWidth, int bmHeight, int nBitCount)
{
	if (IsValid())
		return true;

	m_bmpinfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	m_bmpinfo.bmiHeader.biWidth = bmWidth;
	m_bmpinfo.bmiHeader.biHeight = bmHeight;
	m_bmpinfo.bmiHeader.biPlanes = 1;
	m_bmpinfo.bmiHeader.biBitCount = nBitCount;
	m_bmpinfo.bmiHeader.biCompression = BI_RGB;
	m_bmpinfo.bmiHeader.biSizeImage = bmWidth*bmHeight*(nBitCount/8);
	m_bmpinfo.bmiHeader.biXPelsPerMeter = 0;//100;
	m_bmpinfo.bmiHeader.biYPelsPerMeter = 0;//100;
	m_bmpinfo.bmiHeader.biClrUsed = 0;
	m_bmpinfo.bmiHeader.biClrImportant = 0;

	LPBITMAPINFO     pBmpInfo = &m_bmpinfo;

	if (m_hMemDC == NULL)
		m_hMemDC = CreateCompatibleDC(NULL);

	if (m_hBitmap == NULL)
		m_hBitmap = CreateDIBSection(m_hMemDC, pBmpInfo, DIB_RGB_COLORS,   &m_pBits,   NULL,   0);

	return IsValid();
}

const BITMAPINFO * CBitmapManager::GetBitmapInfo(void) const
{
	return &m_bmpinfo;
}

int CBitmapManager::GetBitmapWidth(void) const
{
	return m_bmpinfo.bmiHeader.biWidth;
}

int CBitmapManager::GetBitmapHeight(void) const
{
	return m_bmpinfo.bmiHeader.biHeight;
}

const HBITMAP & CBitmapManager::MakeBitmap(const void * lpData)
{
	if (IsValid())
		memcpy(m_pBits, lpData, m_bmpinfo.bmiHeader.biSizeImage);
	return   m_hBitmap;
}

const LPVOID CBitmapManager::GetDibBits(void) const
{
	return m_pBits;
}

DWORD CBitmapManager::GetImageSize(void) const
{
	return m_bmpinfo.bmiHeader.biSizeImage;
}

bool CBitmapManager::IsValid(void) const
{
	return (m_pBits != NULL);
}

bool CBitmapManager::IsInvalid(void) const
{
	return (m_pBits == NULL);
}

void CBitmapManager::Release(void)
{
	memset(&m_bmpinfo, 0, sizeof(BITMAPINFO));
	if (m_pBits)
	{
		DeleteObject(m_pBits);
		m_pBits = NULL;
	}
	if (m_hBitmap)
	{
		DeleteObject(m_hBitmap);
		m_hBitmap = NULL;
	}
	if (m_hMemDC)
	{
		DeleteDC(m_hMemDC);
		m_hMemDC = NULL;
	}

}
