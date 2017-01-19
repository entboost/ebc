// ImageEx.h file here
#ifndef __ImageEx_h__
#define __ImageEx_h__

class CImageEx
	: public CImage
{
public:
	bool LoadFilename(LPCTSTR lpszFileName, UINT type = IMAGE_BITMAP);
	bool LoadResource(UINT nIDResource);
	bool Cut(int nCount, HGDIOBJ * pOutBitmaps);
	bool Cut2(int nCount, HGDIOBJ * pOutBitmaps);
	HGDIOBJ Copy(int x, int y, int w, int h);
	HGDIOBJ StrctchCopy(int x, int y, int w, int h);

private:

};

#endif // __ImageEx_h__
