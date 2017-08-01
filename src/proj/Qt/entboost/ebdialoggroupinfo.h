#ifndef EBDIALOGGROUPINFO_H
#define EBDIALOGGROUPINFO_H

#include "ebclientapp.h"
#include <ebdialogbase.h>
#include <eblabel.h>

namespace Ui {
class EbDialogGroupInfo;
}

class EbDialogGroupInfo : public EbDialogBase
{
    Q_OBJECT
public:
    explicit EbDialogGroupInfo(QWidget *parent = 0);
    ~EbDialogGroupInfo(void);

    EB_GroupInfo m_groupInfo;
    void updateLocaleInfo(void);
public slots:
    virtual int exec(void);
    virtual void accept(void);
    void onClickedLabelManagerUserName(void);
private:
    Ui::EbDialogGroupInfo *ui;
    EbLabel * m_labelManagerUserName;
};

#endif // EBDIALOGGROUPINFO_H
