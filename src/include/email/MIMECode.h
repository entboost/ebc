// MIMECode.h: interface for the CMIMECode class.
// 
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MIMECODE_H__FD6A25D0_EE0E_11D1_870E_444553540001__INCLUDED_)
#define AFX_MIMECODE_H__FD6A25D0_EE0E_11D1_870E_444553540001__INCLUDED_

#include <CGCBase/cgcSmartString.h>
using namespace mycp;

class CMIMECode  
{
public:
	CMIMECode(void) {}
	virtual ~CMIMECode(void) {}

	virtual int Decode( const char* szDecoding, char* szOutput ) = 0;
	virtual tstring Encode( const char* szEncoding, int nSize ) = 0;
};

#endif // !defined(AFX_MIMECODE_H__FD6A25D0_EE0E_11D1_870E_444553540001__INCLUDED_)
