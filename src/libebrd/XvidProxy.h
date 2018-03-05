// XvidProxy.h file here
#pragma once
#ifndef __XvidProxy_h__
#define __XvidProxy_h__

#include <xvid.h>

class XInfo
{
public:
	XInfo(const XInfo & v)
	{
		equal(v);
	}
	const XInfo & operator = (const XInfo & v)
	{
		equal(v);
		return *this;
	}
	void csp(int newv) {m_csp = newv;}
	int csp(void) const {return m_csp;}
	void bitcount(int newv) {m_bitcount = newv;}
	int bitcount(void) const {return m_bitcount;}
	int max_key_interval(void) const {return m_nMaxKeyInterval;}

	void width(int newv) {m_width = newv;}
	int width(void) const {return m_width;}
	void height(int newv) {m_height = newv;}
	int height(void) const {return m_height;}	

	int stride(void) const {return m_width * m_bitcount / 8;}
	int framesize(void) const {return stride() * m_height;}

private:
	void equal(const XInfo & v)
	{
		this->m_csp = v.csp();
		this->m_bitcount = v.bitcount();
		this->m_width = v.width();
		this->m_height = v.height();
		this->m_nMaxKeyInterval = v.max_key_interval();
	}
public:
	XInfo(int csp, int bitcount=32, int width=320, int height=240, int nMaxKeyInterval=50)
		: m_csp(csp), m_bitcount(bitcount)
		, m_width(width), m_height(height)
		, m_nMaxKeyInterval(nMaxKeyInterval)
	{}
	XInfo(void)
		: m_csp(XVID_CSP_BGR), m_bitcount(32)
		, m_width(320), m_height(240)
		, m_nMaxKeyInterval(50)
	{
	}
private:
	int m_csp;
	int m_width;
	int m_height;
	int m_bitcount;
	int m_nMaxKeyInterval;
};

class CXvidProxy
{
public:
	// Encoder
	void setenc_info(const XInfo & xinfo) {m_encxInfo = xinfo;}
	void setenc_quant(int quant = 5) {m_encQuant = quant;}
	unsigned char* encode(const unsigned char * inFrame, unsigned char * pOutBuffer, int & pOutSize, bool& pOutKeyFrame);

	// Decoder
	void setdec_info(const XInfo & xinfo) {m_decxInfo = xinfo;}
	bool decode(const unsigned char * inFrame, int inSize, unsigned char ** outBuffer, int & outSize);

	void close(void);

public:
	CXvidProxy(void);
	virtual ~CXvidProxy(void);

private:
	bool init_enc(void);
	bool init_dec(void);

private:
	// encoder
	void *				m_encHandle;
	xvid_enc_frame_t	m_encFrame;
	xvid_enc_stats_t	m_encStats;
	unsigned char *		m_encBuffer;
	int					m_encCsp;
	int					m_encQuant;
	XInfo				m_encxInfo;

	// decoder
	void *				m_decHandle;
	xvid_dec_frame_t	m_decFrame;
	xvid_dec_stats_t	m_decStats;
	unsigned char *		m_decBuffer;
	XInfo				m_decxInfo;

};

#endif // __XvidProxy_h__
