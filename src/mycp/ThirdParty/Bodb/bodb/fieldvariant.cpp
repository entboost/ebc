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
#include <complex>
#include <sys/timeb.h>
#include "fieldvariant.h"
#include "bobase.h"

namespace bo
{
	// 0 -> '0'; 0-9
	char Number2Char(short in)
	{
		return '0' + (in-0);
	}
	// '0' -> 0; 0-9
	short Char2Number(char in)
	{
		return (in-'0');
	}
	//// 0-0; 1-10; 2-100
	//uinteger GetSizeNumber(short size)
	//{
	//	uinteger result = 0;
	//	while ((size--) > 0)
	//	{
	//		if (result == 0)
	//			result = 10;
	//		else
	//			result *= 10;
	//	}
	//	return result;
	//}

	void CopyBuffer(PBUFFER dest, const char * src, uinteger size)
	{
		if (dest != 0 && src != 0 && size > 0)
		{
			dest->size = size;
			dest->buffer = new char[size+1];
			memcpy(dest->buffer, src, size);
			dest->buffer[size] = '\0';
		}
	}

	void CopyBuffer(const PBUFFER src, PBUFFER dest)
	{
		dest->size = 0;
		dest->buffer = 0;
		if (src->size > 0 && src->buffer != 0)
		{
			dest->size = src->size;
			dest->buffer = new char[src->size+1];
			memcpy(dest->buffer, src->buffer, src->size);
			dest->buffer[src->size] = '\0';
		}
	}
	void CopyBuffer(const PNBUFFER src, PNBUFFER dest)
	{
		dest->size = 0;
		dest->buffer = 0;
		if (src->size > 0 && src->buffer != 0)
		{
			dest->size = src->size;
			dest->buffer = new wchar_t[src->size+1];
			memcpy(dest->buffer, src->buffer, src->size);
			dest->buffer[src->size] = L'\0';
		}
	}
	void FreeBuffer(PBUFFER buffer, bool deleteObject)
	{
		if (buffer != 0)
		{
			buffer->size = 0;
			if (buffer->buffer != 0)
			{
				delete[] buffer->buffer;
				buffer->buffer = 0;
			}
			if (deleteObject)
			{
				delete buffer;
			}
		}
	}
	void FreeBuffer(PNBUFFER buffer, bool deleteObject)
	{
		if (buffer != 0)
		{
			buffer->size = 0;
			if (buffer->buffer != 0)
			{
				delete[] buffer->buffer;
				buffer->buffer = 0;
			}
			if (deleteObject)
			{
				delete buffer;
			}
		}
	}
	PFIELDVARIANT NewFieldVariant(void)
	{
		PFIELDVARIANT result = new FIELDVARIANT;
		memset(result, 0, sizeof(FIELDVARIANT));
		return result;
	}

	void CopyFieldVariant(const PFIELDVARIANT src, PFIELDVARIANT dest)
	{
		if (src != 0)
		{
			dest->VARTYPE = src->VARTYPE;
			switch (dest->VARTYPE)
			{
			case FT_BOOLEAN:
			case FT_TINYINT:
			case FT_UTINYINT:
			case FT_SMALLINT:
			case FT_USMALLINT:
			case FT_INTEGER:
			case FT_UINTEGER:
			case FT_BIGINT:
			case FT_UBIGINT:
			case FT_FLOAT:
			case FT_REAL:
			case FT_DOUBLE:
				memcpy(dest, src, sizeof(FIELDVARIANT));
				break;
			case FT_NUMERIC:
				if (src->v.numericVal != 0)
				{
					dest->v.numericVal = new NUMERIC;
					memcpy(dest->v.numericVal, src->v.numericVal, sizeof(NUMERIC));
				}break;
			case FT_DATE:
				{
					memcpy(&dest->v.dateVal, &src->v.dateVal, sizeof(DATE));
				}break;
			case FT_TIME:
				{
					memcpy(&dest->v.timeVal, &src->v.timeVal, sizeof(TIME));
				}break;
			case FT_TIMESTAMP:
				{
					memcpy(&dest->v.timestampVal, &src->v.timestampVal, sizeof(TIMESTAMP));
				}break;
			case FT_CHAR:
				CopyBuffer(&src->v.charVal, &dest->v.charVal);
				break;
			case FT_VARCHAR:
				CopyBuffer(&src->v.varcharVal, &dest->v.varcharVal);
				break;
			case FT_CLOB:
				CopyBuffer(&src->v.clobVal, &dest->v.clobVal);
				break;
			case FT_NCHAR:
				CopyBuffer(&src->v.ncharVal, &dest->v.ncharVal);
				break;
			case FT_NVARCHAR:
				CopyBuffer(&src->v.nvarcharVal, &dest->v.nvarcharVal);
				break;
			case FT_BINARY:
				CopyBuffer(&src->v.lobVal, &dest->v.lobVal);
				break;
			case FT_VARBINARY:
				CopyBuffer(&src->v.varlobVal, &dest->v.varlobVal);
				break;
			default:
				break;
			}
		}
	}

	void FreeFieldVariant(PFIELDVARIANT fieldvariant, bool deleteObject)
	{
		if (fieldvariant != 0)
		{
			switch (fieldvariant->VARTYPE)
			{
			case FT_NUMERIC:
				if (fieldvariant->v.numericVal)
				{
					delete fieldvariant->v.numericVal;
					fieldvariant->v.numericVal = 0;
				}break;
			//case FT_DATE:
			//	if (fieldvariant->v.dateVal)
			//	{
			//		delete fieldvariant->v.dateVal;
			//		fieldvariant->v.dateVal = 0;
			//	}break;
			//case FT_TIME:
			//	if (fieldvariant->v.timeVal)
			//	{
			//		delete fieldvariant->v.timeVal;
			//		fieldvariant->v.timeVal = 0;
			//	}break;
			//case FT_TIMESTAMP:
			//	if (fieldvariant->v.timestampVal)
			//	{
			//		delete fieldvariant->v.timestampVal;
			//		fieldvariant->v.timestampVal = 0;
			//	}break;
			case FT_CHAR:
				fieldvariant->v.charVal.size = 0;
				if (fieldvariant->v.charVal.buffer)
				{
					delete[] fieldvariant->v.charVal.buffer;
					fieldvariant->v.charVal.buffer = 0;
				}break;
			case FT_VARCHAR:
				{
					if (fieldvariant->v.varcharVal.buffer)
					{
						delete[] fieldvariant->v.varcharVal.buffer;
						fieldvariant->v.varcharVal.buffer = 0;
					}
					fieldvariant->v.varcharVal.size = 0;
				}break;
			case FT_CLOB:
				{
					if (fieldvariant->v.clobVal.buffer)
					{
						delete[] fieldvariant->v.clobVal.buffer;
						fieldvariant->v.clobVal.buffer = 0;
					}
					fieldvariant->v.clobVal.size = 0;
				}break;
			case FT_NCHAR:
				fieldvariant->v.ncharVal.size = 0;
				if (fieldvariant->v.ncharVal.buffer)
				{
					delete[] fieldvariant->v.ncharVal.buffer;
					fieldvariant->v.ncharVal.buffer = 0;
				}break;
			case FT_NVARCHAR:
				fieldvariant->v.nvarcharVal.size = 0;
				if (fieldvariant->v.nvarcharVal.buffer)
				{
					delete[] fieldvariant->v.nvarcharVal.buffer;
					fieldvariant->v.nvarcharVal.buffer = 0;
				}break;
			case FT_BINARY:
				fieldvariant->v.lobVal.size = 0;
				if (fieldvariant->v.lobVal.buffer)
				{
					delete[] fieldvariant->v.lobVal.buffer;
					fieldvariant->v.lobVal.buffer = 0;
				}break;
			case FT_VARBINARY:
				fieldvariant->v.varlobVal.size = 0;
				if (fieldvariant->v.varlobVal.buffer)
				{
					delete[] fieldvariant->v.varlobVal.buffer;
					fieldvariant->v.varlobVal.buffer = 0;
				}break;
			default:
				break;
			}
		
			if (deleteObject)
			{
				delete fieldvariant;
			}
		}
	}

	int compareNumeric(const NUMERIC & numeric1, const NUMERIC & numeric2)
	{
		int result = 0;
		// ???

		return result;
	}

	//int compareDate(const DATE & date1, const DATE & date2)
	//{
	//	int result = 0;
	//	if (date1.year > date2.year)
	//	{
	//		result = 1;
	//	}else if (date1.year < date2.year)
	//	{
	//		result = -1;
	//	}else if (date1.month > date2.month)
	//	{
	//		result = 1;
	//	}else if (date1.month < date2.month)
	//	{
	//		result = -1;
	//	}else if (date1.day > date2.day)
	//	{
	//		result = 1;
	//	}else if (date1.day < date2.day)
	//	{
	//		result = -1;
	//	}
	//	return result;
	//}

	//int compareTime(const TIME & time1, const TIME & time2)
	//{
	//	int result = 0;
	//	if (time1.hour > time2.hour)
	//	{
	//		result = 1;
	//	}else if (time1.hour < time2.hour)
	//	{
	//		result = -1;
	//	}else if (time1.minute > time2.minute)
	//	{
	//		result = 1;
	//	}else if (time1.minute < time2.minute)
	//	{
	//		result = -1;
	//	}else if (time1.second > time2.second)
	//	{
	//		result = 1;
	//	}else if (time1.second < time2.second)
	//	{
	//		result = -1;
	//	}
	//	return result;
	//}

	int compareTimestamp(const TIMESTAMP & timestamp1, const TIMESTAMP & timestamp2)
	{
		if (timestamp1.time == timestamp2.time)
		{
			if (timestamp1.millsecond == timestamp2.millsecond)
				return 0;
			return timestamp1.millsecond > timestamp2.millsecond ? 1 : -1;
		}
		return timestamp1.time > timestamp2.time ? 1 : -1;

		//int result = compareDate(timestamp1.date, timestamp2.date);
		//if (result == 0)
		//{
		//	result = compareTime(timestamp1.time, timestamp2.time);
		//}
		//if (result == 0)
		//{
		//	if (timestamp1.millsecond > timestamp2.millsecond)
		//		result = 1;
		//	else if (timestamp1.millsecond < timestamp2.millsecond)
		//		result = -1;
		//}
		//return result;
	}

#define min(a,b) ((a) < (b) ? (a) : (b))

	bool CFieldVariant::equal(const CFieldVariant::pointer& compare) const
	{
		bool result = false;
		if (compare.get() == 0) return result;
		if (VARTYPE != compare->VARTYPE) return result;

		switch (VARTYPE)
		{
		case FT_BOOLEAN:
			result = v.booleanVal == compare->v.booleanVal;
			break;
		case FT_TINYINT:
			result = v.tinyintVal == compare->v.tinyintVal;
			break;
		case FT_UTINYINT:
			result = v.utinyintVal == compare->v.utinyintVal;
			break;
		case FT_SMALLINT:
			result = v.smallintVal == compare->v.smallintVal;
			break;
		case FT_USMALLINT:
			result = v.usmallintVal == compare->v.usmallintVal;
			break;
		case FT_INTEGER:
			result = v.integerVal == compare->v.integerVal;
			break;
		case FT_UINTEGER:
			result = v.uintegerVal == compare->v.uintegerVal;
			break;
		case FT_BIGINT:
			result = v.bigintVal == compare->v.bigintVal;
			break;
		case FT_UBIGINT:
			result = v.ubigintVal == compare->v.ubigintVal;
			break;
		case FT_NUMERIC:
			if (v.numericVal != 0 && compare->v.numericVal != 0)
				result = compareNumeric(*v.numericVal, *compare->v.numericVal) == 0;
			else
				result = v.numericVal == compare->v.numericVal;
			break;
		case FT_FLOAT:
			result = v.floatVal.value == compare->v.floatVal.value;
			break;
		case FT_REAL:
			result = v.realVal == compare->v.realVal;
			break;
		case FT_DOUBLE:
			result = v.doubleVal == compare->v.doubleVal;
			break;
		case FT_DATE:
			result = v.dateVal == compare->v.dateVal;
			break;
		case FT_TIME:
			result = v.timeVal == compare->v.timeVal;
			break;
		case FT_TIMESTAMP:
			result = compareTimestamp(v.timestampVal, compare->v.timestampVal) == 0;
			break;
		case FT_CHAR:
			if (v.charVal.buffer != 0 && compare->v.charVal.buffer != 0)
				result = strcmp(v.charVal.buffer, compare->v.charVal.buffer) == 0;
			else
				result = v.charVal.buffer == compare->v.charVal.buffer;
			break;
		case FT_VARCHAR:
			{
				if (v.varcharVal.buffer != 0 && compare->v.varcharVal.buffer != 0)
					result = strncmp(v.varcharVal.buffer, compare->v.varcharVal.buffer, min(v.varcharVal.size, compare->v.varcharVal.size)) == 0;
				//result = strcmp(v.varcharVal.buffer, compare->v.varcharVal.buffer) == 0;
				else
					result = v.varcharVal.buffer == compare->v.varcharVal.buffer;
			}break;
		case FT_CLOB:
			{
				if (v.clobVal.buffer != 0 && compare->v.clobVal.buffer != 0)
					result = strncmp(v.clobVal.buffer, compare->v.clobVal.buffer, min(v.clobVal.size, compare->v.clobVal.size)) == 0;
				//result = strcmp(v.clobVal.buffer, compare->v.clobVal.buffer) == 0;
				else
					result = v.clobVal.buffer == compare->v.clobVal.buffer;
			}break;
		case FT_NCHAR:
			if (v.ncharVal.buffer != 0 && compare->v.ncharVal.buffer != 0)
				result = wcscmp(v.ncharVal.buffer, compare->v.ncharVal.buffer) == 0;
			else
				result = v.ncharVal.buffer == compare->v.ncharVal.buffer;
			break;
		case FT_NVARCHAR:
			if (v.nvarcharVal.buffer != 0 && compare->v.nvarcharVal.buffer != 0)
				result = wcscmp(v.nvarcharVal.buffer, compare->v.nvarcharVal.buffer) == 0;
			else
				result = v.nvarcharVal.buffer == compare->v.nvarcharVal.buffer;
			break;
		case FT_BINARY:
			if (v.lobVal.buffer != 0 && compare->v.lobVal.buffer != 0)
				result = memcmp(v.lobVal.buffer, compare->v.lobVal.buffer,
					v.lobVal.size < compare->v.lobVal.size ? v.lobVal.size : compare->v.lobVal.size) == 0;
			else
				result = v.lobVal.buffer == compare->v.lobVal.buffer;
			break;
		case FT_VARBINARY:
			if (v.varlobVal.buffer != 0 && compare->v.varlobVal.buffer != 0)
				result = memcmp(v.varlobVal.buffer, compare->v.varlobVal.buffer,
					v.varlobVal.size < compare->v.varlobVal.size ? v.varlobVal.size : compare->v.varlobVal.size) == 0;
			else
				result = v.varlobVal.buffer == compare->v.varlobVal.buffer;
			break;
		default:
			break;
		}

		return result;
	}

