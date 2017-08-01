#ifndef EBDIALOGMEMBERINFO_H
#define EBDIALOGMEMBERINFO_H

#include "ebclientapp.h"
#include <ebdialogbase.h>
#include <eblabel.h>

namespace Ui {
class EbDialogMemberInfo;
}
class EbDialogChangeHead;

class EbDialogMemberInfo : public EbDialogBase
{
    Q_OBJECT
public:
    explicit EbDialogMemberInfo(QWidget *parent = 0);
    ~EbDialogMemberInfo(void);

    EB_GROUP_TYPE m_groupType;
    QString m_groupName;
    EB_MemberInfo m_memberInfo;
    void updateLocaleInfo(void);
public slots:
//    virtual void open(void);
    virtual int exec(void);
    virtual void accept(void);
    virtual void resizeEvent(QResizeEvent *e);
    virtual void onClickedButtonDefaultMember(void);

    void onClickedLabelUserHead(void);
    void onSelectedHeadImage(qint64 resourceId, const QString &resourceFile);
protected:
    void updateUserHeadImage(void);
private:
    Ui::EbDialogMemberInfo *ui;
    EbLabel * m_labelUserHead;
    tstring m_oldFileMd5;
    bool m_newEemployee;
    EbDialogChangeHead * m_dialogChangeHead;
};

#endif // EBDIALOGMEMBERINFO_H
