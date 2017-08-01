// chatroomobject.h file here
#ifndef __chatroomobject_h__
#define __chatroomobject_h__

#include <CGCBase/cgcSmartString.h>
//#include "ebstring.h"
#include <vector>

#ifndef EBFileString
#ifdef _QT_MAKE_
#include <QString>
#define EBFileString QString
#else
#define EBFileString tstring
#endif  // _QT_MAKE_
#endif  // EBFileString

namespace entboost {
namespace cr {

#ifdef WIN32
	typedef __int64				bigint;
#define cr_atoi64(a) _atoi64(a)
#else
	typedef long long			bigint;
#define cr_atoi64(a) atoll(a)
#endif
} // namespace cr

class EB_ChatRoomMsgItem
{
public:
	enum MsgItemType
	{
		MIT_TEXT
		, MIT_CHARFORMAT
		, MIT_OBJECT
		, MIT_RESOURCE
	};

	EB_ChatRoomMsgItem(void)
		: m_type(MIT_TEXT)
		, m_data(NULL), m_size(0)
	{}
	EB_ChatRoomMsgItem(const CEBString & text)
		: m_type(MIT_TEXT)
		, m_text(text)
		, m_data(NULL), m_size(0)
	{}
	EB_ChatRoomMsgItem(const char * text)
		: m_type(MIT_TEXT)
		, m_text(text)
		, m_data(NULL), m_size(0)
	{}
	EB_ChatRoomMsgItem(char * data, unsigned long size, MsgItemType type = MIT_OBJECT)
		: m_type(type)
		, m_text("")
		, m_data(data), m_size(size)
	{}
	EB_ChatRoomMsgItem(const EB_ChatRoomMsgItem& pItem)
		: m_data(NULL)
	{
		EB_ChatRoomMsgItem::operator =(pItem);
	}
	const EB_ChatRoomMsgItem& operator =(const EB_ChatRoomMsgItem& pItem)
	{
		clear();
		m_type = pItem.GetType();
		m_text = pItem.GetText();
		m_size = pItem.GetSize();
		if (m_size > 0)
		{
			m_data = new char[m_size+1];
			memset(m_data,0,m_size+1);
		}
		if (pItem.GetData() != NULL)
		{
			memcpy(m_data,pItem.GetData(),m_size);
		}
		return *this;
	}

	virtual ~EB_ChatRoomMsgItem(void)
	{
		clear();
	}
	void SetType(MsgItemType type) {m_type = type;}
	MsgItemType	GetType(void) const {return m_type;}
	void SetText(const char * value) {m_text = value;}
	void SetText(const CEBString& value) {m_text = value;}
	const CEBString & GetText(void) const {return m_text;}
	const char * GetData(void) const {return m_data;}
	void SetSize(unsigned long size)	// 鍏堢敓鎴愬唴瀛?
	{
		if (m_size < size)
		{
			clear();
		}
		m_size = size;
		if (m_size > 0 && m_data == NULL)
		{
			m_data = new char[m_size+1];
			memset(m_data,0,m_size+1);
		}
	}
	unsigned long GetSize(void) const {return m_size;}
	void SetData(const char * data)		// 鍐嶅～鍏呮暟鎹?
	{
		if (m_size > 0 && m_data != NULL)
		{
			memcpy(m_data,data,m_size);
		}
	}
	void UpdateData(char * data, unsigned long size)
	{
		if (m_size == size && m_data == NULL)
			m_data = data;
	}

private:
	void clear(void)
	{
		m_size = 0;
		if (m_data)
		{
			delete []m_data;
			m_data = NULL;
		}
	}
private:
	MsgItemType		m_type;
	CEBString		m_text;	// text or object filename
	char *			m_data;
	unsigned long	m_size;
};

// CHARFORMAT effects 
#ifndef CFE_BOLD
#define CFE_BOLD		0x0001
#endif
#ifndef CFE_ITALIC
#define CFE_ITALIC		0x0002
#endif
#ifndef CFE_UNDERLINE
#define CFE_UNDERLINE	0x0004
#endif
#ifndef CFE_STRIKEOUT
#define CFE_STRIKEOUT	0x0008
#endif
#ifndef CFE_PROTECTED
#define CFE_PROTECTED	0x0010
#endif
#ifndef CFE_LINK
#define CFE_LINK		0x0020
#endif
typedef struct _cr_charformat
{
	unsigned long	dwEffects;
	long			yHeight;
	unsigned long	crTextColor;
	char			szFaceName[32];
} CR_CHARFORMATA;
#define CR_CHARFORMATA_SIZE sizeof(CR_CHARFORMATA)

class EB_ChatRoomRichMsg
{
public:
	const EB_ChatRoomMsgItem* AddText(const char * sText)
	{
		if (sText != NULL && strlen(sText) > 0)
		{
			EB_ChatRoomMsgItem* pMsgItem = new EB_ChatRoomMsgItem(sText);
			m_list.push_back(pMsgItem);
			return pMsgItem;
		}
		return NULL;
	}
	const EB_ChatRoomMsgItem* AddResource(const char * sResourceId)
	{
		if (sResourceId != NULL && strlen(sResourceId) > 0)
		{
			EB_ChatRoomMsgItem* pMsgItem = new EB_ChatRoomMsgItem(sResourceId);
			pMsgItem->SetType(EB_ChatRoomMsgItem::MIT_RESOURCE);
			m_list.push_back(pMsgItem);
			return pMsgItem;
		}
		return NULL;
	}
	const EB_ChatRoomMsgItem* AddObject(char * lpData, unsigned long nSize)//, EB_ChatRoomMsgItem::MsgItemType type = EB_ChatRoomMsgItem::MIT_OBJECT)
	{
		if (lpData != NULL && nSize > 0)
		{
			EB_ChatRoomMsgItem::MsgItemType type = EB_ChatRoomMsgItem::MIT_OBJECT;
			EB_ChatRoomMsgItem* pMsgItem = new EB_ChatRoomMsgItem(lpData, nSize, type);
			m_list.push_back(pMsgItem);
			return pMsgItem;
		}
		return NULL;
	}

