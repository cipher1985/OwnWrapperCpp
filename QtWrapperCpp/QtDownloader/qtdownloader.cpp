#include "qtdownloader.h"

#include <QUrl>
#include <QTimer>
#include <QEventLoop>
#include <QFileInfo>

#define UNIT_KB 1024            //KB
#define UNIT_MB 1024*1024       //MB
#define UNIT_GB 1024*1024*1024  //GB

QtDownloader::QtDownloader(QObject *parent)
    : QObject{parent}
{
    m_networkManager = new QNetworkAccessManager(this);
    m_loopEvent = new QEventLoop(this);
}

QtDownloader::~QtDownloader()
{
    stop();
}

int QtDownloader::start(const QString &url, const QString &saveFile)
{
    QMutexLocker locker(&m_mutex);
    m_saveFile = new QFile(saveFile, this);
    if(!m_saveFile->open(QIODevice::WriteOnly))
        return -3;
    int ret = start(url, [this](QByteArray data) {
        m_saveFile->write(data); });
    if(ret != 0)
        closeFile();
    return ret;
}

int QtDownloader::start(const QString &url,
    std::function<void(QByteArray)> funcData)
{
    QMutexLocker locker(&m_mutex);
    if(m_reply)
        return -1;
    QUrl newUrl = QUrl::fromUserInput(url);
    if(!newUrl.isValid())
        return -2;
    m_allTime = 0;
    m_downloadStartTime.restart();
    m_reply = m_networkManager->get(QNetworkRequest(newUrl));
    connect(m_reply, &QNetworkReply::finished, this, [this, funcData]{
        QMutexLocker locker(&m_mutex);
        QNetworkReply::NetworkError error = m_reply->error();
        if(error == QNetworkReply::NoError && funcData)
            funcData(m_reply->readAll());
        m_reply->deleteLater();
        m_reply.clear();
        closeFile();
        emit sigDownloadFinish(error);
        m_loopEvent->quit();
    });
    connect(m_reply, &QNetworkReply::readyRead, this, [this, funcData]{
        QMutexLocker locker(&m_mutex);
        if(funcData)
            funcData(m_reply->readAll());
    });
    connect(m_reply, &QNetworkReply::downloadProgress,
        this, [this](qint64 bytesRead, qint64 totalBytes) {
        QMutexLocker locker(&m_mutex);
        double progress = (double)bytesRead / totalBytes * 100;
        //计算实时速度
        double speed = 0;
        m_allTime = m_downloadStartTime.elapsed();
        if (m_allTime > 0)
            speed = bytesRead / (m_allTime / 1000.0); // B/s
        emit sigDownloadProgress(bytesRead, totalBytes, progress, speed);
    });
    return 0;
}

bool QtDownloader::isDownloading()
{
    QMutexLocker locker(&m_mutex);
    return m_reply;
}

void QtDownloader::stop()
{
    if(!m_reply)
        return;
    m_reply->abort();
    m_loopEvent->exec();
}

// 根据输入下载url，请求获取文件信息l
bool QtDownloader::getFileInfo(const QString& url,
    QString& retFileName, qint64& retFileSize, int outTime)
{
    qint64 fileSize = -1;

    // 如果有代理需要设置代理;
    // QNetworkProxy proxy;
    // proxy.setType(QNetworkProxy::HttpProxy);
    // proxy.setHostName("proxy.example.com");
    // proxy.setPort(1080);
    // proxy.setUser("username");
    // proxy.setPassword("password");
    // QNetworkProxy::setApplicationProxy(proxy);

    QNetworkAccessManager manager;
    // 事件循环，等待请求文件头信息结束;
    QEventLoop loop;
    // 超时，结束事件循环;
    QTimer timer;

    //发出请求，获取文件地址的头部信息;
    QNetworkReply *reply = manager.head(QNetworkRequest(url));
    if (!reply)
        return false;

    connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    connect(&timer, SIGNAL(timeout()), &loop, SLOT(quit()));

    timer.start(outTime);
    loop.exec();

    if (reply->error() != QNetworkReply::NoError)
    {
        // 请求发生错误;
        //qDebug() << reply->errorString();
        reply->deleteLater();
        return false;
    }
    else if (!timer.isActive())
    {
        // 请求超时超时,未获取到文件信息;
        //qDebug() << "Request Timeout";
        reply->deleteLater();
        return false;
    }
    timer.stop();
    // 获取文件大小;
    QVariant var = reply->header(QNetworkRequest::ContentLengthHeader);
    fileSize = var.toLongLong();
    // 获取文件名;
    // 先根据请求返回的信息中获取文件名，因为有些文件服务器未必会返回文件名信息(包括文件名，文件大小);可使用下面方法;
    // QFileInfo 会根据 url 返回 下载文件名;但也会失败，返回为空值;
    QString strDisposition = reply->rawHeader("Content-Disposition");
    int index = strDisposition.indexOf("filename=");
    QString fileName = strDisposition.mid(index + 9);
    if (fileName.isEmpty())
    {
        QUrl url(url);
        QFileInfo fileInfo(url.path());
        fileName += fileInfo.fileName();
    }
    reply->deleteLater();
    //设置返回值
    retFileName = fileName;
    retFileSize = fileSize;
    return true;
}

// 转换单位;
QString QtDownloader::transformUnit(double bytes)
{
    QString strUnit = "B";
    if (bytes <= 0)
    {
        bytes = 0;
    }
    else if (bytes < UNIT_KB)
    {
    }
    else if (bytes < UNIT_MB)
    {
        bytes /= UNIT_KB;
        strUnit = "KB";
    }
    else if (bytes < UNIT_GB)
    {
        bytes /= UNIT_MB;
        strUnit = "MB";
    }
    else if (bytes > UNIT_GB)
    {
        bytes /= UNIT_GB;
        strUnit = "GB";
    }
    // 保留小数点后两位;
    return QString("%1 %2").arg(QString::number(bytes, 'f', 2)).arg(strUnit);
}

void QtDownloader::closeFile()
{
    if(!m_saveFile)
        return;
    m_saveFile->close();
    m_saveFile->deleteLater();
    m_saveFile.clear();
}
