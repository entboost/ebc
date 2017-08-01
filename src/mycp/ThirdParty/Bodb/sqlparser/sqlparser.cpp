/*
    The sqlparser library parse the SQL (Structured Query Language)
    Copyright (C) 2009-2014  Akee Yang <akee.yang@gmail.com>

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

#include <memory.h>
#include <stdlib.h>
#include <string.h>
#include "sqlparser.h"

///////////////////////////////////////////////
// 
void free_field(tagField * field);
void free_param(tagParameter * param);
void free_items(tagItems * items);

// ' ' '\t' '\n' '\r'
bool isInvalidateChar(char in)
{
	return ' ' == in || '\t' == in || '\n' == in || '\r' == in;
}

// ' ' '\t' '\n' '\r' '\0'
bool isInvalidateChar2(char in)
{
	return ' ' == in || '\t' == in || '\n' == in || '\r' == in || '\0' == in;
}

// 'A'-'Z' 'a'-'z'
bool isValidateChar(char in)
{
	return (in >= 'A' && in <= 'Z') || (in >= 'a' && in <= 'z');
}

// 'A'-'Z' 'a'-'z' '0'-'9' '_'
bool isValidateChar2(char in)
{
	return (in >= 'A' && in <= 'Z') || (in >= 'a' && in <= 'z') || (in >= '0' && in <= '9') || (in == '_');
}

// 'A'-'Z' 'a'-'z' '0'-'9' '_' ','
bool isValidateChar3(char in)
{
	return (in >= 'A' && in <= 'Z') || (in >= 'a' && in <= 'z') || (in >= '0' && in <= '9') || in == '_' || in == ',';
}

// '0'-'9'
bool isNumber(char in)
{
	return (in >= '0' && in <= '9') || in == '+' || in == '-' || in == '.';
}

bool isBOOLVALUE(char in)
{
	return in == 'T' || in == 'U' || in == 'R' || in == 'E' ||
		in == 't' || in == 'u' || in == 'r' || in == 'e' ||
		in == 'F' || in == 'A' || in == 'L' || in == 'S' ||
		in == 'f' || in == 'a' || in == 'l' || in == 's';
}

bool isNULLVALUE(char in)
{
	return in == 'N' || in == 'U' || in == 'L' ||
		in == 'n' || in == 'u' || in == 'l';
}

bool isDEFAULTVALUE(char in)
{
	return in == 'D' || in == 'E' || in == 'F' || in == 'A' || in == 'U' || in == 'L' || in == 'T' || 
		in == 'd' || in == 'e' || in == 'f' || in == 'a' || in == 'u' || in == 'l' || in == 't';
}

//////////////////
// 'A'-'Z' 'a'-'z'
bool strGetWord(const char * pBuffer, char * outBuffer, int & leftIndex, short * outWordLen = 0)
{
	int i1 = 0, i2 = 0;
	bool result = false;
	leftIndex = 0;
	// ' ' '\t' '\n' '\r'
	while (isInvalidateChar(pBuffer[leftIndex]))
	{
		leftIndex++;
	}

	i1 = leftIndex;
	while (isValidateChar(pBuffer[i1]))
	{
		outBuffer[i2++] = pBuffer[i1++];
		result = true;
	}

	if (outWordLen != 0)
		*outWordLen = i2;

	outBuffer[i2] = '\0';
	return result;
}

//////////////////
// 'A'-'Z' 'a'-'z' '0'-'9' '_'
bool strGetWord2(const char * pBuffer, char * outBuffer, int & leftIndex, short * outWordLen = 0)
{
	int i1 = 0, i2 = 0;
	bool result = false;
	leftIndex = 0;
	// ' ' '\t' '\n'
	while (isInvalidateChar(pBuffer[leftIndex]))
	{
		leftIndex++;
	}

	i1 = leftIndex;
	//while (pBuffer[i1] != '\0' && pBuffer[i1] != ' ' && pBuffer[i1] != '\t' && outBuffer[i2] != '\0')
	while (isValidateChar2(pBuffer[i1]))
	{
		outBuffer[i2++] = pBuffer[i1++];
		result = true;
	}

	if (outWordLen != 0)
		*outWordLen = i2;

	outBuffer[i2] = '\0';
	return result;
}

//////////////////
// 'A'-'Z' 'a'-'z' '0'-'9' '_' ','
bool strGetWord3(const char * pBuffer, char * outBuffer, int & leftIndex, short * outWordLen = 0)
{
	int i1 = 0, i2 = 0;
	bool result = false;
	leftIndex = 0;
	// ' ' '\t' '\n'
	while (isInvalidateChar(pBuffer[leftIndex]))
	{
		leftIndex++;
	}

	i1 = leftIndex;
	while (isValidateChar3(pBuffer[i1]))
	{
		outBuffer[i2++] = pBuffer[i1++];
		result = true;
	}

	if (outWordLen != 0)
		*outWordLen = i2;

	outBuffer[i2] = '\0';
	return result;
}

//////////////////
// '0'-'9'
bool strGetWordNumber(const char * pBuffer, char * outBuffer, int & leftIndex, short * outWordLen = 0)
{
	int i1 = 0, i2 = 0;
	bool result = false;
	leftIndex = 0;
	// ' ' '\t' '\n'
	while (isInvalidateChar(pBuffer[leftIndex]))
	{
		leftIndex++;
	}

	i1 = leftIndex;
	while (isNumber(pBuffer[i1]))
	{
		outBuffer[i2++] = pBuffer[i1++];
		result = true;
	}

	if (outWordLen != 0)
		*outWordLen = i2;

	outBuffer[i2] = '\0';
	return result;
}

//////////////////
// 'xxx'
bool strGetValue(const char * pBuffer, char * outBuffer, int & leftIndex, enum_value_type & valuetype, short * outWordLen = 0)
{
	int i1 = 0, i2 = 0;
	bool result = false;
	leftIndex = 0;
	valuetype = VALUE_UNKNOWN;

	// ' ' '\t' '\n'
	while (isInvalidateChar(pBuffer[leftIndex]))
	{
		leftIndex++;
	}
	if (pBuffer[leftIndex] == '\'')
	{
		valuetype = VALUE_STRING;
		leftIndex++;
	}

	i1 = leftIndex;
	if (valuetype == VALUE_STRING)
	{
		while (pBuffer[i1] != '\0')
		{
			//if (i2 == MAX_VALUE_STRING_SIZE)
			//{
			//	return false;
			//}
			if (pBuffer[i1] == '\'')
			{
				if (pBuffer[i1+1] != '\0' && pBuffer[i1+1] == '\'')
				{
					outBuffer[i2++] = '\'';
					i1 += 2;
					leftIndex++;
					continue;
				}
				result = true;
				break;
			}
			outBuffer[i2++] = pBuffer[i1++];
		}
	}else
	{
		while (true)
		{
			if (isNumber(pBuffer[i1]))
			{
				if (pBuffer[i1] == '.')
				{
					if (valuetype == VALUE_LETTER)
					{
						break;
					}else if (valuetype == VALUE_REAL)
					{
						// Already find '.'
						result = false;
						break;
					}
					valuetype = VALUE_REAL;
				}
				if (valuetype != VALUE_REAL && valuetype != VALUE_INT)
					valuetype = VALUE_INT;
			}else if (isInvalidateChar2(pBuffer[i1]) || ',' == pBuffer[i1] || ')' == pBuffer[i1])
			{
				break;
			}else if (isValidateChar(pBuffer[i1]))
			{
				valuetype = VALUE_LETTER;
			}
			//}else if (isBOOLVALUE(pBuffer[i1]) || isNULLVALUE(pBuffer[i1]) || isDEFAULTVALUE(pBuffer[i1]))
			//{
			//	//valuetype = VALUE_UNKNOWN;
			//}else
			//{
			//	break;
			//}
			outBuffer[i2++] = pBuffer[i1++];
			result = true;

		}
		//while (isNumber(pBuffer[i1]) || isBOOLVALUE(pBuffer[i1]) || isNULLVALUE(pBuffer[i1]) || isDEFAULTVALUE(pBuffer[i1]))
		//{
		//	if (pBuffer[i1++] == '.')
		//	{
		//		if (valuetype == VALUE_REAL)
		//		{
		//			// Already find '.'
		//			result = false;
		//			break;
		//		}
		//		valuetype = VALUE_REAL;
		//	}

		//	outBuffer[i2++] = pBuffer[i1++];
		//	result = true;
		//}
	}

	if (outWordLen != 0)
		*outWordLen = i2;

	outBuffer[i2] = '\0';
	return result;
}

bool charCompare(char c1, char c2)
{
	if (c1 >= 'A' && c1 <= 'Z')
	{
		if (c2 >= 'A' && c2 <= 'Z')
		{
			return c1 == c2;
		}else if (c2 >= 'a' && c2 <= 'z')
		{
			return (c1-'A') == (c2-'a');
		}
		return false;
	}else if (c1 >= 'a' && c1 <= 'z')
	{
		if (c2 >= 'a' && c2 <= 'z')
		{
			return c1 == c2;
		}else if (c2 >= 'A' && c2 <= 'Z')
		{
			return (c1-'a') == (c2-'A');
		}
		return false;
	}

	return c1 == c2;
}

bool strCompare(const char * pBuffer, const char * pCompare, int & leftIndex)
{
	int i1 = 0, i2 = 0;
	leftIndex = 0;
	while (isInvalidateChar(pBuffer[leftIndex]))
	{
		leftIndex++;
	}

	i1 = leftIndex;
	while (pCompare[i2] != '\0')
	{
		//if (pCompare[i2++] != pBuffer[i1] || '\0' == pBuffer[i1])
		if (!charCompare(pCompare[i2++], pBuffer[i1]) || '\0' == pBuffer[i1])
		{
			return false;
		}
		i1++;
	}
	return true;
}

bool GetItemValue(const char * pBuffer, tagItemValue * outItemValue, int & offset)
{
	char * tempBuffer = new char[strlen(pBuffer)+1];
	enum_value_type valuetype = VALUE_UNKNOWN;
	int leftIndex = 0;
	short wordLen = 0;
	if (!strGetValue(pBuffer, tempBuffer, offset, valuetype, &wordLen))
	{
		delete[] tempBuffer;
		return false;
	}

	offset += wordLen;
	char * pTableName = 0;
	if (valuetype==VALUE_LETTER && strCompare(pBuffer+offset, ".", leftIndex))	// Table.field
	{
		offset += leftIndex+1;
		pTableName = tempBuffer;
		tempBuffer = new char[MAX_ITEM_STRING_SIZE];
		if (!strGetWord2(pBuffer+offset, tempBuffer, leftIndex, &wordLen))
		{
			delete[] pTableName;
			delete[] tempBuffer;
			return false;
		}
		offset += leftIndex+wordLen;
		outItemValue->table_name = pTableName;
		outItemValue->field_name = tempBuffer;
		return true;
	}

	if (valuetype == VALUE_STRING)
	{
		offset++;
		outItemValue->value_type = VALUE_STRING;
		outItemValue->u.value_string = tempBuffer;
	}else if (valuetype == VALUE_INT)
	{
		if (wordLen >= 10)
		{
			outItemValue->value_type = VALUE_BIGINT;
			outItemValue->u.value_bigint = bo_atoi64(tempBuffer);
		}else
		{
			outItemValue->value_type = VALUE_INT;
			outItemValue->u.value_int = atoi(tempBuffer);
		}
		delete[] tempBuffer;
	}else if (valuetype == VALUE_REAL)
	{
		outItemValue->value_type = VALUE_REAL;
		outItemValue->u.value_real = atof(tempBuffer);
		delete[] tempBuffer;
	}else if (wordLen > 0)
	{
		if (strCompare(tempBuffer, "NULL", leftIndex))
		{
			outItemValue->value_type = VALUE_NULL;
		}else if (strCompare(tempBuffer, "DEFAULT", leftIndex))
		{
			outItemValue->value_type = VALUE_DEFAULT;
		}else if (strCompare(tempBuffer, "FALSE", leftIndex))
		{
			outItemValue->value_type = VALUE_BOOL;
			outItemValue->u.value_bool = false;
		}else if (strCompare(tempBuffer, "TRUE", leftIndex))
		{
			outItemValue->value_type = VALUE_BOOL;
			outItemValue->u.value_bool = true;
		}else if (strCompare(tempBuffer, "CURRENT_TIMESTAMP", leftIndex))
		{
			outItemValue->value_type = VALUE_FUNC_CURRENT_TIMESTAMP;
		}else if (wordLen >= 10)
		{
			outItemValue->value_type = VALUE_BIGINT;
			outItemValue->u.value_bigint = bo_atoi64(tempBuffer);
			//outItemValue->u.value_real = atof(tempBuffer);
		}else
		{
			outItemValue->value_type = VALUE_INT;
			outItemValue->u.value_int = atoi(tempBuffer);
		}
		delete[] tempBuffer;
	}else
	{
		delete[] tempBuffer;
		return false;
	}

	return true;
}

void AddItem(tagSP * outSP, enum_item_types type, void * itemhandle, char* tableName = 0)
{
	if (outSP->items == 0)
	{
		outSP->items = new tagItems;
		memset(outSP->items, 0, sizeof(tagItems));

		outSP->items->items = new tagItem *[MAX_FIELD_ITEM];
	}
	outSP->items->itemcount++;
	outSP->items->items[outSP->items->itemcount-1] = new tagItem;
	memset(outSP->items->items[outSP->items->itemcount-1], 0, sizeof(tagItem));
	outSP->items->items[outSP->items->itemcount-1]->item_type = type;
	outSP->items->items[outSP->items->itemcount-1]->table_name = tableName;
	outSP->items->items[outSP->items->itemcount-1]->item_handle = itemhandle;
}

tagTable * AddTableItem(tagSP * outSP, char * tablename)
{
	if (outSP->tables == 0)
	{
		outSP->tables = new tagItems;
		memset(outSP->tables, 0, sizeof(tagItems));

		outSP->tables->items = new tagItem *[MAX_FIELD_ITEM];
	}
	outSP->tables->itemcount++;
	outSP->tables->items[outSP->tables->itemcount-1] = new tagItem;
	memset(outSP->tables->items[outSP->tables->itemcount-1], 0, sizeof(tagItem));
	outSP->tables->items[outSP->tables->itemcount-1]->item_type = TABLE_ITEM;
	tagTable * tableHandle = new tagTable;
	memset(tableHandle, 0, sizeof(tagTable));
	outSP->tables->items[outSP->tables->itemcount-1]->item_handle = tableHandle;
	tableHandle->table_name = tablename;
	return tableHandle;
}

tagValue * AddValueItem(tagSP * outSP, char * fieldname = 0)
{
	if (outSP->values == 0)
	{
		outSP->values = new tagItems;
		memset(outSP->values, 0, sizeof(tagItems));

		outSP->values->items = new tagItem *[MAX_FIELD_ITEM];
	}
	outSP->values->itemcount++;
	outSP->values->items[outSP->values->itemcount-1] = new tagItem;
	memset(outSP->values->items[outSP->values->itemcount-1], 0, sizeof(tagItem));
	outSP->values->items[outSP->values->itemcount-1]->item_type = VALUE_ITEM;
	tagValue * valueHandle = new tagValue;
	memset(valueHandle, 0, sizeof(tagValue));
	outSP->values->items[outSP->values->itemcount-1]->item_handle = valueHandle;
	valueHandle->field_name = fieldname;

	return valueHandle;
}

void AddStringItem(tagItems * items, char * stringItem, char* tableName = 0)
{
	items->itemcount++;
	items->items[items->itemcount-1] = new tagItem;
	memset(items->items[items->itemcount-1], 0, sizeof(tagItem));
	items->items[items->itemcount-1]->item_type = STRING_ITEM;
	items->items[items->itemcount-1]->table_name = tableName;
	items->items[items->itemcount-1]->item_handle = stringItem;
}
void AddFuncItem(tagFunc * outFunc, char * stringItem)
{
	if (outFunc->items == 0)
	{
		outFunc->items = new tagItems;
		memset(outFunc->items, 0, sizeof(tagItems));

		outFunc->items->items = new tagItem *[MAX_FIELD_ITEM];
	}
	AddStringItem(outFunc->items, stringItem);
	//outFunc->items->itemcount++;
	//outFunc->items->items[outFunc->items->itemcount-1] = new tagItem;
	//memset(outFunc->items->items[outFunc->items->itemcount-1], 0, sizeof(tagItem));
	//outFunc->items->items[outFunc->items->itemcount-1]->item_type = STRING_ITEM;
	//outFunc->items->items[outFunc->items->itemcount-1]->item_handle = stringItem;
}

bool GetItemWheres(const char * pBuffer, tagSP * outSP, int & outLeftIndex, bool & outOrder)
{
	int leftoffset = 0;
	int leftIndex = 0;
	short wordLen = 0;
	char * tempBuffer;

	outSP->wheres = new tagItems;
	memset(outSP->wheres, 0, sizeof(tagItems));
	// <column_name>,
	outSP->wheres->items = new tagItem *[MAX_FIELD_ITEM];
	bool prevAndWhere = true;
	short nWhereLevel=0;
	char * pTableName = NULL;
	while (true)
	{
		// <(>
		// <)>
		if (strCompare(pBuffer+leftoffset, "(", leftIndex))
		{
			nWhereLevel++;
			leftoffset += leftIndex+1;
			continue;
		//}else if (strCompare(pBuffer+leftoffset, ")", leftIndex))
		//{
		//	nWhereLevel--;
		//	leftoffset += leftIndex+1;
		//	continue;
		}

		// <column_name>
		tempBuffer = new char[MAX_ITEM_STRING_SIZE];
		if (!strGetWord2(pBuffer+leftoffset, tempBuffer, leftIndex, &wordLen))
		{
			delete[] tempBuffer;
			return false;
		}
		leftoffset += leftIndex+wordLen;

		if (strCompare(pBuffer+leftoffset, ".", leftIndex))	// Table.field
		{
			leftoffset += leftIndex+1;
			pTableName = tempBuffer;
			tempBuffer = new char[MAX_ITEM_STRING_SIZE];
			if (!strGetWord2(pBuffer+leftoffset, tempBuffer, leftIndex, &wordLen))
			{
				delete[] pTableName;
				delete[] tempBuffer;
				return false;
			}
			leftoffset += leftIndex+wordLen;
		}

		outSP->wheres->itemcount++;
		outSP->wheres->items[outSP->wheres->itemcount-1] = new tagItem;
		memset(outSP->wheres->items[outSP->wheres->itemcount-1], 0, sizeof(tagItem));
		outSP->wheres->items[outSP->wheres->itemcount-1]->item_type = WHERE_ITEM;
		tagWhere * whereHandle = new tagWhere;
		memset(whereHandle, 0, sizeof(tagWhere));
		outSP->wheres->items[outSP->wheres->itemcount-1]->item_handle = whereHandle;
		whereHandle->table_name = pTableName;
		whereHandle->field_name = tempBuffer;
		whereHandle->and_where = prevAndWhere;
		whereHandle->where_level = nWhereLevel;
		pTableName = 0;

		// IS_NOTNULL - IS_NULL
		// IS NULL - IS NOT NULL
		// BETWEEN a AND b
		// IN (a,b,c)
		// NOT LIKE - LIKE
		// = (SELECT ...)
		// IN (SELECT ...)
		// =[.] ALL[ANY] (SELECT...)

		if (strCompare(pBuffer+leftoffset, "IS_NOTNULL", leftIndex))
		{
			leftoffset += leftIndex+10;
			whereHandle->compare_type = COMPARE_ISNOTNULL;
		}else if (strCompare(pBuffer+leftoffset, "IS_NULL", leftIndex))
		{
			leftoffset += leftIndex+7;
			whereHandle->compare_type = COMPARE_ISNULL;
		}else if (strCompare(pBuffer+leftoffset, "LIKE", leftIndex))
		{
			leftoffset += leftIndex+4;
			whereHandle->compare_type = COMPARE_LIKE;
		}else if (strCompare(pBuffer+leftoffset, "BETWEEN", leftIndex))
		{
			leftoffset += leftIndex+7;
			whereHandle->compare_type = COMPARE_BETWEENAND;
			// ???
		}else if (strCompare(pBuffer+leftoffset, "=", leftIndex))
		{
			leftoffset += leftIndex+1;
			whereHandle->compare_type = COMPARE_EQUAL;
		}else if (strCompare(pBuffer+leftoffset, "!=", leftIndex) || strCompare(pBuffer+leftoffset, "<>", leftIndex))
		{
			leftoffset += leftIndex+2;
			whereHandle->compare_type = COMPARE_UNEQUAL;
		}else if (strCompare(pBuffer+leftoffset, ">=", leftIndex))
		{
			leftoffset += leftIndex+2;
			whereHandle->compare_type = COMPARE_GREATEREQUAL;
		}else if (strCompare(pBuffer+leftoffset, ">", leftIndex))
		{
			leftoffset += leftIndex+1;
			whereHandle->compare_type = COMPARE_GREATER;
		}else if (strCompare(pBuffer+leftoffset, "<=", leftIndex))
		{
			leftoffset += leftIndex+2;
			whereHandle->compare_type = COMPARE_LESSEQUAL;
		}else if (strCompare(pBuffer+leftoffset, "<", leftIndex))
		{
			leftoffset += leftIndex+1;
			whereHandle->compare_type = COMPARE_LESS;
		}else
		{
			return false;
		}

		switch (whereHandle->compare_type)
		{
		case COMPARE_EQUAL:
		case COMPARE_UNEQUAL:
		case COMPARE_GREATER:
		case COMPARE_GREATEREQUAL:
		case COMPARE_LESS:
		case COMPARE_LESSEQUAL:
		case COMPARE_LIKE:
			{
				// <column_value>
				tagItemValue * itemValue = new tagItemValue;
				memset(itemValue, 0, sizeof(tagItemValue));
				whereHandle->value_handle = itemValue;
				if (!GetItemValue(pBuffer+leftoffset, itemValue, leftIndex))
				{
					return false;
				}
				leftoffset += leftIndex;
			}
		default:
			break;
		}

		if (nWhereLevel>0&& strCompare(pBuffer+leftoffset, ")", leftIndex))
		{
			nWhereLevel--;
			leftoffset += leftIndex+1;
		}

		if (strCompare(pBuffer+leftoffset, "ORDER", leftIndex))
		{
			outOrder = true;
			leftoffset += leftIndex+6;
			outLeftIndex = leftoffset;
			return true;
		}else if (strCompare(pBuffer+leftoffset, "AND", leftIndex))
		{
			prevAndWhere = true;
			leftoffset += leftIndex+4;
			continue;
		}else if (strCompare(pBuffer+leftoffset, "OR", leftIndex))
		{
			prevAndWhere = false;
			leftoffset += leftIndex+3;
			continue;
		}else
		{
			char * tempBuffer2 = new char[MAX_ITEM_STRING_SIZE];
			if (!strGetWord2(pBuffer+leftoffset, tempBuffer2, leftIndex))
			{
				delete[] tempBuffer2;
				break;
			}else
			{
				delete[] tempBuffer2;
				return false;
			}
		}
	}
	outLeftIndex = leftoffset;
	return true;
}
bool GetOrderBys(const char * pBuffer, tagSP * outSP, int & outLeftIndex)
{
	int leftoffset = 0;
	int leftIndex = 0;
	short wordLen = 0;
	char * tempBuffer;

	// Get order by
	if (strCompare(pBuffer+leftoffset, "BY", leftIndex))
	{
		leftoffset += leftIndex+3;
		outSP->orderbys = new tagItems;
		memset(outSP->orderbys, 0, sizeof(tagItems));
		// <column_name>,
		outSP->orderbys->items = new tagItem *[MAX_FIELD_ITEM];
		outSP->orderbydesc = 0;
		char * pTableName = 0;
		while (true)
		{
			// <column_name>
			tempBuffer = new char[MAX_ITEM_STRING_SIZE];
			if (!strGetWord2(pBuffer+leftoffset, tempBuffer, leftIndex, &wordLen))
			{
				delete[] tempBuffer;
				break;
			}
			leftoffset += leftIndex+wordLen;

			if (strCompare(pBuffer+leftoffset, ".", leftIndex))	// Table.field
			{
				leftoffset += leftIndex+1;
				pTableName = tempBuffer;
				tempBuffer = new char[MAX_ITEM_STRING_SIZE];
				if (!strGetWord2(pBuffer+leftoffset, tempBuffer, leftIndex, &wordLen))
				{
					delete[] pTableName;
					delete[] tempBuffer;
					break;
				}
				leftoffset += leftIndex+wordLen;
			}

			AddStringItem(outSP->orderbys, tempBuffer, pTableName);
			pTableName = 0;
			if (strCompare(pBuffer+leftoffset, "DESC", leftIndex))
			{
				outSP->orderbydesc = 1;
				leftoffset += leftIndex+5;
				break;
			}else if (strCompare(pBuffer+leftoffset, "ASC", leftIndex))
			{
				leftoffset += leftIndex+4;
				break;
			}else if (strCompare(pBuffer+leftoffset, "LIMIT", leftIndex))
			{
				leftoffset += leftIndex;
				break;
			}else if (strCompare(pBuffer+leftoffset, "OFFSET", leftIndex))
			{
				leftoffset += leftIndex;
				break;
			}
		}
	}
	outLeftIndex = leftoffset;
	return true;
}
bool GetOffsetLimit(const char * pBuffer, tagSP * outSP)
{
	int leftoffset = 0;
	int leftIndex = 0;
	short wordLen = 0;
	bool bResult = true;
	char * tempBuffer;

	tempBuffer = new char[MAX_ITEM_STRING_SIZE];
	bool bFindLimit = false;
	bool bFindOffset = false;
	while (true)
	{
		if (!strGetWord(pBuffer+leftoffset, tempBuffer, leftIndex, &wordLen))
		{
			break;
		}
		leftoffset += leftIndex+wordLen+1;

		if (strCompare(tempBuffer, "OFFSET", leftIndex))
		{
			char * tempBuffer2 = new char[MAX_ITEM_STRING_SIZE];
			if (strGetWordNumber(pBuffer+leftoffset, tempBuffer2, leftIndex, &wordLen))
			{
				leftoffset += leftIndex+wordLen;
				outSP->offset = bo_atoi64(tempBuffer2);
				delete[] tempBuffer2;
			}else
			{
				delete[] tempBuffer2;
				bResult = false;
				break;
			}
			bFindOffset = true;
			if (bFindLimit)
				break;
		}else if (strCompare(tempBuffer, "LIMIT", leftIndex))
		{
			char * tempBuffer2 = new char[MAX_ITEM_STRING_SIZE];
			if (strGetWordNumber(pBuffer+leftoffset, tempBuffer2, leftIndex, &wordLen))
			{
				leftoffset += leftIndex+wordLen;
				outSP->limit = bo_atoi64(tempBuffer2);
				delete[] tempBuffer2;
			}else
			{
				delete[] tempBuffer2;
				bResult = false;
				break;
			}
			bFindLimit = true;
			if (bFindOffset)
				break;
		}
	}
	delete[] tempBuffer;
	return bResult;
}

tagField * GetFieldItem(const char * sql, int & offset)
{
	int leftIndex = 0;
	short wordLen = 0;
	char * tempBuffer = 0;
	tagField * fieldHandle = 0;

	offset = 0;
	// <column_name> <datatype> [<size1>[,<size2>]] [<column_constraint>,...]

	// <column_name>
	tempBuffer = new char[MAX_ITEM_STRING_SIZE];
	if (!strGetWord2(sql+offset, tempBuffer, leftIndex, &wordLen))
	{
		delete[] tempBuffer;
		return 0;
	}
	offset += leftIndex+wordLen+1;

	fieldHandle = new tagField;
	memset(fieldHandle, 0, sizeof(tagField));
	fieldHandle->field_name = tempBuffer;

	// <datatype>
	if (strCompare(sql+offset, "BOOLEAN", leftIndex))
	{
		offset += leftIndex+7;
		fieldHandle->field_type = SQL_TYPE_BOOLEAN;
	}else if (strCompare(sql+offset, "TINYINT", leftIndex))
	{
		offset += leftIndex+7;
		fieldHandle->field_type = SQL_TYPE_TINYINT;
	}else if (strCompare(sql+offset, "SMALLINT", leftIndex))
	{
		offset += leftIndex+8;
		fieldHandle->field_type = SQL_TYPE_SMALLINT;
	}else if (strCompare(sql+offset, "INTEGER", leftIndex))
	{
		offset += leftIndex+7;
		fieldHandle->field_type = SQL_TYPE_INTEGER;
	}else if (strCompare(sql+offset, "INT", leftIndex))
	{
		offset += leftIndex+3;
		fieldHandle->field_type = SQL_TYPE_INTEGER;
	}else if (strCompare(sql+offset, "BIGINT", leftIndex))
	{
		offset += leftIndex+6;
		fieldHandle->field_type = SQL_TYPE_BIGINT;
	}else if (strCompare(sql+offset, "NUMERIC", leftIndex))
	{
		offset += leftIndex+7;
		fieldHandle->field_type = SQL_TYPE_NUMERIC;
	}else if (strCompare(sql+offset, "DECIMAL", leftIndex))
	{
		offset += leftIndex+7;
		fieldHandle->field_type = SQL_TYPE_DECIMAL;
	}else if(strCompare(sql+offset, "DEC", leftIndex))
	{
		offset += leftIndex+3;
		fieldHandle->field_type = SQL_TYPE_DECIMAL;
	}else if (strCompare(sql+offset, "FLOAT", leftIndex))
	{
		offset += leftIndex+5;
		fieldHandle->field_type = SQL_TYPE_FLOAT;
	}else if (strCompare(sql+offset, "REAL", leftIndex))
	{
		offset += leftIndex+4;
		fieldHandle->field_type = SQL_TYPE_REAL;
	}else if (strCompare(sql+offset, "DOUBLE", leftIndex))
	{
		offset += leftIndex+6;
		fieldHandle->field_type = SQL_TYPE_DOUBLE;
	}else if (strCompare(sql+offset, "DATE", leftIndex))
	{
		offset += leftIndex+4;
		fieldHandle->field_type = SQL_TYPE_DATE;
	}else if (strCompare(sql+offset, "TIMESTAMP", leftIndex))
	{
		offset += leftIndex+9;
		fieldHandle->field_type = SQL_TYPE_TIMESTAMP;
	}else if (strCompare(sql+offset, "TIME", leftIndex))
	{
		offset += leftIndex+4;
		fieldHandle->field_type = SQL_TYPE_TIME;
	}else if (strCompare(sql+offset, "CHARACTER", leftIndex))
	{
		offset += leftIndex+9;
		fieldHandle->field_type = SQL_TYPE_CHAR;
	}else if (strCompare(sql+offset, "CHAR", leftIndex))
	{
		offset += leftIndex+4;
		fieldHandle->field_type = SQL_TYPE_CHAR;
	}else if (strCompare(sql+offset, "VARCHAR", leftIndex))
	{
		offset += leftIndex+7;
		fieldHandle->field_type = SQL_TYPE_VARCHAR;
	}else if (strCompare(sql+offset, "CLOB", leftIndex))
	{
		offset += leftIndex+4;
		fieldHandle->field_type = SQL_TYPE_CLOB;
	}else if (strCompare(sql+offset, "NCHARACTER", leftIndex))
	{
		offset += leftIndex+10;
		fieldHandle->field_type = SQL_TYPE_NCHAR;
	}else if (strCompare(sql+offset, "NCHAR", leftIndex))
	{
		offset += leftIndex+5;
		fieldHandle->field_type = SQL_TYPE_NCHAR;
	}else if (strCompare(sql+offset, "NVARCHAR", leftIndex))
	{
		offset += leftIndex+8;
		fieldHandle->field_type = SQL_TYPE_NVARCHAR;
	}else if (strCompare(sql+offset, "NCLOB", leftIndex))
	{
		offset += leftIndex+5;
		fieldHandle->field_type = SQL_TYPE_NCLOB;
	}else if (strCompare(sql+offset, "BINARY", leftIndex))
	{
		offset += leftIndex+6;
		fieldHandle->field_type = SQL_TYPE_BINARY;
	}else if (strCompare(sql+offset, "VARBINARY", leftIndex))
	{
		offset += leftIndex+9;
		fieldHandle->field_type = SQL_TYPE_VARBINARY;
	}else if (strCompare(sql+offset, "BLOB", leftIndex))
	{
		offset += leftIndex+4;
		fieldHandle->field_type = SQL_TYPE_BLOB;
	}else
	{
		delete fieldHandle;
		return 0;
	}

	if (strCompare(sql+offset, "(", leftIndex))
	{
		offset += leftIndex+1;
		// <size1>[,<size2>]

		tempBuffer = new char[MAX_ITEM_STRING_SIZE];
		if (!strGetWordNumber(sql+offset, tempBuffer, leftIndex, &wordLen))
		{
			delete[] tempBuffer;
			delete fieldHandle;
			return 0;
		}
		offset += leftIndex+wordLen;

		fieldHandle->size1 = atoi(tempBuffer);
		if (strCompare(sql+offset, ",", leftIndex))
		{
			offset += leftIndex+1;

			if (!strGetWordNumber(sql+offset, tempBuffer, leftIndex, &wordLen))
			{
				delete[] tempBuffer;
				delete fieldHandle;
				return 0;
			}
			offset += leftIndex+wordLen;
			fieldHandle->size2 = atoi(tempBuffer);

			if (fieldHandle->size2 > fieldHandle->size1)
			{
				delete[] tempBuffer;
				return false;
			}
			if (!strCompare(sql+offset, ")", leftIndex))
			{
				delete[] tempBuffer;
				delete fieldHandle;
				return 0;
			}
			offset += leftIndex+1;
		}else if (!strCompare(sql+offset, ")", leftIndex))
		{
			delete[] tempBuffer;
			delete fieldHandle;
			return 0;
		}else
		{
			// ??
			offset += leftIndex+1;
		}
		delete[] tempBuffer;
	}

	// CONSTRAINT
	while (true)
	{
		/*
		if (strCompare(sql+offset, ",", leftIndex))
		{
			offset += leftIndex+1;
			if (createTable)
				continue;
			else
				return true;
		}else if (strCompare(sql+offset, ")", leftIndex))
		{
			offset += leftIndex+1;
			if (createTable)
				return true;
			else
				return false;
		}
		*/

		// <column_constraint>,...
		if (strCompare(sql+offset, "CONSTRAINT", leftIndex))
		{
			// constraint_name
			tempBuffer = new char[MAX_ITEM_STRING_SIZE];
			if (!strGetWord2(sql+offset, tempBuffer, leftIndex, &wordLen))
			{
				delete[] tempBuffer;
				return false;
			}
			offset += leftIndex+wordLen+1;
			fieldHandle->constraint_name = tempBuffer;

			/*
			tempBuffer = new char[MAX_ITEM_STRING_SIZE];
			if (!strGetWord2(sql+offset, tempBuffer, leftIndex, &wordLen))
			{
			delete[] tempBuffer;
			return false;
			}
			offset += leftIndex+wordLen;
			*/
		}

		if (strCompare(sql+offset, "UNSIGNED", leftIndex))
		{
			offset += leftIndex+8;
			fieldHandle->unsigned_field = true;
		}else if (strCompare(sql+offset, "NOT", leftIndex))
		{
			offset += leftIndex+4;
			// NOT NULL
			if (strCompare(sql+offset, "NULL", leftIndex))
			{
				offset += leftIndex+4;
				fieldHandle->constraint = SQL_CONSTRAINT_NOTNULL;
			}else
			{
				return false;
			}
		}else if (strCompare(sql+offset, "UNIQUE", leftIndex))
		{
			offset += leftIndex+6;
			fieldHandle->constraint = SQL_CONSTRAINT_UNIQUE;
		}else if (strCompare(sql+offset, "PRIMARY", leftIndex))
		{
			offset += leftIndex+8;
			// PRIMARY KEY
			if (strCompare(sql+offset, "KEY", leftIndex))
			{
				offset += leftIndex+3;
				fieldHandle->constraint = SQL_CONSTRAINT_PRIMARYKEY;
			}else
			{
				return false;
			}
		}else if (strCompare(sql+offset, "REFERENCES", leftIndex))
		{
			offset += leftIndex+10;
			// ???
			fieldHandle->references = true;
		}else if (strCompare(sql+offset, "CHECK", leftIndex))
		{
			offset += leftIndex+5;
			// ???

		}else if (strCompare(sql+offset, "DEFAULT", leftIndex))
		{
			offset += leftIndex+7;
			fieldHandle->default_value = new tagItemValue;
			memset(fieldHandle->default_value, 0, sizeof(tagItemValue));
			if (!GetItemValue(sql+offset, fieldHandle->default_value, leftIndex))
			{
				return false;
			}
			offset += leftIndex;
		}else if (strCompare(sql+offset, "AUTO_INCREMENT", leftIndex))
		{
			// AUTO_INCREMENT
			offset += leftIndex+14;
			fieldHandle->auto_increment = true;			
		}else
		{
			//delete fieldHandle;
			//return 0;
			break;
		}
	}
	return fieldHandle;
}

