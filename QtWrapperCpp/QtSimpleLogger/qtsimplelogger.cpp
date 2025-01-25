#include "qtsimplelogger.h"
#include <QFile>
#include <QFileInfo>
#include <QThread>
#include <QDateTime>

static QString g_sSimpleLoggerDefaultFormat =
    "[{time}][{thread}][{type}]: {msg} [FILE:\"{file}\" LINE.{line} FUNC:\"{func}\"]";
static QFile g_sSystemLoggerFile;
static quint64 g_sSystemLoggerFileMaxSize;
static quint64 g_sSystemLoggerBackupFileMaxNum;
static QMutex g_sSystemLoggerMutex;

void QtSimpleLogger::outputMessage(QtMsgType type,
    const QMessageLogContext &context, const QString &msg)
{
    QMutexLocker locker(&g_sSystemLoggerMutex);
    if(!g_sSystemLoggerFile.isOpen())
        return;
    QString curMsg = formatMsg(g_sSimpleLoggerDefaultFormat, msg, type,
        context.file, QString::number(context.line), context.function);
    g_sSystemLoggerFile.write(curMsg.toLocal8Bit());
    if ((quint64)g_sSystemLoggerFile.size() >= g_sSystemLoggerFileMaxSize)
        createNewLogFile(g_sSystemLoggerFile, g_sSystemLoggerBackupFileMaxNum);
}

bool QtSimpleLogger::createSystemLog(
    const QString &logFile, quint64 fileMaxSize, int fileMaxNum)
{
    QMutexLocker locker(&g_sSystemLoggerMutex);
    bool ret = false;
    if(g_sSystemLoggerFile.isOpen())
        return ret;
    g_sSystemLoggerFile.setFileName(logFile);
    if(!g_sSystemLoggerFile.open(QIODevice::ReadWrite | QIODevice::Append))
        return ret;
    ret = true;
    g_sSystemLoggerFileMaxSize = fileMaxSize;
    g_sSystemLoggerBackupFileMaxNum = fileMaxNum;
    if ((quint64)g_sSystemLoggerFile.size() >= g_sSystemLoggerFileMaxSize)
        ret = createNewLogFile(g_sSystemLoggerFile, g_sSystemLoggerBackupFileMaxNum);
    if(ret)
        qInstallMessageHandler(outputMessage);
    else {
        g_sSystemLoggerFile.close();
    }
    return ret;
}

void QtSimpleLogger::setSystemLogFormat(const QString &format)
{
    QMutexLocker locker(&g_sSystemLoggerMutex);
    g_sSimpleLoggerDefaultFormat = format;
}

void QtSimpleLogger::closeSystemLog()
{
    QMutexLocker locker(&g_sSystemLoggerMutex);
    if(!g_sSystemLoggerFile.isOpen())
        return;
    g_sSystemLoggerFile.close();
    qInstallMessageHandler(nullptr);
}

///////////////////////////////////////////////////////////////////////
QtSimpleLogger::QtSimpleLogger(QObject *parent)
    : QObject{parent}
{
    m_file = new QFile(this);
    setFormat();
}

bool QtSimpleLogger::create(const QString &filename,
    quint64 fileMaxSize, int fileMaxNum)
{
    QMutexLocker locker(&m_mutex);
    close();
    m_file->setFileName(filename);
    if(!m_file->open(QIODevice::ReadWrite | QIODevice::Append))
        return false;
    m_fileMaxSize = fileMaxSize;
    m_fileMaxNum = fileMaxNum;
    if ((quint64)m_file->size() >= m_fileMaxSize)
        return createNewLogFile(*m_file, m_fileMaxNum);
    return true;
}

void QtSimpleLogger::setFormat(const QString &format)
{
    QMutexLocker locker(&m_mutex);
    m_format = format;
}