	bool CFieldVariant::unequal(const CFieldVariant::pointer& compare) const
	{
		// != OR <>
		// 
		bool result = true;
		if (compare.get() == 0) return result;
		if (VARTYPE != compare->VARTYPE) return result;

		switch (VARTYPE)
		{
		case FT_BOOLEAN:
			result = v.booleanVal != compare->v.booleanVal;
			break;
		case FT_TINYINT:
			result = v.tinyintVal != compare->v.tinyintVal;
			break;
		case FT_UTINYINT:
			result = v.utinyintVal != compare->v.utinyintVal;
			break;
		case FT_SMALLINT:
			result = v.smallintVal != compare->v.smallintVal;
			break;
		case FT_USMALLINT:
			result = v.usmallintVal != compare->v.usmallintVal;
			break;
		case FT_INTEGER:
			result = v.integerVal != compare->v.integerVal;
			break;
		case FT_UINTEGER:
			result = v.uintegerVal != compare->v.uintegerVal;
			break;
		case FT_BIGINT:
			result = v.bigintVal != compare->v.bigintVal;
			break;
		case FT_UBIGINT:
			result = v.ubigintVal != compare->v.ubigintVal;
			break;
		case FT_NUMERIC:
			if (v.numericVal != 0 && compare->v.numericVal != 0)
				result = compareNumeric(*v.numericVal, *compare->v.numericVal) != 0;
			else
				result = v.numericVal != compare->v.numericVal;
			break;
		case FT_FLOAT:
			result = v.floatVal.value != compare->v.floatVal.value;
			break;
		case FT_REAL:
			result = v.realVal != compare->v.realVal;
			break;
		case FT_DOUBLE:
			result = v.doubleVal != compare->v.doubleVal;
			break;
		case FT_DATE:
			result = v.dateVal != compare->v.dateVal;
			break;
		case FT_TIME:
			result = v.timeVal != compare->v.timeVal;
			break;
		case FT_TIMESTAMP:
			result = compareTimestamp(v.timestampVal, compare->v.timestampVal) != 0;
			break;
		case FT_CHAR:
			if (v.charVal.buffer != 0 && compare->v.charVal.buffer != 0)
				result = strcmp(v.charVal.buffer, compare->v.charVal.buffer) != 0;
			else
				result = v.charVal.buffer != compare->v.charVal.buffer;
			break;
		case FT_VARCHAR:
			if (v.varcharVal.buffer != 0 && compare->v.varcharVal.buffer != 0)
				result = strcmp(v.varcharVal.buffer, compare->v.varcharVal.buffer) != 0;
			else
				result = v.varcharVal.buffer != compare->v.varcharVal.buffer;
			break;
		case FT_CLOB:
			if (v.clobVal.buffer != 0 && compare->v.clobVal.buffer != 0)
				result = strcmp(v.clobVal.buffer, compare->v.clobVal.buffer) != 0;
			else
				result = v.clobVal.buffer != compare->v.clobVal.buffer;
			break;
		case FT_NCHAR:
			if (v.ncharVal.buffer != 0 && compare->v.ncharVal.buffer != 0)
				result = wcscmp(v.ncharVal.buffer, compare->v.ncharVal.buffer) != 0;
			else
				result = v.ncharVal.buffer != compare->v.ncharVal.buffer;
			break;
		case FT_NVARCHAR:
			if (v.nvarcharVal.buffer != 0 && compare->v.nvarcharVal.buffer != 0)
				result = wcscmp(v.nvarcharVal.buffer, compare->v.nvarcharVal.buffer) != 0;
			else
				result = v.nvarcharVal.buffer != compare->v.nvarcharVal.buffer;
			break;
		case FT_BINARY:
			if (v.lobVal.buffer != 0 && compare->v.lobVal.buffer != 0)
				result = memcmp(v.lobVal.buffer, compare->v.lobVal.buffer,
					v.lobVal.size < compare->v.lobVal.size ? v.lobVal.size : compare->v.lobVal.size) != 0;
			else
				result = v.lobVal.buffer != compare->v.lobVal.buffer;
			break;
		case FT_VARBINARY:
			if (v.varlobVal.buffer != 0 && compare->v.varlobVal.buffer != 0)
				result = memcmp(v.varlobVal.buffer, compare->v.varlobVal.buffer,
					v.varlobVal.size < compare->v.varlobVal.size ? v.varlobVal.size : compare->v.varlobVal.size) != 0;
			else
				result = v.varlobVal.buffer != compare->v.varlobVal.buffer;
			break;
		default:
			break;
		}

		return result;
	}

	bool CFieldVariant::greater(const CFieldVariant::pointer& compare) const
	{
		bool result = false;
		if (compare.get() == 0) return result;
		if (VARTYPE != compare->VARTYPE) return result;

		switch (VARTYPE)
		{
		case FT_BOOLEAN:
			// ??
			break;
		case FT_TINYINT:
			result = v.tinyintVal > compare->v.tinyintVal;
			break;
		case FT_UTINYINT:
			result = v.utinyintVal > compare->v.utinyintVal;
			break;
		case FT_SMALLINT:
			result = v.smallintVal > compare->v.smallintVal;
			break;
		case FT_USMALLINT:
			result = v.usmallintVal > compare->v.usmallintVal;
			break;
		case FT_INTEGER:
			result = v.integerVal > compare->v.integerVal;
			break;
		case FT_UINTEGER:
			result = v.uintegerVal > compare->v.uintegerVal;
			break;
		case FT_BIGINT:
			result = v.bigintVal > compare->v.bigintVal;
			break;
		case FT_UBIGINT:
			result = v.ubigintVal > compare->v.ubigintVal;
			break;
		case FT_NUMERIC:
			if (v.numericVal != 0 && compare->v.numericVal != 0)
				result = compareNumeric(*v.numericVal, *compare->v.numericVal) > 0;
			else
				result = compare->v.numericVal == NULL;
			break;
		case FT_FLOAT:
			result = v.floatVal.value > compare->v.floatVal.value;
			break;
		case FT_REAL:
			result = v.realVal > compare->v.realVal;
			break;
		case FT_DOUBLE:
			result = v.doubleVal > compare->v.doubleVal;
			break;
		case FT_DATE:
			result = v.dateVal > compare->v.dateVal;
			break;
		case FT_TIME:
			result = v.timeVal > compare->v.timeVal;
			break;
		case FT_TIMESTAMP:
			result = compareTimestamp(v.timestampVal, compare->v.timestampVal) > 0;
			break;
		case FT_CHAR:
			if (v.charVal.buffer != 0 && compare->v.charVal.buffer != 0)
				result = strcmp(v.charVal.buffer, compare->v.charVal.buffer) > 0;
			else
				result = compare->v.charVal.buffer == NULL;
			break;
		case FT_VARCHAR:
			if (v.varcharVal.buffer != 0 && compare->v.varcharVal.buffer != 0)
				result = strcmp(v.varcharVal.buffer, compare->v.varcharVal.buffer) > 0;
			else
				result = compare->v.varcharVal.buffer == NULL;
			break;
		case FT_CLOB:
			if (v.clobVal.buffer != 0 && compare->v.clobVal.buffer != 0)
				result = strcmp(v.clobVal.buffer, compare->v.clobVal.buffer) > 0;
			else
				result = compare->v.clobVal.buffer == NULL;
			break;
		case FT_NCHAR:
			if (v.ncharVal.buffer != 0 && compare->v.ncharVal.buffer != 0)
				result = wcscmp(v.ncharVal.buffer, compare->v.ncharVal.buffer) > 0;
			else
				result = compare->v.ncharVal.buffer == NULL;
			break;
		case FT_NVARCHAR:
			if (v.nvarcharVal.buffer != 0 && compare->v.nvarcharVal.buffer != 0)
				result = wcscmp(v.nvarcharVal.buffer, compare->v.nvarcharVal.buffer) > 0;
			else
				result = compare->v.nvarcharVal.buffer == NULL;
			break;
		case FT_BINARY:
			if (v.lobVal.buffer != 0 && compare->v.lobVal.buffer != 0)
				result = memcmp(v.lobVal.buffer, compare->v.lobVal.buffer,
					v.lobVal.size < compare->v.lobVal.size ? v.lobVal.size : compare->v.lobVal.size) > 0;
			else
				result = compare->v.lobVal.buffer == NULL;
			break;
		case FT_VARBINARY:
			if (v.varlobVal.buffer != 0 && compare->v.varlobVal.buffer != 0)
				result = memcmp(v.varlobVal.buffer, compare->v.varlobVal.buffer,
					v.varlobVal.size < compare->v.varlobVal.size ? v.varlobVal.size : compare->v.varlobVal.size) > 0;
			else
				result = compare->v.varlobVal.buffer == NULL;
			break;
		default:
			break;
		}

		return result;
	}

	bool CFieldVariant::greaterEqual(const CFieldVariant::pointer& compare) const
	{
		bool result = false;
		if (compare.get() == 0) return result;
		if (VARTYPE != compare->VARTYPE) return result;

		switch (VARTYPE)
		{
		case FT_BOOLEAN:
			result = v.booleanVal == compare->v.booleanVal;
			break;
		case FT_TINYINT:
			result = v.tinyintVal >= compare->v.tinyintVal;
			break;
		case FT_UTINYINT:
			result = v.utinyintVal >= compare->v.utinyintVal;
			break;
		case FT_SMALLINT:
			result = v.smallintVal >= compare->v.smallintVal;
			break;
		case FT_USMALLINT:
			result = v.usmallintVal >= compare->v.usmallintVal;
			break;
		case FT_INTEGER:
			result = v.integerVal >= compare->v.integerVal;
			break;
		case FT_UINTEGER:
			result = v.uintegerVal >= compare->v.uintegerVal;
			break;
		case FT_BIGINT:
			result = v.bigintVal >= compare->v.bigintVal;
			break;
		case FT_UBIGINT:
			result = v.ubigintVal >= compare->v.ubigintVal;
			break;
		case FT_NUMERIC:
			if (v.numericVal != 0 && compare->v.numericVal != 0)
				result = compareNumeric(*v.numericVal, *compare->v.numericVal) >= 0;
			else
				result = compare->v.numericVal == NULL;
			break;
		case FT_FLOAT:
			result = v.floatVal.value >= compare->v.floatVal.value;
			break;
		case FT_REAL:
			result = v.realVal >= compare->v.realVal;
			break;
		case FT_DOUBLE:
			result = v.doubleVal >= compare->v.doubleVal;
			break;
		case FT_DATE:
			result = v.dateVal >= compare->v.dateVal;
			break;
		case FT_TIME:
			result = v.timeVal >= compare->v.timeVal;
			break;
		case FT_TIMESTAMP:
			result = compareTimestamp(v.timestampVal, compare->v.timestampVal) >= 0;
			break;
		case FT_CHAR:
			if (v.charVal.buffer != 0 && compare->v.charVal.buffer != 0)
				result = strcmp(v.charVal.buffer, compare->v.charVal.buffer) >= 0;
			else
				result = compare->v.charVal.buffer == NULL;
			break;
		case FT_VARCHAR:
			if (v.varcharVal.buffer != 0 && compare->v.varcharVal.buffer != 0)
				result = strcmp(v.varcharVal.buffer, compare->v.varcharVal.buffer) >= 0;
			else
				result = compare->v.varcharVal.buffer == NULL;
			break;
		case FT_CLOB:
			if (v.clobVal.buffer != 0 && compare->v.clobVal.buffer != 0)
				result = strcmp(v.clobVal.buffer, compare->v.clobVal.buffer) >= 0;
			else
				result = compare->v.clobVal.buffer == NULL;
			break;
		case FT_NCHAR:
			if (v.ncharVal.buffer != 0 && compare->v.ncharVal.buffer != 0)
				result = wcscmp(v.ncharVal.buffer, compare->v.ncharVal.buffer) >= 0;
			else
				result = compare->v.ncharVal.buffer == NULL;
			break;
		case FT_NVARCHAR:
			if (v.nvarcharVal.buffer != 0 && compare->v.nvarcharVal.buffer != 0)
				result = wcscmp(v.nvarcharVal.buffer, compare->v.nvarcharVal.buffer) >= 0;
			else
				result = compare->v.nvarcharVal.buffer == NULL;
			break;
		case FT_BINARY:
			if (v.lobVal.buffer != 0 && compare->v.lobVal.buffer != 0)
				result = memcmp(v.lobVal.buffer, compare->v.lobVal.buffer,
					v.lobVal.size < compare->v.lobVal.size ? v.lobVal.size : compare->v.lobVal.size) >= 0;
			else
				result = compare->v.lobVal.buffer == NULL;
			break;
		case FT_VARBINARY:
			if (v.varlobVal.buffer != 0 && compare->v.varlobVal.buffer != 0)
				result = memcmp(v.varlobVal.buffer, compare->v.varlobVal.buffer,
					v.varlobVal.size < compare->v.varlobVal.size ? v.varlobVal.size : compare->v.varlobVal.size) >= 0;
			else
				result = compare->v.varlobVal.buffer == NULL;
			break;
		default:
			break;
		}

		return result;
	}

	bool CFieldVariant::less(const CFieldVariant::pointer& compare) const
	{
		bool result = false;
		if (compare.get() == 0) return result;
		if (VARTYPE != compare->VARTYPE) return result;

		switch (VARTYPE)
		{
		case FT_BOOLEAN:
			// ??
			break;
		case FT_TINYINT:
			result = v.tinyintVal < compare->v.tinyintVal;
			break;
		case FT_UTINYINT:
			result = v.utinyintVal < compare->v.utinyintVal;
			break;
		case FT_SMALLINT:
			result = v.smallintVal < compare->v.smallintVal;
			break;
		case FT_USMALLINT:
			result = v.usmallintVal < compare->v.usmallintVal;
			break;
		case FT_INTEGER:
			result = v.integerVal < compare->v.integerVal;
			break;
		case FT_UINTEGER:
			result = v.uintegerVal < compare->v.uintegerVal;
			break;
		case FT_BIGINT:
			result = v.bigintVal < compare->v.bigintVal;
			break;
		case FT_UBIGINT:
			result = v.ubigintVal < compare->v.ubigintVal;
			break;
		case FT_NUMERIC:
			if (v.numericVal != 0 && compare->v.numericVal != 0)
				result = compareNumeric(*v.numericVal, *compare->v.numericVal) < 0;
			else
				result = v.numericVal == NULL;
			break;
		case FT_FLOAT:
			result = v.floatVal.value < compare->v.floatVal.value;
			break;
		case FT_REAL:
			result = v.realVal < compare->v.realVal;
			break;
		case FT_DOUBLE:
			result = v.doubleVal < compare->v.doubleVal;
			break;
		case FT_DATE:
			result = v.dateVal < compare->v.dateVal;
			break;
		case FT_TIME:
			result = v.timeVal < compare->v.timeVal;
			break;
		case FT_TIMESTAMP:
			result = compareTimestamp(v.timestampVal, compare->v.timestampVal) < 0;
			break;
		case FT_CHAR:
			if (v.charVal.buffer != 0 && compare->v.charVal.buffer != 0)
				result = strcmp(v.charVal.buffer, compare->v.charVal.buffer) < 0;
			else
				result = v.charVal.buffer == NULL;
			break;
		case FT_VARCHAR:
			if (v.varcharVal.buffer != 0 && compare->v.varcharVal.buffer != 0)
				result = strcmp(v.varcharVal.buffer, compare->v.varcharVal.buffer) < 0;
			else
				result = v.varcharVal.buffer == NULL;
			break;
		case FT_CLOB:
			if (v.clobVal.buffer != 0 && compare->v.clobVal.buffer != 0)
				result = strcmp(v.clobVal.buffer, compare->v.clobVal.buffer) < 0;
			else
				result = v.clobVal.buffer == NULL;
			break;
		case FT_NCHAR:
			if (v.ncharVal.buffer != 0 && compare->v.ncharVal.buffer != 0)
				result = wcscmp(v.ncharVal.buffer, compare->v.ncharVal.buffer) < 0;
			else
				result = v.ncharVal.buffer == NULL;
			break;
		case FT_NVARCHAR:
			if (v.nvarcharVal.buffer != 0 && compare->v.nvarcharVal.buffer != 0)
				result = wcscmp(v.nvarcharVal.buffer, compare->v.nvarcharVal.buffer) < 0;
			else
				result = v.nvarcharVal.buffer == NULL;
			break;
		case FT_BINARY:
			if (v.lobVal.buffer != 0 && compare->v.lobVal.buffer != 0)
				result = memcmp(v.lobVal.buffer, compare->v.lobVal.buffer,
					v.lobVal.size < compare->v.lobVal.size ? v.lobVal.size : compare->v.lobVal.size) < 0;
			else
				result = v.lobVal.buffer == NULL;
			break;
		case FT_VARBINARY:
			if (v.varlobVal.buffer != 0 && compare->v.varlobVal.buffer != 0)
				result = memcmp(v.varlobVal.buffer, compare->v.varlobVal.buffer,
					v.varlobVal.size < compare->v.varlobVal.size ? v.varlobVal.size : compare->v.varlobVal.size) < 0;
			else
				result = v.varlobVal.buffer == NULL;
			break;
		default:
			break;
		}

		return result;
	}

