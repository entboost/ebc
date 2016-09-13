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

// cgcCommunications.h file here
#ifndef _cgccommunications_head_included__
#define _cgccommunications_head_included__

#include <boost/shared_ptr.hpp>
#include "cgcdef.h"
#include "cgcServices.h"

namespace mycp {

// cgcRemote
class cgcRemote
{
public:
	typedef boost::shared_ptr<cgcRemote> pointer;

	virtual int getProtocol(void) const = 0;
	virtual int getServerPort(void) const = 0;
	virtual const tstring & getServerAddr(void) const = 0;	// ip
	virtual unsigned long getCommId(void) const = 0;
	virtual unsigned long getRemoteId(void) const = 0;
	virtual unsigned long getIpAddress(void) const = 0;

	virtual tstring getScheme(void) const = 0;
	virtual const tstring & getRemoteAddr(void) const = 0;	// ip:port
	virtual const tstring & getRemoteHost(void) const {return getRemoteAddr();}

	virtual int sendData(const unsigned char * data, size_t size) = 0;

	virtual void invalidate(bool bClose=false) = 0;
	virtual bool isInvalidate(void) const = 0;
};
const cgcRemote::pointer NullcgcRemote;

// cgcCommHandler
class cgcCommHandler
{
public:
	typedef boost::shared_ptr<cgcCommHandler> pointer;

	virtual int onRemoteAccept(const cgcRemote::pointer& pcgcRemote) = 0;
	virtual int onRecvData(const cgcRemote::pointer& pcgcRemote, const unsigned char * recvData, size_t recvLen) = 0;
	virtual int onRemoteClose(unsigned long remoteId,int nErrorCode) = 0;
};

class cgcCommunication
	: public cgcServiceInterface
{
public:
	typedef boost::shared_ptr<cgcCommunication> pointer;

	virtual void setCommHandler(cgcCommHandler::pointer handler) {m_commHandler = handler;}
	virtual cgcCommHandler::pointer getCommHandler(void) const {return m_commHandler;}
	virtual void resetCommHandler(void) {m_commHandler.reset();}

	virtual ~cgcCommunication(void)
	{
		m_commHandler.reset();
	}
protected:
	cgcCommHandler::pointer m_commHandler;
};

const cgcCommunication::pointer cgcNullCommunication;

#define CGC_COMMSERVICE_DEF(s) boost::static_pointer_cast<cgcCommunication, cgcServiceInterface>(s)

} // namespace mycp

#endif // _cgccommunications_head_included__
