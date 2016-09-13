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

// IncludeBase.h file here
#ifndef _INCLUDEBASE_HEAD_INCLUDED__
#define _INCLUDEBASE_HEAD_INCLUDED__
//
#ifdef _WIN32
#define _WIN32_WINNT	0x0600
#endif

// std
#include <vector>
#include <list>

// boost
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include "../ThirdParty/Boost/asio/IoService.h"
#include "../ThirdParty/Boost/asio/TcpClient.h"
#include "../ThirdParty/Boost/asio/UdpSocket.h"

// owner
//#include "CidInfo.h"

#include "../CGCBase/cgcuses.h"
#include "../CGCClass/cgcclassinclude.h"

#if (USES_RTP)
#include "../ThirdParty/libRTP/libRTP.h"
#endif // USES_RTP


#endif // _INCLUDEBASE_HEAD_INCLUDED__
