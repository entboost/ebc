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

// sqlparser.h file here
#ifndef __sqlparser_h__
#define __sqlparser_h__

enum enum_item_types
{
	UNKNOWN_ITEM
	, STRING_ITEM
	, TABLE_ITEM, FIELD_ITEM
	, VALUE_ITEM, WHERE_ITEM
	, ITEMS_ITEM	// for where
	, FUNC_ITEM
	, PARAM_ITEM

	/*
, , SUM_FUNC_ITEM, , INT_ITEM, REAL_ITEM, NULL_ITEM,
VARBIN_ITEM, COPY_STR_ITEM, FIELD_AVG_ITEM, DEFAULT_VALUE_ITEM, PROC_ITEM,COND_ITEM,
REF_ITEM, FIELD_STD_ITEM, FIELD_VARIANCE_ITEM, INSERT_VALUE_ITEM, SUBSELECT_ITEM, ROW_ITEM, CACHE_ITEM, TYPE_HOLDER, PARAM_ITEM
*/
};

enum enum_funciton_type
{
	FUNCTION_UNKNOWN
	, FUNCTION_DATE_FORMAT	= 0x01
	, FUNCTION_WDAY
	, FUNCTION_MDAY
	, FUNCTION_YDAY
	, FUNCTION_YEAR
	, FUNCTION_MONTH
	, FUNCTION_HOUR
	, FUNCTION_MINUTE
	, FUNCTION_SECOND
	, FUNCTION_TO_INT
};

enum enum_value_type
{
	VALUE_UNKNOWN
	, VALUE_LETTER	// A-Z & a-z
	, VALUE_STRING
	, VALUE_INT
	, VALUE_BIGINT
	, VALUE_REAL
	, VALUE_BOOL
	, VALUE_NULL
	, VALUE_DEFAULT

	, VALUE_FUNC_CURRENT_TIMESTAMP

};

enum enum_parameter_type
{
	PARAMETER_UNKNOWN
	, PARAMETER_RENAME
	, PARAMETER_DROPTABLE
	, PARAMETER_ADDCOLUMN, PARAMETER_MODIFYCOLUMN, PARAMETER_DROPCOLUMN
	, PARAMETER_COLUMNSETDEFAULT, PARAMETER_COLUMNDROPDEFAULT
	, PARAMETER_SET
};

enum enum_compare_type
{
	COMPARE_EQUAL
	, COMPARE_UNEQUAL
	, COMPARE_GREATER
	, COMPARE_GREATEREQUAL
	, COMPARE_LESS
	, COMPARE_LESSEQUAL	// *
	, COMPARE_ISNULL
	, COMPARE_ISNOTNULL
	, COMPARE_BETWEENAND
	, COMPARE_LIKE
};

enum enum_field_types {
	SQL_TYPE_BOOLEAN
	, SQL_TYPE_TINYINT
	, SQL_TYPE_SMALLINT
	, SQL_TYPE_INTEGER
	, SQL_TYPE_BIGINT
	, SQL_TYPE_NUMERIC, SQL_TYPE_DECIMAL
	, SQL_TYPE_FLOAT
	, SQL_TYPE_REAL
	, SQL_TYPE_DOUBLE
	, SQL_TYPE_DATE
	, SQL_TYPE_TIME
	, SQL_TYPE_TIMESTAMP
	, SQL_TYPE_CHAR, SQL_TYPE_VARCHAR, SQL_TYPE_CLOB
	, SQL_TYPE_NCHAR, SQL_TYPE_NVARCHAR, SQL_TYPE_NCLOB
	, SQL_TYPE_BINARY, SQL_TYPE_VARBINARY, SQL_TYPE_BLOB
};

enum enum_field_constraint
{
	SQL_CONSTRAINT_UNKNOWN
	, SQL_CONSTRAINT_NOTNULL
	, SQL_CONSTRAINT_UNIQUE
	, SQL_CONSTRAINT_PRIMARYKEY
};