bool GetFieldItems(const char * sql, tagSP * outSP, int & offset)
{
	int leftIndex = 0;
	//short wordLen = 0;
	//char * tempBuffer = 0;

	offset = 0;
	outSP->fields = new tagItems;
	memset(outSP->fields, 0, sizeof(tagItems));
	outSP->fields->items = new tagItem *[MAX_FIELD_ITEM];

	while (true)
	{
		tagField * fieldHandle = GetFieldItem(sql+offset, leftIndex);
		offset += leftIndex;

		if (fieldHandle == 0)
		{
			return false;
		}

		outSP->fields->itemcount++;
		outSP->fields->items[outSP->fields->itemcount-1] = new tagItem;
		memset(outSP->fields->items[outSP->fields->itemcount-1], 0, sizeof(tagItem));
		outSP->fields->items[outSP->fields->itemcount-1]->item_type = FIELD_ITEM;
		outSP->fields->items[outSP->fields->itemcount-1]->item_handle = fieldHandle;

		if (strCompare(sql+offset, ",", leftIndex))
		{
			offset += leftIndex+1;
			continue;
		//}else if (strCompare(sql+offset, "CONSTRAINT", leftIndex))
		//{
		//	// CONSTRAINT ebm_group_version_t_PK PRIMARY KEY(group_id)
		//	offset += leftIndex+10;
		}else if (strCompare(sql+offset, ")", leftIndex))
		{
			offset += leftIndex+1;
			return true;
		}else
		{
			return false;
		}

	}
	return true;
}

