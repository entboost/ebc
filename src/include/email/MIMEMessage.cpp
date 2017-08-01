// MIMEMessage.cpp: implementation of the CMIMEMessage class.
// 

#include "MIMEMessage.h"
#include "TextPlain.h"
#include "AppOctetStream.h"


CMIMEMessage::CMIMETypeManager CMIMEMessage::m_MIMETypeManager;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMIMEMessage::CMIMEMessage(const char* sCharset)
: CMailMessage(sCharset)
{
	//m_sMIMEContentType = "multipart/alternative";
	m_sMIMEContentType = "multipart/mixed";
	char lpszBuffer[48];
	sprintf(lpszBuffer, "----=EB_MAIL_%X_%X_%X_",(int)time(0),(int)this,rand());
	m_sPartBoundary = lpszBuffer;
	//m_sPartBoundary = "WC_MAIL_PaRt_BoUnDaRy_05151998";
	m_sNoMIMEText = "This is a multi-part message in MIME format." ;

	CMIMEContentAgent* pType;

	pType = new CTextPlain( TEXT_PLAIN, GetCharsPerLine() );
	register_mime_type( pType );
	pType = new CAppOctetStream( APPLICATION_OCTETSTREAM );
	register_mime_type( pType );
}

CMIMEMessage::~CMIMEMessage(void)
{
	m_MIMEPartList.clear();
}


bool CMIMEMessage::AddMIMEPart(const char* szContent,
							   int nContentType, 
							   const char* szParameters, 
							   int nEncoding, 
							   bool bPath,
							   const char* szFileName)
{
	CMIMEPart MIMEPart;
	MIMEPart.m_nContentType = nContentType;
	MIMEPart.m_sParameters = szParameters;
	if (MIMEPart.m_sParameters.empty())
		MIMEPart.m_sParameters = this->m_sCharset;
	MIMEPart.m_nEncoding = nEncoding;
	MIMEPart.m_bPath = bPath;
	MIMEPart.m_sContent = szContent;
	MIMEPart.m_sFileName = szFileName;
	trim(MIMEPart.m_sContent);
	if(nContentType == TEXT_PLAIN)
		m_MIMEPartList.insert(m_MIMEPartList.begin(), MIMEPart);
	else
	{
		m_MIMEPartList.push_back(MIMEPart);
	}
	return true;
}

void CMIMEMessage::prepare_header()
{

	// 由基类的函数来添加邮件头
	CMailMessage::prepare_header();
	
	// 这里只添加MIME头
	add_header_line("MIME-Version: 1.0");
	char lpszBuffer[260];
	sprintf(lpszBuffer,"Content-Type: %s; boundary=\"%s\"",m_sMIMEContentType.c_str(),m_sPartBoundary.c_str());
	add_header_line(lpszBuffer);
}

void CMIMEMessage::prepare_body()
{
	// 将正文内容加到邮件中
	if(m_sBody != "")
	{
		//const int const_max_size = 10*1024;
		//const int nCount = (m_sBody.size()+const_max_size-1)/const_max_size;
		//for (int i=0;i<nCount; i++)
		//{
		//	//AddMIMEPart(m_sBody.substr(i*const_max_size,const_max_size).c_str(),TEXT_PLAIN,m_sCharset.c_str(),BASE64,false);
		//	AddMIMEPart(m_sBody.substr(i*const_max_size,const_max_size).c_str(),TEXT_PLAIN,m_sCharset.c_str(),_7BIT,false);
		//}
		AddMIMEPart(m_sBody.c_str(),TEXT_PLAIN,m_sCharset.c_str(),BASE64,false );
		//AddMIMEPart(m_sBody.c_str(),TEXT_PLAIN,m_sCharset.c_str(),_7BIT,false );

		//if (m_sCharset.empty())
		//{
		//	AddMIMEPart(m_sBody.c_str(),TEXT_PLAIN,"",_7BIT,false );
		//}else
		//{
		//	char lpszParameter[64];
		//	sprintf(lpszParameter, "; charset=\"%s\"",m_sCharset.c_str());
		//	AddMIMEPart(m_sBody.c_str(),TEXT_PLAIN,lpszParameter,_7BIT, false );
		//}
	}

	// 初始化MIME体
	//m_sBody.clear();
	m_sBody = m_sNoMIMEText;
	m_sBody.append("\r\n\r\n");

	// 加入附件的内容
	append_mime_parts();
	insert_message_end(m_sBody);

	// 基类函数为正文加上结束符
	CMailMessage::prepare_body();
}