	bool CFieldVariant::lessEqual(const CFieldVariant::pointer& compare) const
	{
		bool result = false;
		if (compare.get() == 0) return result;
		if (VARTYPE != compare->VARTYPE) return result;

		switch (VARTYPE)
		{
		case FT_BOOLEAN:
			result = v.booleanVal == compare->v.booleanVal;
			break;
		case FT_TINYINT:
			result = v.tinyintVal <= compare->v.tinyintVal;
			break;
		case FT_UTINYINT:
			result = v.utinyintVal <= compare->v.utinyintVal;
			break;
		case FT_SMALLINT:
			result = v.smallintVal <= compare->v.smallintVal;
			break;
		case FT_USMALLINT:
			result = v.usmallintVal <= compare->v.usmallintVal;
			break;
		case FT_INTEGER:
			result = v.integerVal <= compare->v.integerVal;
			break;
		case FT_UINTEGER:
			result = v.uintegerVal <= compare->v.uintegerVal;
			break;
		case FT_BIGINT:
			result = v.bigintVal <= compare->v.bigintVal;
			break;
		case FT_UBIGINT:
			result = v.ubigintVal <= compare->v.ubigintVal;
			break;
		case FT_NUMERIC:
			if (v.numericVal != 0 && compare->v.numericVal != 0)
				result = compareNumeric(*v.numericVal, *compare->v.numericVal) <= 0;
			else
				result = v.numericVal == NULL;
			break;
		case FT_FLOAT:
			result = v.floatVal.value <= compare->v.floatVal.value;
			break;
		case FT_REAL:
			result = v.realVal <= compare->v.realVal;
			break;
		case FT_DOUBLE:
			result = v.doubleVal <= compare->v.doubleVal;
			break;
		case FT_DATE:
			result = v.dateVal <= compare->v.dateVal;
			break;
		case FT_TIME:
			result = v.timeVal <= compare->v.timeVal;
			break;
		case FT_TIMESTAMP:
			result = compareTimestamp(v.timestampVal, compare->v.timestampVal) <= 0;
			break;
		case FT_CHAR:
			if (v.charVal.buffer != 0 && compare->v.charVal.buffer != 0)
				result = strcmp(v.charVal.buffer, compare->v.charVal.buffer) <= 0;
			else
				result = v.charVal.buffer == NULL;
			break;
		case FT_VARCHAR:
			if (v.varcharVal.buffer != 0 && compare->v.varcharVal.buffer != 0)
				result = strcmp(v.varcharVal.buffer, compare->v.varcharVal.buffer) <= 0;
			else
				result = v.varcharVal.buffer == NULL;
			break;
		case FT_CLOB:
			if (v.clobVal.buffer != 0 && compare->v.clobVal.buffer != 0)
				result = strcmp(v.clobVal.buffer, compare->v.clobVal.buffer) <= 0;
			else
				result = v.clobVal.buffer == NULL;
			break;
		case FT_NCHAR:
			if (v.ncharVal.buffer != 0 && compare->v.ncharVal.buffer != 0)
				result = wcscmp(v.ncharVal.buffer, compare->v.ncharVal.buffer) <= 0;
			break;
		case FT_NVARCHAR:
			if (v.nvarcharVal.buffer != 0 && compare->v.nvarcharVal.buffer != 0)
				result = wcscmp(v.nvarcharVal.buffer, compare->v.nvarcharVal.buffer) <= 0;
			else
				result = v.nvarcharVal.buffer == NULL;
			break;
		case FT_BINARY:
			if (v.lobVal.buffer != 0 && compare->v.lobVal.buffer != 0)
				result = memcmp(v.lobVal.buffer, compare->v.lobVal.buffer,
					v.lobVal.size < compare->v.lobVal.size ? v.lobVal.size : compare->v.lobVal.size) <= 0;
			else
				result = v.lobVal.buffer == NULL;
			break;
		case FT_VARBINARY:
			if (v.varlobVal.buffer != 0 && compare->v.varlobVal.buffer != 0)
				result = memcmp(v.varlobVal.buffer, compare->v.varlobVal.buffer,
					v.varlobVal.size < compare->v.varlobVal.size ? v.varlobVal.size : compare->v.varlobVal.size) <= 0;
			else
				result = v.varlobVal.buffer == NULL;
			break;
		default:
			break;
		}

		return result;
	}
	bool CFieldVariant::isLike(const CFieldVariant::pointer& compare) const
	{
		bool result = false;
		if (compare.get() == 0) return result;
		if (VARTYPE != compare->VARTYPE) return result;

		switch (VARTYPE)
		{
		case FT_BOOLEAN:
			result = v.booleanVal == compare->v.booleanVal;
			break;
		case FT_TINYINT:
			result = v.tinyintVal == compare->v.tinyintVal;
			break;
		case FT_UTINYINT:
			result = v.utinyintVal == compare->v.utinyintVal;
			break;
		case FT_SMALLINT:
			result = v.smallintVal == compare->v.smallintVal;
			break;
		case FT_USMALLINT:
			result = v.usmallintVal == compare->v.usmallintVal;
			break;
		case FT_INTEGER:
			result = v.integerVal == compare->v.integerVal;
			break;
		case FT_UINTEGER:
			result = v.uintegerVal == compare->v.uintegerVal;
			break;
		case FT_BIGINT:
			result = v.bigintVal == compare->v.bigintVal;
			break;
		case FT_UBIGINT:
			result = v.ubigintVal == compare->v.ubigintVal;
			break;
		case FT_NUMERIC:
			if (v.numericVal != 0 && compare->v.numericVal != 0)
				result = compareNumeric(*v.numericVal, *compare->v.numericVal) == 0;
			else
				result = v.numericVal == compare->v.numericVal;
			break;
		case FT_FLOAT:
			result = v.floatVal.value == compare->v.floatVal.value;
			break;
		case FT_REAL:
			result = v.realVal == compare->v.realVal;
			break;
		case FT_DOUBLE:
			result = v.doubleVal == compare->v.doubleVal;
			break;
		case FT_DATE:
			result = v.dateVal == compare->v.dateVal;
			break;
		case FT_TIME:
			result = v.timeVal == compare->v.timeVal;
			break;
		case FT_TIMESTAMP:
			result = compareTimestamp(v.timestampVal, compare->v.timestampVal) == 0;
			break;
		case FT_CHAR:
			{
				result = isLike(v.charVal,compare->v.charVal);
				//if (v.charVal.buffer == 0)
				//	return compare->v.charVal.buffer==0?true:false;
				//else if (compare->v.charVal.buffer==0)
				//	return true;
				//bool bLastLike = false;
				//int nLikeIndex1 = 0;
				//int nLikeIndex2 = 0;
				//for (int i=0;i<compare->v.charVal.size;i++)
				//{
				//	const char pChar = compare->v.charVal.buffer[i];
				//	if (pChar=='%')
				//	{
				//		if (i>nLikeIndex2)
				//		{
				//			if ((nLikeIndex1+(i-nLikeIndex2))>v.charVal.size)
				//				return false;
				//			char * lpszTemp = new char[i-nLikeIndex2+1];
				//			strncpy(lpszTemp,compare->v.charVal.buffer+nLikeIndex2,i-nLikeIndex2);
				//			lpszTemp[i-nLikeIndex2]='\0';
				//			const char * find = strstr(v.charVal.buffer+nLikeIndex1,lpszTemp);
				//			delete[] lpszTemp;
				//			if (find==0)
				//			{
				//				return false;
				//			}
				//			nLikeIndex1 = (find-v.charVal.buffer)+(i-nLikeIndex2);
				//		}
				//		bLastLike = true;
				//		nLikeIndex2 = i+1;
				//	}else if (pChar=='_')
				//	{
				//		if (i>nLikeIndex2)
				//		{
				//			if ((nLikeIndex1+(i-nLikeIndex2))>(v.charVal.size-1))		// 1 = '_'
				//				return false;
				//			char * lpszTemp = new char[i-nLikeIndex2+1];
				//			strncpy(lpszTemp,compare->v.charVal.buffer+nLikeIndex2,i-nLikeIndex2);
				//			lpszTemp[i-nLikeIndex2]='\0';
				//			const char * find = strstr(v.charVal.buffer+nLikeIndex1,lpszTemp);
				//			delete[] lpszTemp;
				//			if (find==0)
				//			{
				//				return false;
				//			}
				//			nLikeIndex1 = (find-v.charVal.buffer)+(i-nLikeIndex2)+1;	// 1 = '_'
				//		}else
				//		{
				//			nLikeIndex1++;	// ++ = 1 ='_'
				//		}
				//		bLastLike = false;
				//		nLikeIndex2 = i+1;
				//	}
				//}
				//if (nLikeIndex1==v.charVal.size)
				//	result = true;
				//else
				//	result = bLastLike;
			}break;
		case FT_VARCHAR:
			result = isLike(v.varcharVal,compare->v.varcharVal);
			break;
		case FT_CLOB:
			result = isLike(v.clobVal,compare->v.clobVal);
			break;
		case FT_NCHAR:
			result = isLike(v.ncharVal,compare->v.ncharVal);
			break;
		case FT_NVARCHAR:
			result = isLike(v.nvarcharVal,compare->v.nvarcharVal);
			break;
		case FT_BINARY:
			result = isLike(v.lobVal,compare->v.lobVal);
			break;
		case FT_VARBINARY:
			result = isLike(v.varlobVal,compare->v.varlobVal);
			break;
		default:
			break;
		}

		return result;
	}
	bool CFieldVariant::isLike(const BUFFER& buffer, const BUFFER& compare) const
	{
		// XX% OR %XX%
		// XX_ OR _XX_
		// ~[_X%]
		if (buffer.buffer == 0)
			return compare.buffer==0?true:false;
		else if (compare.buffer==0)
			return true;
		int nLastLike = 0;	// 0:unknown 1:'%' 2:'_'
		unsigned int nLikeIndex1 = 0;
		unsigned int nLikeIndex2 = 0;
		for (unsigned int i=0;i<compare.size;i++)
		{
			const char pChar = compare.buffer[i];
			if (pChar=='%')
			{
				if (i>nLikeIndex2)
				{
					if ((nLikeIndex1+(i-nLikeIndex2))>buffer.size)
						return false;
					char * lpszTemp = new char[i-nLikeIndex2+1];
					strncpy(lpszTemp,compare.buffer+nLikeIndex2,i-nLikeIndex2);
					lpszTemp[i-nLikeIndex2]='\0';
					const char * find = strstr(buffer.buffer+nLikeIndex1,lpszTemp);
					delete[] lpszTemp;
					if (find==0)
					{
						return false;
					}
					if (nLastLike!=1)	// unknown OR '_'
					{
						if ((find-buffer.buffer)!=nLikeIndex1)
						{
							return false;
						}
					}
					nLikeIndex1 = (find-buffer.buffer)+(i-nLikeIndex2);
				}
				nLastLike = 1;
				nLikeIndex2 = i+1;
			}else if (pChar=='_')
			{
				if (i>nLikeIndex2)
				{
					if ((nLikeIndex1+(i-nLikeIndex2))>(buffer.size-1))		// 1 = '_'
						return false;
					char * lpszTemp = new char[i-nLikeIndex2+1];
					strncpy(lpszTemp,compare.buffer+nLikeIndex2,i-nLikeIndex2);
					lpszTemp[i-nLikeIndex2]='\0';
					const char * find = strstr(buffer.buffer+nLikeIndex1,lpszTemp);
					delete[] lpszTemp;
					if (find==0)
					{
						return false;
					}
					if (nLastLike!=1)	// unknown OR '_'
					{
						if ((find-buffer.buffer)!=nLikeIndex1)
						{
							return false;
						}
					}
					nLikeIndex1 = (find-buffer.buffer)+(i-nLikeIndex2)+1;	// 1 = '_'
				}else
				{
					nLikeIndex1++;	// ++ = 1 ='_'
				}
				nLastLike = 2;
				nLikeIndex2 = i+1;
			}
		}
		if (nLikeIndex1==buffer.size)
			return true;
		else if (nLastLike==0)
			return strcmp(buffer.buffer,compare.buffer)==0?true:false;
		else
			return nLastLike==1?true:false;
	}
	bool CFieldVariant::isLike(const NBUFFER& buffer, const NBUFFER& compare) const
	{
		// XX% OR %XX%
		// XX_ OR _XX_
		// ~[_X%]
		if (buffer.buffer == 0)
			return compare.buffer==0?true:false;
		else if (compare.buffer==0)
			return true;
		int nLastLike = 0;	// 0:unknown 1:'%' 2:'_'
		unsigned int nLikeIndex1 = 0;
		unsigned int nLikeIndex2 = 0;
		for (unsigned int i=0;i<compare.size;i++)
		{
			const wchar_t pChar = compare.buffer[i];
			if (pChar==L'%')
			{
				if (i>nLikeIndex2)
				{
					if ((nLikeIndex1+(i-nLikeIndex2))>buffer.size)
						return false;
					wchar_t * lpszTemp = new wchar_t[i-nLikeIndex2+1];
					wcsncpy(lpszTemp,compare.buffer+nLikeIndex2,i-nLikeIndex2);
					lpszTemp[i-nLikeIndex2]=L'\0';
					const wchar_t * find = wcsstr(buffer.buffer+nLikeIndex1,lpszTemp);
					delete[] lpszTemp;
					if (find==0)
					{
						return false;
					}
					if (nLastLike!=1)	// unknown OR '_'
					{
						if ((find-buffer.buffer)!=nLikeIndex1)
						{
							return false;
						}
					}
					nLikeIndex1 = (find-buffer.buffer)+(i-nLikeIndex2);
				}
				nLastLike = 1;
				nLikeIndex2 = i+1;
			}else if (pChar==L'_')
			{
				if (i>nLikeIndex2)
				{
					if ((nLikeIndex1+(i-nLikeIndex2))>(buffer.size-1))		// 1 = '_'
						return false;
					wchar_t * lpszTemp = new wchar_t[i-nLikeIndex2+1];
					wcsncpy(lpszTemp,compare.buffer+nLikeIndex2,i-nLikeIndex2);
					lpszTemp[i-nLikeIndex2]=L'\0';
					const wchar_t * find = wcsstr(buffer.buffer+nLikeIndex1,lpszTemp);
					delete[] lpszTemp;
					if (find==0)
					{
						return false;
					}
					if (nLastLike!=1)	// unknown OR '_'
					{
						if ((find-buffer.buffer)!=nLikeIndex1)
						{
							return false;
						}
					}
					nLikeIndex1 = (find-buffer.buffer)+(i-nLikeIndex2)+1;	// 1 = '_'
				}else
				{
					nLikeIndex1++;	// ++ = 1 ='_'
				}
				nLastLike = 2;
				nLikeIndex2 = i+1;
			}
		}
		if (nLikeIndex1==buffer.size)
			return true;
		else if (nLastLike==0)
			return wcscmp(buffer.buffer,compare.buffer)==0?true:false;
		else
			return nLastLike==1?true:false;
	}

