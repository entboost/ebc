#include "ebfileinfo.h"

EbFileInfo::EbFileInfo(const CCrFileInfo &other)
    : CCrFileInfo(other)
    , m_progressValue(0)
    , m_isReceive(false)
{

}

EbFileInfo::pointer EbFileInfo::create(const CCrFileInfo &other)
{
    return EbFileInfo::pointer(new EbFileInfo(other));
}
