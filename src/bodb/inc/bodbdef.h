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

// bodbdef.h file here
#ifndef __bodbdef_h__
#define __bodbdef_h__

#ifdef WIN32
#pragma warning(disable:4244 4267 4996)
#endif //WIN32

#include <time.h>

namespace bo
{
	//const unsigned int 
	const unsigned int PAGE_BLOCK_SIZE		= 8 * 1024;	
	const unsigned int EXTENT_BLOCK_SIZE	= 8 * PAGE_BLOCK_SIZE;

	const unsigned int MAX_CHAR_SIZE		= 8000;				// BYTE
	//const unsigned int MAX_BIGVAR_SIZE	= 2*1024*1024*1024;	// 2GB
	const unsigned int MAX_CLOB_SIZE		= 1*1024*1024*1024;	// 1GB
	const unsigned int MAX_BINARY_SIZE		= MAX_CHAR_SIZE;
	const unsigned int MAX_BLOB_SIZE		= MAX_CLOB_SIZE;

	const unsigned short MAX_NAME_SIZE		= 100;
	const unsigned short MAX_NOTE_SIZE		= 255;
	const unsigned short MAX_NUMERIC_SIZE	= 38;

	const unsigned short MAX_DEFAULT_CHAR_SIZE		= 50;
	const unsigned short MAX_DEFAULT_VARIANT_SIZE	= MAX_DEFAULT_CHAR_SIZE + 10;

	typedef bool				boolean;
	typedef char				tinyint;
	typedef unsigned char		utinyint;
	typedef short				smallint;
	typedef unsigned short		usmallint;
	typedef int					integer;
	typedef unsigned int		uinteger;
#ifdef WIN32
	typedef __int64				bigint;
	typedef unsigned __int64	ubigint;
	typedef __int64				botime_t;
#else
	typedef long long			bigint;
	typedef unsigned long long	ubigint;
	typedef long long			botime_t;
#endif
	//typedef unsigned int		botime_t;

	typedef void *				BODBHANDLE;

	typedef enum FieldType
	{
		FT_BOOLEAN		= 0x01		//
		, FT_TINYINT	= 0x11		// 1B	-128 - 127
		, FT_UTINYINT				// 0 - 255
		, FT_SMALLINT				// 2B
		, FT_USMALLINT
		, FT_INTEGER				// 4B	INT[EGER]
		, FT_UINTEGER
		, FT_BIGINT					// 8B
		, FT_UBIGINT
		, FT_NUMERIC				// NUMERIC[(p,[,s])]
		, FT_FLOAT		= 0x21		// 4B FLOAT[(p)]
		, FT_REAL					// 4B
		, FT_DOUBLE					// 8B
		, FT_DATE		= 0x31		// YYYY-MM-DD
		, FT_TIME					// HH:MI:SS
		, FT_TIMESTAMP				// YYYY-MM-DD HH:MI:SS SSS

		, FT_CHAR		= 0x41	// Max MAX_CHAR_SIZE			CHAR[ACTER][(n)]
		, FT_VARCHAR			// Max MAX_CHAR_SIZE			VARCHAR[(n)]
		, FT_CLOB				// Max MAX_CLOB_SIZE
		, FT_NCHAR		= 0x51	// UNICODE Max MAX_CHAR_SIZE
		, FT_NVARCHAR			// UNICODE Max MAX_CHAR_SIZE
		, FT_NCLOB				// UNICODE Max MAX_CLOB_SIZE
		, FT_BINARY		= 0x61	// Max MAX_BINARY_SIZE
		, FT_VARBINARY			// Max MAX_BINARY_SIZE
		, FT_BLOB				// Max MAX_BLOB_SIZE
		, FT_UNKNOWN	= 0xFF
	}FIELDTYPE;

	typedef enum FieldConstraintType
	{
		FIELD_CONSTRAINT_UNKNOWN
		, FIELD_CONSTRAINT_NOTNULL
		, FIELD_CONSTRAINT_UNIQUE
		, FIELD_CONSTRAINT_PRIMARYKEY
	}FIELDCONSTRAINTTYPE;

