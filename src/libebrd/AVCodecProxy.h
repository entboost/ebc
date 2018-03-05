#pragma once
#ifndef _CAVCodecProxy_h_FILE_INCLUDED__
#define _CAVCodecProxy_h_FILE_INCLUDED__

#define nptr NULL

#define USES_FFMPEG_3_3_2
//#define USES_FFMPEG_0_8
//AVPixelFormat
//AVCodecID

extern "C"
{
//#include "C:/Documents and Settings/Administrator/桌面/CLient/drffmpeg/export/drffmpeg/avcodec.h"

#include "libavformat/avformat.h"
#include "libavdevice/avdevice.h"
#include "libavcodec/avcodec.h"
#include "libswscale/swscale.h"
#ifdef USES_FFMPEG_3_3_2
#include "libavutil/imgutils.h"
#endif
}

//
//#define PixelFormat int
//#define PIX_FMT_RGB32 1
//#define PIX_FMT_BGRA 2
//#define CODEC_ID_MPEG4 3

class FVInfo
{
public:
	FVInfo(const FVInfo & v)
	{
		equal(v);
	}
	const FVInfo & operator = (const FVInfo & v)
	{
		equal(v);
		return *this;
	}
#ifdef USES_FFMPEG_3_3_2
	void csp(AVPixelFormat newv) {m_csp = newv;}
	AVPixelFormat csp(void) const {return m_csp;}
#else
	void csp(PixelFormat newv) {m_csp = newv;}
	PixelFormat csp(void) const {return m_csp;}
#endif
	void bitcount(int newv) {m_bitcount = newv;}
	int bitcount(void) const {return m_bitcount;}

	void width(int newv) {m_width = newv;}
	int width(void) const {return m_width;}
	void height(int newv) {m_height = newv;}
	int height(void) const {return m_height;}	

	int stride(void) const {return m_width * m_bitcount / 8;}
	int framesize(void) const {return stride() * m_height;}

private:
	void equal(const FVInfo & v)
	{
		this->m_csp = v.csp();
		this->m_bitcount = v.bitcount();
		this->m_width = v.width();
		this->m_height = v.height();
	}
public:
#ifdef USES_FFMPEG_3_3_2
	FVInfo(AVPixelFormat csp, int bitcount=32, int width=320, int height=240)
#else
	FVInfo(PixelFormat csp, int bitcount=32, int width=320, int height=240)
#endif
		: m_csp(csp), m_bitcount(bitcount)
		, m_width(width), m_height(height)
	{}
	FVInfo(void)
#ifdef USES_FFMPEG_3_3_2
		: m_csp(AV_PIX_FMT_RGB32), m_bitcount(32)
#else
		: m_csp(PIX_FMT_RGB32), m_bitcount(32)
#endif
		, m_width(320), m_height(240)
	{
	}
private:
#ifdef USES_FFMPEG_3_3_2
	AVPixelFormat m_csp;
#else
	PixelFormat m_csp;
#endif
	int m_width;
	int m_height;
	int m_bitcount;
};


//定义目标格式
//#define DEST_FORMAT PIX_FMT_BGR24
//#define DEST_FORMAT PIX_FMT_RGBA

//#define DEST_FORMAT PIX_FMT_RGB32
//#define DEFAULT_BPP	4

////////////////
class _AVReadHandler_
{
public:
	virtual void onReadAudio(const int16_t * audioFrame) = 0;
	virtual void onReadVideo(const AVFrame & videoFrame) = 0;

};

class CAVCodecProxy
{
private:
	AVFormatContext *pFormatCtx;   //
	AVCodecContext  *pVideoCodecCtx, *pAudioCodecCtx;    
	AVCodec         *pVideoCodec, *pAudioCodec;      //编解码器
	int             videoStream, audioStream;
	uint8_t         *m_pBufferDec;
	uint8_t         *m_pBufferEnc;     

	AVFrame         *m_pFrameRGB;			//YUV帧
	int16_t			*pAutionSample;
	
	AVPacket		avPacket;                    //AV包。
#ifdef USES_FFMPEG_3_3_2
	AVFrame * m_frame;
#else
	unsigned char * m_samples;
#endif
	unsigned char * m_bit_buffer;

	// decode
	bool			m_bFindKeyFrame;
	FVInfo			m_decFvinfo;
	AVFrame *		m_pFrameDecode;
	SwsContext *	m_swsContextDecode;

	// encode
	AVCodecContext *m_pVideoCodecCtxEnc;
	AVFrame *		m_pFrameEncode;
	SwsContext *	m_swsContextEncode;

	// handler
	_AVReadHandler_ * m_handler;

public:
	void closeav(void);
	long GetNextFrame(unsigned char *lpdata);

	void setdec_info(const FVInfo & fvinfo) {m_decFvinfo = fvinfo;}
	/// 
#ifdef USES_FFMPEG_3_3_2
	bool openav(enum AVCodecID codecID, const unsigned char * extradata=0, int extrasize=0);
#else
	bool openav(enum CodecID codecID, const unsigned char * extradata=0, int extrasize=0);
#endif
	bool isopenav(void) const;
	unsigned char* decode(const unsigned char * inBuffer, int inLength, unsigned char * outBuffer, int & outSize, bool reversal = false);
	bool encode(const unsigned char * inBuffer, int inLength, unsigned char ** outBuffer, int & outSize);
	void setFindKeyFrameFlag(bool bFind) {m_bFindKeyFrame = bFind;}
protected:
	unsigned char * m_pBufferTemp;
	long m_nBufferTempSize;

public:
	static bool GetNextFrame(AVFormatContext *pFormatCtx, AVCodecContext *pCodecCtx, int videoStream, AVFrame *pFrame);
	CAVCodecProxy(void);
	virtual ~CAVCodecProxy(void);
};

#endif // _CAVCodecProxy_h_FILE_INCLUDED__
