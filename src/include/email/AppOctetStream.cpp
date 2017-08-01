// AppOctetStream.cpp: implementation of the CAppOctetStream class.
// 

#include "AppOctetStream.h"
#include "Base64.h"
#include "MIMEMessage.h"


#define BYTES_TO_READ 54*20	//54 

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAppOctetStream::CAppOctetStream(int nContentType)
	:CMIMEContentAgent( nContentType )
{
}

CAppOctetStream::~CAppOctetStream()
{
}

bool CAppOctetStream::AppendPart(const char* szContent, 
								 const char* szParameters, 
								 int nEncoding, 
								 bool bPath, 
								 const char* szFileName,
								 tstring & sDestination)
{
	if (szContent == NULL)
		return false;

	FILE * fAttachment = fopen(szContent,"rb");
	if (fAttachment==NULL)
		return false;

	//if (!fAttachment.Open(szContent,(CFile::modeRead | CFile::shareDenyWrite
	//	| CFile::typeBinary)))
	//	return FALSE;
	sDestination += build_sub_header(szFileName,szParameters,nEncoding,true);
	attach_file( fAttachment,CMIMEMessage::BASE64,sDestination);
	fclose(fAttachment);
	return true;
}

tstring CAppOctetStream::GetFileName(const tstring & sPathName)
{
	tstring sFileName(sPathName);
	int find = sPathName.rfind("\\");
	if (find < 0)
		find = sPathName.rfind("/");
	if (find > 0)
		sFileName = sPathName.substr(find+1);
	return sFileName;
}
void CAppOctetStream::GetFileExt(const tstring & sFileName, tstring & sOutName, tstring & sOutExt)
{
	const int find = sFileName.rfind(".");
	if (find > 0)
	{
		sOutExt = sFileName.substr(find);
        if (sOutExt.find("/")!=std::string::npos || sOutExt.find("\\")!=std::string::npos) {
            sOutName = sFileName;
            sOutExt.clear();
            return;
        }
		sOutName = sFileName.substr(0, find);
	}else
	{
		sOutName = sFileName;
	}
}
tstring CAppOctetStream::build_sub_header(const char* szContent,const char* szParameters,
										  int nEncoding,bool bPath)
{
	tstring sSubHeader;
	char sTemp[260];
	const tstring szFullName = GetFileName(szContent);
	tstring szFName;
	tstring szExt;
	GetFileExt(szFullName,szFName,szExt);
	const tstring sFileName = szFName + szExt;
	char sMailFileNameFormat[260];
	CBase64 pBase64;
	sprintf(sMailFileNameFormat,"\"=?%s?B?%s?=\"",szParameters,pBase64.Encode(sFileName.c_str(), sFileName.size()).c_str());

	//_splitpath( szContent, NULL, NULL, szFName, szExt );

	//------=_NextPart_000_005A_01D0B67E.04374550	//Content-Type: application/msword;	//	name="=?gb2312?B?xKe3vb3Ms8zNvL3iLmRvYw==?="	//Content-Transfer-Encoding: base64	//Content-Disposition: attachment;	//	filename="=?gb2312?B?xKe3vb3Ms8zNvL3iLmRvYw==?="
	memset(sTemp,0,sizeof(sTemp));
	if (bPath)
	{
		sprintf(sTemp,"; name=%s",sMailFileNameFormat);
		//sprintf(sTemp,"; file=%s%s",szFName.c_str(),szExt.c_str());
	}
	sSubHeader = "Content-Type: application/octet-stream";
	//sSubHeader.append(GetContentTypeString());
	sSubHeader.append(sTemp);
	sSubHeader.append("\r\nContent-Transfer-Encoding: base64\r\n");

	sprintf(sTemp,"Content-Disposition: attachment; filename=%s\r\n",sMailFileNameFormat);
	//sprintf(sTemp,"Content-Disposition: attachment; filename=%s\r\n",sFileName.c_str());
	sSubHeader.append(sTemp);

	// 设置结束标志
	sSubHeader.append("\r\n");
	return sSubHeader;
}

//tstring CAppOctetStream::GetContentTypeString(void) const
//{
//	return _T("application/octet-stream");
//}

void CAppOctetStream::attach_file(FILE* pFileAtt, 
								  int nEncoding, 
								  tstring & sDestination)
{
	//CMIMECode* pEncoder = 0;
	//int nBytesRead;
	//char szBuffer[BYTES_TO_READ + 1];

	if( pFileAtt == NULL )
		return;
	switch( nEncoding )
	{
	case CMIMEMessage::BASE64:
		{
#ifdef WIN32
			_fseeki64(pFileAtt, 0, SEEK_END);
			const int nFileSize = _ftelli64(pFileAtt);
			_fseeki64(pFileAtt, 0, SEEK_SET);
#else
			fseeko(pFileAtt, 0, SEEK_END);
			const int nFileSize = ftello(pFileAtt);
			fseeko(pFileAtt, 0, SEEK_SET);
#endif
			char* pBuf = new char[nFileSize+1];
			fread(pBuf,1,nFileSize,pFileAtt);

			const int m_nWrapPos = 76;
			CBase64 pBase64;
			const tstring sAttachString(pBase64.Encode(pBuf, (int)nFileSize));
			const size_t nAttachSize = sAttachString.size();
			int nInsertIndex = 0;
			while (nInsertIndex*m_nWrapPos<nAttachSize)
			{
				if (nInsertIndex>0)
					sDestination.append("\r\n");
				sDestination.append(sAttachString.substr(nInsertIndex*m_nWrapPos,m_nWrapPos));
				nInsertIndex++;
			}
			delete[] pBuf;
			sDestination.append("\r\n");
			return;
			//try 
			//{
			//	pEncoder = new CBase64;
			//}catch(...)
			//{
			//	return;
			//}
		}break;
	default:
		break;
	}
	//if( pEncoder == NULL )
	//	return;
	//do
	//{
	//	try
	//	{
	//		nBytesRead = fread(szBuffer, 1, BYTES_TO_READ, pFileAtt);
	//	}catch( ... )
	//	{
	//		break;
	//	}
	//	szBuffer[ nBytesRead ] = 0;	
	//	sDestination.append(pEncoder->Encode( szBuffer, nBytesRead ));
	//	sDestination.append("\r\n");
	//} while( nBytesRead == BYTES_TO_READ );
	//sDestination.append("\r\n");
	//delete pEncoder;
}
