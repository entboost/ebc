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
#pragma warning(disable:4584 4996)
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

// cgc head
#include <CGCBase/app.h>
#include "cgcConfiguration.h"
using namespace cgc;

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/ini_parser.hpp>
//#include <boost/foreach.hpp>
using boost::property_tree::ptree;


class CConfigurationService
	: public cgcConfiguration
{
public:
	typedef boost::shared_ptr<CConfigurationService> pointer;

	static CConfigurationService::pointer create(const tstring& filename, cgcConfiguration::ConfigurationType type)
	{
		return CConfigurationService::pointer(new CConfigurationService(filename, type));
	}

	virtual tstring serviceName(void) const {return _T("CONFIGURATIONSERVICE");}
	virtual bool initService(cgcValueInfo::pointer parameter = cgcNullValueInfo)
	{
		if (this->isServiceInited()) return true;

		namespace fs = boost::filesystem;
		fs::path pathFile(m_filename);
		//fs::path pathFile(m_filename, fs::native);
		if (boost::filesystem::exists(pathFile))
		{
			if (m_type == cgcConfiguration::XmlConfiguration)
			{
				read_xml(m_filename, m_ptree);
			}else if (m_type == cgcConfiguration::IniConfiguration)
			{
				read_ini(m_filename, m_ptree);
			}else
			{
				return false;
			}
		}

		return cgcServiceInterface::initService();
	}
	virtual void finalService(void)
	{
		if (!isServiceInited()) return;
		cgcServiceInterface::finalService();

		m_ptree.clear();
		m_filename.clear();
	}

protected:
	virtual tstring get(const tstring& path, const tstring& defaultvalue) const
	{
		if (!isServiceInited()) return defaultvalue;
		return m_ptree.get(path, defaultvalue);
	}
	virtual int get(const tstring& path, int defaultvalue) const
	{
		if (!isServiceInited()) return defaultvalue;
		return m_ptree.get(path, defaultvalue);
	}
	virtual float get(const tstring& path, float defaultvalue) const
	{
		if (!isServiceInited()) return defaultvalue;
		return m_ptree.get(path, defaultvalue);
	}

	virtual void set(const tstring& path, const tstring& value)
	{
		if (isServiceInited())
			m_ptree.put(path, value);
	}
	virtual void set(const tstring& path, int value)
	{
		if (isServiceInited())
			m_ptree.put(path, value);
	}
	virtual void set(const tstring& path, float value)
	{
		if (isServiceInited())
			m_ptree.put(path, value);
	}
	// ?
	virtual void del(const tstring& path)
	{
		if (isServiceInited())
			m_ptree.erase(path);
	}

	virtual bool savefile(void)
	{
		if (!isServiceInited()) return false;
		if (m_filename.empty()) return false;

		if (m_type == cgcConfiguration::XmlConfiguration)
		{
			write_xml(m_filename, m_ptree);
		}else if (m_type == cgcConfiguration::IniConfiguration)
		{
			write_ini(m_filename, m_ptree);
		}else
		{
			return false;
		}

		return true;
	}
	virtual bool saveto_xml(const tstring& xmlfile)
	{
		if (!isServiceInited()) return false;
		write_xml(xmlfile, m_ptree);

		if (m_filename.empty())
		{
			m_filename = xmlfile;
			m_type = cgcConfiguration::XmlConfiguration;
		}
		return true;
	}
	virtual bool saveto_ini(const tstring& inifile)
	{
		if (!isServiceInited()) return false;
		write_ini(inifile, m_ptree);

		if (m_filename.empty())
		{
			m_filename = inifile;
			m_type = cgcConfiguration::IniConfiguration;
		}
		return true;
	}

	virtual const tstring& filename(void) const {return m_filename;}
	virtual ConfigurationType configuration_type(void) const {return m_type;}

public:
	CConfigurationService(const tstring& filename, cgcConfiguration::ConfigurationType type)
		: m_filename(filename), m_type(type)
	{
	}
	~CConfigurationService(void)
	{
		finalService();
	}

private:
	tstring m_filename;
	cgcConfiguration::ConfigurationType m_type;
	ptree m_ptree;
};

const int ATTRIBUTE_NAME = 1;
cgcAttributes::pointer theAppAttributes;

extern "C" bool CGC_API CGC_Module_Init(void)
{
	theAppAttributes = theApplication->getAttributes(true);
	assert (theAppAttributes.get() != NULL);
	return true;
}

extern "C" void CGC_API CGC_Module_Free(void)
{
	VoidObjectMapPointer mapLogServices = theAppAttributes->getVoidAttributes(ATTRIBUTE_NAME, false);
	if (mapLogServices.get() != NULL)
	{
		CObjectMap<void*>::iterator iter;
		for (iter=mapLogServices->begin(); iter!=mapLogServices->end(); iter++)
		{
			CConfigurationService::pointer service = CGC_OBJECT_CAST<CConfigurationService>(iter->second);
			if (service.get() != NULL)
			{
				service->finalService();
			}
		}
	}
	theAppAttributes.reset();
}

extern "C" void CGC_API CGC_GetService(cgcServiceInterface::pointer & outService, const cgcValueInfo::pointer& parameter)
{
	if (parameter.get() == NULL) return;
	if (parameter->getType() != cgcValueInfo::TYPE_VECTOR || parameter->getVector().size() < 2) return;

	tstring filename = parameter->getVector()[0]->getStr();
	cgcConfiguration::ConfigurationType type = (cgcConfiguration::ConfigurationType)parameter->getVector()[1]->getInt();

	CConfigurationService::pointer serviceCreate = CConfigurationService::create(filename, type);
	if (!serviceCreate->initService()) return;

	outService = serviceCreate;
	theAppAttributes->setAttribute(ATTRIBUTE_NAME, outService.get(), serviceCreate);
}

extern "C" void CGC_API CGC_ResetService(const cgcServiceInterface::pointer & inService)
{
	if (inService.get() == NULL) return;
	theAppAttributes->removeAttribute(ATTRIBUTE_NAME, inService.get());
	inService->finalService();
}
