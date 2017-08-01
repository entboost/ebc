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

#include <string.h>
#include "bobase.h"

namespace bo
{
	bool ItemValue2Variant(tagItemValue * itemValue, CFieldVariant::pointer variant)
	{
		BOOST_ASSERT (itemValue != 0 && variant.get() != 0);

		switch (itemValue->value_type)
		{
		case VALUE_STRING:
			{
				if (itemValue->u.value_string == 0)
					return false;
				int size = strlen(itemValue->u.value_string);
				variant->setString(itemValue->u.value_string, size);
			}break;
		case VALUE_INT:
			{
				variant->setInt(itemValue->u.value_int);
			}break;
		case VALUE_BIGINT:
			{
				variant->setBigInt(itemValue->u.value_bigint);
			}break;
		case VALUE_REAL:
			{
				variant->setReal(itemValue->u.value_real);
			}break;
		case VALUE_BOOL:
			{
				variant->setBool(itemValue->u.value_bool);
			}break;
		case VALUE_NULL:
			{
				variant->setNull();
			}break;
		case VALUE_DEFAULT:
			{
				// ??
			}break;
		case VALUE_FUNC_CURRENT_TIMESTAMP:
			{
				// ??
				//variant->setCurrentTime();
			}break;
		default:
			return false;
			break;
		}

		return true;
	}

	char * W2Char(const wchar_t * src, size_t & outSize, const char * stringcode)
	{
		char * oldLocale = 0;
		char * result = 0;

		if (stringcode != 0)
		{
			oldLocale = setlocale(LC_ALL, NULL);
			setlocale(LC_ALL, stringcode);
		}

		outSize = wcsrtombs(NULL, &src, 0, NULL);
		if (outSize > 0)
		{
			result = new char[outSize+1];
			//memset(pTargetData,0,(outSizev)*sizeof(char));
			wcsrtombs(result, &src, outSize, NULL);
			result[outSize] = '\0';
		}

		if (oldLocale != 0)
		{
			// ? setlocale(LC_ALL, oldLocale);
		}
		return result;
	}
	//
	// test CODE
	// char * cIn = "我的中国ab1";
	// std::wstring wOut = cgcString::Char2W(cIn);
	//
	wchar_t * Char2W(const char * src, size_t & outSize, const char * stringcode)
	{
		// ? GBK	.936
		// ? UTF8	.65001
		// gcc: zh_CN.GBK
		char * oldLocale = 0;
		wchar_t * result = 0;

		if (stringcode != 0)
		{
			oldLocale = setlocale(LC_ALL, NULL);
			setlocale(LC_ALL, stringcode);
		}

		outSize = mbsrtowcs(NULL, (const char**)&src, 0, 0);
		if (outSize > 0)
		{
			outSize += 1;

			result = new wchar_t[outSize+1];
			//memset(pTargetData,0,(outSize)*sizeof(wchar_t));
			mbsrtowcs(result, &src, outSize, 0);
			result[outSize] = L'\0';
		}
		if (oldLocale != 0)
		{
			// ? setlocale(LC_ALL, oldLocale);
		}
		return result;
	}

}	// namespace

