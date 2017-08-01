// Base64.cpp: implementation of the CBase64 class.
// 

#include "Base64.h"
#include "string.h"

// Base64.cpp: implementation of the CBase64 class.
//
#include <string.h>

char Base2Chr( unsigned char n )
{
    n &= 0x3F;
    if ( n < 26 )
        return ( char )( n + 'A' );
    else if ( n < 52 )
        return ( char )( n - 26 + 'a' );
    else if ( n < 62 )
        return ( char )( n - 52 + '0' );
    else if ( n == 62 )
        return '+';
    else
        return '/';
}
//---------------------------------------------------------------------------
unsigned char Chr2Base( char c )
{
    if ( c >= 'A' && c <= 'Z' )
        return ( unsigned char )( c - 'A' );
    else if ( c >= 'a' && c <= 'z' )
        return ( unsigned char )( c - 'a' + 26 );
    else if ( c >= '0' && c <= '9' )
        return ( unsigned char )( c - '0' + 52 );
    else if ( c == '+' )
        return 62;
    else if ( c == '/' )
        return 63;
    else
        return 64;  //  无效字符
}

//  aLen 为 aSrc 的长度， aDest 所指的缓冲区必须至少为 aLen 的 1.33 倍！！！
//  返回 aDest 的长度
unsigned int Base64Encode( char * const aDest, const unsigned char * aSrc, unsigned int aLen )
{
    char        * p = aDest;
    unsigned int          i;
    unsigned char t;

    for ( i = 0; i < aLen; i++ )
    {
        switch ( i % 3 )
        {
        case 0 :
            *p++ = Base2Chr( *aSrc >> 2 );
            t = ( *aSrc++ << 4 ) & 0x3F;
            break;
        case 1 :
            *p++ = Base2Chr( t | ( *aSrc >> 4 ) );
            t = ( *aSrc++ << 2 ) & 0x3F;
            break;
        case 2 :
            *p++ = Base2Chr( t | ( *aSrc >> 6 ) );
            *p++ = Base2Chr( *aSrc++ );
            break;
        }
    }
    if ( aLen % 3 != 0 )
    {
        *p++ = Base2Chr( t );
        if ( aLen % 3 == 1 )
            *p++ = '=';
        *p++ = '=';
    }
    *p = 0;  //  aDest is an ASCIIZ string
    return (unsigned int)( p - aDest );  //  exclude the end of zero
}

//  aDest 所指的缓冲区必须至少为 aSrc 长度的 0.75 倍！！！
//  返回 aDest 的长度
unsigned int Base64Decode( unsigned char * const aDest, const char * aSrc, unsigned int nSrcLen)
{
	unsigned char * p = aDest;
	unsigned int             i, n = nSrcLen;//strlen( aSrc );
	unsigned char   c, t;

	for ( i = 0; i < n; i++ )
	{
		if ( *aSrc == '=' )
			break;
		do {
			if ( *aSrc )
				c = Chr2Base( *aSrc++ );
			else
				return (unsigned int)( p - aDest );
			//c = 65;  //  字符串结束
		} while ( c == 64 );  //  跳过无效字符，如回车等
		//if ( c == 65 )
		//	break;
		switch ( i % 4 )
		{
		case 0 :
			t = c << 2;
			break;
		case 1 :
			*p++ = ( unsigned char )( t | ( c >> 4 ) );
			t = ( unsigned char )( c << 4 );
			break;
		case 2 :
			*p++ = ( unsigned char )( t | ( c >> 2 ) );
			t = ( unsigned char )( c << 6 );
			break;
		case 3 :
			*p++ = ( unsigned char )( t | c );
			break;
		}
	}
	return (unsigned int)( p - aDest );
}


// Static Member Initializers
//

//tstring CBase64::m_sBase64Alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
//int CBase64::m_nMask[] = { 0, 1, 3, 7, 15, 31, 63, 127, 255 };

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBase64::CBase64(void)
//: m_nInputSize(0), m_nBitsRemaining(0), m_lBitStorage(0)

{
}

CBase64::~CBase64(void)
{
}

