// MailMessage.cpp: implementation of the CMailMessage class.
// 

//#include "Windows.h"
#include "MailMessage.h"
#include "Base64.h"
#include <time.h>
#include "string.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMailMessage::CMailMessage(const char* sCharset)
: m_sCharset(sCharset)
{
	SetCharsPerLine( 76 );
}

CMailMessage::~CMailMessage(void)
{
}

bool CMailMessage::AddRecipient(const char* szEmailAddress,const char* szFriendName,RECIPIENTS_TYPE type)
{
	if (szEmailAddress == NULL || strlen(szEmailAddress)==0 || szFriendName == NULL)
		return false;

	CRecipient to;
	to.m_sEmailAddress = szEmailAddress;
	to.m_sFriendName = szFriendName;

	// 根据收信人的性质分类存放
    switch (type)
	{
    case TO: 
		// 处理普通的收信人地址
		m_Recipients.push_back(to);
		break;
    case CC:
		// 处理抄送的收信人地址
		m_CCRecipients.push_back(to);
		break;
    case BCC:
		// 处理暗送的收信人地址
		m_BCCRecipients.push_back(to);
		break;
    }

	return true;
}

bool CMailMessage::GetRecipient(tstring & sEmailAddress, tstring & sFriendName, int nIndex, RECIPIENTS_TYPE type /* <JFO> */)
{
	CRecipient to;
	if( nIndex < 0 || nIndex >= m_Recipients.size() )
		return false;

	{
		switch(type)
		{
		case TO:  to = m_Recipients[ nIndex ];    break;
		case CC:  to = m_CCRecipients[ nIndex ];  break;
		case BCC: to = m_BCCRecipients[ nIndex ]; break;
		}
	}

	sEmailAddress = to.m_sEmailAddress;
	sFriendName = to.m_sFriendName;

	return true;
}

int CMailMessage::GetNumRecipients(RECIPIENTS_TYPE type /* <JFO> */) const
{
      int number = 0;

      switch(type)
      {
	  case TO:  number = m_Recipients.size();    break;
	  case CC:  number = m_CCRecipients.size();  break;
	  case BCC: number = m_BCCRecipients.size(); break;
      }

      return number;
}

bool CMailMessage::GetMainAddressName(const tstring & sFullAddress, tstring& pOutName, tstring& pOutMail)
{
	const int nTemp = sFullAddress.find('<');
	if (nTemp >= 0)
	{
		pOutName = sFullAddress.substr(0,nTemp);
		const int nMark = sFullAddress.find('>',nTemp+1);
		if(nMark < 0)
		{
			return false;
		}
		pOutMail = sFullAddress.substr(nTemp + 1,nMark - (nTemp + 1));
		TrimMailName(pOutName);
	}
	else
	{
		pOutMail = sFullAddress;
		pOutName = "";
	}
	return true;
}

bool CMailMessage::AddMultipleRecipients(const char* szRecipients,RECIPIENTS_TYPE type)
{
	char *pBuf = 0;
	size_t nPos,nStart,nLength;
	tstring strTemp,strEmail,strFriend;
	//int nTemp,nMark;

	if (szRecipients == NULL )
		return false;
	
	// 分析并添加收信人
	nLength = strlen( szRecipients );
	pBuf = new char[nLength + 1];
	// 复制传入的参数，处理这个拷贝，而不是参数本身
	strcpy(pBuf,szRecipients);
	for (nPos = 0,nStart = 0;nPos <= nLength;nPos++)
	{
		if (pBuf[nPos] == ';' || !pBuf[nPos])
		{
			// 提取用;分开的子串
			pBuf[nPos] = '\0';
			strTemp = &pBuf[nStart];

			// 将名字和邮件地址分开
			if (!GetMainAddressName(strTemp,strFriend,strEmail))
			{
				delete[] pBuf;
				return false;
			}
			//nTemp = strTemp.find('<');
			//if (nTemp >= 0)
			//{
			//	strFriend = strTemp.substr(0,nTemp);
			//	nMark = strTemp.find('>');
			//	if(nMark < nTemp)
			//	{
			//		delete[] pBuf;
			//		return false;
			//	}
			//	if (nMark > -1)
			//		nMark = strTemp.length() - 1;
			//	strEmail = strTemp.substr(nTemp + 1,nMark - (nTemp + 1));
			//}
			//else
			//{
			//	strEmail = strTemp;
			//	strFriend = "";
			//}

			// 将分析的收信人地址加到记录中
			AddRecipient(strEmail.c_str(),strFriend.c_str(),type);
			nStart = nPos + 1;
		}
	}
	delete[] pBuf;
	return true;
}

void CMailMessage::FormatMailMessage(void)
{
	start_header();
	prepare_header();
	end_header();
	prepare_body();
}

