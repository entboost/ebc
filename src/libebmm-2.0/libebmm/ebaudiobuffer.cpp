#include "ebaudiobuffer.h"

namespace entboost {

EbAudioBuffer::EbAudioBuffer()
    : m_bufferSize(0)
{
    ssrc = 0;
    seq = 0;
    ts = 0;
    expire_ts = 0;
    payload_type = 0;
    data = 0;
    len = 0;
}

EbAudioBuffer::~EbAudioBuffer()
{
    if (data!=0) {
        delete[] data;
        data = 0;
    }
}

EbAudioBuffer::pointer EbAudioBuffer::create()
{
    return EbAudioBuffer::pointer(new EbAudioBuffer());
}

void EbAudioBuffer::allocBuffer(unsigned int nSize)
{
    if (data==NULL)
    {
        m_bufferSize = nSize;
        data = new unsigned char[m_bufferSize];
    }else if (m_bufferSize < nSize)
    {
        delete[] data;
        data = 0;
        m_bufferSize = nSize;
        data = new unsigned char[m_bufferSize];
    }
//    memset(data,0,nSize);
}

}   /// namespace entboost
