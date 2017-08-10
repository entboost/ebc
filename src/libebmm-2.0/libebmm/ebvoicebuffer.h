#ifndef EBVOICEBUFFER_H
#define EBVOICEBUFFER_H

#include <boost/shared_ptr.hpp>
#include <CGCBase/cgcobject.h>

namespace entboost {

class EbVoiceBuffer
{
public:
    typedef boost::shared_ptr<EbVoiceBuffer> pointer;
    EbVoiceBuffer(void);
    EbVoiceBuffer(const unsigned char *data, int size);
    virtual ~EbVoiceBuffer(void);
    static EbVoiceBuffer::pointer create(void);
    static EbVoiceBuffer::pointer create(const unsigned char *data, int size);

    unsigned int m_timestamp;
    unsigned char *m_data;
//    short* m_data;
    int m_size;
    mycp::bigint m_userId;
};

}   /// namespace entboost

#endif // EBVOICEBUFFER_H
