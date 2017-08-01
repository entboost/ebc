// TextPlain.h: interface for the CTextPlain class.
// 
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TEXTPLAIN_H__333BA6C3_F593_11D1_870E_444553540001__INCLUDED_)
#define AFX_TEXTPLAIN_H__333BA6C3_F593_11D1_870E_444553540001__INCLUDED_

#include "MIMEContentAgent.h"

class CTextPlain : public CMIMEContentAgent  
{
public:
	CTextPlain( int nContentType, size_t nWrapPos = 72 );
	virtual ~CTextPlain(void);

	virtual bool AppendPart( const char* szContent, const char* szParameters, int nEncoding, bool bPath, const char* szFileName, tstring& sDestination );
	//virtual tstring GetContentTypeString(void) const;

protected:
	size_t m_nWrapPos;

	tstring wrap_text( const char* szText );
	virtual tstring build_sub_header( const char* szContent, const char* szParameters, int nEncoding, bool bPath );
};

#endif // !defined(AFX_TEXTPLAIN_H__333BA6C3_F593_11D1_870E_444553540001__INCLUDED_)
