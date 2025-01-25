#ifndef QTDOWNLOADER_H
#define QTDOWNLOADER_H

#include <QObject>

#include <QFile>
#include <QMutex>
#include <QPointer>
#include <QEventLoop>
#include <QElapsedTimer>
#include <QNetworkAccessManager>
#include <QNetworkReply>

//文件下载器
class QtDownloader : public QObject
{
    Q_OBJECT
public:
    explicit QtDownloader(QObject *parent = nullptr);
    ~QtDownloader();
    //返回值0.成功，-1.下载中，-2.无效链接，-3.无法创建文件
    int start(const QString& url, const QString& saveFile);
    //返回值0.成功，-1.下载中，-2.无效链接
    int start(const QString& url,
        std::function<void(QByteArray data)> funcData);
    bool isDownloading();
    void stop();
    //获得文件信息
    static bool getFileInfo(const QString& url,
        QString& retFileName, qint64& retFileSize, int outTime = 3000);
    //转换空间占用单位
    static QString transformUnit(double bytes);
Q_SIGNALS:
    void sigDownloadFinish(QNetworkReply::NetworkError error);
    void sigDownloadProgress(
        qint64 bytesRead, qint64 totalBytes,
        double progress, double speed);
private:
    void closeFile();
    QRecursiveMutex m_mutex;
    QNetworkAccessManager* m_networkManager{};
    QPointer<QNetworkReply> m_reply{};
    QPointer<QFile> m_saveFile{};
    QEventLoop* m_loopEvent{};
    QElapsedTimer m_downloadStartTime;
    qint64 m_allTime = 0;
};

#endif // QTDOWNLOADER_H
