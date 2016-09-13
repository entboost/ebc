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

#ifdef WIN32
#pragma warning(disable:4800)
#include <windows.h>
BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}
#endif // WIN32

#include <boost/shared_ptr.hpp>
#include <boost/date_time.hpp>

// cgc head
#include <CGCBase/app.h>
#include <CGCBase/cgcService.h>
using namespace cgc;
using namespace boost::gregorian;  
using namespace boost::local_time;  
using namespace boost::posix_time; 

class CDateTimeService
	: public cgcServiceInterface
{
public:
	typedef boost::shared_ptr<CDateTimeService> pointer;

	static CDateTimeService::pointer create(void)
	{
		return CDateTimeService::pointer(new CDateTimeService());
	}

	virtual bool initService(cgcValueInfo::pointer parameter = cgcNullValueInfo)
	{
		m_timezone = "+8";
		m_tz = boost::local_time::time_zone_ptr(new boost::local_time::posix_time_zone(m_timezone.c_str())); 
		return cgcServiceInterface::initService();
	}
protected:
	virtual tstring serviceName(void) const {return _T("DATETIMESERVICE");}

	virtual bool callService(const tstring& function, const cgcValueInfo::pointer& inParam, cgcValueInfo::pointer outParam)
	{
		if (function == "fmtime")
		{
			if (inParam.get() == NULL || outParam.get() == NULL) return false;

			tstring sFormatString = "%Y-%m-%d %H:%M:%S";
			time_t tFormatTime = time(0);
			if (inParam->getType() == cgcValueInfo::TYPE_MAP)
			{
				cgcValueInfo::pointer findValue;
				if (inParam->getMap().find("format", findValue))
				{
					sFormatString = findValue->toString();
				}

				if (inParam->getMap().find("timestamp", findValue))
				{
					findValue->totype(cgcValueInfo::TYPE_BIGINT);
					tFormatTime = findValue->getBigInt();
				}
			}else
			{
				sFormatString = inParam->toString();
			}

			boost::posix_time::ptime pt = from_time_t(tFormatTime);
			boost::local_time::local_date_time dt(pt, m_tz); 

			time_facet * facet = new time_facet(sFormatString.c_str());
			std::stringstream ss;
			ss.imbue(std::locale(std::cout.getloc(), facet));
			ss << dt.local_time();

			char buffer[50];
			memset(buffer, 0, 50);
			ss.get(buffer, 50);
			//delete facet;

			outParam->totype(cgcValueInfo::TYPE_STRING);
			outParam->setStr((tstring)buffer);
//
//ptime second_clock::local_time()
//
//	取得本地时间，秒级精度，基于计算机的时区设置。
//
//ptime t(second_clock::local_time());
//
//ptime second_clock::universal_time()
//
//	取得 UTC 时间。
//
//ptime t(second_clock::universal_time())
//
//ptime microsec_clock::local_time()
//
//	使用次秒级精度的时钟取得本地时间。在 Unix 系统上，以 GetTimeOfDay 来实现。在大多数 Win32 平台上则以 ftime 实现。Win32 系统通常不能通过此 API 达到微秒级精度。如果你的应用程序需要更高的精度，请测试你的平台，以确定可达到的精度。
//
//ptime t(microsec_clock::local_time());
//
//ptime microsec_clock::universal_time()
//
//	使用次秒级精度的时钟取得 UTC 时间。在 Unix 系统上，以 GetTimeOfDay 来实现。在大多数 Win32 平台上则以 ftime 实现。Win32 系统通常不能通过此 API 达到微秒级精度。如果你的应用程序需要更高的精度，请测试你的平台，以确定可达到的精度。
//
//ptime t(microsec_clock::universal_time());

//ptime from_time_t(time_t t);
//
//	将 time_t 转换为 ptime.
//
//ptime t = from_time_t(tt);
//
//ptime from_ftime<ptime>(FILETIME ft);
//
//	从 FILETIME 结构创建 ptime 对象。
//
//ptime t = from_ftime<ptime>(ft);

			//std::cout.imbue(std::locale(std::cout.getloc(), facet));
			//std::cout << d << std::endl;

			//boost::local_time::time_zone_ptr tz(new boost::local_time::posix_time_zone("CET+0")); 
			//boost::posix_time::ptime pt1(boost::gregorian::date(2009, 1, 5), boost::posix_time::time_duration(12, 0, 0)); 
			//boost::local_time::local_date_time dt1(pt1, tz); 

			//boost::gregorian::date d = boost::gregorian::day_clock::universal_day(); 
			//boost::posix_time::ptime pt = boost::posix_time::second_clock::universal_time();


			//const time_t tt = time(NULL);  
			//tm* pLocalTime = localtime(&tt);  
			//date record_date(pLocalTime->tm_year+1900,pLocalTime->tm_mon+1,pLocalTime->tm_mday);  
			//time_duration record_time(pLocalTime->tm_hour,pLocalTime->tm_min,pLocalTime->tm_sec);  
			/* 
			*本地当前时间 
			*/  
			//local_date_time time_t_now(record_date,  
			//	record_time,  
			//	time_zone,  
			//	local_date_time::NOT_DATE_TIME_ON_ERROR);  

			//boost::posix_time::ptime pt(record_date, record_time);
			//pt.zone_as_posix_string(
			//boost::gregorian::date 
			//boost::gregorian::date d(2009, 1, 7); 
		}else if (function == "fmdate")
		{
			if (inParam.get() == NULL || outParam.get() == NULL) return false;

			tstring sFormatString = "%Y-%m-%d";
			time_t tFormatTime = time(0);
			if (inParam->getType() == cgcValueInfo::TYPE_MAP)
			{
				cgcValueInfo::pointer findValue;
				if (inParam->getMap().find("format", findValue))
				{
					sFormatString = findValue->toString();
				}

				if (inParam->getMap().find("timestamp", findValue))
				{
					findValue->totype(cgcValueInfo::TYPE_BIGINT);
					tFormatTime = findValue->getBigInt();
				}
			}else
			{
				sFormatString = inParam->toString();
			}

			boost::posix_time::ptime pt = from_time_t(tFormatTime);
			boost::local_time::local_date_time dt(pt, m_tz); 

			date_facet * facet = new date_facet(sFormatString.c_str());
			std::stringstream ss;
			ss.imbue(std::locale(std::cout.getloc(), facet));
			ss << dt.date();

			char buffer[50];
			memset(buffer, 0, 50);
			ss.get(buffer, 50);
			//delete facet;

			outParam->totype(cgcValueInfo::TYPE_STRING);
			outParam->setStr((tstring)buffer);
		}else if (function == "strtodate")
		{
			// 20110101 or 2011-1-01 or 2011/01/01 or 2003-Jan-25
			if (inParam.get() == NULL || outParam.get() == NULL) return false;
			tstring sDateString = inParam->toString();
			boost::gregorian::date d;
			try
			{
				d = boost::gregorian::from_simple_string(sDateString);
			}catch(std::exception const &e1)
			{
				try
				{
					d = boost::gregorian::from_undelimited_string(sDateString);
				}catch(std::exception const &e2)
				{
					return false;
				}
			}
			tm tmMakeTime;
			memset(&tmMakeTime, 0, sizeof(tm));
			tmMakeTime.tm_year = d.year() - 1900;
			tmMakeTime.tm_mon = d.month() - 1;
			tmMakeTime.tm_mday = d.day();
			time_t mt = mktime(&tmMakeTime);
			outParam->totype(cgcValueInfo::TYPE_BIGINT);
			outParam->setBigInt(mt);
		}else if (function == "strtotime")
		{
			// 2011-01-10 0:0:0
			if (inParam.get() == NULL || outParam.get() == NULL) return false;
			tstring sTimeString = inParam->toString();
			boost::posix_time::ptime t;
			try
			{
				t = boost::posix_time::time_from_string(sTimeString);
			}catch(std::exception const &e1)
			{
				try
				{
					t = boost::posix_time::from_iso_string(sTimeString);
				}catch(std::exception const &e2)
				{
					return false;
				}
			}
			tm tmMakeTime = boost::posix_time::to_tm(t);
			time_t mt = mktime(&tmMakeTime);
			outParam->totype(cgcValueInfo::TYPE_BIGINT);
			outParam->setBigInt(mt);
		}else if (function == "timezone_get")
		{
			if (outParam.get() == NULL) return false;
			outParam->totype(cgcValueInfo::TYPE_STRING);
			outParam->setStr(m_timezone);
		}else if (function == "timezone_set")
		{
			if (inParam.get() == NULL) return false;

			// "UTC" or "Europe/Paris"。
			// "+8" "CET+0" "CET+8"
			const tstring & sInString = inParam->getStr();
			try
			{
				boost::local_time::time_zone_ptr tz = boost::local_time::time_zone_ptr(new boost::local_time::posix_time_zone(sInString));
				m_tz = tz;
				m_timezone = sInString;
			}catch(...)
			{
				return false;
			}
		}else if (function == "localtime")
		{
			if (outParam.get() == NULL) return false;

			//* "tm_sec" - 秒数
			//* "tm_min" - 分钟数
			//* "tm_hour" - 小时
			//* "tm_mday" - 月份中的第几日
			//* "tm_mon" - 年份中的第几个月，从 0 开始表示一月
			//* "tm_year" - 年份，从 1900 开始
			//* "tm_wday" - 星期中的第几天
			//* "tm_yday" - 一年中的第几天
			//* "tm_isdst" - 夏令时当前是否生效

			tm * tmLocalTime = NULL;
			time_t t = 0;
			if (inParam.get() == NULL)
				t = time(0);
			else
				t = inParam->getBigInt();
			tmLocalTime = localtime(&t);
			outParam->totype(cgcValueInfo::TYPE_MAP);
			outParam->reset();
			outParam->insertMap("tm_year", CGC_VALUEINFO(tmLocalTime->tm_year+1900));
			outParam->insertMap("tm_mon", CGC_VALUEINFO(tmLocalTime->tm_mon+1));
			outParam->insertMap("tm_mday", CGC_VALUEINFO(tmLocalTime->tm_mday));
			outParam->insertMap("tm_hour", CGC_VALUEINFO(tmLocalTime->tm_hour));
			outParam->insertMap("tm_min", CGC_VALUEINFO(tmLocalTime->tm_min));
			outParam->insertMap("tm_sec", CGC_VALUEINFO(tmLocalTime->tm_sec));
			outParam->insertMap("tm_wday", CGC_VALUEINFO(tmLocalTime->tm_wday));
			outParam->insertMap("tm_yday", CGC_VALUEINFO(tmLocalTime->tm_yday));
			outParam->insertMap("tm_isdst", CGC_VALUEINFO(tmLocalTime->tm_isdst));
		}else if (function == "mktime")
		{
			if (inParam.get() == NULL || outParam.get() == NULL) return false;
			tm tmMakeTime;
			memset(&tmMakeTime, 0, sizeof(tm));
			if (inParam->getType() == cgcValueInfo::TYPE_MAP)
			{
				cgcValueInfo::pointer findValue;
				if (inParam->getMap().find("tm_year", findValue))
				{
					tmMakeTime.tm_year = findValue->getInt() - 1900;
				}
				if (inParam->getMap().find("tm_mon", findValue))
				{
					tmMakeTime.tm_mon = findValue->getInt() - 1;
				}
				if (inParam->getMap().find("tm_mday", findValue))
				{
					tmMakeTime.tm_mday = findValue->getInt();
				}
				if (inParam->getMap().find("tm_hour", findValue))
				{
					tmMakeTime.tm_hour = findValue->getInt();
				}
				if (inParam->getMap().find("tm_min", findValue))
				{
					tmMakeTime.tm_min = findValue->getInt();
				}
				if (inParam->getMap().find("tm_sec", findValue))
				{
					tmMakeTime.tm_sec = findValue->getInt();
				}
			}

			time_t mt = mktime(&tmMakeTime);
			outParam->totype(cgcValueInfo::TYPE_BIGINT);
			outParam->setBigInt(mt);

		}else if (function == "time")
		{
			if (outParam.get() == NULL) return false;
			time_t localt = time(0);
			outParam->totype(cgcValueInfo::TYPE_BIGINT);
			outParam->setBigInt(localt);
		}else if (function == "checkdate")
		{
			if (inParam.get() == NULL || outParam.get() == NULL) return false;

			tm tmMakeTime;
			memset(&tmMakeTime, 0, sizeof(tm));
			if (inParam->getType() == cgcValueInfo::TYPE_MAP)
			{
				cgcValueInfo::pointer findValue;
				if (inParam->getMap().find("year", findValue))
				{
					tmMakeTime.tm_year = findValue->getInt()-1900;
				}
				if (inParam->getMap().find("month", findValue))
				{
					tmMakeTime.tm_mon = findValue->getInt() - 1;
				}
				if (inParam->getMap().find("mday", findValue))
				{
					tmMakeTime.tm_mday = findValue->getInt();
				}
			}

			outParam->totype(cgcValueInfo::TYPE_BOOLEAN);
			try
			{
				boost::gregorian::date d = date_from_tm(tmMakeTime);
				outParam->setBoolean(true);
			}catch(std::exception const &e1)
			{
				outParam->setBoolean(false);
			}
		}else
		{
			return false;
		}

		return true;
	}

	private:
		boost::local_time::time_zone_ptr m_tz;
		tstring m_timezone;

};

CDateTimeService::pointer gServicePointer;

extern "C" bool CGC_API CGC_Module_Init(void)
{
	return true;
}

extern "C" void CGC_API CGC_Module_Free(void)
{
	gServicePointer.reset();
}

extern "C" void CGC_API CGC_GetService(cgcServiceInterface::pointer & outService, const cgcValueInfo::pointer& parameter)
{
	if (gServicePointer.get() == NULL)
	{
		gServicePointer = CDateTimeService::create();
		gServicePointer->initService();
	}
	outService = gServicePointer;
}
