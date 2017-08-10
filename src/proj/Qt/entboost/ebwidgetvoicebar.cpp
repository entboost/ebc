#include "ebwidgetvoicebar.h"
#include <time.h>
#include <QPainter>

EbWidgetVoiceBar::EbWidgetVoiceBar(QWidget *parent) : QWidget(parent)
  , m_verticalCount(8)
  , m_verticalInterval(3)
  , m_lineCount(10)
  , m_lineIntervale(3)
  , m_color(128, 255, 0)
  , m_value(0)
  , m_lastTime(0)
  , m_painter(0)
{
    m_timerIdCheckState = this->startTimer(1000);
}

EbWidgetVoiceBar::~EbWidgetVoiceBar()
{
    this->killTimer(m_timerIdCheckState);
    if (m_painter!=0) {
        delete m_painter;
        m_painter = 0;
    }
}

void EbWidgetVoiceBar::setValue(int value)
{
    m_lastTime = time(0);
    if (m_value!=value) {
        m_value = value;
    }
}

void EbWidgetVoiceBar::paintEvent(QPaintEvent *e)
{
    if (m_painter==0) {
        m_painter = new QPainter(this);
    }
    const int value = m_value;
    m_painter->begin(this);
    m_painter->setPen( QPen(m_color) );
    m_painter->setBrush( QBrush(m_color) );
    /// 先画垂直竖线
    const int const_width = (width()+m_verticalInterval)/m_verticalCount-m_verticalInterval;
    const int const_height = (height()+m_lineIntervale)/m_lineCount-m_lineIntervale;
    QRect rect(0, 0, const_width, const_height);
    for (int i=0; i<m_verticalCount; i++) {
        if (value==0) {
            break;
        }
        const int x = i*(const_width+m_verticalInterval);
        for (int j=0; j<m_lineCount; j++) {
            /// value 音量＝1 时，有线垂直竖线也不显示，保证效果真实
            const int y = (m_lineCount-j)*(const_height+m_lineIntervale);
            const int voiceFactor = rand()%4;   /// 音量条上下浮动因素
            if (j+1>=value && voiceFactor<2) {
                break;
            }
            rect.moveTo(x, y);
            m_painter->drawRect(rect);
        }
    }
    m_painter->end();

    QWidget::paintEvent(e);
}

void EbWidgetVoiceBar::timerEvent(QTimerEvent *e)
{
    if (m_timerIdCheckState!=0 && e->timerId()==m_timerIdCheckState) {
        if (m_value>0 && time(0)-m_timerIdCheckState>1) {
            m_value = 0;
        }
    }
    QWidget::timerEvent(e);
}
