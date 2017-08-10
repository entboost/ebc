#ifndef EBAUDIOBUFFER_H
#define EBAUDIOBUFFER_H

#include <CGCBase/cgcobject.h>

namespace entboost {

typedef struct tagAUDIOBUFFER
{
    mycp::bigint   ssrc;           //ssrc
    unsigned int   seq;            //seq
    unsigned int   ts;             //timestamp
    unsigned long  expire_ts;      //expired timestamp
    int            payload_type;   //RTP payload type
    unsigned char  *data;          //buffer
    unsigned int    len;           //buffer length
}AUDIOBUFFER;

class EbAudioBuffer : public AUDIOBUFFER
{
public:
    typedef boost::shared_ptr<EbAudioBuffer> pointer;
    EbAudioBuffer(void);
    virtual ~EbAudioBuffer(void);
    static EbAudioBuffer::pointer create(void);
    void allocBuffer(unsigned int nSize);
private:
    unsigned int m_bufferSize;
};

}   /// namespace entboost

#endif // EBAUDIOBUFFER_H
