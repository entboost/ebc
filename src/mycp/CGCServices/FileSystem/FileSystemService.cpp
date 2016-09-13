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
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>

#ifdef _UNICODE
typedef boost::filesystem::wpath	tpath;
#else
typedef boost::filesystem::path		tpath;
#endif // _UNICODE

// cgc head
#include <CGCBase/app.h>
#include <CGCBase/cgcService.h>
//#include "cgcFileSystem.h"
using namespace cgc;

class CFileSystemService
	//: public cgcFileSystem
	: public cgcServiceInterface
{
public:
	typedef boost::shared_ptr<CFileSystemService> pointer;

	static CFileSystemService::pointer create(void)
	{
		return CFileSystemService::pointer(new CFileSystemService());
	}

protected:
	virtual tstring serviceName(void) const {return _T("FILESYSTEMSERVICE");}

	virtual bool callService(const tstring& function, const cgcValueInfo::pointer& inParam, cgcValueInfo::pointer outParam)
	{
		if (function == "exists")
		{
			if (inParam.get() == NULL || outParam.get() == NULL) return false;
			outParam->totype(cgcValueInfo::TYPE_BOOLEAN);
			outParam->setBoolean(exists(inParam->getStr()));
		}else if (function == "size")
		{
			if (inParam.get() == NULL || outParam.get() == NULL) return false;
			outParam->totype(cgcValueInfo::TYPE_BIGINT);
			outParam->setBigInt((bigint)file_size(inParam->getStr()));
		}else if (function == "delete")
		{
			if (inParam.get() == NULL) return false;
			return remove(inParam->getStr());
		}else if (function == "delete_all")
		{
			if (inParam.get() == NULL) return false;
			return remove_all(inParam->getStr());
		}else if (function == "rename")
		{
			if (inParam.get() == NULL || inParam->getType() != cgcValueInfo::TYPE_VECTOR || inParam->size() != 2) return false;
			rename(inParam->getVector()[0]->getStr(), inParam->getVector()[1]->getStr());
		}else if (function == "copyto")
		{
			if (inParam.get() == NULL || inParam->getType() != cgcValueInfo::TYPE_VECTOR || inParam->size() != 2) return false;
			remove(inParam->getVector()[1]->getStr());
			copy_file(inParam->getVector()[0]->getStr(), inParam->getVector()[1]->getStr());
		}else
		{
			return false;
		}
		return true;
	}

	virtual bool exists(const tstring & path) const {
		namespace fs = boost::filesystem;
		tpath src_path(path);
		//tpath src_path(path, fs::native);
		return fs::exists(src_path);
	}
	virtual bool is_directory(const tstring & path) const {
		namespace fs = boost::filesystem;
		tpath src_path(path);
		//tpath src_path(path, fs::native);
		return fs::is_directory(src_path);
	}
	virtual bool is_regular_file(const tstring & path) const {
		namespace fs = boost::filesystem;
		tpath src_path(path);
		//tpath src_path(path, fs::native);
		return fs::is_regular_file(src_path);
	}
	virtual bool is_other(const tstring & path) const {
		namespace fs = boost::filesystem;
		tpath src_path(path);
		//tpath src_path(path, fs::native);
		return fs::is_other(src_path);
	}
	virtual bool is_symlink(const tstring & path) const {
		namespace fs = boost::filesystem;
		tpath src_path(path);
		//tpath src_path(path, fs::native);
		return fs::is_symlink(src_path);

	}
	virtual bool is_empty(const tstring & path) const {
		namespace fs = boost::filesystem;
		tpath src_path(path);
		//tpath src_path(path, fs::native);
		return fs::is_empty(src_path);
	}
	virtual bool equivalent(const tstring & path1, const tstring & path2) const {
		namespace fs = boost::filesystem;
		tpath src_path(path1);
		//tpath src_path(path1, fs::native);
		tpath des_path(path2);
		//tpath des_path(path2, fs::native);
		return fs::equivalent(src_path, des_path);
	}

	virtual boost::uintmax_t file_size(const tstring & path) const {
		namespace fs = boost::filesystem;
		tpath src_path(path);
		//tpath src_path(path, fs::native);
		return fs::file_size(src_path);
	}
	virtual std::time_t last_write_time(const tstring & path) const {
		namespace fs = boost::filesystem;
		tpath src_path(path);
		//tpath src_path(path, fs::native);
		return fs::last_write_time(src_path);
	}
	virtual void last_write_time(const tstring & path, std::time_t new_time) const {
		namespace fs = boost::filesystem;
		tpath src_path(path);
		//tpath src_path(path, fs::native);
		fs::last_write_time(src_path, new_time);
	}

	virtual bool create_directory(const tstring & dir_path) const {
		namespace fs = boost::filesystem;
		tpath src_path(dir_path);
		//tpath src_path(dir_path, fs::native);
		return fs::create_directory(src_path);
	}
	virtual void create_symlink(const tstring & to_path, const tstring & from_path) const {
		namespace fs = boost::filesystem;
		tpath toPath(to_path);
		//tpath toPath(to_path, fs::native);
		tpath fromPath(from_path);
		//tpath fromPath(from_path, fs::native);
		fs::create_symlink(toPath, fromPath);
	}

	virtual bool remove(const tstring & path) const {
		namespace fs = boost::filesystem;
		tpath src_path(path);
		//tpath src_path(path, fs::native);
		return fs::remove(src_path);
	}
	virtual bool remove_all(const tstring & path) const {
		namespace fs = boost::filesystem;
		tpath src_path(path);
		//tpath src_path(path, fs::native);
		return fs::remove_all(src_path);
	}

	virtual void rename(const tstring & from_path, const tstring & to_path) const {
		namespace fs = boost::filesystem;
		tpath fromPath(from_path);
		//tpath fromPath(from_path, fs::native);
		tpath toPath(to_path);
		//tpath toPath(to_path, fs::native);
		return fs::rename(fromPath, toPath);
	}
	virtual void copy_file(const tstring & from_path, const tstring & to_path) const {
		namespace fs = boost::filesystem;
		tpath fromPath(from_path);
		//tpath fromPath(from_path, fs::native);
		tpath toPath(to_path);
		//tpath toPath(to_path, fs::native);
		return fs::copy_file(fromPath, toPath);
	}

	virtual tstring initial_path(void) const {
		namespace fs = boost::filesystem;
		tpath initialPath( fs::initial_path());
		return initialPath.string();
	}
	virtual tstring current_path(void) const {
		namespace fs = boost::filesystem;
		tpath currentPath( fs::current_path());
		return currentPath.string();
	}
	virtual void current_path(const tstring & path) const {
		namespace fs = boost::filesystem;
		tpath currentPath(path);
		//tpath currentPath(path, fs::native);
		return fs::current_path(currentPath);
	}

};

CFileSystemService::pointer gServicePointer;

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
		gServicePointer = CFileSystemService::create();
	}
	outService = gServicePointer;
}
