// BitmapManager.h file here
#ifndef __BitmapManager_h__
#define __BitmapManager_h__

class CBitmapManager
{
private:
	HDC	m_hMemDC;
	HBITMAP m_hBitmap;
	LPVOID m_pBits;
	//DWORD m_dwBitSize;
	BITMAPINFO m_bmpinfo;

public:
	bool Create(int bmWidth, int bmHeight, int nBitCount);
	const BITMAPINFO * GetBitmapInfo(void) const;
	int GetBitmapWidth(void) const;
	int GetBitmapHeight(void) const;
	const HBITMAP & MakeBitmap(const void * lpData);
	const LPVOID GetDibBits(void) const;
	DWORD GetImageSize(void) const;
	

	bool IsValid(void) const;
	bool IsInvalid(void) const;
	void Release(void);

public:
	CBitmapManager(void);
	virtual ~CBitmapManager(void);

};

#endif // __BitmapManager_h__
