// data2offsetinfo.h file here
#ifndef __data2offsetinfo_h__
#define __data2offsetinfo_h__

#include "../stl/stldef.h"
#include <boost/shared_ptr.hpp>
#include "pageheadinfo.h"

namespace bo
{
	class CData2OffsetInfo
	{
	public:
		typedef boost::shared_ptr<CData2OffsetInfo> pointer;
		static CData2OffsetInfo::pointer create(CPageHeadInfo::pointer pageHeadInfo, unsigned short offset, unsigned short index)
		{
			return CData2OffsetInfo::pointer(new CData2OffsetInfo(pageHeadInfo, offset, index));
		}

	public:
		CPageHeadInfo::pointer page(void) const {return m_pageHeadInfo;}
		unsigned short offset(void) const {return m_offset;}
		unsigned short index(void) const {return m_index;}

	public:
		CData2OffsetInfo(CPageHeadInfo::pointer pageHeadInfo, unsigned short offset, unsigned short index)
			: m_pageHeadInfo(pageHeadInfo), m_offset(offset), m_index(index)
		{
			BOOST_ASSERT (m_pageHeadInfo.get() != NULL);
		}
		virtual ~CData2OffsetInfo(void)
		{
			m_pageHeadInfo.reset();
		}

	private:
		CPageHeadInfo::pointer	m_pageHeadInfo;
		unsigned short			m_offset;
		unsigned short			m_index;
	};

} // namespace bo

#endif // __data2offsetinfo_h__


