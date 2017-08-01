#ifndef EBDIALOGCONTACTINFO_H
#define EBDIALOGCONTACTINFO_H

#include "ebclientapp.h"
#include <ebdialogbase.h>

namespace Ui {
class EbDialogContactInfo;
}

class EbDialogContactInfo : public EbDialogBase
{
    Q_OBJECT
public:
    explicit EbDialogContactInfo(QWidget *parent = 0);
    ~EbDialogContactInfo(void);

    EB_ContactInfo m_contactInfo;
    void updateLocaleInfo(void);
public slots:
    virtual int exec(void);
    virtual void accept(void);
    void onLoadContactGroup(void);
private:
    Ui::EbDialogContactInfo *ui;
};

#endif // EBDIALOGCONTACTINFO_H
