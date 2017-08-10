#ifndef EBWIDGETVOICEBAR_H
#define EBWIDGETVOICEBAR_H

#include <QWidget>
#include <QPainter>
#include <QTimerEvent>

/// 音量显示条
class EbWidgetVoiceBar : public QWidget
{
    Q_OBJECT
public:
    explicit EbWidgetVoiceBar(QWidget *parent = nullptr);
    virtual ~EbWidgetVoiceBar(void);

    /// 0-10
    void setValue(int value);
signals:

public slots:

protected:
    virtual void paintEvent(QPaintEvent *e);
    virtual void timerEvent(QTimerEvent *e);
private:
    int m_verticalCount;    /// 垂直线 default 8
    int m_verticalInterval; /// 垂直线 间隔 default 3
    int m_lineCount;        /// 水平线 default 10
    int m_lineIntervale;    /// 水平线 间隔 default 3
    QColor m_color;

    int m_value;
    time_t m_lastTime;
    int m_timerIdCheckState;
    QPainter *m_painter;
};

#endif // EBWIDGETVOICEBAR_H