enum_funciton_type IsFunctionString(const char * lpszString)
{
	if (strcmp(lpszString, "DATE_FORMAT") == 0)
		return FUNCTION_DATE_FORMAT;
	else if (strcmp(lpszString, "WDAY") == 0)
		return FUNCTION_WDAY;
	else if (strcmp(lpszString, "MDAY") == 0)
		return FUNCTION_MDAY;
	else if (strcmp(lpszString, "YDAY") == 0)
		return FUNCTION_YDAY;
	else if (strcmp(lpszString, "YEAR") == 0)
		return FUNCTION_YEAR;
	else if (strcmp(lpszString, "MONTH") == 0)
		return FUNCTION_MONTH;
	else if (strcmp(lpszString, "HOUR") == 0)
		return FUNCTION_HOUR;
	else if (strcmp(lpszString, "MINUTE") == 0)
		return FUNCTION_MINUTE;
	else if (strcmp(lpszString, "SECOND") == 0)
		return FUNCTION_SECOND;
	else if (strcmp(lpszString, "TO_INT") == 0)
		return FUNCTION_TO_INT;

	return FUNCTION_UNKNOWN;
}

tagFunc * GetFuncItem(enum_funciton_type functionType, const char * sql, int & leftoffset)
{
	tagFunc * pFunc = new tagFunc;
	memset(pFunc, 0, sizeof(tagFunc));
	pFunc->function_type = functionType;

	leftoffset = 0;
	int leftIndex = 0;
	if (strCompare(sql+leftoffset, "(", leftIndex))
	{
		leftoffset += leftIndex+1;

		while (true)
		{
			char * tempBuffer = new char[MAX_ITEM_STRING_SIZE];
			short wordLen = 0;
			//if (!strGetWord2(sql+leftoffset, tempBuffer, leftIndex, &wordLen))
			enum_value_type valueType;
			if (!strGetValue(sql+leftoffset, tempBuffer, leftIndex, valueType, &wordLen))
			{
				delete[] tempBuffer;
				free_items(pFunc->items);
				delete pFunc;
				return NULL;
			}
			leftoffset += leftIndex+wordLen;

			AddFuncItem(pFunc, tempBuffer);
			if (valueType == VALUE_STRING && sql[leftoffset] == '\'')
				leftoffset += 1;

			if (strCompare(sql+leftoffset, ",", leftIndex))
			{
				leftoffset += leftIndex+1;
				continue;
			}else if (strCompare(sql+leftoffset, ")", leftIndex))
			{
				leftoffset += leftIndex+1;
				break;
			}else
			{
				delete[] tempBuffer;
				free_items(pFunc->items);
				delete pFunc;
				return 0;
			}
		}
	}

	return pFunc;
}

