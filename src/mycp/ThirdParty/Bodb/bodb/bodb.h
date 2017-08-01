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

// bodb.h file here
#ifndef __bodb_h__
#define __bodb_h__

#include "dbservice.h"

#ifdef WIN32
#ifdef _DEBUG
#pragma comment(lib, "bodbd.lib")
#else // _DEBUG
#pragma comment(lib, "bodb.lib")
#endif // _DEBUG
#endif // WIN32

#endif // __bodb_h__