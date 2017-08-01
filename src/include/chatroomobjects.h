// chatroomobjects.h file here
#ifndef __chatroomobjects_h__
#define __chatroomobjects_h__

#include <boost/shared_ptr.hpp>
#include "chatroomobject.h"

namespace entboost {

class CEBChatRoomRichMsg
	: public EB_ChatRoomRichMsg
{
public:
	typedef boost::shared_ptr<CEBChatRoomRichMsg> pointer;
	static CEBChatRoomRichMsg::pointer create(mycp::bigint nMsgId)
	{
		return CEBChatRoomRichMsg::pointer(new CEBChatRoomRichMsg(nMsgId));
	}
	static CEBChatRoomRichMsg::pointer create(const EB_ChatRoomRichMsg& pRichMsg)
	{
		return CEBChatRoomRichMsg::pointer(new CEBChatRoomRichMsg(pRichMsg));
	}

	bool GetFormatMsg(tstring & pOutFormat, std::vector<size_t> & pOutObjectIndex)
	{
		tstring sFormat;
		char buffer[20];
		for (size_t i=0; i<m_list.size(); i++)
		{
			const EB_ChatRoomMsgItem* pMsgItem = m_list[i];
			if (pMsgItem->GetType() == EB_ChatRoomMsgItem::MIT_TEXT)
			{
				sprintf(buffer, "t,%d,", (int)pMsgItem->GetText().size());
				sFormat.append(buffer);
				sFormat.append(pMsgItem->GetText());
			}else if (pMsgItem->GetType() == EB_ChatRoomMsgItem::MIT_RESOURCE)
			{
				sprintf(buffer, "r,%d,", (int)pMsgItem->GetText().size());
				sFormat.append(buffer);
				sFormat.append(pMsgItem->GetText());
			}else if (pMsgItem->GetType() == EB_ChatRoomMsgItem::MIT_OBJECT)
			{
				sprintf(buffer, "o,%d,", (int)pMsgItem->GetSize());
				sFormat.append(buffer);
				pOutObjectIndex.push_back(i);
			}
		}
		sprintf(buffer, "%d,", (int)sFormat.size());
		pOutFormat.append(buffer);
		pOutFormat.append(sFormat);
		return m_list.empty() ? false : true;
	}
	unsigned long GetMsgSize(bool bGetObjectSize = true) const
	{
		int pOutSize = 0;
		char buffer[20];
		// 先计算 string format长度
		for (size_t i=0; i<m_list.size(); i++)
		{
			const EB_ChatRoomMsgItem* pMsgItem = m_list[i];
			if (pMsgItem->GetType() == EB_ChatRoomMsgItem::MIT_TEXT)
			{
				sprintf(buffer, "t,%d,", (int)pMsgItem->GetText().size());
				pOutSize += (int)(strlen(buffer)+pMsgItem->GetText().size());
			}else if (pMsgItem->GetType() == EB_ChatRoomMsgItem::MIT_RESOURCE)
			{
				sprintf(buffer, "r,%d,", (int)pMsgItem->GetText().size());
				pOutSize += (int)(strlen(buffer)+pMsgItem->GetText().size());
			}else if (pMsgItem->GetType() == EB_ChatRoomMsgItem::MIT_OBJECT)
			{
				sprintf(buffer, "o,%d,", (int)pMsgItem->GetSize());
				pOutSize += (int)strlen(buffer);
			}
		}
		if (pOutSize==0) return 0;
		// 加上字体格式长度（字体可加可不加）
		if (IsInvalidateCharFormat())
			pOutSize += (4 + (int)m_charFormat.GetSize());	// 4="f%02d"
		// 加上协议头长度
		sprintf(buffer, "%d,", (int)pOutSize);
		pOutSize += (int)strlen(buffer);
		if (!bGetObjectSize)
			return (unsigned long)pOutSize;

		// 加上对象长度
		for (size_t i=0; i<m_list.size(); i++)
		{
			const EB_ChatRoomMsgItem* pMsgItem = m_list[i];
			if (pMsgItem->GetType() == EB_ChatRoomMsgItem::MIT_OBJECT)
			{
				pOutSize += pMsgItem->GetSize();
			}
		}
		return (unsigned long)pOutSize;
	}
	void SetFormatMsg(const std::string & pInFormat)
	{
		size_t index = 0;
		while (pInFormat.size() > (index+4))
		{
			const std::string sKey = pInFormat.substr(index, 2);
			index += 2;
			if (sKey == "t,")
			{
				const int nlen = atoi(pInFormat.substr(index, 6).c_str());
				if (nlen <= 0) break;
				std::string::size_type find = pInFormat.find(",",index+1);
				if (find==std::string::npos) break;
				index = find+1;
				const std::string sText = pInFormat.substr(index, nlen);
				m_list.push_back(new EB_ChatRoomMsgItem(sText));
				index += nlen;
			}else if (sKey == "r,")
			{
				const int nlen = atoi(pInFormat.substr(index, 6).c_str());
				if (nlen <= 0) break;
				std::string::size_type find = pInFormat.find(",",index+1);
				if (find==std::string::npos) break;
				index = find+1;
				const std::string sText = pInFormat.substr(index, nlen);
				EB_ChatRoomMsgItem* pMsgItem = new EB_ChatRoomMsgItem(sText);
				pMsgItem->SetType(EB_ChatRoomMsgItem::MIT_RESOURCE);
				m_list.push_back(pMsgItem);
				index += nlen;
			}else if (sKey == "o,")
			{
				const int nlen = atoi(pInFormat.substr(index, 8).c_str());
				if (nlen <= 0) break;
				std::string::size_type find = pInFormat.find(",",index+1);
				if (find==std::string::npos) break;
				index = find+1;
				m_list.push_back(new EB_ChatRoomMsgItem(NULL, nlen, EB_ChatRoomMsgItem::MIT_OBJECT));
			}else
			{
				break;
			}
		}
	}
	bool ExistItemType(const std::string & pInFormat, EB_ChatRoomMsgItem::MsgItemType nItemType) const
	{
		size_t index = 0;
		while (pInFormat.size() > (index+4))
		{
			const std::string sKey = pInFormat.substr(index, 2);
			index += 2;
			if (sKey == "t,")
			{
				if (nItemType==EB_ChatRoomMsgItem::MIT_TEXT) return true;
				const int nlen = atoi(pInFormat.substr(index, 6).c_str());
				if (nlen <= 0) break;
				std::string::size_type find = pInFormat.find(",",index+1);
				if (find==std::string::npos) break;
				index = find+1;
				index += nlen;
			}else if (sKey == "r,")
			{
				if (nItemType==EB_ChatRoomMsgItem::MIT_RESOURCE) return true;
				const int nlen = atoi(pInFormat.substr(index, 6).c_str());
				if (nlen <= 0) break;
				std::string::size_type find = pInFormat.find(",",index+1);
				if (find==std::string::npos) break;
				index = find+1;
				index += nlen;
			}else if (sKey == "o,")
			{
				if (nItemType==EB_ChatRoomMsgItem::MIT_OBJECT) return true;
				const int nlen = atoi(pInFormat.substr(index, 8).c_str());
				if (nlen <= 0) break;
				std::string::size_type find = pInFormat.find(",",index+1);
				if (find==std::string::npos) break;
				index = find+1;
			}else
			{
				break;
			}
		}
		return false;
	}

	CEBChatRoomRichMsg(mycp::bigint nMsgId)
		: EB_ChatRoomRichMsg(nMsgId)
	{
	}
	CEBChatRoomRichMsg(void)
		: EB_ChatRoomRichMsg()
	{}
	CEBChatRoomRichMsg(const EB_ChatRoomRichMsg& pRichMsg)
		: EB_ChatRoomRichMsg(pRichMsg)
	{
	}
	virtual ~CEBChatRoomRichMsg(void)
	{
	}
};

} // namespace entboost

#endif // __chatroomobjects_h__