tagSP * parse_exec(const char * sql)
{
	//BOOST_ASSERT (sql != 0);

	tagSP * result = 0;
	int leftoffset = 0;
	int leftIndex = 0;
	short wordLen = 0;

	if (strCompare(sql, "SELECT", leftIndex))
	{
		leftoffset = leftIndex+7;
		// SELECT
		result = new tagSP;
		memset(result, 0, sizeof(tagSP));
		result->sql_command = SQLCOM_SELECT;
	}else if (strCompare(sql, "CREATE", leftIndex))
	{
		leftoffset = leftIndex+7;

		char * tempBuffer = new char[MAX_ITEM_STRING_SIZE];
		if (!strGetWord2(sql+leftoffset, tempBuffer, leftIndex, &wordLen))
		{
			delete[] tempBuffer;
			return result;
		}
		leftoffset += leftIndex+wordLen+1;

		// [{GLOBAL | LOCAL} TEMPORARY]
		bool bGlobalTemporaryTable = false;
		bool bLocalTemporaryTable = false;
		if (strCompare(tempBuffer, "GLOBAL", leftIndex))
		{
			bGlobalTemporaryTable = true;
		}else if (strCompare(tempBuffer, "LOCAL", leftIndex))
		{
			bLocalTemporaryTable = true;
		}

		if (bGlobalTemporaryTable || bLocalTemporaryTable)
		{
			// CREATE [{GLOBAL | LOCAL} TEMPORARY]
			if (!strGetWord(sql+leftoffset, tempBuffer, leftIndex, &wordLen))
			{
				delete[] tempBuffer;
				return result;
			}
			leftoffset += leftIndex+wordLen+1;

			if (!strCompare(tempBuffer, "TEMPORARY", leftIndex))
			{
				delete[] tempBuffer;
				return result;
			}

			if (!strGetWord(sql+leftoffset, tempBuffer, leftIndex, &wordLen))
			{
				delete[] tempBuffer;
				return result;
			}
			leftoffset += leftIndex+wordLen+1;

			if (!strCompare(tempBuffer, "TABLE", leftIndex))
			{
				delete[] tempBuffer;
				return result;
			}

			result = new tagSP;
			memset(result, 0, sizeof(tagSP));
			if (bGlobalTemporaryTable)
				result->sql_command = SQLCOM_CREATE_GTTABLE;
			else
				result->sql_command = SQLCOM_CREATE_LTTABLE;
		}else if (strCompare(tempBuffer, "DATABASE", leftIndex))
		{
			// CREATE DATABASE
			result = new tagSP;
			memset(result, 0, sizeof(tagSP));
			result->sql_command = SQLCOM_CREATE_DATABASE;
		}else if (strCompare(tempBuffer, "TABLE", leftIndex))
		{
			// CREATE TABLE
			result = new tagSP;
			memset(result, 0, sizeof(tagSP));
			result->sql_command = SQLCOM_CREATE_TABLE;
		}else if (strCompare(tempBuffer, "VIEW", leftIndex))
		{
			// CREATE VIEW
			result = new tagSP;
			memset(result, 0, sizeof(tagSP));
			result->sql_command = SQLCOM_CREATE_VIEW;
		}else if (strCompare(tempBuffer, "INDEX", leftIndex))
		{
			// CREATE INDEX
			result = new tagSP;
			memset(result, 0, sizeof(tagSP));
			result->sql_command = SQLCOM_CREATE_INDEX;
		}

		delete[] tempBuffer;
	}else if (strCompare(sql, "USE", leftIndex))
	{
		leftoffset = leftIndex+4;
		// USE DATABASE
		result = new tagSP;
		memset(result, 0, sizeof(tagSP));
		result->sql_command = SQLCOM_USE_DATABASE;

	}else if (strCompare(sql, "UPDATE", leftIndex))
	{
		leftoffset = leftIndex+7;
		// UPDATE
		result = new tagSP;
		memset(result, 0, sizeof(tagSP));
		result->sql_command = SQLCOM_UPDATE;
	}else if (strCompare(sql, "INSERT", leftIndex))
	{
		leftoffset = leftIndex+7;
		// INSERT
		result = new tagSP;
		memset(result, 0, sizeof(tagSP));
		result->sql_command = SQLCOM_INSERT;
	}else if (strCompare(sql, "DELETE", leftIndex))
	{
		leftoffset = leftIndex+7;
		// DELETE
		result = new tagSP;
		memset(result, 0, sizeof(tagSP));
		result->sql_command = SQLCOM_DELETE;
	}else if (strCompare(sql, "ALTER", leftIndex))
	{
		leftoffset = leftIndex+6;
		// ALTER
		if (strCompare(sql+leftoffset, "DATABASE", leftIndex))
		{
			leftoffset += leftIndex+9;
			// ALTER DATABASE
			result = new tagSP;
			memset(result, 0, sizeof(tagSP));
			result->sql_command = SQLCOM_ALTER_DATABASE;
		}else if (strCompare(sql+leftoffset, "TABLE", leftIndex))
		{
			leftoffset += leftIndex+6;
			// ALTER TABLE
			result = new tagSP;
			memset(result, 0, sizeof(tagSP));
			result->sql_command = SQLCOM_ALTER_TABLE;
		}

	}else if (strCompare(sql, "DROP", leftIndex))
	{
		leftoffset = leftIndex+5;

		if (strCompare(sql+leftoffset, "DATABASE", leftIndex))
		{
			leftoffset += leftIndex+9;
			// DROP DATABASE
			result = new tagSP;
			memset(result, 0, sizeof(tagSP));
			result->sql_command = SQLCOM_DROP_DATABASE;
		}else if (strCompare(sql+leftoffset, "TABLE", leftIndex))
		{
			leftoffset += leftIndex+6;
			// DROP TABLE
			result = new tagSP;
			memset(result, 0, sizeof(tagSP));
			result->sql_command = SQLCOM_DROP_TABLE;
		}/*else if (strCompare(tempBuffer, "VIEW", leftIndex))
		{
			// CREATE VIEW
			result = new tagSP;
			memset(result, 0, sizeof(tagSP));
			result->sql_command = SQLCOM_CREATE_VIEW;
		}else if (strCompare(tempBuffer, "INDEX", leftIndex))
		{
			// CREATE INDEX
			result = new tagSP;
			memset(result, 0, sizeof(tagSP));
			result->sql_command = SQLCOM_CREATE_INDEX;
		}*/
	}

	if (result == 0)
		return result;
	//// default
	//result->limit = -1;
	//result->offset = -1;

	char * tempBuffer = new char[MAX_ITEM_STRING_SIZE];
	if (result->sql_command != SQLCOM_SELECT)
	{
		if (!strGetWord2(sql+leftoffset, tempBuffer, leftIndex, &wordLen))
		{
			delete[] tempBuffer;
			parse_free(result);
			result = 0;
			return result;
		}
		leftoffset += leftIndex+wordLen;
	}

	switch (result->sql_command)
	{
	case SQLCOM_SELECT:
		{
			bool bGetFromKeyword = false;
			// <SELECT> ...
			if (strCompare(sql+leftoffset, "DISTINCT", leftIndex))
			{
				leftoffset += leftIndex+9;
				result->parameter = (void*)1;
			}
			if (strCompare(sql+leftoffset, "*", leftIndex))
			{
				leftoffset += 2;
				// ???

			}else
			{
				char * pTableName = 0;
				while (true)
				{
					if (!strGetWord2(sql+leftoffset, tempBuffer, leftIndex, &wordLen))
					{
						delete[] tempBuffer;
						parse_free(result);
						result = 0;
						return result;
					}
					leftoffset += leftIndex+wordLen;
					if (strCompare(sql+leftoffset, ".", leftIndex))	// Table.field
					{
						leftoffset += leftIndex+1;
						pTableName = tempBuffer;
						tempBuffer = new char[MAX_ITEM_STRING_SIZE];
						if (!strGetWord2(sql+leftoffset, tempBuffer, leftIndex, &wordLen))
						{
							delete[] pTableName;
							delete[] tempBuffer;
							parse_free(result);
							result = 0;
							return result;
						}
						leftoffset += leftIndex+wordLen;
					}

					enum_funciton_type functionType = IsFunctionString(tempBuffer);
					if (functionType != FUNCTION_UNKNOWN)
					{
						// DATE_FORMAT('1997-10-04 22:23:00', '%W %M %Y')
						tagFunc * pFunc = GetFuncItem(functionType, sql+leftoffset, leftIndex);
						if (pFunc == NULL)
						{
							parse_free(result);
							result = 0;
							return result;
						}
						leftoffset += leftIndex;
						AddItem(result, FUNC_ITEM, pFunc, pTableName);
						pTableName = 0;
						memset(tempBuffer,0,MAX_ITEM_STRING_SIZE);
					}else
					{
						AddItem(result, STRING_ITEM, tempBuffer, pTableName);
						pTableName = 0;
						tempBuffer = new char[MAX_ITEM_STRING_SIZE];
					}

					if (strCompare(sql+leftoffset, ",", leftIndex))
					{
						leftoffset += leftIndex+1;
						continue;
					//}else if (strCompare(sql+leftoffset, "FROM", leftIndex) && (sql[leftoffset+leftIndex+4]==' '||sql[leftoffset+leftIndex+4]=='\t'))
					}else if (strCompare(sql+leftoffset, "FROM", leftIndex))
					{
						leftoffset += leftIndex+5;
						bGetFromKeyword = true;
						break;
					}
				}

			}

			// <FROM> ...
			if (!bGetFromKeyword)
			{
				if (!strCompare(sql+leftoffset, "FROM", leftIndex))
				{
					parse_free(result);
					result = 0;
					return result;
				}
				leftoffset += leftIndex+5;
			}

			bool bGetWhereKeyword = false;
			bool bGetOrderKeyword = false;
			while (true)
			{
				// FROM <table_name>
				if (!strGetWord2(sql+leftoffset, tempBuffer, leftIndex, &wordLen))
				{
					delete[] tempBuffer;
					parse_free(result);
					result = 0;
					return result;
				}
				leftoffset += leftIndex+wordLen;

				// <table_name>
				tagTable * tableHandle = AddTableItem(result, tempBuffer);
				tempBuffer = new char[MAX_ITEM_STRING_SIZE];

				if (strCompare(sql+leftoffset, ",", leftIndex))
				{
					leftoffset += leftIndex+1;
					continue;
				}else if (strCompare(sql+leftoffset, "LIMIT", leftIndex))
				{
					leftoffset += leftIndex;
					break;
				}else if (strCompare(sql+leftoffset, "OFFSET", leftIndex))
				{
					leftoffset += leftIndex;
					break;
				}
				if (!strGetWord2(sql+leftoffset, tempBuffer, leftIndex, &wordLen))
				{
					break;
				}
				leftoffset += leftIndex+wordLen;

				if (strCompare(tempBuffer, "WHERE", leftIndex))
				{
					bGetWhereKeyword = true;
					leftoffset++;
					break;
				}else if (strCompare(tempBuffer, "ORDER", leftIndex))
				{
					// 没有WHERE后面带的ORDER
					bGetOrderKeyword = true;
					leftoffset++;
					break;
				}else if (strCompare(tempBuffer, "AS", leftIndex))
				{
					leftoffset++;
					if (!strGetWord2(sql+leftoffset, tempBuffer, leftIndex, &wordLen))
					{
						delete[] tempBuffer;
						parse_free(result);
						result = 0;
						return result;
					}
					tableHandle->alias_name = tempBuffer;
					tempBuffer = new char[MAX_ITEM_STRING_SIZE];
				}else
				{
					tableHandle->alias_name = tempBuffer;
					tempBuffer = new char[MAX_ITEM_STRING_SIZE];

					if (strCompare(sql+leftoffset, ",", leftIndex))
					{
						leftoffset += leftIndex+1;
						continue;
					}else if (strCompare(sql+leftoffset, "WHERE", leftIndex))
					{
						leftoffset += leftIndex+6;
						bGetWhereKeyword = true;
						break;
					}else
					{
						break;
					}
				}
			}

			delete[] tempBuffer;
			if (!bGetWhereKeyword)
			{
				if (bGetOrderKeyword)
				{
					GetOrderBys(sql+leftoffset, result, leftIndex);
					leftoffset += leftIndex;
				}
				if (!GetOffsetLimit(sql+leftoffset, result))
				{
					parse_free(result);
					result = 0;
					return result;
				}
				break;
			}

			// WHERE
			if (!GetItemWheres(sql+leftoffset, result, leftIndex, bGetOrderKeyword))
			{
				parse_free(result);
				result = 0;
				return result;
			}
			leftoffset += leftIndex;
			if (bGetOrderKeyword)
			{
				// WHERE后面带的ORDER
				GetOrderBys(sql+leftoffset, result, leftIndex);
				leftoffset += leftIndex;
			}
			//// LIMIT X OFFSET Y
			if (!GetOffsetLimit(sql+leftoffset, result))
			{
				parse_free(result);
				result = 0;
				return result;
			}
		}break;
	case SQLCOM_DELETE:
		{
			if (!strCompare(tempBuffer, "FROM", leftIndex))
			{
				delete[] tempBuffer;
				parse_free(result);
				result = 0;
				return result;
			}
			leftoffset += 1;

			if (!strGetWord2(sql+leftoffset, tempBuffer, leftIndex, &wordLen))
			{
				delete[] tempBuffer;
				parse_free(result);
				result = 0;
				return result;
			}
			leftoffset += leftIndex+wordLen+1;

			// <table_name>
			AddTableItem(result, tempBuffer);

			bool bGetWhereKeyword = false;
			if (strCompare(sql+leftoffset, "WHERE", leftIndex))
			{
				bGetWhereKeyword = true;
				leftoffset += leftIndex+6;
			}else
			{
				char * tempBuffer2 = new char[MAX_ITEM_STRING_SIZE];
				if (!strGetWord2(sql+leftoffset, tempBuffer2, leftIndex))
				{
					delete[] tempBuffer2;
				}else
				{
					delete[] tempBuffer2;
					parse_free(result);
					result = 0;
					return result;
				}
			}

			if (!bGetWhereKeyword)
			{
				break;
			}

			// WHERE
			bool bGetOrderKeyword = false;
			if (!GetItemWheres(sql+leftoffset, result, leftIndex, bGetOrderKeyword))
			{
				parse_free(result);
				result = 0;
				return result;
			}

		}break;
	case SQLCOM_UPDATE:
		{
			AddTableItem(result, tempBuffer);

			// SET <column_name>=<column_value>,
			if (!strCompare(sql+leftoffset, "SET", leftIndex))
			{
				parse_free(result);
				result = 0;
				return result;
			}
			leftoffset += leftIndex+4;

			bool bGetWhereKeyword = false;
			// <column_name>,
			while (true)
			{
				// <column_name>
				tempBuffer = new char[MAX_ITEM_STRING_SIZE];
				if (!strGetWord2(sql+leftoffset, tempBuffer, leftIndex, &wordLen))
				{
					delete[] tempBuffer;
					parse_free(result);
					result = 0;
					return result;
				}
				leftoffset += leftIndex+wordLen;

				if (!strCompare(sql+leftoffset, "=", leftIndex))
				{
					delete[] tempBuffer;
					parse_free(result);
					result = 0;
					return result;
				}
				leftoffset += leftIndex + 1;

				tagValue * valueHandle = AddValueItem(result, tempBuffer);

				// ???
				// SET f2 = f2*1.1
				// Set f2 = (SELECT ...)
				// <column_value>
				if (!GetItemValue(sql+leftoffset, &valueHandle->value, leftIndex))
				{
					parse_free(result);
					result = 0;
					return result;
				}
				leftoffset += leftIndex;

				if (strCompare(sql+leftoffset, ",", leftIndex))
				{
					leftoffset += leftIndex+1;
					continue;
				}else if (strCompare(sql+leftoffset, "WHERE", leftIndex))
				{
					bGetWhereKeyword = true;
					leftoffset += leftIndex+6;
					break;
				}else
				{
					char * tempBuffer2 = new char[MAX_ITEM_STRING_SIZE];
					 if (!strGetWord2(sql+leftoffset, tempBuffer2, leftIndex))
					 {
						 delete[] tempBuffer2;
						 break;
					 }else
					 {
						 delete[] tempBuffer2;
						 parse_free(result);
						 result = 0;
						 return result;
					 }
				}
			}

			if (!bGetWhereKeyword)
			{
				break;
			}

			// WHERE
			bool bGetOrderKeyword = false;
			if (!GetItemWheres(sql+leftoffset, result, leftIndex, bGetOrderKeyword))
			{
				parse_free(result);
				result = 0;
				return result;
			}
		}break;
	case SQLCOM_INSERT:
		{
			// INSERT [INTO]
			if (strCompare(tempBuffer, "INTO", leftIndex))
			{
				// <table_name>
				if (!strGetWord2(sql+leftoffset, tempBuffer, leftIndex, &wordLen))
				{
					delete[] tempBuffer;
					parse_free(result);
					result = 0;
					return result;
				}
				leftoffset += leftIndex+wordLen;
			}

			AddTableItem(result, tempBuffer);

			bool specileColumn = false;
			// <column_name>,
			if (strCompare(sql+leftoffset, "(", leftIndex))
			{
				leftoffset += leftIndex+1;
				specileColumn = true;

				while (true)
				{
					// <column_name>
					tempBuffer = new char[MAX_ITEM_STRING_SIZE];
					if (!strGetWord2(sql+leftoffset, tempBuffer, leftIndex, &wordLen))
					{
						delete[] tempBuffer;
						parse_free(result);
						result = 0;
						return result;
					}
					leftoffset += leftIndex+wordLen;

					AddValueItem(result, tempBuffer);

					if (strCompare(sql+leftoffset, ",", leftIndex))
					{
						leftoffset += leftIndex+1;
						continue;
					}else if (strCompare(sql+leftoffset, ")", leftIndex))
					{
						leftoffset += leftIndex+1;
						break;
					}else
					{
						parse_free(result);
						result = 0;
						return result;
					}
				}
			}

			// <values>,
			if (!strCompare(sql+leftoffset, "VALUES", leftIndex))
			{
				parse_free(result);
				result = 0;
				return result;
			}
			leftoffset += leftIndex+6;
			if (!strCompare(sql+leftoffset, "(", leftIndex))
			{
				parse_free(result);
				result = 0;
				return result;
			}
			leftoffset += leftIndex+1;

			short valueCount = 0;
			while (true)
			{
				tagValue * valueHandle = 0;
				if (specileColumn)
				{
					if (++valueCount > result->values->itemcount)
					{
						parse_free(result);
						result = 0;
						return result;
					}
					valueHandle = (tagValue*)result->values->items[valueCount-1]->item_handle;
				}else
				{
					valueHandle = AddValueItem(result, tempBuffer);
				}

				if (!GetItemValue(sql+leftoffset, &valueHandle->value, leftIndex))
				{
					parse_free(result);
					result = 0;
					return result;
				}
				leftoffset += leftIndex;

				if (strCompare(sql+leftoffset, ",", leftIndex))
				{
					leftoffset += leftIndex+1;
					continue;
				}else if (strCompare(sql+leftoffset, ")", leftIndex))
				{
					leftoffset += leftIndex+1;
					break;
				}else
				{
					parse_free(result);
					result = 0;
					return result;
				}
			}
		}break;
	case SQLCOM_USE_DATABASE:
		{
			result->dbname = tempBuffer;
		}break;
	case SQLCOM_DROP_DATABASE:
		{
			result->dbname = tempBuffer;
		}break;
	case SQLCOM_CREATE_DATABASE:
		{
			result->dbname = tempBuffer;
		}break;
	case SQLCOM_ALTER_DATABASE:
		{
			//ALTER DATABASE name [ [ WITH ] option [ ... ] ]
			// 
			//where option can be:
			// 
			//    CONNECTION LIMIT connlimit
			// 
			//ALTER DATABASE name SET parameter { TO | = } { value | DEFAULT }
			//ALTER DATABASE name RESET parameter
			//
			//ALTER DATABASE name RENAME TO newname
			//
			//ALTER DATABASE name OWNER TO new_owner

			result->dbname = tempBuffer;
			if (strCompare(sql+leftoffset, "RENAME", leftIndex))
			{
				// ???未实现；
				leftoffset += leftIndex+7;
				if (!strCompare(sql+leftoffset, "TO", leftIndex))
				{
					parse_free(result);
					result = 0;
					return result;
				}
				leftoffset += leftIndex+3;

				tempBuffer = new char[MAX_ITEM_STRING_SIZE];
				if (!strGetWord2(sql+leftoffset, tempBuffer, leftIndex, &wordLen))
				{
					delete[] tempBuffer;
					parse_free(result);
					result = 0;
					return result;
				}
				leftoffset += leftIndex+wordLen;

				tagParameter * parameter = new tagParameter;
				memset(parameter, 0, sizeof(tagParameter));
				parameter->param_type = PARAMETER_RENAME;
				parameter->parameter = tempBuffer;
				AddItem(result, PARAM_ITEM, parameter);
			}else if (strCompare(sql+leftoffset, "SET", leftIndex))
			{
				leftoffset += leftIndex+4;

				tempBuffer = new char[MAX_ITEM_STRING_SIZE];
				if (!strGetWord2(sql+leftoffset, tempBuffer, leftIndex, &wordLen))
				{
					delete[] tempBuffer;
					parse_free(result);
					result = 0;
					return result;
				}
				leftoffset += leftIndex+wordLen;

				if (!strCompare(sql+leftoffset, "=", leftIndex))
				{
					delete[] tempBuffer;
					parse_free(result);
					result = 0;
					return result;
				}
				leftoffset += leftIndex + 1;

				char* tempBuffer2 = new char[MAX_ITEM_STRING_SIZE];
				if (!strGetWord2(sql+leftoffset, tempBuffer2, leftIndex, &wordLen))
				{
					delete[] tempBuffer;
					delete[] tempBuffer2;
					parse_free(result);
					result = 0;
					return result;
				}
				leftoffset += leftIndex+wordLen;


				tagParameter * parameter = new tagParameter;
				memset(parameter, 0, sizeof(tagParameter));
				parameter->param_type = PARAMETER_SET;
				parameter->parameter = tempBuffer;
				parameter->parameter2 = tempBuffer2;
				AddItem(result, PARAM_ITEM, parameter);
			}else
			{
				break;
			}

		}break;
	case SQLCOM_ALTER_TABLE:
		{
			AddTableItem(result, tempBuffer);
			//AddItem(result, STRING_ITEM, tempBuffer);

			while (true)
			{
				if (strCompare(sql+leftoffset, "RENAME", leftIndex))
				{
					leftoffset += leftIndex+7;
					if (!strCompare(sql+leftoffset, "TO", leftIndex))
					{
						parse_free(result);
						result = 0;
						return result;
					}
					leftoffset += leftIndex+3;

					tempBuffer = new char[MAX_ITEM_STRING_SIZE];
					if (!strGetWord2(sql+leftoffset, tempBuffer, leftIndex, &wordLen))
					{
						delete[] tempBuffer;
						parse_free(result);
						result = 0;
						return result;
					}
					leftoffset += leftIndex+wordLen;

					tagParameter * parameter = new tagParameter;
					memset(parameter, 0, sizeof(tagParameter));
					parameter->param_type = PARAMETER_RENAME;
					parameter->parameter = tempBuffer;
					AddItem(result, PARAM_ITEM, parameter);
				}else if (strCompare(sql+leftoffset, "ADD", leftIndex))
				{
					leftoffset += leftIndex+4;
					if (strCompare(sql+leftoffset, "COLUMN", leftIndex))
					{
						leftoffset += leftIndex+7;
					}

					tagField * fieldHandle = GetFieldItem(sql+leftoffset, leftIndex);
					if (fieldHandle == 0)
					{
						parse_free(result);
						result = 0;
						return result;
					}
					leftoffset += leftIndex;

					tagParameter * parameter = new tagParameter;
					memset(parameter, 0, sizeof(tagParameter));
					parameter->param_type = PARAMETER_ADDCOLUMN;
					parameter->parameter = fieldHandle;
					AddItem(result, PARAM_ITEM, parameter);
				}else if (strCompare(sql+leftoffset, "MODIFY", leftIndex) || strCompare(sql+leftoffset, "CHANGE", leftIndex))
				{
					leftoffset += leftIndex+7;
					if (strCompare(sql+leftoffset, "COLUMN", leftIndex))
					{
						leftoffset += leftIndex+7;
					}

					tagField * fieldHandle = GetFieldItem(sql+leftoffset, leftIndex);
					if (fieldHandle == 0)
					{
						parse_free(result);
						result = 0;
						return result;
					}
					leftoffset += leftIndex;

					tagParameter * parameter = new tagParameter;
					memset(parameter, 0, sizeof(tagParameter));
					parameter->param_type = PARAMETER_MODIFYCOLUMN;
					parameter->parameter = fieldHandle;
					AddItem(result, PARAM_ITEM, parameter);
				}else if (strCompare(sql+leftoffset, "DROP", leftIndex))
				{
					leftoffset += leftIndex+5;
					if (strCompare(sql+leftoffset, "COLUMN", leftIndex))
					{
						leftoffset += leftIndex+7;
					}

					// <clumn_name>
					tempBuffer = new char[MAX_ITEM_STRING_SIZE];
					if (!strGetWord2(sql+leftoffset, tempBuffer, leftIndex, &wordLen))
					{
						delete[] tempBuffer;
						parse_free(result);
						result = 0;
						return result;
					}
					leftoffset += leftIndex+wordLen+1;

					tagField * fieldHandle = new tagField;
					memset(fieldHandle, 0, sizeof(tagField));
					fieldHandle->field_name = tempBuffer;

					if (strCompare(sql+leftoffset, "CASCADE", leftIndex))
					{
						result->parameter = (void*)1;
					}

					tagParameter * parameter = new tagParameter;
					memset(parameter, 0, sizeof(tagParameter));
					parameter->param_type = PARAMETER_DROPCOLUMN;
					parameter->parameter = fieldHandle;
					AddItem(result, PARAM_ITEM, parameter);

				}else if (strCompare(sql+leftoffset, "ALTER", leftIndex))
				{
					leftoffset += leftIndex+6;
					if (strCompare(sql+leftoffset, "COLUMN", leftIndex))
					{
						leftoffset += leftIndex+7;
					}

					// <clumn_name>
					tempBuffer = new char[MAX_ITEM_STRING_SIZE];
					if (!strGetWord2(sql+leftoffset, tempBuffer, leftIndex, &wordLen))
					{
						delete[] tempBuffer;
						parse_free(result);
						result = 0;
						return result;
					}
					leftoffset += leftIndex+wordLen+1;

					tagField * fieldHandle = new tagField;
					memset(fieldHandle, 0, sizeof(tagField));
					fieldHandle->field_name = tempBuffer;

					if (strCompare(sql+leftoffset, "SET", leftIndex))
					{
						// SET DEFAULT <default>
						leftoffset += leftIndex+4;
						if (!strCompare(sql+leftoffset, "DEFAULT", leftIndex))
						{
							free_field(fieldHandle);
							parse_free(result);
							result = 0;
							return result;
						}
						leftoffset += leftIndex+8;

						tagParameter * parameter = new tagParameter;
						memset(parameter, 0, sizeof(tagParameter));
						parameter->param_type = PARAMETER_COLUMNSETDEFAULT;
						parameter->parameter = fieldHandle;
						AddItem(result, PARAM_ITEM, parameter);

						fieldHandle->default_value = new tagItemValue;
						memset(fieldHandle->default_value, 0, sizeof(tagItemValue));
						if (!GetItemValue(sql+leftoffset, fieldHandle->default_value, leftIndex))
						{
							parse_free(result);
							result = 0;
							return result;
						}
						leftoffset += leftIndex+1;
					}else if (strCompare(sql+leftoffset, "DROP", leftIndex))
					{
						// DROP DEFAULT
						leftoffset += leftIndex+5;
						if (!strCompare(sql+leftoffset, "DEFAULT", leftIndex))
						{
							free_field(fieldHandle);
							parse_free(result);
							result = 0;
							return result;
						}
						leftoffset += leftIndex+8;

						tagParameter * parameter = new tagParameter;
						memset(parameter, 0, sizeof(tagParameter));
						parameter->param_type = PARAMETER_COLUMNDROPDEFAULT;
						parameter->parameter = fieldHandle;
						AddItem(result, PARAM_ITEM, parameter);
					}else
					{
						free_field(fieldHandle);
						parse_free(result);
						result = 0;
						return result;
					}
				}else
				{
					break;
				}
			}

		}break;
	case SQLCOM_DROP_TABLE:
		{
			AddTableItem(result, tempBuffer);

			if (strCompare(sql+leftoffset, "CASCADE", leftIndex))
			{
				result->parameter = (void*)1;
			}

		}break;
	case SQLCOM_CREATE_TABLE:
	case SQLCOM_CREATE_GTTABLE:
	case SQLCOM_CREATE_LTTABLE:
		{
			AddTableItem(result, tempBuffer);
			
			//tempBuffer = new char[MAX_ITEM_STRING_SIZE];
			if (!strCompare(sql+leftoffset, "(", leftIndex))
			{
				parse_free(result);
				result = 0;
				return result;
			}
			leftoffset += leftIndex+1;

			if (!GetFieldItems(sql+leftoffset, result, leftIndex))
			{
				parse_free(result);
				result = 0;
				return result;
			}
		}break;
	default:
		{
			delete[] tempBuffer;
			parse_free(result);
			result = 0;
		}break;
	}

	return result;
}