void QtSimpleLogger::write(
    const QString &msg, QtMsgType type,
    const QString& extCurFile,
    const QString& extCurLine,
    const QString& extCurFunc)
{
    QMutexLocker locker(&m_mutex);
    if(!m_file->isOpen()) {
        switch (type) {
        case QtInfoMsg:
            QMessageLogger(extCurFile.toLocal8Bit().data(),
                extCurLine.toInt(), extCurFunc.toLocal8Bit().data()).info()<<msg;
            break;
        case QtWarningMsg:
            QMessageLogger(extCurFile.toLocal8Bit().data(),
                extCurLine.toInt(), extCurFunc.toLocal8Bit().data()).warning()<<msg;
            break;
        case QtCriticalMsg:
            QMessageLogger(extCurFile.toLocal8Bit().data(),
                extCurLine.toInt(), extCurFunc.toLocal8Bit().data()).critical()<<msg;
            break;
        case QtFatalMsg:
            QMessageLogger(extCurFile.toLocal8Bit().data(),
                extCurLine.toInt(), extCurFunc.toLocal8Bit().data()).fatal(msg.toLocal8Bit().data());
            break;
        default:
            QMessageLogger(extCurFile.toLocal8Bit().data(),
                extCurLine.toInt(), extCurFunc.toLocal8Bit().data()).debug()<<msg;
            break;
        }
        return;
    }
    //输出文件
    QString curMsg = formatMsg(m_format, msg, type, extCurFile, extCurLine, extCurFunc);
    m_file->write(curMsg.toLocal8Bit());
    if ((quint64)m_file->size() >= m_fileMaxSize)
        createNewLogFile(*m_file, m_fileMaxNum);
}

void QtSimpleLogger::close()
{
    QMutexLocker locker(&m_mutex);
    if(!m_file->isOpen())
        return;
    m_file->close();
}

void QtSimpleLogger::debug(const QString &msg, const QString& extCurFile,
    const QString& extCurLine, const QString& extCurFunc)
{
    write(msg, QtDebugMsg, extCurFile, extCurLine, extCurFunc);
}

void QtSimpleLogger::info(const QString &msg, const QString& extCurFile,
    const QString& extCurLine, const QString& extCurFunc)
{
    write(msg, QtInfoMsg, extCurFile, extCurLine, extCurFunc);
}

void QtSimpleLogger::warning(const QString &msg, const QString& extCurFile,
    const QString& extCurLine, const QString& extCurFunc)
{
    write(msg, QtWarningMsg, extCurFile, extCurLine, extCurFunc);
}

void QtSimpleLogger::critical(const QString &msg, const QString& extCurFile,
    const QString& extCurLine, const QString& extCurFunc)
{
    write(msg, QtCriticalMsg, extCurFile, extCurLine, extCurFunc);
}

void QtSimpleLogger::fatal(const QString &msg, const QString& extCurFile,
    const QString& extCurLine, const QString& extCurFunc)
{
    write(msg, QtFatalMsg, extCurFile, extCurLine, extCurFunc);
}

QString QtSimpleLogger::formatMsg(
    const QString& format,
    const QString &msg, QtMsgType type,
    const QString &extCurFile,
    const QString &extCurLine,
    const QString &extCurFunc)
{
    QString curMsg = format;
    QString typeText;
    switch (type) {
    case QtInfoMsg:
        typeText = QString("I");
        break;
    case QtWarningMsg:
        typeText = QString("W");
        break;
    case QtCriticalMsg:
        typeText = QString("C");
        break;
    case QtFatalMsg:
        typeText = QString("F");
        break;
    default:
        typeText = QString("D");
        break;
    }
    QString curDateTime =
        QDateTime::currentDateTime().toString("yyyy/MM/dd hh:mm:ss.zzz");
    curMsg.replace("{time}", curDateTime);
    curMsg.replace("{thread}", QString::number(quint64(QThread::currentThreadId())));
    curMsg.replace("{type}", typeText);
    curMsg.replace("{msg}", msg);

    curMsg.replace("{file}", extCurFile);
    curMsg.replace("{line}", extCurLine);
    curMsg.replace("{func}", extCurFunc);
    curMsg.append("\n");
    return curMsg;
}

bool QtSimpleLogger::createNewLogFile(QFile& file, int backupFileMaxNum)
{
    QFileInfo fileInfo(file.fileName());
    QString baseName = fileInfo.completeBaseName();
    QString extName = fileInfo.suffix();
    QString curFile = QString("%1.%2.%3").arg(baseName).arg(backupFileMaxNum).arg(extName);
    QString nextFile;
    if(QFile::exists(curFile))
        QFile::remove(curFile);
    for (int i = backupFileMaxNum - 1; i > 0; --i) {
        curFile = QString("%1.%2.%3").arg(baseName).arg(i).arg(extName);
        nextFile = QString("%1.%2.%3").arg(baseName).arg(i + 1).arg(extName);
        if(QFile::exists(curFile)) {
            QFile::rename(curFile, nextFile);
        }
    }
    file.close();
    curFile = QString("%1.%2").arg(baseName, extName);
    nextFile = QString("%1.1.%2").arg(baseName, extName);
    QFile::rename(curFile, nextFile);
    if(!file.open(QIODevice::ReadWrite))
        return false;
    return true;
}

