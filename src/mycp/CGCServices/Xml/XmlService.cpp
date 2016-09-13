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

#include <boost/asio.hpp>
#ifdef WIN32
#pragma warning(disable:4996)
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

#include <string.h>
#include <stdarg.h>
#include <boost/shared_ptr.hpp>
#include <CGCBase/app.h>
#include <CGCBase/cgcService.h>
using namespace cgc;

// cgc head
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/foreach.hpp>
using boost::property_tree::ptree;

cgcAttributes::pointer theAppAttributes;

class CXmlService
	: public cgcServiceInterface
{
public:
	typedef boost::shared_ptr<CXmlService> pointer;

	static CXmlService::pointer create(void)
	{
		return CXmlService::pointer(new CXmlService());
	}

	virtual tstring serviceName(void) const {return _T("XMLSERVICE");}

protected:
	virtual bool initService(cgcValueInfo::pointer parameter = cgcNullValueInfo)
	{

		return cgcServiceInterface::initService();
	}
	virtual void finalService(void)
	{
		cgcServiceInterface::finalService();
	}

	virtual cgcAttributes::pointer getAttributes(void) const {return theAppAttributes;}
	virtual bool callService(const tstring& function, const cgcValueInfo::pointer& inParam, cgcValueInfo::pointer outParam)
	{
		if (function == "load_xml")
		{
			if (inParam.get() == NULL) return false;

			const tstring & sInString = inParam->getStr();

			try
			{
				m_ptree.clear();
				boost::asio::streambuf b;
				b.sputn(sInString.c_str(), (std::streamsize)sInString.size());
				std::istream stream(&b);
				read_xml(stream, m_ptree);
			}catch(...)
			{
				return false;
			}

		}else if (function == "load_in")
		{
			if (inParam.get() == NULL) return false;
			if (inParam->getType() != cgcValueInfo::TYPE_VECTOR || inParam->getVector().size() < 3) return false;
			tstring sChildPath = inParam->getVector()[0]->toString();
			tstring sChildItem = inParam->getVector()[1]->toString();

			m_listString.clear();
			for (size_t i=2; i<inParam->getVector().size(); i++)
			{
				m_listString.push_back(inParam->getVector()[i]->toString());
			}

			m_nCount = 0;
			try
			{
				BOOST_FOREACH(const ptree::value_type &v, m_ptree.get_child(sChildPath))
					Insert(sChildItem, v);

			}catch(...)
			{}

		}else if (function == "getindex")
		{
			if (inParam.get() == NULL || outParam.get() == NULL) return false;

			const tstring & sInString = inParam->getStr();
			std::vector<cgcValueInfo::pointer> v;
			if (!theAppAttributes->getProperty(sInString, v))
				return false;

			outParam->totype(cgcValueInfo::TYPE_VECTOR);
			outParam->reset();
			for (size_t i=0; i<v.size(); i++)
				outParam->addVector(v[i]);
		}else if (function == "get")
		{

			if (inParam.get() == NULL || outParam.get() == NULL) return false;

			const tstring &sPath = inParam->getStr();
			tstring sValue = m_ptree.get(sPath.c_str(), "");
			outParam->totype(cgcValueInfo::TYPE_STRING);
			outParam->setStr(sValue);
		}else
		{
			return false;
		}

		return true;
	}

	void Insert(const tstring sGetItem, const boost::property_tree::ptree::value_type & v)
	{
		if (v.first.compare(sGetItem) == 0)
		{
			char buffer[10];
			sprintf(buffer, "%d", m_nCount);
			theAppAttributes->delProperty((tstring)buffer);
			std::list<tstring>::iterator iter;
			for (iter=m_listString.begin(); iter!=m_listString.end(); iter++)
			{
				tstring sPath = *iter;
				tstring sValue = v.second.get(sPath, "");
				theAppAttributes->setProperty((tstring)buffer, CGC_VALUEINFO(sValue));
			}
			m_nCount++;
		}
	}

	//~CXmlService(void)
	//{
	//	m_ptree.clear();
	//}
private:
	ptree m_ptree;
	std::list<tstring> m_listString;
	int					m_nCount;
};

CXmlService::pointer gServicePointer;

extern "C" bool CGC_API CGC_Module_Init(void)
{
	theAppAttributes = theApplication->getAttributes(true);
	assert (theAppAttributes.get() != NULL);
	return true;
}

extern "C" void CGC_API CGC_Module_Free(void)
{
	theAppAttributes.reset();
	gServicePointer.reset();
}

extern "C" void CGC_API CGC_GetService(cgcServiceInterface::pointer & outService, const cgcValueInfo::pointer& parameter)
{
	if (gServicePointer.get() == NULL)
	{
		gServicePointer = CXmlService::create();
	}
	outService = gServicePointer;
}
