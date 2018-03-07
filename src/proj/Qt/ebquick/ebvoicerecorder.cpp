#include "ebvoicerecorder.h"

//const qint64 TIME_TRANSFORM = 1000 * 1000;              // 微妙转秒;

EbVoiceRecorder::EbVoiceRecorder(QObject *parent) : QIODevice(parent)
  , m_timerIdStop(0)
  , m_audioInput(0)
{
    memset(&m_wavFileHeader, 0, sizeof(m_wavFileHeader));
}

EbVoiceRecorder::~EbVoiceRecorder()
{
    stopRecorder();
}

bool EbVoiceRecorder::startRecorder(const QString &wavFilePath, int maxSeconds)
{
    if (m_audioInput!=0) {
        stopRecorder();
    }
    // 判断本地设备是否支持该格式
    QAudioDeviceInfo audioDeviceInfo = QAudioDeviceInfo::defaultInputDevice();
    // 判断本地是否有录音设备;
    if (audioDeviceInfo.isNull()) {
        return false;
    }
    m_wavFilePath = wavFilePath;
    m_wavFile.setFileName(wavFilePath);
    if (!m_wavFile.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        return false;
    }
    if (m_timerIdStop!=0) {
        this->killTimer(m_timerIdStop);
        m_timerIdStop = 0;
    }
    if (maxSeconds>0) {
        m_timerIdStop = this->startTimer(maxSeconds*1000);
    }
    memset(&m_wavFileHeader, 0, sizeof(m_wavFileHeader));
    qstrcpy(m_wavFileHeader.RiffName, "RIFF");
    qstrcpy(m_wavFileHeader.WavName, "WAVE");
    qstrcpy(m_wavFileHeader.FmtName, "fmt ");
    qstrcpy(m_wavFileHeader.DATANAME, "data");

    // 表示 FMT块 的长度
    m_wavFileHeader.nFmtLength = 16;
    // 表示 按照PCM 编码;
    m_wavFileHeader.nAudioFormat = 1;
    // 声道数目;
    m_wavFileHeader.nChannleNumber = 1;
    // 采样频率;
    m_wavFileHeader.nSampleRate = 8000;

    // nBytesPerSample 和 nBytesPerSecond这两个值通过设置的参数计算得到;
    // 数据块对齐单位(每个采样需要的字节数 = 通道数 × 每次采样得到的样本数据位数 / 8 )
    m_wavFileHeader.nBytesPerSample = 2;
    // 波形数据传输速率
    // (每秒平均字节数 = 采样频率 × 通道数 × 每次采样得到的样本数据位数 / 8  = 采样频率 × 每个采样需要的字节数 )
    m_wavFileHeader.nBytesPerSecond = 16000;

    // 每次采样得到的样本数据位数;
    m_wavFileHeader.nBitsPerSample = 16;

    qint64 nFileLen = 0;
//    qint64 nFileLen = cacheFile.bytesAvailable();

    m_wavFileHeader.nRiffLength = nFileLen - 8 + const_wav_file_header_size;
    m_wavFileHeader.nDataLength = nFileLen;

    // 先将wav文件头信息写入，再将音频数据写入;
    m_wavFile.write((char *)&m_wavFileHeader, const_wav_file_header_size);

//    QAudioDeviceInfo d = QAudioDeviceInfo::defaultInputDevice();
//    QList<int> supportedSampleRates = d.supportedSampleRates();

    // 设置音频文件格式;
    QAudioFormat format;
    // 设置采样频率;
    format.setSampleRate(8000);
    // 设置通道数;
    format.setChannelCount(1);
    // 设置每次采样得到的样本数据位值;
    format.setSampleSize(16);
    // 设置编码方法;
    format.setCodec("audio/pcm");
    // 设置采样字节存储顺序;
    format.setByteOrder(QAudioFormat::LittleEndian);
    // 设置采样类型;
    format.setSampleType(QAudioFormat::SignedInt);
//    format.setSampleType(QAudioFormat::UnSignedInt);

    // 判断当前设备设置是否支持该音频格式;
    if (!audioDeviceInfo.isFormatSupported(format))
    {
//        qDebug() << "Default format not supported, trying to use the nearest.";
        format = audioDeviceInfo.nearestFormat(format);
    }
    /// 开始录音;
    QIODevice::open(QIODevice::WriteOnly);
    m_audioInput = new QAudioInput(format, this);
    m_audioInput->start(this);
    return true;
}

