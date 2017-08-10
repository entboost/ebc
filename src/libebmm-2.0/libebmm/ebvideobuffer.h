#ifndef EbVideoBuffer_H
#define EbVideoBuffer_H

#include <CGCBase/cgcobject.h>
#include <stl/locklist.h>

namespace entboost {

typedef struct tagVIDEOBUFFER
{
  mycp::bigint   userId;           //ssrc
  unsigned int   seq;            //seq
  unsigned int   ts;             //timestamp
  unsigned long  expire_ts;      //expired timestamp
  int            payload_type;   //RTP payload type
  int			 pt;				// 0:audio; 1:video; 11:key vedio
  unsigned char  *data;          //buffer
  unsigned int    size;           //buffer length
}VIDEOBUFFER;

class EbVideoBuffer : public tagVIDEOBUFFER
{
public:
    EbVideoBuffer(void);
    virtual ~EbVideoBuffer(void);

    typedef boost::shared_ptr<EbVideoBuffer> pointer;
    static EbVideoBuffer::pointer create(void);
    void allocBuffer(unsigned int nSize);
private:
    unsigned int m_bufferSize;
};

}   /// namespace entboost

#endif // EbVideoBuffer_H