	bool CFieldVariant::isNull(void) const
	{
		bool result = false;

		switch (VARTYPE)
		{
		//case FT_TINYINT:
		//	result = v.tinyintVal == 0;
		//	break;
		//case FT_SMALLINT:
		//	result = v.smallintVal == 0;
		//	break;
		//case FT_USMALLINT:
		//	result = v.usmallintVal == 0;
		//	break;
		//case FT_INTEGER:
		//	result = v.integerVal == 0;
		//	break;
		//case FT_UINTEGER:
		//	result = v.uintegerVal == 0;
		//	break;
		//case FT_BIGINT:
		//	result = v.bigintVal == 0;
		//	break;
		//case FT_UBIGINT:
		//	result = v.ubigintVal == 0;
		//	break;
		case FT_NUMERIC:
			result = v.numericVal == NULL;
			break;
		//case FT_FLOAT:
		//	result = v.floatVal.value == 0.0;
		//	break;
		//case FT_REAL:
		//	result = v.realVal == 0.0;
		//	break;
		//case FT_DOUBLE:
		//	result = v.doubleVal == 0.0;
		//	break;
		//case FT_DATE:
		//	result = v.dateVal == NULL;
		//	break;
		//case FT_TIME:
		//	result = v.timeVal == NULL;
		//	break;
		//case FT_TIMESTAMP:
		//	result = v.timestampVal == NULL;
		//	break;
		case FT_CHAR:
			result = v.charVal.size == 0 || v.charVal.buffer == NULL;
			break;
		case FT_VARCHAR:
			result = v.varcharVal.size == 0 || v.varcharVal.buffer == NULL;
			break;
		case FT_CLOB:
			result = v.clobVal.size == 0 || v.clobVal.buffer == NULL;
			break;
		case FT_NCHAR:
			result = v.ncharVal.size == 0 || v.ncharVal.buffer == NULL;
			break;
		case FT_NVARCHAR:
			result = v.nvarcharVal.size == 0 || v.nvarcharVal.buffer == NULL;
			break;
		case FT_BINARY:
			result = v.lobVal.size == 0 || v.lobVal.buffer == NULL;
			break;
		case FT_VARBINARY:
			result = v.varlobVal.size == 0 || v.varlobVal.buffer == NULL;
			break;
		default:
			break;
		}

		return result;
	}
	bool CFieldVariant::isNotNull(void) const
	{
		bool result = false;

		switch (VARTYPE)
		{
		case FT_BOOLEAN:
			// ??
			break;
		case FT_TINYINT:
			result = v.tinyintVal != 0;
			break;
		case FT_UTINYINT:
			result = v.utinyintVal != 0;
			break;
		case FT_SMALLINT:
			result = v.smallintVal != 0;
			break;
		case FT_USMALLINT:
			result = v.usmallintVal != 0;
			break;
		case FT_INTEGER:
			result = v.integerVal != 0;
			break;
		case FT_UINTEGER:
			result = v.uintegerVal != 0;
			break;
		case FT_BIGINT:
			result = v.bigintVal != 0;
			break;
		case FT_UBIGINT:
			result = v.ubigintVal != 0;
			break;
		case FT_NUMERIC:
			result = v.numericVal != 0;
			break;
		case FT_FLOAT:
			result = v.floatVal.value != 0.0;
			break;
		case FT_REAL:
			result = v.realVal != 0.0;
			break;
		case FT_DOUBLE:
			result = v.doubleVal != 0.0;
			break;
		//case FT_DATE:
		//	result = v.dateVal != 0;
		//	break;
		//case FT_TIME:
		//	result = v.timeVal != 0;
		//	break;
		//case FT_TIMESTAMP:
		//	result = v.timestampVal != 0;
		//	break;
		case FT_CHAR:
			result = v.charVal.size > 0 && v.charVal.buffer != 0;
			break;
		case FT_VARCHAR:
			result = v.varcharVal.size > 0 && v.varcharVal.buffer != 0;
			break;
		case FT_CLOB:
			result = v.clobVal.size > 0 && v.clobVal.buffer != 0;
			break;
		case FT_NCHAR:
			result = v.ncharVal.size > 0 && v.ncharVal.buffer != 0;
			break;
		case FT_NVARCHAR:
			result = v.nvarcharVal.size > 0 && v.nvarcharVal.buffer != 0;
			break;
		case FT_BINARY:
			result = v.lobVal.size > 0 && v.lobVal.buffer != 0;
			break;
		case FT_VARBINARY:
			result = v.varlobVal.size > 0 && v.varlobVal.buffer != 0;
			break;
		default:
			break;
		}

		return result;
	}

	void CFieldVariant::setNull(void)
	{
		switch (VARTYPE)
		{
		case FT_BOOLEAN:
			v.booleanVal = false;
			break;
		case FT_TINYINT:
			v.tinyintVal = 0;
			break;
		case FT_UTINYINT:
			v.utinyintVal = 0;
			break;
		case FT_SMALLINT:
			v.smallintVal = 0;
			break;
		case FT_USMALLINT:
			v.usmallintVal = 0;
			break;
		case FT_INTEGER:
			v.integerVal = 0;
			break;
		case FT_UINTEGER:
			v.uintegerVal = 0;
			break;
		case FT_BIGINT:
			v.bigintVal = 0;
			break;
		case FT_UBIGINT:
			v.ubigintVal = 0;
			break;
		case FT_NUMERIC:
			if (v.numericVal != 0)
			{
				delete v.numericVal;
				v.numericVal = 0;
			}break;
		case FT_FLOAT:
			v.floatVal.value = 0.0;
			break;
		case FT_REAL:
			v.realVal = 0.0;
			break;
		case FT_DOUBLE:
			v.doubleVal = 0.0;
			break;
		//case FT_DATE:
		//	if (v.dateVal != 0)
		//	{
		//		delete v.dateVal;
		//		v.dateVal = 0;
		//	}break;
		//case FT_TIME:
		//	if (v.timeVal != 0)
		//	{
		//		delete v.timeVal;
		//		v.timeVal = 0;
		//	}break;
		//case FT_TIMESTAMP:
		//	if (v.timestampVal != 0)
		//	{
		//		delete v.timestampVal;
		//		v.timestampVal = 0;
		//	}break;
		case FT_CHAR:
			FreeBuffer(&v.charVal, false);
			break;
		case FT_VARCHAR:
			FreeBuffer(&v.varcharVal, false);
			break;
		case FT_CLOB:
			FreeBuffer(&v.clobVal, false);
			break;
		case FT_NCHAR:
			FreeBuffer(&v.ncharVal, false);
			break;
		case FT_NVARCHAR:
			FreeBuffer(&v.nvarcharVal, false);
			break;
		case FT_BINARY:
			FreeBuffer(&v.lobVal, false);
			break;
		case FT_VARBINARY:
			FreeBuffer(&v.varlobVal, false);
			break;
		default:
			break;
		}
	}
	bool CFieldVariant::setString(const char * value, int size)
	{
		switch (VARTYPE)
		{
		case FT_CHAR:
			{
				FreeBuffer(&v.charVal, false);
				CopyBuffer(&v.charVal, value, size);
			}break;
		case FT_VARCHAR:
			{
				FreeBuffer(&v.varcharVal, false);
				CopyBuffer(&v.varcharVal, value, size);
			}break;
		case FT_CLOB:
			{
				FreeBuffer(&v.clobVal, false);
				CopyBuffer(&v.clobVal, value, size);
			}break;
		case FT_NCHAR:
			{

			}break;
		case FT_NVARCHAR:
			// ???
			break;
		case FT_BINARY:
			FreeBuffer(&v.lobVal, false);
			CopyBuffer(&v.lobVal, value, size);
			break;
		case FT_VARBINARY:
			FreeBuffer(&v.varlobVal, false);
			CopyBuffer(&v.varlobVal, value, size);
			break;
		case FT_BOOLEAN:
			v.booleanVal = value!=NULL&&(strcmp(value,"1")==0 || strcmp(value,"true")==0 || strcmp(value,"TRUE")==0) ? true : false;
			break;
		case FT_TINYINT:
			v.tinyintVal = static_cast<tinyint>(atoi(value));
			break;
		case FT_UTINYINT:
			v.utinyintVal = static_cast<utinyint>(atoi(value));
			break;
		case FT_SMALLINT:
			v.smallintVal = static_cast<smallint>(atoi(value));
			break;
		case FT_USMALLINT:
			v.usmallintVal = static_cast<usmallint>(atoi(value));
			break;
		case FT_INTEGER:
			v.integerVal = atoi(value);
			break;
		case FT_UINTEGER:
			v.uintegerVal = static_cast<uinteger>(bo_atoi64(value));
			break;
		case FT_BIGINT:
			v.bigintVal = bo_atoi64(value);
			break;
		case FT_UBIGINT:
			v.ubigintVal = bo_atoi64(value);
			break;
		case FT_NUMERIC:
			{
				bigint tempValue = bo_atoi64(value);
				if (v.numericVal == 0)
				{
					v.numericVal = new NUMERIC;
				}
				v.numericVal->negative = tempValue < 0;
				if (v.numericVal->negative)
					tempValue *= -1;

				memset(v.numericVal->numeric, 0, sizeof(v.numericVal->numeric));
				for (short i=v.numericVal->size1-v.numericVal->size2-1; i>=0; i--)
				{
					v.numericVal->numeric[i] = Number2Char(tempValue % 10);
					tempValue = tempValue / 10;
					if (tempValue == 0) break;
				}

				//memset(v.numericVal->numeric1, 0, sizeof(v.numericVal->numeric1));
				//memset(v.numericVal->numeric2, 0, sizeof(v.numericVal->numeric2));
				//if (value > MAX_DEFAULT_NUMERIC_ITEM)
				//{
				//	v.numericVal->numeric1[MAX_NUMERIC_SIZE-1] = value % (MAX_DEFAULT_NUMERIC_ITEM+1);
				//	v.numericVal->numeric1[MAX_NUMERIC_SIZE-2] = value / (MAX_DEFAULT_NUMERIC_ITEM+1);
				//}else
				//{
				//	v.numericVal->numeric1[MAX_NUMERIC_SIZE-1] = value;
				//}
			}break;
		case FT_FLOAT:
			v.floatVal.value = static_cast<float>(atof(value));
			break;
		case FT_REAL:
			v.realVal = static_cast<float>(atof(value));
			break;
		case FT_DOUBLE:
			v.doubleVal = static_cast<double>(atof(value));
			break;
		case FT_DATE:
			v.dateVal = bo_atoi64(value);
			break;
		case FT_TIME:
			v.timeVal = bo_atoi64(value);
			break;
		case FT_TIMESTAMP:
			{
				v.timestampVal.time = bo_atoi64(value);
				v.timestampVal.millsecond = 0;
			}break;
		default:
			return false;
			break;
		}
		return true;
	}
	bool CFieldVariant::setCurrentTime(void)
	{
		switch (VARTYPE)
		{
		case FT_DATE:
			{
				struct timeb tbNow;
				ftime(&tbNow);
				tbNow.time -= (tbNow.timezone*60);
				//struct tm  * tmNow = gmtime(&tbNow.time);

				//v.dateVal.year = 1900+tmNow->tm_year;
				//v.dateVal.month = 1+tmNow->tm_mon;
				//v.dateVal.day = tmNow->tm_mday;
				v.dateVal = tbNow.time;
			}break;
		case FT_TIME:
			{
				struct timeb tbNow;
				ftime(&tbNow);
				tbNow.time -= (tbNow.timezone*60);
				//struct tm  * tmNow = gmtime(&tbNow.time);

				//v.timeVal.hour = tmNow->tm_hour;
				//v.timeVal.minute = tmNow->tm_min;
				//v.timeVal.second = tmNow->tm_sec;
				v.timeVal = tbNow.time;
			}break;
		case FT_TIMESTAMP:
			{
				//_tzset();
				//long timezone = 0;
				//_get_timezone( &timezone );
 
				struct timeb tbNow;
				ftime(&tbNow);
				//tbNow.time -= timezone;
				tbNow.time -= (tbNow.timezone*60);

				//struct tm  * tmNow = gmtime(&tbNow.time);
				//v.timestampVal.date.year = 1900+tmNow->tm_year;
				//v.timestampVal.date.month = 1+tmNow->tm_mon;
				//v.timestampVal.date.day = tmNow->tm_mday;
				//v.timestampVal.time.hour = tmNow->tm_hour;
				//v.timestampVal.time.minute = tmNow->tm_min;
				//v.timestampVal.time.second = tmNow->tm_sec;

				v.timestampVal.time = tbNow.time;
				v.timestampVal.millsecond = tbNow.millitm;
				v.timestampVal.timezone = tbNow.timezone;
			}break;
		default:
			return false;
			break;
		}
		return true;
	}