bool EbVoiceRecorder::isStarted() const
{
    return m_audioInput==0?false:true;
}

void EbVoiceRecorder::stopRecorder()
{
    if (m_timerIdStop!=0) {
        this->killTimer(m_timerIdStop);
        m_timerIdStop = 0;
    }

    if (m_audioInput!=0) {
        m_audioInput->stop();
        delete m_audioInput;
        m_audioInput = 0;

        if (m_wavFile.isOpen()) {
            m_wavFile.seek(0);
            m_wavFile.write((char *)&m_wavFileHeader, const_wav_file_header_size);
            m_wavFile.close();
        }
        // 将生成的.raw文件转成.wav格式文件;
//        addWavHeader(RAW_RECORD_FILENAME, WAV_RECORD_FILENAME);
    }
    QIODevice::close();
}

unsigned long EbVoiceRecorder::dataLength() const
{
    return m_wavFileHeader.nDataLength;
}

unsigned int EbVoiceRecorder::dataSeconds() const
{
    if (m_wavFileHeader.nBytesPerSecond==0 ||
            m_wavFileHeader.nDataLength==0) {
        return 0;
    }
    return m_wavFileHeader.nDataLength/m_wavFileHeader.nBytesPerSecond;
}

void EbVoiceRecorder::timerEvent(QTimerEvent *e)
{
    if (m_timerIdStop!=0 && m_timerIdStop==e->timerId()) {
        this->killTimer(m_timerIdStop);
        m_timerIdStop = 0;
        stopRecorder();
    }

    QIODevice::timerEvent(e);
}

//qint64 EbVoiceRecorder::addWavHeader(const QString &catheFileName, const QString &wavFileName)
//{
//    // 开始设置WAV的文件头
//    // 这里具体的数据代表什么含义请看上一篇文章（Qt之WAV文件解析）中对wav文件头的介绍
//    WAVFILEHEADER WavFileHeader;
//    qstrcpy(WavFileHeader.RiffName, "RIFF");
//    qstrcpy(WavFileHeader.WavName, "WAVE");
//    qstrcpy(WavFileHeader.FmtName, "fmt ");
//    qstrcpy(WavFileHeader.DATANAME, "data");

//    // 表示 FMT块 的长度
//    WavFileHeader.nFmtLength = 16;
//    // 表示 按照PCM 编码;
//    WavFileHeader.nAudioFormat = 1;
//    // 声道数目;
//    WavFileHeader.nChannleNumber = 1;
//    // 采样频率;
//    WavFileHeader.nSampleRate = 8000;

//    // nBytesPerSample 和 nBytesPerSecond这两个值通过设置的参数计算得到;
//    // 数据块对齐单位(每个采样需要的字节数 = 通道数 × 每次采样得到的样本数据位数 / 8 )
//    WavFileHeader.nBytesPerSample = 2;
//    // 波形数据传输速率
//    // (每秒平均字节数 = 采样频率 × 通道数 × 每次采样得到的样本数据位数 / 8  = 采样频率 × 每个采样需要的字节数 )
//    WavFileHeader.nBytesPerSecond = 16000;

//    // 每次采样得到的样本数据位数;
//    WavFileHeader.nBitsPerSample = 16;

//    QFile cacheFile(catheFileName);
//    QFile wavFile(wavFileName);

//    if (!cacheFile.open(QIODevice::ReadWrite))
//    {
//        return -1;
//    }
//    if (!wavFile.open(QIODevice::WriteOnly))
//    {
//        return -2;
//    }

//    int nSize = sizeof(WavFileHeader);
//    qint64 nFileLen = cacheFile.bytesAvailable();

//    WavFileHeader.nRiffLength = nFileLen - 8 + nSize;
//    WavFileHeader.nDataLength = nFileLen;

//    // 先将wav文件头信息写入，再将音频数据写入;
//    wavFile.write((char *)&WavFileHeader, nSize);
//    wavFile.write(cacheFile.readAll());

//    cacheFile.close();
//    wavFile.close();

//    return nFileLen;
//}

qint64 EbVoiceRecorder::writeData(const char *data, qint64 len)
{
    const unsigned long nFileLen = m_wavFileHeader.nDataLength + len;
    m_wavFileHeader.nRiffLength = nFileLen - 8 + const_wav_file_header_size;
    m_wavFileHeader.nDataLength = nFileLen;
    return m_wavFile.write(data, len);
}
