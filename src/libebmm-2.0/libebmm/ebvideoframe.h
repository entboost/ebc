#ifndef EBVIDEOFRAME_H
#define EBVIDEOFRAME_H

#include <boost/shared_ptr.hpp>

namespace entboost {

typedef struct tagVIDEOFRAME
{
    unsigned long type;    ///frame type
    unsigned short  width;
    unsigned short  height;
    unsigned char  *data;
    unsigned long bufferSize;
    unsigned long usedSize;
    unsigned long timastamp;
}VIDEOFRAME;

class EbVideoFrame : public VIDEOFRAME
{
public:
    typedef boost::shared_ptr<EbVideoFrame> pointer;
    EbVideoFrame(unsigned long allocSize=0);
    virtual ~EbVideoFrame(void);

    static EbVideoFrame::pointer create(unsigned long allocSize=0);
    void allocBuffer(unsigned int nSize);
};

}   /// namespace entboost

#endif // EBVIDEOFRAME_H
