// SkinMemDC.h file here
#ifndef __SkinMemDC_h__
#define __SkinMemDC_h__

class CSkinMemDC : public CDC 
{
private:
	CBitmap*	m_bitmap;
	CBitmap*	m_oldBitmap;
	CDC*		m_pDC;
	CRect		m_rcBounds;
public:
	CSkinMemDC(CDC* pDC, const CRect& rcBounds);
	virtual ~CSkinMemDC(void);
	CSkinMemDC* operator->() 
	{
		return this;
	}
};

#endif // __SkinMemDC_h__
