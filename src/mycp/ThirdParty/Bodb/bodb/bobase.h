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

// bobase.h file here
#ifndef __bobase_h__
#define __bobase_h__

#include "../sqlparser/sqlparser.h"
#include "bodbdef.h"
#include "fieldvariant.h"

namespace bo
{
	bool ItemValue2Variant(tagItemValue * itemValue, CFieldVariant::pointer variant);

	char * W2Char(const wchar_t * src, size_t & outSize, const char * stringcode = 0);
	wchar_t * Char2W(const char * src, size_t & outSize, const char * stringcode = 0);

} // namespace


#endif // __bobase_h__
