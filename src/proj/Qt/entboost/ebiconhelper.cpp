#include "ebiconhelper.h"

///  字体图标: http://fontawesome.io/cheatsheet/

EbIconHelper*EbIconHelper::_instance = NULL;

EbIconHelper::EbIconHelper(QObject *)
//    : QObject(parent)
    : QObject(qApp)
{
    const int fontId = QFontDatabase::addApplicationFont( QStringLiteral(":/res/fontawesome-webfont.ttf") );
//    const int fontId = QFontDatabase::addApplicationFont(qApp->applicationDirPath()+"/res/fontawesome-webfont.ttf");
    QString fontName = QFontDatabase::applicationFontFamilies(fontId).at(0);
    iconFont = QFont(fontName);
}

EbIconHelper* EbIconHelper::Instance(void)
{
    static QMutex mutex;
    if (_instance == NULL) {
        QMutexLocker locker(&mutex);
        if (_instance == NULL) {
            _instance = new EbIconHelper;
        }
    }
    return _instance;
}
void EbIconHelper::SetIcon(QLabel*ctl,QChar c,int size)
{
    if (ctl==NULL) {
        return;
    }
    iconFont.setPointSize(size);
    ctl->setFont(iconFont);
    ctl->setText(c);
}
void EbIconHelper::SetIcon(QPushButton*ctl,QChar c,int size)
{
    if (ctl==NULL) {
        return;
    }
    iconFont.setPointSize(size);
    ctl->setFont(iconFont);
    ctl->setText(c);
}

void EbIconHelper::SetIcon( qint64 subId, QLabel* ctl, int size ) {
    SetIcon( ctl,GetIcon(subId),size );
}

void EbIconHelper::SetIcon(qint64 subId, QPushButton *ctl, int size)
{
    SetIcon( ctl,GetIcon(subId),size );
}

QChar EbIconHelper::GetIcon(qint64 subId) const
{
    switch (subId) {
    case 1002300102:    /// 找群找人
        return QChar(0xf002);
        break;
    case 1002300103:    /// 我的消息
        return QChar(0xf0e5);
        break;
    case 1002300104: /// 我的邮件
        return QChar(0xf003);
        break;
    case 1002300105: /// [个人]云盘
        return QChar(0xf0c2);
        break;
    case 1002300110:    /// 工作台
        return QChar(0xf015);
        break;
    case 1002300111: /// 计划 subid_
        return QChar(0xf0cb);
        break;
    case 1002300112: /// 任务
        return QChar(0xf0ae);
        break;
    case 1002300113: /// 日报
        return QChar(0xf044);
        break;
//        case 1002300114: /// 报告
//            return QChar(0xf044);
//            break;
    case 1002300115: /// 考勤
        return QChar(0xf274);
        break;
    default:    /// 其他
        return QChar(0xf009);
    }
}

