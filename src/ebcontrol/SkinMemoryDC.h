// SkinMemoryDC.h file here
#ifndef __SkinMemoryDC_h__
#define __SkinMemoryDC_h__

class CSkinMemoryDC// : public HDC 
{
private:
	HBITMAP	m_bitmap;
	HBITMAP	m_oldBitmap;
	HDC		m_pDC;
	RECT	m_rcBounds;

	HDC		m_pOwnerDC;
public:
	int GetLeft(void) const {return m_rcBounds.left;}
	int GetRight(void) const {return m_rcBounds.right;}
	int GetTop(void) const {return m_rcBounds.top;}
	int GetBottom(void) const {return m_rcBounds.bottom;}
	int Width(void) const {return m_rcBounds.right-m_rcBounds.left;}
	int Height(void) const {return m_rcBounds.bottom-m_rcBounds.top;}
	HDC GetDC(void) const {return m_pOwnerDC;}
	CSkinMemoryDC(HDC pDC, const RECT& rcBounds);
	~CSkinMemoryDC(void);
	CSkinMemoryDC* operator->() 
	{
		return this;
	}
};

#endif // __SkinMemoryDC_h__
