/*
    Bodb is a software library that implements a simple SQL database engine.
    Copyright (C) 2009-2010  Akee Yang <akee.yang@gmail.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

// fieldvariant.h file here
#ifndef __fieldvariant_h__
#define __fieldvariant_h__

#include <stl/locklist.h>
#include <boost/shared_ptr.hpp>
#include "bodbdef.h"

namespace bo
{
	void CopyBuffer(PBUFFER dest, const char * src, uinteger size);

	PFIELDVARIANT NewFieldVariant(void);
	void CopyFieldVariant(const PFIELDVARIANT src, PFIELDVARIANT dest);
	void FreeFieldVariant(PFIELDVARIANT fieldvariant, bool deleteObject);
/*
	class CBuffer
	{
	public:
		typedef boost::shared_ptr<CBuffer> pointer;
		static CBuffer::pointer create(void)
		{
			return CBuffer::pointer(new CBuffer());
		}
		static CBuffer::pointer create(void * buffer, uinteger size)
		{
			return CBuffer::pointer(new CBuffer(buffer, size));
		}
		static CBuffer::pointer create(const void * buffer, uinteger size)
		{
			return CBuffer::pointer(new CBuffer(buffer, size));
		}

	public:
		CBuffer(void * buffer, uinteger size)
			: m_buffer(buffer), m_size(size)
		{
		}
		CBuffer(const void * buffer, uinteger size)
			: m_buffer(0), m_size(size)
		{
			if (buffer != 0 && m_size > 0)
			{
				m_buffer = new void *[m_size];
				memcpy(m_buffer, buffer, m_size);
			}
		}
		CBuffer(void)
			: m_buffer(0), m_size(0)
		{
		}
		~CBuffer(void)
		{
			if (m_buffer != 0)
				delete[] m_buffer;
		}
	private:
		void * m_buffer;
		uinteger m_size;
	};
*/
	class CFieldVariant
		: public FIELDVARIANT
	{
	public:
		typedef boost::shared_ptr<CFieldVariant> pointer;
		static CFieldVariant::pointer create(FieldType vartype)
		{
			return CFieldVariant::pointer(new CFieldVariant(vartype));
		}
		static CFieldVariant::pointer create(const PFIELDVARIANT variant)
		{
			return CFieldVariant::pointer(new CFieldVariant(variant));
		}

		//CLockList<CBuffer::pointer> m_value;

		//void realsize(uinteger newv) {m_realsize = newv;}
		//uinteger realsize(void) const {return m_realsize;}

		// Compare
		bool equal(const CFieldVariant::pointer& compare) const;
		bool unequal(const CFieldVariant::pointer& compare) const;
		bool greater(const CFieldVariant::pointer& compare) const;
		bool greaterEqual(const CFieldVariant::pointer& compare) const;
		bool less(const CFieldVariant::pointer& compare) const;
		bool lessEqual(const CFieldVariant::pointer& compare) const;
		bool isLike(const CFieldVariant::pointer& compare) const;
		bool isLike(const BUFFER& buffer, const BUFFER& compare) const;
		bool isLike(const NBUFFER& buffer, const NBUFFER& compare) const;
		bool isNull(void) const;
		bool isNotNull(void) const;

		// Set
		void setNull(void);
		bool setString(const char * value, int size);
		bool setCurrentTime(void);
		bool setBool(bool value);
		bool setInt(int value);
		bool setBigInt(bo::bigint value);
		bool setReal(double value);
		bool setIntu(uinteger value);

		// Get
		bool getBool(bool defv = false) const;
		int getInt(int defv = 0) const;
		uinteger getIntu(uinteger defv = 0) const;
		bigint getBigInt(bigint defv = 0) const;
		double getReal(double defv = 0.0) const;
		char * getString(int & outSize) const;
		std::string getString(void) const;
		wchar_t * getNString(int & outSize) const;
		std::wstring getNString(void) const;
		PFIELDVARIANT getValue(void) const;

	public:
		CFieldVariant(FieldType vartype);
		CFieldVariant(const PFIELDVARIANT variant);
		CFieldVariant(void);
		virtual ~CFieldVariant(void);

		//virtual void Serialize(bool isStoring, tfstream& ar, usmallint & outDoSize);
		virtual void Serialize(bool isStoring, tfstream& ar, uinteger size1, bool bDefaultVariant, uinteger & outDoSize);

	private:
		//uinteger m_realsize;
	};

	const CFieldVariant::pointer boNullFieldVariant;

} // namespace

#endif // __fieldvariant_h__