void CMIMEMessage::insert_boundary(tstring& strText)
{
	const size_t nSize = strText.size();
	if (nSize<2 || strText.substr(nSize-2) != "\r\n")
		strText.append("\r\n\r\n");
	char lpszBuffer[260];
	sprintf(lpszBuffer,"--%s\r\n",m_sPartBoundary.c_str());
	strText.append(lpszBuffer);
}

void CMIMEMessage::insert_message_end(tstring& strText)
{
	const size_t nSize = strText.size();
	if (nSize<2 || strText.substr(nSize-2) != "\r\n")
		strText.append("\r\n");
	char lpszBuffer[260];
	sprintf(lpszBuffer,"--%s--\r\n",m_sPartBoundary.c_str());
	strText.append(lpszBuffer);
}

void CMIMEMessage::register_mime_type(CMIMEContentAgent* pMIMEType)
{
	if( pMIMEType == NULL )
		return;
	m_MIMETypeManager.RegisterMIMEType( pMIMEType );
}


void CMIMEMessage::append_mime_parts()
{
	for (size_t i=0; i<m_MIMEPartList.size(); i++)
	{
		CMIMEPart pMIMEPart = m_MIMEPartList[i];
		CMIMEContentAgent * pMIMEType = m_MIMETypeManager.GetHandler(pMIMEPart.m_nContentType);

		// 根据附件的类型分别处理
		if (pMIMEType != 0)
		{
			insert_boundary(m_sBody);
			pMIMEType->AppendPart(pMIMEPart.m_sContent.c_str(),pMIMEPart.m_sParameters.c_str(),
								   pMIMEPart.m_nEncoding,pMIMEPart.m_bPath,pMIMEPart.m_sFileName.c_str(),m_sBody);
		}
	}
	//CMIMEPart* pMIMEPart = NULL;
	//CMIMEContentAgent* pMIMEType = NULL;
	//POSITION pos = m_MIMEPartList.GetHeadPosition();
	//while (pos)
	//{
	//	pMIMEPart = &m_MIMEPartList.GetNext(pos);
	//	pMIMEType = m_MIMETypeManager.GetHandler(pMIMEPart->m_nContentType);

	//	// 根据附件的类型分别处理
	//	if (pMIMEType)
	//	{
	//		insert_boundary(m_sBody);
	//		pMIMEType->AppendPart(pMIMEPart->m_sContent,pMIMEPart->m_sParameters,
	//							   pMIMEPart->m_nEncoding,pMIMEPart->m_bPath,m_sBody);
	//	}
	//}
}

//////////////////////////////////////////////////////////////////////
// CMIMETypeManager Implementation
//////////////////////////////////////////////////////////////////////

CMIMEMessage::CMIMETypeManager::CMIMETypeManager(void)
{
}

CMIMEMessage::CMIMETypeManager::~CMIMETypeManager(void)
{
	m_MIMETypeList.clear(true,true);
	//POSITION pos;
	//CMIMEContentAgent* p;
	//m_csAccess.Lock();
	//pos = m_MIMETypeList.GetHeadPosition();
	//while( pos != NULL )
	//{
	//	p = m_MIMETypeList.GetNext( pos );
	//	delete p;
	//}
}

void CMIMEMessage::CMIMETypeManager::RegisterMIMEType(CMIMEContentAgent *pMIMEType)
{
	if( pMIMEType == NULL )
		return;
	m_MIMETypeList.add(pMIMEType);
}



CMIMEContentAgent* CMIMEMessage::CMIMETypeManager::GetHandler(int nContentType)
{
	CMIMEContentAgent* pType = NULL;

	BoostWriteLock wtlock(m_MIMETypeList.mutex());
	CLockListPtr< CMIMEContentAgent* >::iterator pIter = m_MIMETypeList.begin();
	for (; pIter!=m_MIMETypeList.end(); pIter++)
	{
		pType = *pIter;
		if (pType->QueryType(nContentType))
		{
			return pType;
		}
	}
	return 0;

	//CMIMEContentAgent* pType = NULL;
	//POSITION pos;
	//m_csAccess.Lock();
	//pos = m_MIMETypeList.GetHeadPosition();
	//while (pos)
	//{
	//	pType = m_MIMETypeList.GetNext(pos);
	//	if (pType->QueryType(nContentType))
	//		break;
	//}
	//return pType;
}

