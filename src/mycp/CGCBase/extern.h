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

// extern.h file here
#ifndef __extern_head__
#define __extern_head__

#include "cgcApplication.h"
#include "cgcSystem.h"
#include "cgcServices.h"

namespace mycp {

extern cgcApplication::pointer theApplication;
extern cgcSystem::pointer theSystem;
extern cgcServiceManager::pointer theServiceManager;

#define CGC_LOG(X) theApplication->log X
#define CGC_APP_PARAMETER(N) theApplication->getInitParameter(N)
#define CGC_APP_VALUE(N, D) theApplication->getInitParameterValue(N, D)
#define CGC_APP_VALUE2(N, D) theApplication->getInitParameterValue2(N, D)

#define CGC_SYS_PARAMETER(N) theSystem->getInitParameter(N)
#define CGC_SYS_VALUE(N, D) theSystem->getInitParameterValue(N, D)
//#define CGC_SYS_VALUE2(N, D) gSystem->getInitParameterValue2(N, D)

} // namespace mycp


#endif // __extern_head__
