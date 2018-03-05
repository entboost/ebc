// DesktopWindow.h file here
#ifndef __DesktopWindow_h__
#define __DesktopWindow_h__

class CDesktopWindow
{
private:
	SIZE	m_sizeScreen;
	SIZE	m_sizeReal;
	HWND	m_hDesktopWindow;
	HDC		m_hScreenDC;
	BITMAPINFOHEADER   m_biHeader;            

	HDC		m_hMemDC;
	HBITMAP m_hCurrentBitmap;
	char *	m_pScreenData;

public:
	// 0: not limit
	bool	Create(int nLimitWith = 1024);
	bool	IsValid(void) const;
	bool	IsInvalid(void) const;
	void	Release(void);

	const HBITMAP &	CopyScreen(void) const;
	const char *	GetScreenData(void);


	static bool SaveToFile(const char* filename, const unsigned char * pRGBBuffer, int width,int height,int bpp = 32);
	static bool SaveToFile(const HBITMAP & hBitmap, int nBitCount, const char * lpFileName);	// ?
	bool SaveToFile(const char * fileName);

	UINT GetBitmapSize(void) const;
	UINT GetWidth(void) const;
	UINT GetHeight(void) const;
	UINT GetRealWidth(void) const {return m_sizeReal.cx;}
	UINT GetRealHeight(void) const {return m_sizeReal.cy;}
	UINT GetBitCount(void) const;

public:
	CDesktopWindow(void);
	virtual ~CDesktopWindow(void);

};

#endif // __DesktopWindow_h__
