// MIMEMessage.h: interface for the CMIMEMessage class.
// 
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MIMEMESSAGE_H__6233D556_EBBF_11D1_870E_444553540001__INCLUDED_)
#define AFX_MIMEMESSAGE_H__6233D556_EBBF_11D1_870E_444553540001__INCLUDED_

#include <string>
#include <vector>
#include <stl/locklist.h>
#include "MailMessage.h"
#include "MIMEContentAgent.h"

class CMIMEMessage : public CMailMessage  
{
public:
	CMIMEMessage(const char* sCharset= "gbk");
	virtual ~CMIMEMessage(void);

	// MIME ÀàÐÍµÄ±àÂë
	enum eMIMETypeCode
	{
		TEXT_PLAIN = 0,
		APPLICATION_OCTETSTREAM,
		NEXT_FREE_MIME_CODE
	};
	enum eMIMEEncodingCode
	{
		_7BIT = 0,
		_8BIT,
		BINARY,
		QUOTED_PRINTABLE,
		BASE64,
		NEXT_FREE_ENCODING_CODE
	};
	bool AddMIMEPart( const char* szContent,
					  int nContentType = APPLICATION_OCTETSTREAM,
					  const char* szParameters = "", 
					  int nEncoding = BASE64, 
					  bool bPath = true,
					  const char* szFileName="");
protected:
	void insert_message_end(tstring& strText);
	void register_mime_type(CMIMEContentAgent* pMIMEType);
	void insert_boundary(tstring& strText);

	virtual void append_mime_parts(void);
	virtual void prepare_header(void);
	virtual void prepare_body(void);

	tstring m_sNoMIMEText;
	tstring m_sPartBoundary;
	tstring m_sMIMEContentType;
private:
	class CMIMEPart
	{
	public:
		int m_nEncoding;
		int m_nContentType;
		tstring m_sParameters;
		bool m_bPath;
		tstring m_sContent;
		tstring m_sFileName;
		CMIMEPart(void)
			: m_nEncoding(0), m_nContentType(0), m_bPath(false)
		{}
	};
	std::vector<CMIMEPart> m_MIMEPartList;

	class CMIMETypeManager
	{
	public:
		CMIMEContentAgent* GetHandler( int nContentType );
		void RegisterMIMEType( CMIMEContentAgent* pMIMEType);
		CMIMETypeManager(void);
		virtual  ~CMIMETypeManager(void);
	private:
		CLockListPtr< CMIMEContentAgent* > m_MIMETypeList;
	};

	static CMIMETypeManager m_MIMETypeManager;
};

#endif // !defined(AFX_MIMEMESSAGE_H__6233D556_EBBF_11D1_870E_444553540001__INCLUDED_)
