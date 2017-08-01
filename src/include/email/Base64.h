// Base64.h: interface for the CBase64 class.
// 
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BASE64_H__FD6A25D1_EE0E_11D1_870E_444553540001__INCLUDED_)
#define AFX_BASE64_H__FD6A25D1_EE0E_11D1_870E_444553540001__INCLUDED_

#include "MIMECode.h"

//  aLen 为 aSrc 的长度， aDest 所指的缓冲区必须至少为 aLen 的 1.33 倍！！！
//  返回 aDest 的长度
unsigned int Base64Encode( char * const aDest, const unsigned char * aSrc, unsigned int aLen );

//  aDest 所指的缓冲区必须至少为 aSrc 长度的 0.75 倍！！！
//  返回 aDest 的长度
unsigned int Base64Decode( unsigned char * const aDest, const char * aSrc, unsigned int nSrcLen);


class CBase64 : public CMIMECode  
{
public:
	CBase64(void);
	virtual ~CBase64(void);

	virtual int Decode( const char* szDecoding, char* szOutput );
	virtual tstring Encode( const char* szEncoding, int nSize );

//protected:
//	void write_bits( size_t nBits, int nNumBts, char* szOutput, int& lp );
//	size_t read_bits( int nNumBits, int* pBitsRead, int& lp );
//
//	int m_nInputSize;
//	int m_nBitsRemaining;
//	unsigned long m_lBitStorage;
//	const char* m_szInput;
//
//	static int m_nMask[];
//	static tstring m_sBase64Alphabet;
//private:
};

#endif // !defined(AFX_BASE64_H__FD6A25D1_EE0E_11D1_870E_444553540001__INCLUDED_)
