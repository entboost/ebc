// AppOctetStream.h: interface for the CAppOctetStream class.
// 
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_APPOCTETSTREAM_H__333BA6C2_F593_11D1_870E_444553540001__INCLUDED_)
#define AFX_APPOCTETSTREAM_H__333BA6C2_F593_11D1_870E_444553540001__INCLUDED_


#include "MIMEContentAgent.h"

class CAppOctetStream : public CMIMEContentAgent  
{
public:
	//virtual tstring GetContentTypeString(void) const;
	CAppOctetStream( int nContentType );
	virtual ~CAppOctetStream();

	virtual bool AppendPart( const char* szContent, 
							 const char* szParameters, 
							 int nEncoding, 
							 bool bPath, 
							 const char* szFileName,
							 tstring& sDestination );
	static tstring GetFileName(const tstring & sPathName);
	static void GetFileExt(const tstring & sFileName, tstring & sOutName, tstring & sOutExt);

protected:
	virtual void attach_file( FILE* pFileAtt, int nEncoding, tstring& sDestination );
	virtual tstring build_sub_header( const char* szContent, 
									  const char* szParameters, 
									  int nEncoding, 
									  bool bPath );
};

#endif // !defined(AFX_APPOCTETSTREAM_H__333BA6C2_F593_11D1_870E_444553540001__INCLUDED_)