void free_field(tagField * field)
{
	if (field != 0)
	{
		//if (field->table_name)
		//	delete[] field->table_name;
		if (field->field_name)
			delete[] field->field_name;
		if (field->constraint_name)
			delete[] field->constraint_name;
		if (field->default_value != 0)
		{
			if (field->default_value->value_type == VALUE_STRING && field->default_value->u.value_string != 0)
				delete[] field->default_value->u.value_string;
			delete field->default_value;
		}

		delete field;
	}
}

void free_param(tagParameter * param)
{
	if (param != 0)
	{
		switch (param->param_type)
		{
		case PARAMETER_DROPTABLE:
		case PARAMETER_RENAME:
		case PARAMETER_SET:
			{
				char * buffer = (char*)param->parameter;
				if (buffer != 0)
					delete[] buffer;
				buffer = (char*)param->parameter2;
				if (buffer != 0)
					delete[] buffer;
			}break;
		case PARAMETER_ADDCOLUMN:
		case PARAMETER_DROPCOLUMN:
		case PARAMETER_COLUMNSETDEFAULT:
		case PARAMETER_COLUMNDROPDEFAULT:
			{
				free_field((tagField*)param->parameter);
			}break;
		default:
			break;
		}

		delete param;
	}
}

void free_item(tagItem * item)
{
	if (item != 0)
	{
		switch (item->item_type)
		{
		case STRING_ITEM:
			{
				if (item->table_name != 0)
					delete[] item->table_name;
				char * buffer = (char*)item->item_handle;
				if (buffer != 0)
					delete[] buffer;
			}break;
		case TABLE_ITEM:
			{
				tagTable * buffer = (tagTable*)item->item_handle;
				if (buffer != 0)
				{
					if (buffer->table_name)
						delete[] buffer->table_name;
					if (buffer->alias_name)
						delete[] buffer->alias_name;
					delete buffer;
				}

			}break;
		case FIELD_ITEM:
			{
				free_field((tagField*)item->item_handle);
			}break;
		case VALUE_ITEM:
			{
				tagValue * buffer = (tagValue*)item->item_handle;
				if (buffer != 0)
				{
					if (buffer->field_name)
						delete[] buffer->field_name;
					if (buffer->value.value_type == VALUE_STRING && buffer->value.u.value_string != 0)
						delete[] buffer->value.u.value_string;
					delete buffer;
				}

			}break;
		case WHERE_ITEM:
			{
				tagWhere * buffer = (tagWhere*)item->item_handle;
				if (buffer != 0)
				{
					if (buffer->table_name)
						delete[] buffer->table_name;
					if (buffer->field_name)
						delete[] buffer->field_name;

					switch (buffer->compare_type)
					{
					case COMPARE_EQUAL:
					case COMPARE_UNEQUAL:
					case COMPARE_GREATER:
					case COMPARE_GREATEREQUAL:
					case COMPARE_LESS:
					case COMPARE_LESSEQUAL:
					case COMPARE_LIKE:
						{
							tagItemValue * itemValue = (tagItemValue*)buffer->value_handle;
							if (itemValue != 0)
							{
								if (itemValue->table_name != 0)
									delete[] itemValue->table_name;
								if (itemValue->field_name != 0)
									delete[] itemValue->field_name;
								if (itemValue->value_type == VALUE_STRING && itemValue->u.value_string != 0)
									delete[] itemValue->u.value_string;

								delete itemValue;
							}
						}break;
					default:
						break;
					}
					delete buffer;
				}
			}break;
		case FUNC_ITEM:
			{
				if (item->table_name != 0)
					delete[] item->table_name;
				tagFunc * buffer = (tagFunc*)item->item_handle;
				if (buffer != 0)
				{
					free_items(buffer->items);
					delete buffer;
				}
			}break;
		case PARAM_ITEM:
			{
				free_param((tagParameter*)item->item_handle);
			}break;
		default:
			{
				// ?
				//if (item->item_handle)
				//	delete item->item_handle;
			}break;
		}

		delete item;
	}

}

void free_items(tagItems * items)
{
	short i=0;
	if (items != 0)
	{
		for (i=0; i<items->itemcount; i++)
		{
			free_item(items->items[i]);
		}
		if (items->items != 0)
		{
			delete[] items->items;
		}
		delete items;
	}
}

void parse_free(tagSP * in)
{
	if (in != 0)
	{
		if (in->dbname != 0)
			delete[] in->dbname;

		free_items(in->items);
		free_items(in->tables);
		free_items(in->fields);
		free_items(in->values);
		free_items(in->wheres);
		free_items(in->orderbys);
		//free_param(in->parameter);

		delete in;
	}
}