	bool CFieldVariant::setBool(bool value)
	{
		switch (VARTYPE)
		{
		case FT_CHAR:
			{
				FreeBuffer(&v.charVal, false);
				CopyBuffer(&v.charVal, value?"1":"0", 1);
			}break;
		case FT_VARCHAR:
			{
				FreeBuffer(&v.varcharVal, false);
				CopyBuffer(&v.varcharVal, value?"1":"0", 1);
			}break;
		case FT_CLOB:
			{
				FreeBuffer(&v.clobVal, false);
				CopyBuffer(&v.clobVal, value?"1":"0", 1);
			}break;
		case FT_NCHAR:
			{

			}break;
		case FT_NVARCHAR:
			// ???
			break;
		case FT_BINARY:
			{
				FreeBuffer(&v.lobVal, false);
				CopyBuffer(&v.lobVal, value?"1":"0", 1);
			}break;
		case FT_VARBINARY:
			{
				FreeBuffer(&v.varlobVal, false);
				CopyBuffer(&v.varlobVal, value?"1":"0", 1);
			}break;
		case FT_BOOLEAN:
			v.booleanVal = value;
			break;
		case FT_TINYINT:
			v.tinyintVal = value?1:0;
			break;
		case FT_UTINYINT:
			v.utinyintVal = value?1:0;
			break;
		case FT_SMALLINT:
			v.smallintVal = value?1:0;
			break;
		case FT_USMALLINT:
			v.usmallintVal = value?1:0;
			break;
		case FT_INTEGER:
			v.integerVal = value?1:0;
			break;
		case FT_UINTEGER:
			v.uintegerVal = value?1:0;
			break;
		case FT_BIGINT:
			v.bigintVal = value?1:0;
			break;
		case FT_UBIGINT:
			v.ubigintVal = value?1:0;
			break;
		case FT_NUMERIC:
			{
				if (v.numericVal == 0)
				{
					v.numericVal = new NUMERIC;
				}
				v.numericVal->negative = false;
				memset(v.numericVal->numeric, 0, sizeof(v.numericVal->numeric));
				v.numericVal->numeric[0] = value?'1':'0';
			}break;
		case FT_FLOAT:
			v.floatVal.value = value?1.0:0.0;
			break;
		case FT_REAL:
			v.realVal = value?1.0:0.0;
			break;
		case FT_DOUBLE:
			v.doubleVal = value?1.0:0.0;
			break;
		case FT_DATE:
			v.dateVal = static_cast<DATE>(value);
			break;
		case FT_TIME:
			v.timeVal = static_cast<TIME>(value);
			break;
		case FT_TIMESTAMP:
			{
				v.timestampVal.time = static_cast<botime_t>(value);
				v.timestampVal.millsecond = 0;
			}break;
		default:
			return false;
			break;
		}
		return true;
	}

	bool CFieldVariant::setInt(int value)
	{
		switch (VARTYPE)
		{
		case FT_CHAR:
			{
				FreeBuffer(&v.charVal, false);
				char lpszBuffer[24];
				sprintf(lpszBuffer,"%d",value);
				CopyBuffer(&v.charVal, lpszBuffer, strlen(lpszBuffer));
			}break;
		case FT_VARCHAR:
			{
				FreeBuffer(&v.varcharVal, false);
				char lpszBuffer[24];
				sprintf(lpszBuffer,"%d",value);
				CopyBuffer(&v.varcharVal, lpszBuffer, strlen(lpszBuffer));
			}break;
		case FT_CLOB:
			{
				FreeBuffer(&v.clobVal, false);
				char lpszBuffer[24];
				sprintf(lpszBuffer,"%d",value);
				CopyBuffer(&v.clobVal, lpszBuffer, strlen(lpszBuffer));
			}break;
		case FT_NCHAR:
			{

			}break;
		case FT_NVARCHAR:
			// ???
			break;
		case FT_BINARY:
			{
				FreeBuffer(&v.lobVal, false);
				char lpszBuffer[24];
				sprintf(lpszBuffer,"%d",value);
				CopyBuffer(&v.lobVal, lpszBuffer, strlen(lpszBuffer));
			}break;
		case FT_VARBINARY:
			{
				FreeBuffer(&v.varlobVal, false);
				char lpszBuffer[24];
				sprintf(lpszBuffer,"%d",value);
				CopyBuffer(&v.varlobVal, lpszBuffer, strlen(lpszBuffer));
			}break;
		case FT_BOOLEAN:
			v.booleanVal = value == 1 ? true : false;
			break;
		case FT_TINYINT:
			v.tinyintVal = static_cast<tinyint>(value);
			break;
		case FT_UTINYINT:
			v.utinyintVal = static_cast<utinyint>(value);
			break;
		case FT_SMALLINT:
			v.smallintVal = static_cast<smallint>(value);
			break;
		case FT_USMALLINT:
			v.usmallintVal = static_cast<usmallint>(value);
			break;
		case FT_INTEGER:
			v.integerVal = value;
			break;
		case FT_UINTEGER:
			v.uintegerVal = value;
			break;
		case FT_BIGINT:
			v.bigintVal = value;
			break;
		case FT_UBIGINT:
			v.ubigintVal = value;
			break;
		case FT_NUMERIC:
			{
				if (v.numericVal == 0)
				{
					v.numericVal = new NUMERIC;
				}
				v.numericVal->negative = value < 0;
				if (v.numericVal->negative)
					value *= -1;

				memset(v.numericVal->numeric, 0, sizeof(v.numericVal->numeric));
				for (short i=v.numericVal->size1-v.numericVal->size2-1; i>=0; i--)
				{
					v.numericVal->numeric[i] = Number2Char(value % 10);
					value = value / 10;
					if (value == 0) break;
				}

				//memset(v.numericVal->numeric1, 0, sizeof(v.numericVal->numeric1));
				//memset(v.numericVal->numeric2, 0, sizeof(v.numericVal->numeric2));
				//if (value > MAX_DEFAULT_NUMERIC_ITEM)
				//{
				//	v.numericVal->numeric1[MAX_NUMERIC_SIZE-1] = value % (MAX_DEFAULT_NUMERIC_ITEM+1);
				//	v.numericVal->numeric1[MAX_NUMERIC_SIZE-2] = value / (MAX_DEFAULT_NUMERIC_ITEM+1);
				//}else
				//{
				//	v.numericVal->numeric1[MAX_NUMERIC_SIZE-1] = value;
				//}
			}break;
		case FT_FLOAT:
			v.floatVal.value = static_cast<float>(value);
			break;
		case FT_REAL:
			v.realVal = static_cast<float>(value);
			break;
		case FT_DOUBLE:
			v.doubleVal = static_cast<double>(value);
			break;
		case FT_DATE:
			v.dateVal = static_cast<DATE>(value);
			break;
		case FT_TIME:
			v.timeVal = static_cast<TIME>(value);
			break;
		case FT_TIMESTAMP:
			{
				v.timestampVal.time = static_cast<botime_t>(value);
				v.timestampVal.millsecond = 0;
			}break;
		default:
			return false;
			break;
		}
		return true;
	}
	bool CFieldVariant::setBigInt(bo::bigint value)
	{
		switch (VARTYPE)
		{
		case FT_CHAR:
			{
				FreeBuffer(&v.charVal, false);
				char lpszBuffer[24];
				sprintf(lpszBuffer,"%lld",value);
				CopyBuffer(&v.charVal, lpszBuffer, strlen(lpszBuffer));
			}break;
		case FT_VARCHAR:
			{
				FreeBuffer(&v.varcharVal, false);
				char lpszBuffer[24];
				sprintf(lpszBuffer,"%lld",value);
				CopyBuffer(&v.varcharVal, lpszBuffer, strlen(lpszBuffer));
			}break;
		case FT_CLOB:
			{
				FreeBuffer(&v.clobVal, false);
				char lpszBuffer[24];
				sprintf(lpszBuffer,"%lld",value);
				CopyBuffer(&v.clobVal, lpszBuffer, strlen(lpszBuffer));
			}break;
		case FT_NCHAR:
			{

			}break;
		case FT_NVARCHAR:
			// ???
			break;
		case FT_BINARY:
			{
				FreeBuffer(&v.lobVal, false);
				char lpszBuffer[24];
				sprintf(lpszBuffer,"%lld",value);
				CopyBuffer(&v.lobVal, lpszBuffer, strlen(lpszBuffer));
			}break;
		case FT_VARBINARY:
			{
				FreeBuffer(&v.varlobVal, false);
				char lpszBuffer[24];
				sprintf(lpszBuffer,"%lld",value);
				CopyBuffer(&v.varlobVal, lpszBuffer, strlen(lpszBuffer));
			}break;
		case FT_BOOLEAN:
			v.booleanVal = value == 1 ? true : false;
			break;
		case FT_TINYINT:
			v.tinyintVal = static_cast<char>(value);
			break;
		case FT_UTINYINT:
			v.utinyintVal = static_cast<unsigned char>(value);
			break;
		case FT_SMALLINT:
			v.smallintVal = static_cast<short>(value);
			break;
		case FT_USMALLINT:
			v.usmallintVal = static_cast<usmallint>(value);
			break;
		case FT_INTEGER:
			v.integerVal = static_cast<int>(value);
			break;
		case FT_UINTEGER:
			v.uintegerVal = static_cast<uinteger>(value);
			break;
		case FT_BIGINT:
			v.bigintVal = static_cast<bigint>(value);
			break;
		case FT_UBIGINT:
			v.ubigintVal = static_cast<ubigint>(value);
			break;
		case FT_NUMERIC:
			{
				/*
				// ???
				if (v.numericVal == 0)
				{
					v.numericVal = new NUMERIC;
				}
				v.numericVal->negative = value < 0;
				if (v.numericVal->negative)
					value *= -1;

				memset(v.numericVal->numeric, 0, sizeof(v.numericVal->numeric));
				for (short i=v.numericVal->size1-v.numericVal->size2-1; i>=0; i--)
				{
					v.numericVal->numeric[i] = Number2Char(value % 10);
					value = value / 10;
					if (value == 0) break;
				}

				//memset(v.numericVal->numeric1, 0, sizeof(v.numericVal->numeric1));
				//memset(v.numericVal->numeric2, 0, sizeof(v.numericVal->numeric2));
				//if (value > MAX_DEFAULT_NUMERIC_ITEM)
				//{
				//	v.numericVal->numeric1[MAX_NUMERIC_SIZE-1] = value % (MAX_DEFAULT_NUMERIC_ITEM+1);
				//	v.numericVal->numeric1[MAX_NUMERIC_SIZE-2] = value / (MAX_DEFAULT_NUMERIC_ITEM+1);
				//}else
				//{
				//	v.numericVal->numeric1[MAX_NUMERIC_SIZE-1] = value;
				//}
				*/
			}break;
		case FT_FLOAT:
			v.floatVal.value = static_cast<float>(value);
			break;
		case FT_REAL:
			v.realVal = static_cast<float>(value);
			break;
		case FT_DOUBLE:
			v.doubleVal = static_cast<double>(value);
			break;
		case FT_DATE:
			v.dateVal = static_cast<DATE>(value);
			break;
		case FT_TIME:
			v.timeVal = static_cast<TIME>(value);
			break;
		case FT_TIMESTAMP:
			{
				v.timestampVal.time = static_cast<botime_t>(value);
				v.timestampVal.millsecond = 0;
			}break;
		default:
			return false;
			break;
		}
		return true;
	}
	bool CFieldVariant::setReal(double value)
	{
		switch (VARTYPE)
		{
		case FT_CHAR:
			{
				FreeBuffer(&v.charVal, false);
				char lpszBuffer[24];
				sprintf(lpszBuffer,"%f",value);
				CopyBuffer(&v.charVal, lpszBuffer, strlen(lpszBuffer));
			}break;
		case FT_VARCHAR:
			{
				FreeBuffer(&v.varcharVal, false);
				char lpszBuffer[24];
				sprintf(lpszBuffer,"%f",value);
				CopyBuffer(&v.varcharVal, lpszBuffer, strlen(lpszBuffer));
			}break;
		case FT_CLOB:
			{
				FreeBuffer(&v.clobVal, false);
				char lpszBuffer[24];
				sprintf(lpszBuffer,"%f",value);
				CopyBuffer(&v.clobVal, lpszBuffer, strlen(lpszBuffer));
			}break;
		case FT_NCHAR:
			{

			}break;
		case FT_NVARCHAR:
			// ???
			break;
		case FT_BINARY:
			{
				FreeBuffer(&v.lobVal, false);
				char lpszBuffer[24];
				sprintf(lpszBuffer,"%f",value);
				CopyBuffer(&v.lobVal, lpszBuffer, strlen(lpszBuffer));
			}break;
		case FT_VARBINARY:
			{
				FreeBuffer(&v.varlobVal, false);
				char lpszBuffer[24];
				sprintf(lpszBuffer,"%f",value);
				CopyBuffer(&v.varlobVal, lpszBuffer, strlen(lpszBuffer));
			}break;
		case FT_BOOLEAN:
			v.booleanVal = value == 1.0 ? true : false;
			break;
		case FT_TINYINT:
			v.tinyintVal = static_cast<char>(value);
			break;
		case FT_UTINYINT:
			v.utinyintVal = static_cast<unsigned char>(value);
			break;
		case FT_SMALLINT:
			v.smallintVal = static_cast<short>(value);
			break;
		case FT_USMALLINT:
			v.usmallintVal = static_cast<usmallint>(value);
			break;
		case FT_INTEGER:
			v.integerVal = static_cast<int>(value);
			break;
		case FT_UINTEGER:
			v.uintegerVal = static_cast<uinteger>(value);
			break;
		case FT_BIGINT:
			v.bigintVal = static_cast<bigint>(value);
			break;
		case FT_UBIGINT:
			v.ubigintVal = static_cast<ubigint>(value);
			break;
		case FT_NUMERIC:
			{
				/*
				// ???
				if (v.numericVal == 0)
				{
					v.numericVal = new NUMERIC;
				}
				v.numericVal->negative = value < 0;
				if (v.numericVal->negative)
					value *= -1;

				memset(v.numericVal->numeric, 0, sizeof(v.numericVal->numeric));
				for (short i=v.numericVal->size1-v.numericVal->size2-1; i>=0; i--)
				{
					v.numericVal->numeric[i] = Number2Char(value % 10);
					value = value / 10;
					if (value == 0) break;
				}

				//memset(v.numericVal->numeric1, 0, sizeof(v.numericVal->numeric1));
				//memset(v.numericVal->numeric2, 0, sizeof(v.numericVal->numeric2));
				//if (value > MAX_DEFAULT_NUMERIC_ITEM)
				//{
				//	v.numericVal->numeric1[MAX_NUMERIC_SIZE-1] = value % (MAX_DEFAULT_NUMERIC_ITEM+1);
				//	v.numericVal->numeric1[MAX_NUMERIC_SIZE-2] = value / (MAX_DEFAULT_NUMERIC_ITEM+1);
				//}else
				//{
				//	v.numericVal->numeric1[MAX_NUMERIC_SIZE-1] = value;
				//}
				*/
			}break;
		case FT_FLOAT:
			v.floatVal.value = static_cast<float>(value);
			break;
		case FT_REAL:
			v.realVal = static_cast<float>(value);
			break;
		case FT_DOUBLE:
			v.doubleVal = static_cast<double>(value);
			break;
		case FT_DATE:
			v.dateVal = static_cast<DATE>(value);
			break;
		case FT_TIME:
			v.timeVal = static_cast<TIME>(value);
			break;
		case FT_TIMESTAMP:
			{
				v.timestampVal.time = static_cast<botime_t>(value);
				v.timestampVal.millsecond = 0;
			}break;
		default:
			return false;
			break;
		}
		return true;
	}
	bool CFieldVariant::setIntu(uinteger value)
	{
		switch (VARTYPE)
		{
		case FT_CHAR:
			{
				FreeBuffer(&v.charVal, false);
				char lpszBuffer[24];
				sprintf(lpszBuffer,"%u",value);
				CopyBuffer(&v.charVal, lpszBuffer, strlen(lpszBuffer));
			}break;
		case FT_VARCHAR:
			{
				FreeBuffer(&v.varcharVal, false);
				char lpszBuffer[24];
				sprintf(lpszBuffer,"%u",value);
				CopyBuffer(&v.varcharVal, lpszBuffer, strlen(lpszBuffer));
			}break;
		case FT_CLOB:
			{
				FreeBuffer(&v.clobVal, false);
				char lpszBuffer[24];
				sprintf(lpszBuffer,"%u",value);
				CopyBuffer(&v.clobVal, lpszBuffer, strlen(lpszBuffer));
			}break;
		case FT_NCHAR:
			{

			}break;
		case FT_NVARCHAR:
			// ???
			break;
		case FT_BINARY:
			{
				FreeBuffer(&v.lobVal, false);
				char lpszBuffer[24];
				sprintf(lpszBuffer,"%u",value);
				CopyBuffer(&v.lobVal, lpszBuffer, strlen(lpszBuffer));
			}break;
		case FT_VARBINARY:
			{
				FreeBuffer(&v.varlobVal, false);
				char lpszBuffer[24];
				sprintf(lpszBuffer,"%u",value);
				CopyBuffer(&v.varlobVal, lpszBuffer, strlen(lpszBuffer));
			}break;
		case FT_BOOLEAN:
			v.booleanVal = value == 1 ? true : false;
			break;
		case FT_TINYINT:
			v.tinyintVal = value;
			break;
		case FT_UTINYINT:
			v.utinyintVal = value;
			break;
		case FT_SMALLINT:
			v.smallintVal = value;
			break;
		case FT_USMALLINT:
			v.usmallintVal = value;
			break;
		case FT_INTEGER:
			v.integerVal = value;
			break;
		case FT_UINTEGER:
			v.uintegerVal = value;
			break;
		case FT_BIGINT:
			v.bigintVal = value;
			break;
		case FT_UBIGINT:
			v.ubigintVal = value;
			break;
		case FT_NUMERIC:
			{
				if (v.numericVal == 0)
				{
					v.numericVal = new NUMERIC;
				}
				v.numericVal->negative = false;

				memset(v.numericVal->numeric, 0, sizeof(v.numericVal->numeric));
				for (short i=v.numericVal->size1-v.numericVal->size2-1; i>=0; i--)
				{
					v.numericVal->numeric[i] = Number2Char(value % 10);
					value = value / 10;
					if (value == 0) break;
				}

				//memset(v.numericVal->numeric1, 0, sizeof(v.numericVal->numeric1));
				//memset(v.numericVal->numeric2, 0, sizeof(v.numericVal->numeric2));
				//if (value > MAX_DEFAULT_NUMERIC_ITEM)
				//{
				//	v.numericVal->numeric1[MAX_NUMERIC_SIZE-1] = value % (MAX_DEFAULT_NUMERIC_ITEM+1);
				//	v.numericVal->numeric1[MAX_NUMERIC_SIZE-2] = value / (MAX_DEFAULT_NUMERIC_ITEM+1);
				//}else
				//{
				//	v.numericVal->numeric1[MAX_NUMERIC_SIZE-1] = value;
				//}
			}break;
		case FT_FLOAT:
			v.floatVal.value = static_cast<float>(value);
			break;
		case FT_REAL:
			v.realVal = static_cast<float>(value);
			break;
		case FT_DOUBLE:
			v.doubleVal = static_cast<double>(value);
			break;
		case FT_DATE:
			v.dateVal = static_cast<DATE>(value);
			break;
		case FT_TIME:
			v.timeVal = static_cast<TIME>(value);
			break;
		case FT_TIMESTAMP:
			{
				v.timestampVal.time = static_cast<botime_t>(value);
				v.timestampVal.millsecond = 0;
			}break;
		default:
			return false;
			break;
		}
		return true;
	}

