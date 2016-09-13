#include "StdAfx.h"
#include "EbcLanguages.h"
#include <iostream>
#include "ebc_public.h"

CEbcLanguages::CEbcLanguages(void)
{

}

CEbcLanguages::~CEbcLanguages(void)
{

}

bool CEbcLanguages::LoadLanguage(const char* lpszLanguageFilePath)
{
	std::ifstream pLanguageFile(lpszLanguageFilePath);
	if (!pLanguageFile.is_open()) return false;
	char lpszBuffer[1024];
	int nEmptyCount = 0;
	while (!pLanguageFile.eof())
	{
		memset(lpszBuffer,0,1024);
		pLanguageFile.getline(lpszBuffer,1024);
		if (lpszBuffer[0]=='#') continue;
		if (strlen(lpszBuffer)<=0)
		{
			if ((++nEmptyCount)>=100)
				break;
			continue;
		}else if (nEmptyCount>0)
			nEmptyCount = 0;

		// key:value
		const std::string s(lpszBuffer);
		const std::string::size_type find = s.find("=");
		if (find==std::string::npos) continue;
		const tstring sKey = s.substr(0,find);
		tstring sValue = s.substr(find+1);
		libEbc::replace(sValue,"\\r\\n","\r\n");
		const tstring sRealValue = libEbc::UTF82ACP(sValue.c_str());
		m_pLanguageList.insert(sKey,sRealValue,true,NULL,false);
	}
	pLanguageFile.close();
	return true;
}

bool CEbcLanguages::Get(const char* lpszKey, tstring& pOutValue) const
{
	return m_pLanguageList.find(lpszKey,pOutValue);
}