enum enum_sql_command
{
	SQLCOM_SELECT
	, SQLCOM_USE_DATABASE
	, SQLCOM_UPDATE
	, SQLCOM_INSERT
	, SQLCOM_DELETE
	, SQLCOM_CREATE_DATABASE, SQLCOM_CREATE_TABLE, SQLCOM_CREATE_VIEW, SQLCOM_CREATE_INDEX
	, SQLCOM_CREATE_GTTABLE, SQLCOM_CREATE_LTTABLE
	, SQLCOM_DROP_DATABASE, SQLCOM_DROP_TABLE
	, SQLCOM_ALTER_DATABASE, SQLCOM_ALTER_TABLE

	/*
  , , , SQLCOM_ALTER_TABLE,
  , , SQLCOM_INSERT_SELECT,
  , SQLCOM_TRUNCATE, SQLCOM_DROP_TABLE, SQLCOM_DROP_INDEX,
  SQLCOM_SHOW_DATABASES, SQLCOM_SHOW_TABLES, SQLCOM_SHOW_FIELDS,
  SQLCOM_SHOW_KEYS, SQLCOM_SHOW_VARIABLES, SQLCOM_SHOW_LOGS, SQLCOM_SHOW_STATUS,
  SQLCOM_SHOW_INNODB_STATUS,SQLCOM_SHOW_NDBCLUSTER_STATUS,
  SQLCOM_SHOW_PROCESSLIST, SQLCOM_SHOW_MASTER_STAT, SQLCOM_SHOW_SLAVE_STAT,
  SQLCOM_SHOW_GRANTS, SQLCOM_SHOW_CREATE, SQLCOM_SHOW_CHARSETS,
  SQLCOM_SHOW_COLLATIONS, SQLCOM_SHOW_CREATE_DB,
  SQLCOM_LOAD,SQLCOM_SET_OPTION,SQLCOM_LOCK_TABLES,SQLCOM_UNLOCK_TABLES,
  SQLCOM_GRANT,
  SQLCOM_CHANGE_DB, SQLCOM_CREATE_DB, SQLCOM_DROP_DB, SQLCOM_ALTER_DB,
  SQLCOM_REPAIR, SQLCOM_REPLACE, SQLCOM_REPLACE_SELECT,
  SQLCOM_CREATE_FUNCTION, SQLCOM_DROP_FUNCTION,
  SQLCOM_REVOKE,SQLCOM_OPTIMIZE, SQLCOM_CHECK, 
  SQLCOM_ASSIGN_TO_KEYCACHE, SQLCOM_PRELOAD_KEYS,
  SQLCOM_FLUSH, SQLCOM_KILL, SQLCOM_ANALYZE,
  SQLCOM_ROLLBACK, SQLCOM_ROLLBACK_TO_SAVEPOINT,
  SQLCOM_COMMIT, SQLCOM_SAVEPOINT,
  SQLCOM_SLAVE_START, SQLCOM_SLAVE_STOP,
  SQLCOM_BEGIN, SQLCOM_LOAD_MASTER_TABLE, SQLCOM_CHANGE_MASTER,
  SQLCOM_RENAME_TABLE, SQLCOM_BACKUP_TABLE, SQLCOM_RESTORE_TABLE,
  SQLCOM_RESET, SQLCOM_PURGE, SQLCOM_PURGE_BEFORE, SQLCOM_SHOW_BINLOGS,
  SQLCOM_SHOW_OPEN_TABLES, SQLCOM_LOAD_MASTER_DATA,
  SQLCOM_HA_OPEN, SQLCOM_HA_CLOSE, SQLCOM_HA_READ,
  SQLCOM_SHOW_SLAVE_HOSTS, SQLCOM_DELETE_MULTI, SQLCOM_UPDATE_MULTI,
  SQLCOM_SHOW_BINLOG_EVENTS, SQLCOM_SHOW_NEW_MASTER, SQLCOM_DO,
  SQLCOM_SHOW_WARNS, SQLCOM_EMPTY_QUERY, SQLCOM_SHOW_ERRORS,
  SQLCOM_SHOW_COLUMN_TYPES, SQLCOM_SHOW_STORAGE_ENGINES, SQLCOM_SHOW_PRIVILEGES,
  SQLCOM_HELP, SQLCOM_DROP_USER, SQLCOM_REVOKE_ALL, SQLCOM_CHECKSUM,
  SQLCOM_PREPARE, SQLCOM_EXECUTE, SQLCOM_DEALLOCATE_PREPARE,
  SQLCOM_END
  */
};

