// Base64.h file here
//
//////////////////////////////////////////////////////////////////////

#ifndef __Base64_h__
#define __Base64_h__

//  aLen 为 aSrc 的长度， aDest 所指的缓冲区必须至少为 aLen 的 1.33 倍！！！
//  返回 aDest 的长度
unsigned int Base64Encode( char * const aDest, const unsigned char * aSrc, unsigned int aLen );

//  aDest 所指的缓冲区必须至少为 aSrc 长度的 0.75 倍！！！
//  返回 aDest 的长度
unsigned int Base64Decode( unsigned char * const aDest, const char * aSrc );

#endif // __Base64_h__
