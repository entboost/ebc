// data2headinfo.h file here
#ifndef __data2headinfo_h__
#define __data2headinfo_h__

#include "../stl/stldef.h"

namespace bo
{
	class CData2HeadInfo
	{
	public:
		enum Data2HeadInfoType
		{
			D2HIT_NORMAL	= 0x1

			, D2HIT_UNKNOWN	= 0xf
		};

		typedef boost::shared_ptr<CData2HeadInfo> pointer;
		static CData2HeadInfo::pointer create(Data2HeadInfoType type, unsigned int objectid)
		{
			return CData2HeadInfo::pointer(new CData2HeadInfo(type, objectid));
		}

		//unsigned int id(void) const {return m_id;}
		Data2HeadInfoType type(void) const {return m_type;}
		unsigned int objectid(void) const {return m_objectid;}
		unsigned short	count(void) const {return m_count;}

		static unsigned short getdata2headinfoSize(void)
		{
			return sizeof(Data2HeadInfoType)	// m_type
				+ sizeof(unsigned int)			// m_objectid
				+ sizeof(unsigned short);		// m_count
		}
	public:
		CData2HeadInfo(void)
			: m_type(D2HIT_UNKNOWN), m_objectid(0)
			, m_count(0)
		{}
		CData2HeadInfo(Data2HeadInfoType type, unsigned int objectid)
			: m_type(type), m_objectid(objectid)
			, m_count(0)
		{}
		virtual ~CData2HeadInfo(void)
		{}

		virtual void Serialize(bool isStoring, tfstream& ar)
		{
			ar.clear();
			if (isStoring)
			{
				// m_id
				//ar.write((const char*)&m_id, sizeof(m_id));

				// m_type
				ar.write((const char*)&m_type, sizeof(m_type));
				// m_objectid
				ar.write((const char*)&m_objectid, sizeof(m_objectid));
				// m_count
				ar.write((const char*)&m_count, sizeof(m_count));
			}else
			{
				// m_id
				//ar.read((char*)&m_id, sizeof(m_id));

				// m_type
				ar.read((char*)&m_type, sizeof(m_type));
				// m_objectid
				ar.read((char*)&m_objectid, sizeof(m_objectid));
				// m_count
				ar.read((char*)&m_count, sizeof(m_count));
			}
			
		}
	private:
		//unsigned int	m_id;
		Data2HeadInfoType	m_type;
		unsigned int	m_objectid;
		unsigned short	m_count;
	};

} // namespace bo

#endif // __data2headinfo_h__

