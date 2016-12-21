/*
    MYCP is a HTTP and C++ Web Application Server.
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

// cgcdef.h file here
#ifndef __cgcdef_head__
#define __cgcdef_head__

#include "cgcSmartString.h"
//#include <string>

namespace mycp {

#define CGC_THREAD_STACK_MIN 1024*200	// 200K
#define CGC_THREAD_STACK_MAX 1024000	// 1M

	enum SOTP_PROTO_VERSION
	{
		SOTP_PROTO_VERSION_20	= 20
		, SOTP_PROTO_VERSION_21	= 21
	};
	enum SOTP_PROTO_TYPE
	{
		SOTP_PROTO_TYPE_UNKNOWN	= 0
		, SOTP_PROTO_TYPE_OPEN			// '1'
		, SOTP_PROTO_TYPE_CLOSE			// '2'
		, SOTP_PROTO_TYPE_ACTIVE		// '3'
		, SOTP_PROTO_TYPE_RTP			// '4'
		, SOTP_PROTO_TYPE_SYNC			// '5'
		, SOTP_PROTO_TYPE_CALL	= 10	// 'A'
		, SOTP_PROTO_TYPE_ACK			// 'B'
		, SOTP_PROTO_TYPE_P2P			// 'C'
	};
	enum SOTP_PROTO_ITEM_TYPE
	{
		SOTP_PROTO_ITEM_TYPE_UNKNOWN		= 0
		, SOTP_PROTO_ITEM_TYPE_SID				// '1'
		, SOTP_PROTO_ITEM_TYPE_SSLDATA			// '2'
		, SOTP_PROTO_ITEM_TYPE_CID				// '3'
		, SOTP_PROTO_ITEM_TYPE_SEQ				// '4'
		, SOTP_PROTO_ITEM_TYPE_NACK				// '5'
		, SOTP_PROTO_ITEM_TYPE_SIGN				// '6'
		, SOTP_PROTO_ITEM_TYPE_API				// '7'
		, SOTP_PROTO_ITEM_TYPE_PV				// '8'
		, SOTP_PROTO_ITEM_TYPE_AT				// '9'
		, SOTP_PROTO_ITEM_TYPE_APP				// 'A'
		, SOTP_PROTO_ITEM_TYPE_SSL				// 'B'
		, SOTP_PROTO_ITEM_TYPE_UA				// 'C'
		, SOTP_PROTO_ITEM_TYPE_RTP_COMMAND		// 'D'
		, SOTP_PROTO_ITEM_TYPE_RTP_DATA							// 'E'
		, SOTP_PROTO_ITEM_TYPE_ACCEPT_ENCODING			// 'F'
		, SOTP_PROTO_ITEM_TYPE_DATA_ENCODING				// 'G'
	};
	enum SOTP_RTP_COMMAND_TYPE
	{
		SOTP_RTP_COMMAND_REGISTER_SOURCE		= 1
		, SOTP_RTP_COMMAND_UNREGISTER_SOURCE
		, SOTP_RTP_COMMAND_REGISTER_SINK
		, SOTP_RTP_COMMAND_UNREGISTER_SINK
		, SOTP_RTP_COMMAND_UNREGISTER_ALLSINK
		, SOTP_RTP_COMMAND_DATA_REQUEST
	};
	enum SOTP_DATA_ENCODING
	{
		SOTP_DATA_ENCODING_UNKNOWN			= 0x0
		, SOTP_DATA_ENCODING_GZIP				= 0x1
		, SOTP_DATA_ENCODING_DEFLATE		= 0x2
	};
	enum SOTP_RTP_DATA_TYPE
	{
		SOTP_RTP_NAK_DATA_AUDIO			= 0
		, SOTP_RTP_NAK_DATA_VIDEO
		, SOTP_RTP_NAK_DATA_VIDEO_I			// **Key Frame Video
		, SOTP_RTP_NAK_DATA_SCREEN
		, SOTP_RTP_NAK_DATA_MOUSE
		, SOTP_RTP_NAK_DATA_OTHER
	};
	// net ack type
	enum SOTP_RTP_NAK_TYPE
	{
		SOTP_RTP_NAK_NONE				= 0
		, SOTP_RTP_NAK_REQUEST_1			// audio etc.
		, SOTP_RTP_NAK_REQUEST_2			// video etc.
	};
	inline char SotpInt2Char(int nChar)
	{
		// '1'=1
		// 'A'=10
		if (nChar>=10)
			return 'A'+(nChar-10);
		else
			return '1'+(nChar-1);
	}
	inline int SotpChar2Int(char pChar)
	{
		// '1'=1
		// 'A'=10
		if (pChar>='A')
			return 10+(pChar-'A');
		else
			return 1+(pChar-'1');
	}
	// ModuleType
	typedef enum ModuleType
	{
		MODULE_UNKNOWN		= 0xff
		, MODULE_COMM			= 0x1
		, MODULE_PARSER		= 0x2
		, MODULE_APP			= 0x4
		, MODULE_SERVER		= 0x8
		, MODULE_LOG			= 0x10

	}MODULETYPE;
	//// Module_SubType
	//typedef enum Module_SubType
	//{
	//	MODULE_SUBTYPE_UNKNOWN		= 0
	//	, MODULE_SUBTYPE_SOTP_CLIENT_SERVICE
	//}MODULE_SUBTYPE;
	// Module_Protocol
	typedef enum Module_Protocol
	{
		MODULE_PROTOCOL_SOTP									= 0
		, MODULE_PROTOCOL_HTTP								= 0x1
		//, MODULE_PROTOCOL_WEB_PROXY						= 0x2
		, MODULE_PROTOCOL_SOTP_CLIENT_SERVICE	= 0x8
	}MODULE_PROTOCOL;

	typedef enum Module_Init_Type
	{
		MODULE_INIT_TYPE_NORMAL
		, MODULE_INIT_TYPE_AUTO_UPDATE
	}MODULE_INIT_TYPE;
	typedef enum Module_Free_Type
	{
		MODULE_FREE_TYPE_NORMAL
		, MODULE_FREE_TYPE_AUTO_UPDATE
	}MODULE_FREE_TYPE;

	// LogLevel
	typedef enum LogLevel
	{
		LOG_TRACE		= 0x1
		, LOG_DEBUG		= 0x2
		, LOG_INFO		= 0x4
		, LOG_WARNING	= 0x8
		, LOG_ERROR		= 0x10
		, LOG_ALERT		= 0x20
	}LOGLEVEL;

	const int MAX_LOG_SIZE = 8*1024;

	// ProtocolType
	typedef enum ProtocolType
	{
		PROTOCOL_SOTP			= 0
		, PROTOCOL_HTTP			= 0x1
		, PROTOCOL_HSOTP		= 0x2			// http sotp
		, PROTOCOL_SSL			= 0x4
		, PROTOCOL_OTHER		= 0x80000
	}PROTOCOLTYPE;

	typedef enum Http_Method
	{
		HTTP_NONE
		, HTTP_GET
		, HTTP_HEAD
		, HTTP_POST
		, HTTP_PUT
		, HTTP_DELETE
		, HTTP_OPTIONS
		, HTTP_TRACE
		, HTTP_CONNECT
	}HTTP_METHOD;

	typedef enum Http_StatusCode
	{
		STATUS_CODE_100		= 100			// 100 Continue
		, STATUS_CODE_101					// 101 Switching Protocols
		, STATUS_CODE_200	= 200			// 200 OK
		, STATUS_CODE_201					// 201 Created
		, STATUS_CODE_202					// 202 Accepted
		, STATUS_CODE_203					// 203 Non-Authoritative Information
		, STATUS_CODE_204					// 204 No Content
		, STATUS_CODE_205					// 205 Reset Content
		, STATUS_CODE_206					// 206 Partial Content
		, STATUS_CODE_300	= 300			// 300 Multiple Choices
		, STATUS_CODE_301					// 301 Moved Permanently
		, STATUS_CODE_302					// 302 Found
		, STATUS_CODE_303					// 303 See Other
		, STATUS_CODE_304					// 304 Not Modified
		, STATUS_CODE_305					// 305 Use Proxy
		, STATUS_CODE_306					// 306 (Unused)
		, STATUS_CODE_307					// 307 Temporary Redirect
		, STATUS_CODE_400	= 400			// 400 Bad Request
		, STATUS_CODE_401					// 401 Unauthorized
		, STATUS_CODE_402					// 402 Payment Required
		, STATUS_CODE_403					// 403 Forbidden
		, STATUS_CODE_404					// 404 Not Found
		, STATUS_CODE_405					// 405 Method Not Allowed
		, STATUS_CODE_406					// 406 Not Acceptable
		, STATUS_CODE_407					// 407 Proxy Authentication Required
		, STATUS_CODE_408					// 408 Request Timeout
		, STATUS_CODE_409					// 409 Conflict
		, STATUS_CODE_410					// 410 Gone
		, STATUS_CODE_411					// 411 Length Required
		, STATUS_CODE_412					// 412 Precondition Failed
		, STATUS_CODE_413					// 413 Request Entity Too Large
		, STATUS_CODE_414					// 414 Request-URI Too Long
		, STATUS_CODE_415					// 415 Unsupported Media Type
		, STATUS_CODE_416					// 416 Requested Range Not Satisfiable
		, STATUS_CODE_417					// 417 Expectation Failed
		, STATUS_CODE_500	= 500			// 500 Internal Server Error
		, STATUS_CODE_501					// 501 Not Implemented
		, STATUS_CODE_502					// 502 Bad Gateway
		, STATUS_CODE_503					// 503 Service Unavailable
		, STATUS_CODE_504					// 504 Gateway Timeout
		, STATUS_CODE_505					// 505 HTTP Version Not Supported

	}HTTP_STATUSCODE;


	inline tstring cgcGetStatusCode(HTTP_STATUSCODE statusCode)
	{
		switch (statusCode)
		{
		case STATUS_CODE_100:
			return "100 Continue";
		case STATUS_CODE_101:
			return "101 Switching Protocols";
		case STATUS_CODE_200:
			return "200 OK";
		case STATUS_CODE_201:
			return "201 Created";
		case STATUS_CODE_202:
			return "202 Accepted";
		case STATUS_CODE_203:
			return "203 Non-Authoritative Information";
		case STATUS_CODE_204:
			return "204 No Content";
		case STATUS_CODE_205:
			return "205 Reset Content";
		case STATUS_CODE_206:
			return "206 Partial Content";
		case STATUS_CODE_300:
			return "300 Multiple Choices";
		case STATUS_CODE_301:
			return "301 Moved Permanently";
		case STATUS_CODE_302:
			return "302 Found";
		case STATUS_CODE_303:
			return "303 See Other";
		case STATUS_CODE_304:
			return "304 Not Modified";
		case STATUS_CODE_305:
			return "305 Use Proxy";
		case STATUS_CODE_306:
			return "306 (Unused)";
		case STATUS_CODE_307:
			return "307 Temporary Redirect";
		case STATUS_CODE_400:
			return "400 Bad Request";
		case STATUS_CODE_401:
			return "401 Unauthorized";
		case STATUS_CODE_402:
			return "402 Payment Required";
		case STATUS_CODE_403:
			return "403 Forbidden";
		case STATUS_CODE_404:
			return "404 Not Found";
		case STATUS_CODE_405:
			return "405 Method Not Allowed";
		case STATUS_CODE_406:
			return "406 Not Acceptable";
		case STATUS_CODE_407:
			return "407 Proxy Authentication Required";
		case STATUS_CODE_408:
			return "408 Request Timeout";
		case STATUS_CODE_409:
			return "409 Conflict";
		case STATUS_CODE_410:
			return "410 Gone";
		case STATUS_CODE_411:
			return "411 Length Required";
		case STATUS_CODE_412:
			return "412 Precondition Failed";
		case STATUS_CODE_413:
			return "413 Request Entity Too Large";
		case STATUS_CODE_414:
			return "414 Request-URI Too Long";
		case STATUS_CODE_415:
			return "415 Unsupported Media Type";
		case STATUS_CODE_416:
			return "416 Requested Range Not Satisfiable";
		case STATUS_CODE_417:
			return "417 Expectation Failed";
		case STATUS_CODE_500:
			return "500 Internal Server Error";
		case STATUS_CODE_501:
			return "501 Not Implemented";
		case STATUS_CODE_502:
			return "502 Bad Gateway";
		case STATUS_CODE_503:
			return "503 Service Unavailable";
		case STATUS_CODE_504:
			return "504 Gateway Timeout";
		case STATUS_CODE_505:
			return "505 HTTP Version Not Supported";
		default:
			break;
		}
		return "";
	}

	// General Header Fields
	const tstring Http_CacheControl			= "Cache-Control";
	const tstring Http_Connection				= "Connection";
	const tstring Http_Date					= "Date";
	const tstring Http_Pragma					= "Pragma";
	const tstring Http_Trailer				= "Trailer";
	const tstring Http_TransferEncoding		= "Transfer-Encoding";
	const tstring Http_Upgrade				= "Upgrade";
	const tstring Http_Via					= "Via";

	// Request Header Fields
	const tstring Http_Accept					= "Accept";
	const tstring Http_AcceptCharset			= "Accept-Charset";
	const tstring Http_AcceptEncoding			= "Accept-Encoding";
	const tstring Http_AcceptLanguage			= "Accept-Language";
	const tstring Http_Authorization			= "Authorization";
	const tstring Http_Expect					= "Expect";
	const tstring Http_From					= "From";
	const tstring Http_Host					= "Host";
	const tstring Http_IfMatch				= "If-Match";
	const tstring Http_IfModifiedSince		= "If-Modified-Since";
	const tstring Http_IfNoneMatch			= "If-None-Match";
	const tstring Http_IfRange				= "If-Range";
	const tstring Http_IfUnmodifiedSince		= "If-Unmodified-Since";
	const tstring Http_MaxForwards			= "Max-Forwards";
	const tstring Http_ProxyAuthorization		= "Proxy-Authorization";
	const tstring Http_Range					= "Range";
	const tstring Http_Referer				= "Referer";
	const tstring Http_TE						= "TE";
	const tstring Http_UserAgent				= "User-Agent";

	// Response Header Fields
	const tstring Http_AcceptRanges			= "Accept-Ranges";
	const tstring Http_Age					= "Age";
	const tstring Http_ETag					= "ETag";
	const tstring Http_Location 				= "Location";
	const tstring Http_ProxyAuthenticate		= "Proxy-Authenticate";
	const tstring Http_RetryAfter 			= "Retry-After";
	const tstring Http_Server 				= "Server";
	const tstring Http_Vary  					= "Vary";
	const tstring Http_WWWAuthenticate		= "WWW-Authenticate";

	// Entity Header Fields
	const tstring Http_Allow					= "Allow";
	const tstring Http_ContentEncoding		= "Content-Encoding";
	const tstring Http_ContentLanguage		= "Content-Language";
	const tstring Http_ContentLength			= "Content-Length";
	const tstring Http_ContentLocation		= "Content-Location";
	const tstring Http_ContentMD5				= "Content-MD5";
	const tstring Http_ContentRange			= "Content-Range";
	const tstring Http_ContentType			= "Content-Type";
	const tstring Http_Expires				= "Expires";
	const tstring Http_LastModified			= "Last-Modified";
	const tstring Http_extensionheader		= "extension-header";

	const tstring Http_ContentDisposition		= "Content-Disposition";
	const tstring Http_KeepAlive				= "Keep-Alive";

	const tstring Http_Cookie					= "Cookie";
	const tstring Http_CookieSessionId		= "MYSESSIONID";

	// MYCP Variants
	const tstring $MYCP_ROOT_PATH				= "$MYCP_ROOT_PATH";
	const tstring $MYCP_CONF_PATH				= "$MYCP_CONF_PATH";
	const tstring $MYCP_THIRDPARTY_PATH		= "$MYCP_THIRDPARTY_PATH";

}

#endif // __cgcdef_head__
