#include "ebvideoframe.h"

namespace entboost {

EbVideoFrame::EbVideoFrame(unsigned long allocSize)
{
    type = 0;
    width = 0;
    height = 0;
    data = 0;
    bufferSize = 0;
    if (allocSize>0) {
        allocBuffer(allocSize);
    }
    usedSize = 0;
    timastamp = 0;
}

EbVideoFrame::~EbVideoFrame()
{
    if (data!=0) {
        delete[] data;
        data = 0;
    }
}

EbVideoFrame::pointer EbVideoFrame::create(unsigned long allocSize)
{
    return EbVideoFrame::pointer(new EbVideoFrame(allocSize));
}

void EbVideoFrame::allocBuffer(unsigned int nSize)
{
    if (data==0) {
        bufferSize = nSize+1;
        data = new unsigned char[bufferSize];
    }
    else if (bufferSize < nSize) {
        delete[] data;
        bufferSize = nSize+1;
        data = new unsigned char[bufferSize];
    }
//    memset(data,0,nSize);
}

}   /// namespace entboost
