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

// cgcConfiguration.h file here
#ifndef __cgcconfiguration_head__
#define __cgcconfiguration_head__

#include <boost/shared_ptr.hpp>
#include <CGCBase/cgcService.h>

namespace cgc{

class cgcConfiguration
	: public cgcServiceInterface
{
public:
	typedef boost::shared_ptr<cgcConfiguration> pointer;

	enum ConfigurationType
	{
		XmlConfiguration	= 1
		, IniConfiguration
	};

	virtual tstring get(const tstring& path, const tstring& defaultvalue = _T("")) const = 0;
	virtual int get(const tstring& path, int defaultvalue) const = 0;
	virtual float get(const tstring& path, float defaultvalue) const = 0;

	virtual void set(const tstring& path, const tstring& value) = 0;
	virtual void set(const tstring& path, int value) = 0;
	virtual void set(const tstring& path, float value) = 0;

	// ?
	virtual void del(const tstring& path) = 0;

	virtual bool savefile(void) = 0;
	virtual bool saveto_xml(const tstring& xmlfile) = 0;
	virtual bool saveto_ini(const tstring& inifile) = 0;

	virtual const tstring& filename(void) const = 0;
	virtual ConfigurationType configuration_type(void) const = 0;

};

#define CGC_CONFIGURATIONSERVICE_DEF(s) boost::static_pointer_cast<cgcConfiguration, cgcServiceInterface>(s)

} // cgc namespace

#endif // __cgcconfiguration_head__