//inline tstring str_convert(const char * strSource, int sourceCodepage, int targetCodepage)
//{
//	int unicodeLen = MultiByteToWideChar(sourceCodepage, 0, strSource, -1, NULL, 0);
//	if (unicodeLen <= 0) return "";
//
//	wchar_t * pUnicode = new wchar_t[unicodeLen];
//	memset(pUnicode,0,(unicodeLen)*sizeof(wchar_t));
//
//	MultiByteToWideChar(sourceCodepage, 0, strSource, -1, (wchar_t*)pUnicode, unicodeLen);
//
//	char * pTargetData = 0;
//	int targetLen = WideCharToMultiByte(targetCodepage, 0, (wchar_t*)pUnicode, -1, (char*)pTargetData, 0, NULL, NULL);
//	if (targetLen <= 0)
//	{
//		delete[] pUnicode;
//		return "";
//	}
//
//	pTargetData = new char[targetLen];
//	memset(pTargetData, 0, targetLen);
//
//	WideCharToMultiByte(targetCodepage, 0, (wchar_t*)pUnicode, -1, (char *)pTargetData, targetLen, NULL, NULL);
//
//	tstring result(pTargetData);
//	//	tstring result(pTargetData, targetLen);
//	delete[] pUnicode;
//	delete[] pTargetData;
//	return   result;
//}
//inline tstring ACP2UTF8(const char* sString)
//{
//	return str_convert(sString,CP_ACP,CP_UTF8);
//}
//inline tstring UTF82ACP(const char* sString)
//{
//	return str_convert(sString,CP_UTF8,CP_ACP);
//}

// 创建邮件头
void CMailMessage::prepare_header()
{
	tstring strTemp;
	CBase64 pBase64;

	// 发信人地址部分
	// From:  =?utf-8?B?RlTkuK3mlofnvZEgLSBGVENoaW5lc2UuY29t?= <weeklynewsletter@newsletter.ftchinese.com>
	tstring sFromName;
	tstring sFromMail;
	GetMainAddressName(m_sFrom,sFromName,sFromMail);
	if (sFromName.empty())
	{
		strTemp = "From: " + sFromMail;
	}
	else
	{
		strTemp = "From: =?";
		strTemp.append(m_sCharset);
		strTemp.append("?B?");
		strTemp.append(pBase64.Encode(sFromName.c_str(), sFromName.size()));
		strTemp.append("?= <");
		strTemp.append(sFromMail);
		strTemp.append(">");
	}
	add_header_line(strTemp.c_str());

	// 收信人地址部分
	// To: "=?utf-8?B?YWtlZS55YW5nQGdtYWlsLmNvbQ==?=" <akee.yang@gmail.com>
	// ****
	strTemp = "To: ";
	tstring strEmail;
	tstring strFriend;

	// 逐个处理收信人
	int nCount = GetNumRecipients();
	for(int i = 0;i < nCount;i++)
	{
		GetRecipient(strEmail,strFriend,i);
		strTemp += (i > 0 ? "," :"");
		if (!strFriend.empty())
		{
			strTemp.append("\"=?");
			strTemp.append(m_sCharset);
			strTemp.append("?B?");
			strTemp.append(pBase64.Encode(strFriend.c_str(), strFriend.size()));
			strTemp.append("?=\" ");
		}
		//strTemp += strFriend;
		strTemp += "<";
		strTemp += strEmail;
		strTemp += ">";
	}
	add_header_line(strTemp.c_str());


	// 处理抄送部分
	{
		strTemp = "Cc: ";
		tstring strEmail;
		tstring strFriend;
		nCount = GetNumRecipients(CC);
		for(int i = 0; i < nCount; i++)
		{
			GetRecipient(strEmail,strFriend,i,CC);
			strTemp += (i > 0 ? "," : "" );
			if (!strFriend.empty())
			{
				strTemp.append("\"=?");
				strTemp.append(m_sCharset);
				strTemp.append("?B?");
				strTemp.append(pBase64.Encode(strFriend.c_str(), strFriend.size()));
				strTemp.append("?=\" ");
			}
			//strTemp += strFriend;
			strTemp += "<";
			strTemp += strEmail;
			strTemp += ">";
		}
		add_header_line(strTemp.c_str());
	}

	// 处理日期,日期的格式为Fri, 21 Dec 01 21:51:30
	struct tm *newtime;
	time_t ltime;
	time(&ltime);
	newtime = gmtime(&ltime);
	char szDT[128];
	strftime(szDT, 128, "%a, %d %b %Y %H:%M:%S %Z", newtime);
	//strftime(szDT, 128, "%a, %d %b %y %H:%M:%S %Z", newtime);
	strTemp = "Date: ";
	strTemp.append(szDT);
	add_header_line(strTemp.c_str());

	// 处理主题部分
	strTemp = "Subject: =?";
	strTemp.append(m_sCharset);
	strTemp.append("?B?");
	strTemp.append(pBase64.Encode(m_sSubject.c_str(), m_sSubject.size()));
	strTemp.append("?=");
	//strTemp = "Subject: " + m_sSubject;
	add_header_line(strTemp.c_str());
}

void CMailMessage::prepare_body()
{
	// 加上必要的结束符CR/LF
	const size_t nSize = m_sBody.size();
	if (nSize<2 || m_sBody.substr(nSize-2) != "\r\n")
		m_sBody.append("\r\n");
}

void CMailMessage::start_header()
{
	m_sHeader.clear();
}

void CMailMessage::end_header()
{
	m_sHeader.append("\r\n");
}

void CMailMessage::add_header_line(const char* szHeaderLine)
{
	m_sHeader.append(szHeaderLine);
	m_sHeader.append("\r\n");
}

