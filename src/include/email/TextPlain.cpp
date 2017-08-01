// TextPlain.cpp: implementation of the CTextPlain class.
// 

#include "TextPlain.h"
#include "Base64.h"
#include "MIMEMessage.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTextPlain::CTextPlain( int nContentType, size_t nWrapPos )
	: CMIMEContentAgent( nContentType )
	, m_nWrapPos(nWrapPos)
{
}

CTextPlain::~CTextPlain(void)
{
}

//tstring CTextPlain::GetContentTypeString(void) const
//{
//	return "text/html; charset=\"gbk\"";
//	//return "text/plain";
//}

bool CTextPlain::AppendPart(const char* szContent, 
							const char* szParameters, 
							int nEncoding, 
							bool bPath, 
							const char* szFileName,
							tstring & sDestination)
{
	tstring sSubHeader;
	tstring sWrapped;
	sSubHeader = build_sub_header( szContent,
								   szParameters,
								   nEncoding,
								   bPath );
	if (nEncoding==CMIMEMessage::_7BIT)
		sWrapped = wrap_text( szContent );	// for _7BIT
	else
	{
		CBase64 pBase64;
		sWrapped = pBase64.Encode(szContent, strlen(szContent));
		int nInsertIndex = 1;
		while (nInsertIndex*m_nWrapPos<sWrapped.size())
		{
			sWrapped.insert(nInsertIndex*m_nWrapPos,"\r\n");
			nInsertIndex++;
		}
	}
	sDestination.append(sSubHeader);
	sDestination.append(sWrapped);
	return true;
}
//Content-Type:text/html; charset="gbk"//Content-Transfer-Encoding:7Bit

//Content-Type:text/html; charset="UTF-8"//Content-Transfer-Encoding:quoted-printable

//MIME-Version: 1.0
//Content-Type: text/html; charset="utf-8"
//Content-Transfer-Encoding: base64

// multipart/alternative; 

tstring CTextPlain::build_sub_header(const char* szContent, 
									 const char* szParameters, 
									 int nEncoding, 
									 bool bPath)
{
	//tstring sSubHeader("Content-Type: text/plain");
	tstring sSubHeader("Content-Type: text/html");
	//sSubHeader.append(GetContentTypeString());
	if (szParameters!=0 && strlen(szParameters)>0)
	{
		char sCharset[64];
		sprintf(sCharset, "; charset=\"%s\"",szParameters);
		sSubHeader.append(sCharset);
	}
	sSubHeader.append("\r\nContent-Transfer-Encoding: ");
	switch( nEncoding )
	{
	case CMIMEMessage::_7BIT:
		sSubHeader.append( "7Bit" );
		break;
	case CMIMEMessage::BASE64:
		sSubHeader.append( "base64" );
		break;
	default:
		break;
	}
	sSubHeader.append("\r\n\r\n" );
	return sSubHeader;
}

tstring CTextPlain::wrap_text(const char* szText)
{
	tstring sTemp;
	tstring sLeft;
	tstring sRight;
	int lp = 0;
	size_t nCount = 0;
	int nSpacePos = 0;

	if( szText == NULL )
		return sTemp;
	sTemp = szText;
	while( lp < sTemp.size())
	{
		if( sTemp[ lp ] == ' ' )
			nSpacePos = lp;
		if( sTemp.substr( lp, 2 ) == "\r\n")
			nCount = 0;
		if( nCount > m_nWrapPos )
		{
			sLeft = sTemp.substr(0, nSpacePos);
			const size_t nRightLen = sTemp.size() - nSpacePos;
			sRight = sTemp.substr(sTemp.size()-nRightLen);
			rtrim(sLeft);
			ltrim(sRight);
			sLeft.append("\r\n");
			sTemp = sLeft + sRight;
			nCount = 0;
		}
		else
			nCount++;
		lp++;
	}
	return tstring(sTemp.c_str());
}
