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

// cgcmultipart.h file here
#ifndef __cgcmultipart__head__
#define __cgcmultipart__head__

#include <boost/shared_ptr.hpp>
#include <string>
#include "cgcParserBase.h"
#include "cgcuploadfile.h"

namespace mycp {

class cgcMultiPart
{
public:
	typedef boost::shared_ptr<cgcMultiPart> pointer;

	const tstring& getBoundary(void) const {return m_boundary;}


	void setName(const tstring& v) {m_uploadFile->setName(v);}
	const tstring& getName(void) const {return m_uploadFile->getName();}
	void setFileName(const tstring& v) {m_uploadFile->setFileName(v);}
	const tstring& getFileName(void) const {return m_uploadFile->getFileName();}
	void setContentType(const tstring& v) {m_uploadFile->setContentType(v);}

	cgcUploadFile::pointer getUploadFile(void) const {return m_uploadFile;}

	bool open(const tstring & savepath)
	{
		if (m_stream.is_open()) return true;

		tstring filepath(savepath);
		filepath.append("/");
		filepath.append(m_boundary);
		char lpszBuffer[48];
		sprintf(lpszBuffer,"_%lld_%d",(mycp::bigint)time(0),rand());
		filepath.append(lpszBuffer);
		//filepath.append(m_uploadFile->getFileName());

		m_uploadFile->setFilePath(filepath);
		m_stream.open(filepath.c_str(), std::ios::out|std::ios::binary);
		return m_stream.is_open();
	}
	bool write(const char * data, size_t size)
	{
		if (!m_stream.is_open()) return false;

		const size_t writeSize = m_uploadFile->getFileSize() + size;
		m_uploadFile->setFileSize(writeSize);
		m_stream.clear();
		m_stream.write(data, (std::streamsize)size);
		m_stream.flush();
		return true;
	}
	void close(void)
	{
		m_stream.close();
	}

	cgcMultiPart::pointer copy(void) const
	{
		cgcMultiPart::pointer result = cgcMultiPart::pointer(new cgcMultiPart(m_boundary));
		return result;
	}

	const tfstream& fstream(void) const {return m_stream;}
	tfstream& fstream(void) {return m_stream;}

	void setParser(cgcParserBase::pointer v) {m_parser = v;}
	cgcParserBase::pointer getParser(void) const {return m_parser;}

	cgcMultiPart(const tstring& boundary)
		: m_boundary(boundary)
	{
		m_uploadFile = CGC_UPLOADFILE();
	}
	virtual ~cgcMultiPart(void)
	{
		close();
	}
private:
	tstring m_boundary;
	cgcUploadFile::pointer m_uploadFile;
	tfstream m_stream;
	cgcParserBase::pointer	m_parser;
};

const cgcMultiPart::pointer cgcNullMultiPart;

#define CGC_MULTIPART(b) cgcMultiPart::pointer(new cgcMultiPart(b))

} // namespace mycp

#endif // __cgcmultipart__head__