	bool CFieldVariant::getBool(bool defv) const
	{
		bool result = defv;
		switch (VARTYPE)
		{
		case FT_BOOLEAN:
			result = v.booleanVal;
			break;
		case FT_TINYINT:
			result = v.tinyintVal == 1;
			break;
		case FT_UTINYINT:
			result = v.utinyintVal == 1;
			break;
		case FT_SMALLINT:
			result = v.smallintVal == 1;
			break;
		case FT_USMALLINT:
			result = v.usmallintVal == 1;
			break;
		case FT_INTEGER:
			result = v.integerVal == 1;
			break;
		case FT_UINTEGER:
			result = v.uintegerVal == 1;
			break;
		case FT_BIGINT:
			result = v.bigintVal == 1;
			break;
		case FT_UBIGINT:
			result = v.ubigintVal == 1;
			break;
		case FT_FLOAT:
			result = v.floatVal.value == 1.0;
			break;
		case FT_REAL:
			result = v.realVal == 1.0;
			break;
		case FT_DOUBLE:
			result = v.doubleVal == 1.0;
			break;
			// ??
		default:
			break;
		}
		return result;
	}

	int CFieldVariant::getInt(int defv) const
	{
		int result = defv;
		switch (VARTYPE)
		{
		case FT_BOOLEAN:
			result = v.booleanVal ? 1 : 0;
			break;
		case FT_TINYINT:
			result = v.tinyintVal;
			break;
		case FT_UTINYINT:
			result = v.utinyintVal;
			break;
		case FT_SMALLINT:
			result = v.smallintVal;
			break;
		case FT_USMALLINT:
			result = v.usmallintVal;
			break;
		case FT_INTEGER:
			result = v.integerVal;
			break;
		case FT_UINTEGER:
			result = v.uintegerVal;
			break;
		case FT_BIGINT:
			result = static_cast<int>(v.bigintVal);
			break;
		case FT_UBIGINT:
			result = static_cast<int>(v.ubigintVal);
			break;
		case FT_NUMERIC:
			{
				if (v.numericVal != 0)
				{
					std::complex <double> cb1 ( 10 , 0);

					bool bGetNumeric = false;
					for (short i=0; i<v.numericVal->size1-v.numericVal->size2; i++)
					{
						if (!bGetNumeric && v.numericVal->numeric[i] == '0')
							continue;
						bGetNumeric = true;

						std::complex <double> ce1 = pow(cb1, (v.numericVal->size1-v.numericVal->size2)-i-1);

						result += Char2Number(v.numericVal->numeric[i]) * ce1.real();
					}

					if (v.numericVal->negative)
						result *= -1;

					//result = v.numericVal->numeric1[MAX_NUMERIC_SIZE-2] * MAX_DEFAULT_NUMERIC_ITEM;
					//result += v.numericVal->numeric1[MAX_NUMERIC_SIZE-1];
				}
			}break;
		case FT_FLOAT:
			result = static_cast<int>(v.floatVal.value);
			break;
		case FT_REAL:
			result = static_cast<int>(v.realVal);
			break;
		case FT_DOUBLE:
			result = static_cast<int>(v.doubleVal);
			break;
		case FT_DATE:
			result = static_cast<int>(v.dateVal);
			break;
		case FT_TIME:
			result = static_cast<int>(v.timeVal);
			break;
		case FT_TIMESTAMP:
			result = static_cast<int>(v.timestampVal.time);
			break;
		case FT_CHAR:
			{
				if (v.charVal.size > 0 && v.charVal.buffer != 0)
					result = atoi(v.charVal.buffer);
			}break;
		case FT_VARCHAR:
			{
				if (v.varcharVal.size > 0 && v.varcharVal.buffer != 0)
					result = atoi(v.varcharVal.buffer);
			}break;
		//case FT_NCHAR:
		//	{
		//		if (v.ncharVal.size > 0 && v.ncharVal.buffer != 0)
		//			result = atoi((v.ncharVal.buffer);
		//	}break;
		default:
			break;
		}
		return result;
	}
	uinteger CFieldVariant::getIntu(uinteger defv) const
	{
		uinteger result = defv;
		switch (VARTYPE)
		{
		case FT_BOOLEAN:
			result = v.booleanVal ? 1 : 0;
			break;
		case FT_TINYINT:
			result = v.tinyintVal;
			break;
		case FT_UTINYINT:
			result = v.utinyintVal;
			break;
		case FT_SMALLINT:
			result = v.smallintVal;
			break;
		case FT_USMALLINT:
			result = v.usmallintVal;
			break;
		case FT_INTEGER:
			result = v.integerVal;
			break;
		case FT_UINTEGER:
			result = v.uintegerVal;
			break;
		case FT_BIGINT:
			result = static_cast<uinteger>(v.bigintVal);
			break;
		case FT_UBIGINT:
			result = static_cast<uinteger>(v.ubigintVal);
			break;
		case FT_NUMERIC:
			{
				if (v.numericVal != 0)
				{
					std::complex <double> cb1 ( 10 , 0);

					bool bGetNumeric = false;
					for (short i=0; i<v.numericVal->size1-v.numericVal->size2; i++)
					{
						if (!bGetNumeric && v.numericVal->numeric[i] == '0')
							continue;
						bGetNumeric = true;

						std::complex <double> ce1 = pow(cb1, (v.numericVal->size1-v.numericVal->size2)-i-1);

						result += Char2Number(v.numericVal->numeric[i]) * ce1.real();
					}

					// ?
					if (v.numericVal->negative)
						result *= -1;
				}
			}break;
		case FT_FLOAT:
			result = static_cast<uinteger>(v.floatVal.value);
			break;
		case FT_REAL:
			result = static_cast<uinteger>(v.realVal);
			break;
		case FT_DOUBLE:
			result = static_cast<uinteger>(v.doubleVal);
			break;
		case FT_DATE:
			result = static_cast<uinteger>(v.dateVal);
			break;
		case FT_TIME:
			result = static_cast<uinteger>(v.timeVal);
			break;
		case FT_TIMESTAMP:
			result = static_cast<uinteger>(v.timestampVal.time);
			break;
		case FT_CHAR:
			{
				if (v.charVal.size > 0 && v.charVal.buffer != 0)
					result = atoi(v.charVal.buffer);
			}break;
		case FT_VARCHAR:
			{
				if (v.varcharVal.size > 0 && v.varcharVal.buffer != 0)
					result = atoi(v.varcharVal.buffer);
			}break;
		default:
			break;
		}
		return result;
	}
	bigint CFieldVariant::getBigInt(bigint defv) const
	{
		bigint result = defv;
		switch (VARTYPE)
		{
		case FT_BOOLEAN:
			result = v.booleanVal ? 1 : 0;
			break;
		case FT_TINYINT:
			result = v.tinyintVal;
			break;
		case FT_UTINYINT:
			result = v.utinyintVal;
			break;
		case FT_SMALLINT:
			result = v.smallintVal;
			break;
		case FT_USMALLINT:
			result = v.usmallintVal;
			break;
		case FT_INTEGER:
			result = v.integerVal;
			break;
		case FT_UINTEGER:
			result = v.uintegerVal;
			break;
		case FT_BIGINT:
			result = static_cast<bigint>(v.bigintVal);
			break;
		case FT_UBIGINT:
			result = static_cast<bigint>(v.ubigintVal);
			break;
		case FT_NUMERIC:
			{
				if (v.numericVal != 0)
				{
					std::complex <double> cb1 ( 10 , 0);

					bool bGetNumeric = false;
					for (short i=0; i<v.numericVal->size1-v.numericVal->size2; i++)
					{
						if (!bGetNumeric && v.numericVal->numeric[i] == '0')
							continue;
						bGetNumeric = true;

						std::complex <double> ce1 = pow(cb1, (v.numericVal->size1-v.numericVal->size2)-i-1);

						result += Char2Number(v.numericVal->numeric[i]) * ce1.real();
					}

					// ?
					if (v.numericVal->negative)
						result *= -1;
				}
			}break;
		case FT_FLOAT:
			result = static_cast<bigint>(v.floatVal.value);
			break;
		case FT_REAL:
			result = static_cast<bigint>(v.realVal);
			break;
		case FT_DOUBLE:
			result = static_cast<bigint>(v.doubleVal);
			break;
		case FT_DATE:
			result = static_cast<bigint>(v.dateVal);
			break;
		case FT_TIME:
			result = static_cast<bigint>(v.timeVal);
			break;
		case FT_TIMESTAMP:
			result = static_cast<bigint>(v.timestampVal.time);
			break;
		case FT_CHAR:
			{
				if (v.charVal.size > 0 && v.charVal.buffer != 0)
					result = atol(v.charVal.buffer);
			}break;
		case FT_VARCHAR:
			{
				if (v.varcharVal.size > 0 && v.varcharVal.buffer != 0)
					result = atol(v.varcharVal.buffer);
			}break;
		default:
			break;
		}
		return result;
	}
	double CFieldVariant::getReal(double defv) const
	{
		double result = defv;
		switch (VARTYPE)
		{
		case FT_BOOLEAN:
			result = v.booleanVal ? 1.0 : 0.0;
			break;
		case FT_TINYINT:
			result = static_cast<double>(v.tinyintVal);
			break;
		case FT_UTINYINT:
			result = static_cast<double>(v.utinyintVal);
			break;
		case FT_SMALLINT:
			result = static_cast<double>(v.smallintVal);
			break;
		case FT_USMALLINT:
			result = static_cast<double>(v.usmallintVal);
			break;
		case FT_INTEGER:
			result = static_cast<double>(v.integerVal);
			break;
		case FT_UINTEGER:
			result = static_cast<double>(v.uintegerVal);
			break;
		case FT_BIGINT:
			result = static_cast<double>(v.bigintVal);
			break;
		case FT_UBIGINT:
			result = static_cast<double>(v.ubigintVal);
			break;
		case FT_NUMERIC:
			{
				if (v.numericVal != 0)
				{
					std::complex <double> cb1 ( 10 , 0);

					bool bGetNumeric = false;
					for (short i=0; i<v.numericVal->size1-v.numericVal->size2; i++)
					{
						if (!bGetNumeric && v.numericVal->numeric[i] == '0')
							continue;
						bGetNumeric = true;

						std::complex <double> ce1 = pow(cb1, (v.numericVal->size1-v.numericVal->size2)-i-1);

						result += Char2Number(v.numericVal->numeric[i]) * ce1.real();
					}

					// ?
					if (v.numericVal->negative)
						result *= -1;
				}
			}break;
		case FT_FLOAT:
			result = static_cast<double>(v.floatVal.value);
			break;
		case FT_REAL:
			result = static_cast<double>(v.realVal);
			break;
		case FT_DOUBLE:
			result = v.doubleVal;
			break;
		case FT_DATE:
			result = static_cast<double>(v.dateVal);
			break;
		case FT_TIME:
			result = static_cast<double>(v.timeVal);
			break;
		case FT_TIMESTAMP:
			result = static_cast<double>(v.timestampVal.time);
			break;
		case FT_CHAR:
			{
				if (v.charVal.size > 0 && v.charVal.buffer != 0)
					result = atof(v.charVal.buffer);
			}break;
		case FT_VARCHAR:
			{
				if (v.varcharVal.size > 0 && v.varcharVal.buffer != 0)
					result = atof(v.varcharVal.buffer);
			}break;
		default:
			break;
		}
		return result;
	}