	const int	MAX_DEFAULT_NUMERIC_ITEM	= 999999999;
	typedef struct tagNUMERIC
	{
		bool			negative;
		char			size1;							// 0 - MAX_NUMERIC_SIZE
		char			size2;							// 0 - size1
		char			numeric[MAX_NUMERIC_SIZE];		// 9[{0-MAX_NUMERIC_SIZE}]
		//int				numeric1[MAX_NUMERIC_SIZE];
		//int				numeric2[MAX_NUMERIC_SIZE];
	}NUMERIC, * PNUMERIC;

	typedef struct tagFLOAT
	{
		float			value;
		char			size1;			// 0-x
	}FLOAT;

	//typedef struct tagDATE
	//{
	//	unsigned short	year;			// 0000-9999
	//	unsigned char	month;			// 00-12
	//	unsigned char	day;			// 00-28[29,30,31]
	//}DATE, * PDATE;

	//typedef struct tagTIME
	//{
	//	unsigned char	hour;			// 00-23
	//	unsigned char	minute;			// 00-59
	//	unsigned char	second;			// 00-59
	//}TIME, * PTIME;

	//typedef struct tagTIMESTAMP
	//{
	//	tagDATE			date;
	//	tagTIME			time;
	//	unsigned short	millsecond;		// 000-999
	//	short			timezone;
	//}TIMESTAMP, * PTIMESTAMP;

	typedef botime_t TIME;
	typedef botime_t DATE;
	typedef struct tagTIMESTAMP
	{
		botime_t		time;
		short			timezone;
		unsigned short	millsecond;		// 000-999
	}TIMESTAMP;

	typedef struct tagBuffer
	{
		unsigned int	size;
		char *			buffer;
	}BUFFER, * PBUFFER;
#define BODB_BUFFER_TEXT(A) ((A.size>0&&A.buffer!=0)?A.buffer:"")

	typedef struct tagNBuffer
	{
		unsigned int	size;
		wchar_t *		buffer;
	}NBUFFER, * PNBUFFER;

	typedef struct tagFIELDVARIANT
	{
		FieldType VARTYPE;

		union
		{
			bool booleanVal;
			tinyint tinyintVal;
			utinyint utinyintVal;
			smallint smallintVal;
			usmallint usmallintVal;
			integer integerVal;
			uinteger uintegerVal;
			bigint bigintVal;
			ubigint ubigintVal;
			PNUMERIC numericVal;
			FLOAT floatVal;
			float realVal;
			double doubleVal;
			DATE dateVal;
			TIME timeVal;
			TIMESTAMP timestampVal;
			BUFFER charVal;
			BUFFER varcharVal;
			BUFFER clobVal;
			NBUFFER ncharVal;
			NBUFFER nvarcharVal;
			NBUFFER nclobVal;
			BUFFER lobVal;
			BUFFER varlobVal;
			BUFFER blobVal;
		}v;

	}FIELDVARIANT, * PFIELDVARIANT;

	typedef struct tagRecordLine
	{
		short				fieldcount;
		PFIELDVARIANT *		fieldvalues;
	}RECORDLINE, * PRECORDLINE;

	typedef struct tagResultSet
	{
		int					rscount;
		PRECORDLINE *		rsvalues;
	}RESULTSET, * PRESULTSET;

	const unsigned short BOOLEAN_SIZE	= 1;
	const unsigned short TINYINT_SIZE	= 1;
	const unsigned short SMALLINT_SIZE	= 2;
	const unsigned short INTEGER_SIZE	= 4;
	const unsigned short BIGINT_SIZE	= 8;
	const unsigned short FLOAT_SIZE		= sizeof(tagFLOAT);
	const unsigned short REAL_SIZE		= 4;
	const unsigned short DOUBLE_SIZE	= 8;
	const unsigned short NUMERIC_SIZE	= sizeof(tagNUMERIC);
	const unsigned short DATE_SIZE		= sizeof(DATE);
	const unsigned short TIME_SIZE		= sizeof(TIME);
	const unsigned short TIMESTAMP_SIZE	= sizeof(tagTIMESTAMP);

	typedef enum OptionType
	{
		OPTION_FULL_MEMORY_MODE			// 0/1 default 0
	}OptionType;

} // namespace

#endif // __bodbdef_h__