//const short MAX_VALUE_STRING_SIZE	= 8*1024;
const short MAX_ITEM_STRING_SIZE	= 255;
const short MAX_FIELD_ITEM			= 255;
namespace bo
{
#ifdef WIN32
	typedef __int64				bigint;
#define bo_atoi64(a) _atoi64(a)
#else
	typedef long long			bigint;
#define bo_atoi64(a) atoll(a)
#endif
} // cgc namespace

struct tagItemValue
{
	enum_value_type		value_type;
	char *				table_name;
	char *				field_name;
	union
	{
		char *			value_string;
		int				value_int;
		bo::bigint		value_bigint;
		bool			value_bool;
		double			value_real;
	}u;
};

struct tagTable
{
	char *					table_name;
	char *					alias_name;
};

struct tagField
{
	enum_field_types		field_type;
	char *					field_name;
	bool					unsigned_field;
	bool					auto_increment;
	short					size1;
	short					size2;
	enum_field_constraint	constraint;
	char *					constraint_name;
	bool					references;
	tagItemValue *			default_value;
};

struct tagValue
{
	char *				field_name;
	tagItemValue		value;
};

struct tagWhere
{
	bool				and_where;	// true: AND; false: OR
	char *				table_name;	// default null
	char *				field_name;
	enum_compare_type	compare_type;
	void *				value_handle;
	short				where_level;	// (xxx)
};


struct tagItem
{
	enum_item_types		item_type;
	char *				table_name;
	void *				item_handle;
};

struct tagItems
{
	short itemcount;
	tagItem ** items;
};

struct tagFunc
{
	enum_funciton_type	function_type;
	tagItems *			items;
};

struct tagParameter
{
	enum_parameter_type	param_type;
	void *				parameter;
	void *				parameter2;
};

struct tagSP
{
	enum_sql_command	sql_command;
	char *				dbname;
	tagItems *			items;
	tagItems *			tables;
	tagItems *			fields;
	tagItems *			values;
	tagItems *			wheres;
	tagItems *			orderbys;
	short				orderbydesc;
	void *				parameter;
	bo::bigint			limit;
	bo::bigint			offset;
};



//////////////////////////////////////////
// 
tagSP * parse_exec(const char * sql);

void parse_free(tagSP * in);


#ifdef WIN32
#ifdef _DEBUG

#if (_MSC_VER == 1200)
#pragma comment(lib, "sqlparser60d.lib")
#elif (_MSC_VER == 1300)
#pragma comment(lib, "sqlparser70d.lib")
#elif (_MSC_VER == 1310)
#pragma comment(lib, "sqlparser71d.lib")
#elif (_MSC_VER == 1400)
#pragma comment(lib, "sqlparser80d.lib")
#elif (_MSC_VER == 1500)
#pragma comment(lib, "sqlparser90d.lib")
#endif

#else // _DEBUG

#if (_MSC_VER == 1200)
#pragma comment(lib, "sqlparser60.lib")
#elif (_MSC_VER == 1300)
#pragma comment(lib, "sqlparser70.lib")
#elif (_MSC_VER == 1310)
#pragma comment(lib, "sqlparser71.lib")
#elif (_MSC_VER == 1400)
#pragma comment(lib, "sqlparser80.lib")
#elif (_MSC_VER == 1500)
#pragma comment(lib, "sqlparser90.lib")
#endif

#endif // _DEBUG
#endif // WIN32

#endif // __sqlparser_h__