	char * CFieldVariant::getString(int & outSize) const
	{
		char * result = 0;
		switch (VARTYPE)
		{
		case FT_BOOLEAN:
			{
				result = new char[2];
				result[0] = v.booleanVal ? '1' : '0';
				result[1] = '\0';
			}break;
		case FT_TINYINT:
			{
				result = new char[10];
				outSize = sprintf(result, "%d", v.tinyintVal);	// ?
			}break;
		case FT_UTINYINT:
			{
				result = new char[10];
				outSize = sprintf(result, "%d", v.utinyintVal);	// ?
			}break;
		case FT_SMALLINT:
			{
				result = new char[10];
				outSize = sprintf(result, "%d", v.smallintVal);
			}break;
		case FT_USMALLINT:
			{
				result = new char[10];
				outSize = sprintf(result, "%d", v.usmallintVal);
			}break;
		case FT_INTEGER:
			{
				result = new char[12];
				outSize = sprintf(result, "%d", v.integerVal);
			}break;
		case FT_UINTEGER:
			{
				result = new char[12];
				outSize = sprintf(result, "%d", v.uintegerVal);
			}break;
		case FT_BIGINT:
			{
				result = new char[20];
#ifdef WIN32
				outSize = sprintf(result, "%I64d", v.bigintVal);	// ??
#else
				outSize = sprintf(result, "%lld", v.bigintVal);	// ??
#endif
			}break;
		case FT_UBIGINT:
			{
				result = new char[20];
#ifdef WIN32
				outSize = sprintf(result, "%I64d", v.ubigintVal);	// ??
#else
				outSize = sprintf(result, "%lld", v.ubigintVal);	// ??
#endif
			}break;
		case FT_NUMERIC:
			{
				if (v.numericVal != 0)
				{
					bool bGetNumeric = false;
					outSize = 0;
					result = new char[v.numericVal->size1+1];

					if (v.numericVal->negative)
					{
						result[outSize++] = '-';
					}

					for (short i=0; i<v.numericVal->size1-v.numericVal->size2; i++)
					{
						if (!bGetNumeric && v.numericVal->numeric[i] == '0')
							continue;
						bGetNumeric = true;
						result[outSize++] = v.numericVal->numeric[i];
					}
					if (v.numericVal->size2 > 0)
					{
						result[outSize++] = '.';
						for (short i=v.numericVal->size1-v.numericVal->size2; i<v.numericVal->size1; i++)
						{
							result[outSize++] = v.numericVal->numeric[i];
						}
					}
					result[outSize] = '\0';
				}
			}break;
		case FT_FLOAT:
			{
				// v.floatVal.size1;
				result = new char[20];
				outSize = sprintf(result, "%f", v.floatVal.value);	// ??
			}break;
		case FT_REAL:
			{
				result = new char[20];
				outSize = sprintf(result, "%f", v.realVal);
			}break;
		case FT_DOUBLE:
			{
				result = new char[20];
				outSize = sprintf(result, "%f", v.doubleVal);
			}break;
		case FT_DATE:
			{
				struct tm  * tmVal = gmtime((const time_t*)&v.dateVal);
				result = new char[20];
				outSize = sprintf(result, "%04d-%02d-%02d", 1900+tmVal->tm_year, 1+tmVal->tm_mon, tmVal->tm_mday);
				//outSize = sprintf(result, "%04d-%02d-%02d", v.dateVal.year, v.dateVal.month, v.dateVal.day);
			}break;
		case FT_TIME:
			{
				struct tm  * tmVal = gmtime((const time_t*)&v.timeVal);
				result = new char[20];
				outSize = sprintf(result, "%02d:%02d:%02d", tmVal->tm_hour, tmVal->tm_min, tmVal->tm_sec);
				//outSize = sprintf(result, "%02d:%02d:%02d", v.timeVal.hour, v.timeVal.minute, v.timeVal.second);
			}break;
		case FT_TIMESTAMP:
			{
				// ?? timezone
				struct tm  * tmVal = gmtime((const time_t*)&v.timestampVal.time);

				result = new char[40];
				outSize = sprintf(result, "%04d-%02d-%02d %02d:%02d:%02d %03d",
					1900+tmVal->tm_year, 1+tmVal->tm_mon, tmVal->tm_mday,
					tmVal->tm_hour, tmVal->tm_min, tmVal->tm_sec, v.timestampVal.millsecond);

				//outSize = sprintf(result, "%04d-%02d-%02d %02d:%02d:%02d %03d",
				//	v.timestampVal.date.year, v.timestampVal.date.month, v.timestampVal.date.day,
				//	v.timestampVal.time.hour, v.timestampVal.time.minute, v.timestampVal.time.second,
				//	v.timestampVal.millsecond);
			}break;
		case FT_CHAR:
			if (v.charVal.size > 0 && v.charVal.buffer != 0)
			{
				outSize = v.charVal.size;
				result = new char[outSize+1];
				strcpy(result, v.charVal.buffer);
				result[outSize] = '\0';
			}break;
		case FT_VARCHAR:
			if (v.varcharVal.size > 0 && v.varcharVal.buffer != 0)
			{
				outSize = v.varcharVal.size;
				result = new char[outSize+1];
				strcpy(result, v.varcharVal.buffer);
				result[outSize] = '\0';
			}break;
		case FT_CLOB:
			{
				if (v.clobVal.size > 0 && v.clobVal.buffer != 0)
				{
					outSize = v.clobVal.size;
					result = new char[outSize+1];
					strcpy(result, v.clobVal.buffer);
					result[outSize] = '\0';
				}
			}break;
		case FT_NCHAR:
			{
				// ?? string_code
				if (v.ncharVal.size > 0 && v.ncharVal.buffer != 0)
				{
					size_t size = 0;
					result = W2Char(v.ncharVal.buffer, size);
					outSize = (int)size;
				}
			}break;
		case FT_NVARCHAR:
			{
				// ?? string_code
				if (v.nvarcharVal.size > 0 && v.nvarcharVal.buffer != 0)
				{
					size_t size = 0;
					result = W2Char(v.nvarcharVal.buffer, size);
					outSize = (int)size;
				}
			}break;
		case FT_BINARY:
			if (v.lobVal.size > 0 && v.lobVal.buffer != 0)
			{
				outSize = v.lobVal.size;
				result = new char[outSize+1];
				memcpy(result, v.lobVal.buffer, outSize);
				result[outSize] = '\0';
			}break;
		case FT_VARBINARY:
			if (v.varlobVal.size > 0 && v.varlobVal.buffer != 0)
			{
				outSize = v.varlobVal.size;
				result = new char[outSize+1];
				memcpy(result, v.varlobVal.buffer, outSize);
				result[outSize] = '\0';
			}break;
		default:
			break;
		}

		return result;
	}
	std::string CFieldVariant::getString(void) const
	{
		std::string result = "";
		int size = 0;
		char * str = getString(size);
		if (str != 0)
		{
			result = std::string(str, size);
			delete[] str;
		}
		return result;
	}

	wchar_t * CFieldVariant::getNString(int & outSize) const
	{
		wchar_t * result = 0;
		switch (VARTYPE)
		{
		case FT_CHAR:
			{
				// ?? string_code
				if (v.charVal.size > 0 && v.charVal.buffer != 0)
				{
					size_t size = 0;
					result = Char2W(v.charVal.buffer, size);
					outSize = (int)size;
				}
			}break;
		case FT_VARCHAR:
			{
				// ?? string_code
				if (v.varcharVal.size > 0 && v.varcharVal.buffer != 0)
				{
					size_t size = 0;
					result = Char2W(v.varcharVal.buffer, size);
					outSize = (int)size;
				}
			}break;
		case FT_CLOB:
			{
				if (v.clobVal.size > 0 && v.clobVal.buffer != 0)
				{
					size_t size = 0;
					result = Char2W(v.clobVal.buffer, size);
					outSize = (int)size;
				}
			}break;
		case FT_NCHAR:
			if (v.ncharVal.size > 0 && v.ncharVal.buffer != 0)
			{
				outSize = v.ncharVal.size;
				result = new wchar_t[outSize+1];
				memcpy(result, v.ncharVal.buffer, outSize);	// ?? * 2
				result[outSize] = L'\0';
			}break;
		case FT_NVARCHAR:
			if (v.nvarcharVal.size > 0 && v.nvarcharVal.buffer != 0)
			{
				outSize = v.nvarcharVal.size;
				result = new wchar_t[outSize+1];
				memcpy(result, v.nvarcharVal.buffer, outSize);	// ?? * 2
				result[outSize] = L'\0';
			}break;
		case FT_BINARY:
			if (v.lobVal.size > 0 && v.lobVal.buffer != 0)
			{
				size_t size = 0;
				result = Char2W(v.lobVal.buffer, size);	// ??
				outSize = (int)size;
			}break;
		case FT_VARBINARY:
			if (v.varlobVal.size > 0 && v.varlobVal.buffer != 0)
			{
				size_t size = 0;
				result = Char2W(v.varlobVal.buffer, size);	// ??
				outSize = (int)size;
			}break;
		default:
			break;
		}

		return result;
	}
	std::wstring CFieldVariant::getNString(void) const
	{
		std::wstring result = L"";
		int size = 0;
		wchar_t * str = getNString(size);
		if (str != 0)
		{
			result = std::wstring(str, size);
			delete[] str;
		}
		return result;
	}

	PFIELDVARIANT CFieldVariant::getValue(void) const
	{
		PFIELDVARIANT result = NewFieldVariant();
		CopyFieldVariant((PFIELDVARIANT)(this), result);
		return result;
	}

	CFieldVariant::CFieldVariant(FieldType vartype)
	{
		memset((PFIELDVARIANT)(this), 0, sizeof(FIELDVARIANT));
		this->VARTYPE = vartype;
	}
	CFieldVariant::CFieldVariant(const PFIELDVARIANT variant)
	{
		BOOST_ASSERT (variant != 0);
		memset((PFIELDVARIANT)(this), 0, sizeof(FIELDVARIANT));
		CopyFieldVariant(variant, (PFIELDVARIANT)(this));
	}
	CFieldVariant::CFieldVariant(void)
	{
		memset((PFIELDVARIANT)(this), 0, sizeof(FIELDVARIANT));
		VARTYPE = FT_UNKNOWN;
	}
	CFieldVariant::~CFieldVariant(void)
	{
		//m_value.clear();
		FreeFieldVariant((PFIELDVARIANT)(this), false);
	}

