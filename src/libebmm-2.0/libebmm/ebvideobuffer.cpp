#include "ebvideobuffer.h"

namespace entboost {

EbVideoBuffer::EbVideoBuffer()
    : m_bufferSize(0)
{
    userId = 0;
    seq = 0;
    ts = 0;
    expire_ts = 0;
    payload_type = 0;
    pt = 0;
    data = 0;
    size = 0;
}

EbVideoBuffer::~EbVideoBuffer()
{
    if (data!=0) {
        delete[] data;
        data = 0;
    }
}

EbVideoBuffer::pointer EbVideoBuffer::create()
{
    return EbVideoBuffer::pointer(new EbVideoBuffer());
}

void EbVideoBuffer::allocBuffer(unsigned int nSize)
{
    if (data==0) {
        m_bufferSize = nSize+1;
        data = new unsigned char[m_bufferSize];
    }
    else if (m_bufferSize < nSize) {
        delete[] data;
        data = NULL;
        m_bufferSize = nSize+1;
        data = new unsigned char[m_bufferSize];
    }
//    memset(data,0,nSize);

}

}   /// namespace entboost

