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

// cgcuploadfile.h file here
#ifndef __cgcuploadfile__head__
#define __cgcuploadfile__head__

#include <boost/shared_ptr.hpp>
#include "cgcSmartString.h"

namespace mycp {

class cgcUploadFile
{
public:
	typedef boost::shared_ptr<cgcUploadFile> pointer;

	void setName(const tstring& v) {m_name = v;}
	const tstring& getName(void) const {return m_name;}
	void setFileName(const tstring& v) {m_fileName = v;}
	const tstring& getFileName(void) const {return m_fileName;}
	void setContentType(const tstring& v) {m_contentType = v;}
	const tstring& getContentType(void) const {return m_contentType;}
	void setFileSize(size_t v) {m_fileSize = v;}
	size_t getFileSize(void) const {return m_fileSize;}

	void setFilePath(const tstring& v) {m_filepath = v;}
	const tstring& getFilePath(void) const {return m_filepath;}

	cgcUploadFile(void)
		: m_name(""), m_fileName(""), m_contentType("")
		, m_fileSize(0), m_filepath("")
	{}

private:
	tstring m_name;
	tstring m_fileName;
	tstring m_contentType;
	size_t m_fileSize;
	tstring m_filepath;
};
const cgcUploadFile::pointer cgcNullUploadFile;

#define CGC_UPLOADFILE() cgcUploadFile::pointer(new cgcUploadFile())

} // namespace mycp

#endif // __cgcuploadfile__head__