tstring CBase64::Encode(const char* szEncoding, int nSize)
{
	const size_t nBufferSize = (nSize*4)/3 + ((nSize+75)/76)*2+10;
	char * lpszBuf = new char[nBufferSize];
	memset(lpszBuf,0,nBufferSize);
	const unsigned int nLen = Base64Encode(lpszBuf,(const unsigned char*)szEncoding,nSize);
	tstring sOutput(lpszBuf,nLen);
	delete[] lpszBuf;
	return sOutput;

	//tstring sOutput;
	//int nNumBits = 6;
	//size_t nDigit = 0;
	//int lp = 0;

	//if( szEncoding == NULL )
	//	return sOutput;
	//m_szInput = szEncoding;
	//m_nInputSize = nSize;

	//m_nBitsRemaining = 0;
	//nDigit = read_bits( nNumBits, &nNumBits, lp );
	//while( nNumBits > 0 )
	//{
	//	sOutput += m_sBase64Alphabet[ (int)nDigit ];
	//	nDigit = read_bits( nNumBits, &nNumBits, lp );
	//}

	//while( sOutput.size() % 4 != 0 )
	//{
	//	sOutput.append("=");
	//}
	//return sOutput;
}

int CBase64::Decode(const char* szDecoding, char* szOutput)
{
	return Base64Decode((unsigned char*)szOutput,szDecoding, strlen(szDecoding));
	//tstring sInput;
 //   int c, lp =0;
	//int nDigit;
 //   int nDecode[ 256 ];

	//if( szOutput == NULL )
	//	return 0;
	//if( szDecoding == NULL )
	//	return 0;
	//sInput = szDecoding;
	//if( sInput.empty())
	//	return 0;

	//int i = 0;
	//for( i = 0; i < 256; i++ ) 
	//	nDecode[i] = -2; 
	//for( i=0; i < 64; i++ )
	//{
	//	nDecode[ m_sBase64Alphabet[ i ] ] = i;
	//	nDecode[ m_sBase64Alphabet[ i ] | 0x80 ] = i; 
	//	nDecode[ '=' ] = -1; 
	//	nDecode[ '=' | 0x80 ] = -1; 
 //   }

	//const size_t nInputSize = sInput.size();
	//memset( szOutput, 0, nInputSize + 1 );

	//for( lp = 0, i = 0; lp < nInputSize; lp++ )
	//{
	//	c = sInput.c_str()[lp];
	//	nDigit = nDecode[ c & 0x7F ];
	//	if( nDigit < -1 ) 
	//	{
	//		return 0;
	//	} 
	//	else if( nDigit >= 0 )
	//	{
	//		write_bits( nDigit & 0x3F, 6, szOutput, i );
	//	}
 //   }	
	//return i;
}

//
//size_t CBase64::read_bits(int nNumBits, int * pBitsRead, int& lp)
//{
//    unsigned long lScratch = 0;
//    while( ( m_nBitsRemaining < nNumBits ) && 
//		   ( lp < m_nInputSize ) ) 
//	{
//		int c = m_szInput[ lp++ ];
//        m_lBitStorage <<= 8;
//        m_lBitStorage |= (c & 0xff);
//		m_nBitsRemaining += 8;
//    }
//    if( m_nBitsRemaining < nNumBits ) 
//	{
//		lScratch = m_lBitStorage << ( nNumBits - m_nBitsRemaining );
//		*pBitsRead = m_nBitsRemaining;
//		m_nBitsRemaining = 0;
//    } 
//	else 
//	{
//		lScratch = m_lBitStorage >> ( m_nBitsRemaining - nNumBits );
//		*pBitsRead = nNumBits;
//		m_nBitsRemaining -= nNumBits;
//    }
//    return (size_t)lScratch & m_nMask[nNumBits];
//}
//
//
//void CBase64::write_bits(size_t nBits,
//						 int nNumBits,
//						 char* szOutput,
//						 int& i)
//{
//	size_t nScratch;
//
//	m_lBitStorage = (m_lBitStorage << nNumBits) | nBits;
//	m_nBitsRemaining += nNumBits;
//	while( m_nBitsRemaining > 7 ) 
//	{
//		nScratch = m_lBitStorage >> (m_nBitsRemaining - 8);
//		szOutput[ i++ ] = nScratch & 0xFF;
//		m_nBitsRemaining -= 8;
//	}
//}
//
