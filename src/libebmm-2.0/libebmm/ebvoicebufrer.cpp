#include "ebvoicebuffer.h"

namespace entboost {

EbVoiceBuffer::EbVoiceBuffer(void)
    : m_timestamp(0)
    , m_data(0)
    , m_size(0)
    , m_userId(0)
{

}

EbVoiceBuffer::EbVoiceBuffer(const unsigned char *data, int size)
    : m_timestamp(0)
    , m_data(0)
    , m_size(size)
    , m_userId(0)
{
    if (data!=0 && m_size>0) {
        m_data = new unsigned char[m_size];
        memcpy(m_data, data, m_size);
    }
}

EbVoiceBuffer::~EbVoiceBuffer()
{
    if (m_data!=0) {
        delete[] m_data;
    }
}

EbVoiceBuffer::pointer EbVoiceBuffer::create()
{
    return EbVoiceBuffer::pointer(new EbVoiceBuffer());
}

EbVoiceBuffer::pointer EbVoiceBuffer::create(const unsigned char *data, int size)
{
    return EbVoiceBuffer::pointer(new EbVoiceBuffer(data, size));
}

}   /// namespace entboost
