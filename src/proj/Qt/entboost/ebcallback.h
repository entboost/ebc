#ifndef EBCALLBACK_H
#define EBCALLBACK_H

#include <ebwidgetiteminfo.h>

class EbSelectedUserCallback
{
public:
    virtual bool isSelectedUser(const EbWidgetItemInfo::pointer &itemInfo) = 0;
};


#endif // EBCALLBACK_H