	void CFieldVariant::Serialize(bool isStoring, tfstream& ar, uinteger size1, bool bDefaultVariant, uinteger & outDoSize)
	{
		ar.clear();
		if (isStoring)
		{
			switch (VARTYPE)
			{
			case FT_BOOLEAN:
				{
					ar.write((const char*)&v.booleanVal, BOOLEAN_SIZE);
					outDoSize = BOOLEAN_SIZE;
				}break;
			case FT_TINYINT:
				{
					ar.write((const char*)&v.tinyintVal, TINYINT_SIZE);
					outDoSize = TINYINT_SIZE;
				}break;
			case FT_UTINYINT:
				{
					ar.write((const char*)&v.utinyintVal, TINYINT_SIZE);
					outDoSize = TINYINT_SIZE;
				}break;
			case FT_SMALLINT:
				{
					ar.write((const char*)&v.smallintVal, SMALLINT_SIZE);
					outDoSize = SMALLINT_SIZE;
				}break;
			case FT_USMALLINT:
				{
					ar.write((const char*)&v.usmallintVal, SMALLINT_SIZE);
					outDoSize = SMALLINT_SIZE;
				}break;
			case FT_INTEGER:
				{
					ar.write((const char*)&v.integerVal, INTEGER_SIZE);
					outDoSize = INTEGER_SIZE;
				}break;
			case FT_UINTEGER:
				{
					ar.write((const char*)&v.uintegerVal, INTEGER_SIZE);
					outDoSize = INTEGER_SIZE;
				}break;
			case FT_BIGINT:
				{
					ar.write((const char*)&v.bigintVal, BIGINT_SIZE);
					outDoSize = BIGINT_SIZE;
				}break;
			case FT_UBIGINT:
				{
					ar.write((const char*)&v.ubigintVal, BIGINT_SIZE);
					outDoSize = BIGINT_SIZE;
				}break;
			case FT_NUMERIC:
				{
					if (v.numericVal != 0)
						ar.write((const char*)v.numericVal, NUMERIC_SIZE);
					else
						ar.seekp(NUMERIC_SIZE, std::ios::cur);
					outDoSize = NUMERIC_SIZE;
				}break;
			case FT_FLOAT:
				ar.write((const char*)&v.floatVal, FLOAT_SIZE);
				outDoSize = FLOAT_SIZE;
				break;
			case FT_REAL:
				ar.write((const char*)&v.realVal, REAL_SIZE);
				outDoSize = REAL_SIZE;
				break;
			case FT_DOUBLE:
				{
					ar.write((const char*)&v.doubleVal, DOUBLE_SIZE);
					outDoSize = DOUBLE_SIZE;
				}break;
			case FT_DATE:
				{
					ar.write((const char*)&v.dateVal, DATE_SIZE);
				outDoSize = DATE_SIZE;
				}break;
			case FT_TIME:
				{
					ar.write((const char*)&v.timeVal, TIME_SIZE);
					outDoSize = TIME_SIZE;
				}break;
			case FT_TIMESTAMP:
				{
					ar.write((const char*)&v.timestampVal, TIMESTAMP_SIZE);
				outDoSize = TIMESTAMP_SIZE;
				}break;
			case FT_CHAR:
				{
					if (v.charVal.size > size1)
						v.charVal.size = size1;
					ar.write((const char*)&v.charVal.size, INTEGER_SIZE);
					if (v.charVal.size > 0)
					{
						char * buffer = new char[v.charVal.size+1];
						strcpy(buffer, v.charVal.buffer);
						for (unsigned int i=0; i<v.charVal.size; i+=2)
						{
							buffer[i] = buffer[i]+(char)i+(char)v.charVal.size;
						}
						ar.write(buffer, v.charVal.size);
						delete[] buffer;
						//ar.write((const char*)v.charVal.buffer, v.charVal.size);
					}
					if (size1 > v.charVal.size)
						ar.seekp(size1 - v.charVal.size, std::ios::cur);
					outDoSize = INTEGER_SIZE + size1;
				}break;
			case FT_VARCHAR:
				{
					if (bDefaultVariant)
					{
						if (v.varcharVal.size > size1)
							v.varcharVal.size = size1;
						ar.write((const char*)&v.varcharVal.size, INTEGER_SIZE);
						if (v.varcharVal.size > 0)
						{
							char * buffer = new char[v.varcharVal.size+1];
							strcpy(buffer, v.varcharVal.buffer);
							for (unsigned int i=0; i<v.varcharVal.size; i+=2)
							{
								buffer[i] = buffer[i]+(char)i+(char)v.varcharVal.size;
							}
							ar.write(buffer, v.varcharVal.size);
							delete[] buffer;
							//ar.write((const char*)v.varcharVal.buffer, v.varcharVal.size);
						}
						if (size1 > v.varcharVal.size)
							ar.seekp(size1 - v.varcharVal.size, std::ios::cur);
						outDoSize = INTEGER_SIZE + size1;
					}else
					{
						ar.write((const char*)&v.varcharVal.size, INTEGER_SIZE);
						outDoSize = INTEGER_SIZE;
					}
				}break;
			case FT_CLOB:
				{
					if (bDefaultVariant)
					{
						if (v.clobVal.size > size1)
							v.clobVal.size = size1;
						ar.write((const char*)&v.clobVal.size, INTEGER_SIZE);
						if (v.clobVal.size > 0)
						{
							char * buffer = new char[v.clobVal.size+1];
							strcpy(buffer, v.clobVal.buffer);
							for (unsigned int i=0; i<v.clobVal.size; i+=2)
							{
								buffer[i] = buffer[i]+(char)i+(char)v.clobVal.size;
							}
							ar.write(buffer, v.clobVal.size);
							delete[] buffer;
							//ar.write((const char*)v.clobVal.buffer, v.clobVal.size);
						}
						if (size1 > v.clobVal.size)
							ar.seekp(size1 - v.clobVal.size, std::ios::cur);
						outDoSize = INTEGER_SIZE + size1;
					}else
					{
						ar.write((const char*)&v.clobVal.size, INTEGER_SIZE);
						outDoSize = INTEGER_SIZE;
					}
				}break;
			case FT_NCHAR:
				{
					// ???
					if (v.ncharVal.size > size1)
						v.ncharVal.size = size1;
					ar.write((const char*)&v.ncharVal.size, INTEGER_SIZE);
					if (v.ncharVal.size > 0)
						ar.write((const char*)v.ncharVal.buffer, v.ncharVal.size);
					if (size1 > v.ncharVal.size)
						ar.seekp(size1 - v.ncharVal.size, std::ios::cur);
					outDoSize = INTEGER_SIZE + size1;
				}break;
			case FT_NVARCHAR:
				{
					if (bDefaultVariant)
					{
						// ???
						if (v.nvarcharVal.size > size1)
						v.nvarcharVal.size = size1;
						ar.write((const char*)&v.nvarcharVal.size, INTEGER_SIZE);
						if (v.nvarcharVal.size > 0)
						ar.write((const char*)v.nvarcharVal.buffer, v.nvarcharVal.size);
						if (size1 > v.nvarcharVal.size)
						ar.seekp(size1 - v.nvarcharVal.size, std::ios::cur);
						outDoSize = INTEGER_SIZE + size1;
					}else
					{
						ar.write((const char*)&v.nvarcharVal.size, INTEGER_SIZE);
						outDoSize = INTEGER_SIZE;
					}
				}break;
			case FT_NCLOB:
				{
					if (bDefaultVariant)
					{
						// ???
						if (v.nclobVal.size > size1)
						v.nclobVal.size = size1;
						ar.write((const char*)&v.nclobVal.size, INTEGER_SIZE);
						if (v.nclobVal.size > 0)
						ar.write((const char*)v.nclobVal.buffer, v.nclobVal.size);
						if (size1 > v.nclobVal.size)
						ar.seekp(size1 - v.nclobVal.size, std::ios::cur);
						outDoSize = INTEGER_SIZE + size1;
					}else
					{
						ar.write((const char*)&v.nclobVal.size, INTEGER_SIZE);
						outDoSize = INTEGER_SIZE;
					}
				}break;
			case FT_BINARY:
				{
					if (v.lobVal.size > size1)
						v.lobVal.size = size1;
					ar.write((const char*)&v.lobVal.size, INTEGER_SIZE);
					if (v.lobVal.size > 0)
						ar.write((const char*)v.lobVal.buffer, v.lobVal.size);
					if (size1 > v.lobVal.size)
						ar.seekp(size1 - v.lobVal.size, std::ios::cur);
					outDoSize = INTEGER_SIZE + size1;
				}break;
			case FT_VARBINARY:
				{
					if (bDefaultVariant)
					{
						if (v.varlobVal.size > size1)
							v.varlobVal.size = size1;
						ar.write((const char*)&v.varlobVal.size, INTEGER_SIZE);
						if (v.varlobVal.size > 0)
							ar.write((const char*)v.varlobVal.buffer, v.varlobVal.size);
						if (size1 > v.varlobVal.size)
							ar.seekp(size1 - v.varlobVal.size, std::ios::cur);
						outDoSize = INTEGER_SIZE + size1;
					}else
					{
						ar.write((const char*)&v.varlobVal.size, INTEGER_SIZE);
						outDoSize = INTEGER_SIZE;
					}
				}break;
			case FT_BLOB:
				{
					if (bDefaultVariant)
					{
						if (v.blobVal.size > size1)
							v.blobVal.size = size1;
						ar.write((const char*)&v.blobVal.size, INTEGER_SIZE);
						if (v.blobVal.size > 0)
							ar.write((const char*)v.blobVal.buffer, v.blobVal.size);
						if (size1 > v.blobVal.size)
							ar.seekp(size1 - v.blobVal.size, std::ios::cur);
						outDoSize = INTEGER_SIZE + size1;
					}else
					{
						ar.write((const char*)&v.blobVal.size, INTEGER_SIZE);
						outDoSize = INTEGER_SIZE;
					}
				}break;
			default:
				break;
			}
		}else
		{
			switch (VARTYPE)
			{
			case FT_BOOLEAN:
				{
					ar.read((char*)&v.booleanVal, BOOLEAN_SIZE);
					outDoSize = BOOLEAN_SIZE;
				}break;
			case FT_TINYINT:
				{
					ar.read((char*)&v.tinyintVal, TINYINT_SIZE);
					outDoSize = TINYINT_SIZE;
				}break;
			case FT_UTINYINT:
				{
					ar.read((char*)&v.utinyintVal, TINYINT_SIZE);
					outDoSize = TINYINT_SIZE;
				}break;
			case FT_SMALLINT:
				{
					ar.read((char*)&v.smallintVal, SMALLINT_SIZE);
					outDoSize = SMALLINT_SIZE;
				}break;
			case FT_USMALLINT:
				{
					ar.read((char*)&v.usmallintVal, SMALLINT_SIZE);
					outDoSize = SMALLINT_SIZE;
				}break;
			case FT_INTEGER:
				{
					ar.read((char*)&v.integerVal, INTEGER_SIZE);
					outDoSize = INTEGER_SIZE;
				}break;
			case FT_UINTEGER:
				{
					ar.read((char*)&v.uintegerVal, INTEGER_SIZE);
					outDoSize = INTEGER_SIZE;
				}break;
			case FT_BIGINT:
				{
					ar.read((char*)&v.bigintVal, BIGINT_SIZE);
					outDoSize = BIGINT_SIZE;
				}break;
			case FT_UBIGINT:
				{
					ar.read((char*)&v.ubigintVal, BIGINT_SIZE);
					outDoSize = BIGINT_SIZE;
				}break;
			case FT_NUMERIC:
				{
					if (v.numericVal == 0)
						v.numericVal = new tagNUMERIC;
					memset(v.numericVal, 0, NUMERIC_SIZE);
					ar.read((char*)v.numericVal, NUMERIC_SIZE);
					outDoSize = NUMERIC_SIZE;
				}break;
			case FT_FLOAT:
				ar.read((char*)&v.floatVal, FLOAT_SIZE);
					outDoSize = FLOAT_SIZE;
				break;
			case FT_REAL:
				ar.read((char*)&v.realVal, REAL_SIZE);
					outDoSize = REAL_SIZE;
				break;
			case FT_DOUBLE:
				{
					ar.read((char*)&v.doubleVal, DOUBLE_SIZE);
					outDoSize = DOUBLE_SIZE;
				}break;
			case FT_DATE:
				{
					ar.read((char*)&v.dateVal, DATE_SIZE);
					outDoSize = DATE_SIZE;
				}break;
			case FT_TIME:
				{
					ar.read((char*)&v.timeVal, TIME_SIZE);
					outDoSize = TIME_SIZE;
				}break;
			case FT_TIMESTAMP:
				{
					ar.read((char*)&v.timestampVal, TIMESTAMP_SIZE);
					outDoSize = TIMESTAMP_SIZE;
				}break;
			case FT_CHAR:
				{
					ar.read((char*)(&v.charVal.size), INTEGER_SIZE);
					if (v.charVal.size > size1)
						v.charVal.size = size1;
					if (v.charVal.size > 0)
					{
						v.charVal.buffer = new char[v.charVal.size+1];
						ar.read(v.charVal.buffer, v.charVal.size);
						for (unsigned int i=0; i<v.charVal.size; i+=2)
						{
							v.charVal.buffer[i] = v.charVal.buffer[i]-(char)i-(char)v.charVal.size;
						}
						v.charVal.buffer[v.charVal.size] = '\0';
					}
					if (size1 > v.charVal.size)
						ar.seekg(size1 - v.charVal.size, std::ios::cur);
					outDoSize = INTEGER_SIZE + size1;
				}break;
			case FT_VARCHAR:
				{
					if (bDefaultVariant)
					{
						ar.read((char*)(&v.varcharVal.size), INTEGER_SIZE);
						if (v.varcharVal.size > size1)
							v.varcharVal.size = size1;
						if (v.varcharVal.size > 0)
						{
							v.varcharVal.buffer = new char[v.varcharVal.size+1];
							ar.read(v.varcharVal.buffer, v.varcharVal.size);
							for (unsigned int i=0; i<v.varcharVal.size; i+=2)
							{
								v.varcharVal.buffer[i] = v.varcharVal.buffer[i]-(char)i-(char)v.varcharVal.size;
							}
							v.varcharVal.buffer[v.varcharVal.size] = '\0';
						}
						if (size1 > v.varcharVal.size)
							ar.seekg(size1 - v.varcharVal.size, std::ios::cur);
						outDoSize = INTEGER_SIZE + size1;
					}else
					{
						ar.read((char*)(&v.varcharVal.size), INTEGER_SIZE);
						outDoSize = INTEGER_SIZE;
					}
				}break;
			case FT_CLOB:
				{
					if (bDefaultVariant)
					{
						ar.read((char*)(&v.clobVal.size), INTEGER_SIZE);
						if (v.clobVal.size > size1)
							v.clobVal.size = size1;
						if (v.clobVal.size > 0)
						{
							v.clobVal.buffer = new char[v.clobVal.size+1];
							ar.read(v.clobVal.buffer, v.clobVal.size);
							for (unsigned int i=0; i<v.clobVal.size; i+=2)
							{
								v.clobVal.buffer[i] = v.clobVal.buffer[i]-(char)i-(char)v.clobVal.size;
							}
							v.clobVal.buffer[v.clobVal.size] = '\0';
						}
						if (size1 > v.clobVal.size)
							ar.seekg(size1 - v.clobVal.size, std::ios::cur);
						outDoSize = INTEGER_SIZE + size1;
					}else
					{
						ar.read((char*)(&v.clobVal.size), INTEGER_SIZE);
						outDoSize = INTEGER_SIZE;
					}
				}break;
			case FT_NCHAR:
				{
					// ???
					ar.read((char*)(&v.ncharVal.size), INTEGER_SIZE);
					if (v.ncharVal.size > size1)
						v.ncharVal.size = size1;
					if (v.ncharVal.size > 0)
					{
						v.ncharVal.buffer = new wchar_t[v.ncharVal.size+1];
						ar.read((char*)v.ncharVal.buffer, v.ncharVal.size);
						v.ncharVal.buffer[v.ncharVal.size] = L'\0';
					}
					if (size1 > v.ncharVal.size)
						ar.seekg(size1 - v.ncharVal.size, std::ios::cur);
					outDoSize = INTEGER_SIZE + size1;
				}break;
			case FT_NVARCHAR:
				{
					if (bDefaultVariant)
					{
						// ???
						ar.read((char*)(&v.nvarcharVal.size), INTEGER_SIZE);
						if (v.nvarcharVal.size > size1)
							v.nvarcharVal.size = size1;
						if (v.nvarcharVal.size > 0)
						{
							v.nvarcharVal.buffer = new wchar_t[v.nvarcharVal.size+1];
							ar.read((char*)v.nvarcharVal.buffer, v.nvarcharVal.size);
							v.nvarcharVal.buffer[v.nvarcharVal.size] = L'\0';
						}
						if (size1 > v.nvarcharVal.size)
							ar.seekg(size1 - v.nvarcharVal.size, std::ios::cur);
						outDoSize = INTEGER_SIZE + size1;
					}else
					{
						ar.read((char*)(&v.nvarcharVal.size), INTEGER_SIZE);
						outDoSize = INTEGER_SIZE;
					}
				}break;
			case FT_NCLOB:
				{
					if (bDefaultVariant)
					{
						// ???
						ar.read((char*)(&v.nclobVal.size), INTEGER_SIZE);
						if (v.nclobVal.size > size1)
							v.nclobVal.size = size1;
						if (v.nclobVal.size > 0)
						{
							v.nclobVal.buffer = new wchar_t[v.nclobVal.size+1];
							ar.read((char*)v.nclobVal.buffer, v.nclobVal.size);
							v.nclobVal.buffer[v.nclobVal.size] = L'\0';
						}
						if (size1 > v.nclobVal.size)
							ar.seekg(size1 - v.nclobVal.size, std::ios::cur);
						outDoSize = INTEGER_SIZE + size1;
					}else
					{
						ar.read((char*)(&v.nclobVal.size), INTEGER_SIZE);
						outDoSize = INTEGER_SIZE;
					}
				}break;
			case FT_BINARY:
				{
					ar.read((char*)(&v.lobVal.size), INTEGER_SIZE);
					if (v.lobVal.size > size1)
						v.lobVal.size = size1;
					if (v.lobVal.size > 0)
					{
						v.lobVal.buffer = new char[v.lobVal.size+1];
						ar.read(v.lobVal.buffer, v.lobVal.size);
						v.lobVal.buffer[v.lobVal.size] = '\0';
					}
					if (size1 > v.lobVal.size)
						ar.seekg(size1 - v.lobVal.size, std::ios::cur);
					outDoSize = INTEGER_SIZE + size1;
				}break;
			case FT_VARBINARY:
				{
					if (bDefaultVariant)
					{
						ar.read((char*)(&v.varlobVal.size), INTEGER_SIZE);
						if (v.varlobVal.size > size1)
							v.varlobVal.size = size1;
						if (v.varlobVal.size > 0)
						{
							v.varlobVal.buffer = new char[v.varlobVal.size+1];
							ar.read(v.varlobVal.buffer, v.varlobVal.size);
							v.varlobVal.buffer[v.varlobVal.size] = '\0';
						}
						if (size1 > v.varlobVal.size)
							ar.seekg(size1 - v.varlobVal.size, std::ios::cur);
						outDoSize = INTEGER_SIZE + size1;
					}else
					{
						ar.read((char*)(&v.varlobVal.size), INTEGER_SIZE);
						outDoSize = INTEGER_SIZE;
					}
				}break;
			case FT_BLOB:
				{
					if (bDefaultVariant)
					{
						ar.read((char*)(&v.blobVal.size), INTEGER_SIZE);
						if (v.blobVal.size > size1)
							v.blobVal.size = size1;
						if (v.blobVal.size > 0)
						{
							v.blobVal.buffer = new char[v.blobVal.size+1];
							ar.read(v.blobVal.buffer, v.blobVal.size);
							v.blobVal.buffer[v.blobVal.size] = '\0';
						}
						if (size1 > v.blobVal.size)
							ar.seekg(size1 - v.blobVal.size, std::ios::cur);
						outDoSize = INTEGER_SIZE + size1;
					}else
					{
						ar.read((char*)(&v.blobVal.size), INTEGER_SIZE);
						outDoSize = INTEGER_SIZE;
					}
				}break;
			default:
				break;
			}
		}
	}

} // namespace
