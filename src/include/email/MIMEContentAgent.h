// MIMEContentAgent.h: interface for the CMIMEContentAgent class.
// 
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MIMECONTENTAGENT_H__74AE0FE2_F4D9_11D1_870E_444553540001__INCLUDED_)
#define AFX_MIMECONTENTAGENT_H__74AE0FE2_F4D9_11D1_870E_444553540001__INCLUDED_

#include <CGCBase/cgcSmartString.h>
using namespace mycp;

class CMIMEContentAgent  
{
public:
	CMIMEContentAgent( int nMIMEType )
	{
		m_nMIMETypeIHandle = nMIMEType;
	}
	virtual ~CMIMEContentAgent(void)
	{}

	bool QueryType( int nContentType ) const {return nContentType == m_nMIMETypeIHandle ? true : false;}

	virtual bool AppendPart( const char* szContent,
		const char* szParameters, 
		int nEncoding, 
		bool bPath, 
		const char* szFileName,
		tstring& sDestination ) = 0;
	//virtual std::string GetContentTypeString(void) const = 0;

protected:
	virtual tstring build_sub_header( const char* szContent,
		const char* szParameters,
		int nEncoding,
		bool bPath ) = 0;
private:
	int m_nMIMETypeIHandle;
};

#endif // !defined(AFX_MIMECONTENTAGENT_H__74AE0FE2_F4D9_11D1_870E_444553540001__INCLUDED_)