	const EB_ChatRoomMsgItem* GetMsgItem(size_t nIndex)
	{
		if (nIndex >= 0 && nIndex < m_list.size())
		{
			return m_list[nIndex];
		}
		return NULL;
	}

	bool IsInvalidateCharFormat(void) const {return (bool)(m_charFormat.GetType() == EB_ChatRoomMsgItem::MIT_CHARFORMAT && m_charFormat.GetSize()==CR_CHARFORMATA_SIZE);}
	void SetCharFormat(const EB_ChatRoomMsgItem& pCharFormat) {m_charFormat=pCharFormat;}
	const EB_ChatRoomMsgItem& GetCharFormat(void) const {return m_charFormat;}

	bool IsEmpty(void) const {return m_list.empty();}
	const std::vector<EB_ChatRoomMsgItem*> & GetList(void) const {return m_list;}
	void SetMsgId(cr::bigint nMsgId) {m_nMsgId=nMsgId;}
	cr::bigint GetMsgId(void) const {return m_nMsgId;}
	void SetSubType(int v) {m_nSubType = v;}
	int GetSubType(void) const {return m_nSubType;}

	//bool m_bOffLineMsg;
	CEBString m_sMsgTime;	// null is current time
	EB_ChatRoomRichMsg(cr::bigint nMsgId)
		: m_nMsgId(nMsgId)
		, m_nSubType(0)
	{
	}
	EB_ChatRoomRichMsg(void)
		: m_nMsgId(0)
		, m_nSubType(0)
	{}
	EB_ChatRoomRichMsg(const EB_ChatRoomRichMsg& pRichMsg)
		: m_nMsgId(0)
		, m_nSubType(0)
	{
		EB_ChatRoomRichMsg::operator =(pRichMsg);
	}
	const EB_ChatRoomRichMsg& operator =(const EB_ChatRoomRichMsg& pRichMsg)
	{
		clear();
		//m_bOffLineMsg = pRichMsg.m_bOffLineMsg;
		m_sMsgTime = pRichMsg.m_sMsgTime;
		m_nMsgId = pRichMsg.GetMsgId();
		m_nSubType = pRichMsg.GetSubType();
		m_charFormat = pRichMsg.GetCharFormat();
		const std::vector<EB_ChatRoomMsgItem*> & pList = pRichMsg.GetList();
		for (size_t i=0; i<pList.size(); i++)
		{
			const EB_ChatRoomMsgItem* pMsgItem = pList[i];
			m_list.push_back(new EB_ChatRoomMsgItem(*pMsgItem));
		}
		return *this;
	}
	virtual ~EB_ChatRoomRichMsg(void)
	{
		clear();
	}
	void clear(void)
	{
		for (size_t i=0; i<m_list.size(); i++)
		{
			delete m_list[i];
		}
		m_list.clear();
	}
protected:
	cr::bigint m_nMsgId;
	std::vector<EB_ChatRoomMsgItem*> m_list;
	EB_ChatRoomMsgItem m_charFormat;
	int m_nSubType;
};

} // namespace entboost

#endif // __chatroomobject_h__

