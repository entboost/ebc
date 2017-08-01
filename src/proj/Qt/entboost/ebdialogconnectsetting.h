#ifndef EBDIALOGCONNECTSETTING_H
#define EBDIALOGCONNECTSETTING_H

#include <ebdialogbase.h>

namespace Ui {
class EbDialogConnectSetting;
}

class EbDialogConnectSetting : public EbDialogBase
{
    Q_OBJECT
public:
    explicit EbDialogConnectSetting(QWidget *parent = 0);
    ~EbDialogConnectSetting(void);

    const QString& oldServer(void) const {return m_oldServer;}
    const QString& newServer(void) const {return m_newServer;}
    bool isServerModified(void) const {return m_oldServer==m_newServer?false:true;}
protected:
    virtual void accept(void);
private:
    Ui::EbDialogConnectSetting *ui;
    QString m_oldServer;
    QString m_newServer;
};

#endif // EBDIALOGCONNECTSETTING_H
