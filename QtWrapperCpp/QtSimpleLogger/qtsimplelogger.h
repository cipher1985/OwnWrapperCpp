/********************************************************
*
* 文件名:  qtsimplelogger.h
* 版权:   ChinaEdu Co. Ltd. Copyright 2020 All Rights Reserved.
* 描述:   基于QT的简易日志类
*
* 修改人:    傅祯勇
* 修改内容:
* 版本:      1.0
* 修改时间:  2024-12-20
*
********************************************************/
#ifndef QTSIMPLELOGGER_H
#define QTSIMPLELOGGER_H

#include <QObject>
#include <QDebug>
#include <QMutex>

class QFile;

#define DEFAULT_SIMPLE_LOGFILE_SIZE (quint64(2) << 10)
//简易日志类
class QtSimpleLogger : public QObject
{
    Q_OBJECT
public:
    //接管Qt系统日志输出
    static bool createSystemLog(const QString& logFile,
        quint64 fileMaxSize = DEFAULT_SIMPLE_LOGFILE_SIZE,
        int backupFileMaxNum = 4);
    //设置接管Qt系统日志输出格式
    static void setSystemLogFormat(const QString& format);
    //关闭文件并取消Qt系统日志接管
    static void closeSystemLog();
public:
    explicit QtSimpleLogger(QObject *parent = nullptr);
    //创建日志文件
    bool create(const QString& filename,
        quint64 fileMaxSize = DEFAULT_SIMPLE_LOGFILE_SIZE,
        int backupFileMaxNum = 4);
    //设置日志输出格式
    //固定类型:写入时间{time}，当前线程{thread}，日志类型{type}，日志内容{msg}
    //固定扩展类型(需输出时设置):当前调用文件{file}，当前调用行数{line}，当前调用函数{func}
    void setFormat(const QString& format = "[{time}][{thread}][{type}]: {msg}");
    //输出日志(创建日志文件后可用，扩展内容可直接使用预定义_LOG_FLF_)
    void write(const QString& msg, QtMsgType type = QtInfoMsg,
        const QString& extCurFile = QString(),
        const QString& extCurLine = QString(),
        const QString& extCurFunc = QString());
    //关闭日志文件
    void close();
public:
    //输出日志
    void debug(const QString& msg,
        const QString& extCurFile = QString(),
        const QString& extCurLine = QString(),
        const QString& extCurFunc = QString());
    void info(const QString& msg,
        const QString& extCurFile = QString(),
        const QString& extCurLine = QString(),
        const QString& extCurFunc = QString());
    void warning(const QString& msg,
        const QString& extCurFile = QString(),
        const QString& extCurLine = QString(),
        const QString& extCurFunc = QString());
    void critical(const QString& msg,
        const QString& extCurFile = QString(),
        const QString& extCurLine = QString(),
        const QString& extCurFunc = QString());
    void fatal(const QString& msg,
        const QString& extCurFile = QString(),
        const QString& extCurLine = QString(),
        const QString& extCurFunc = QString());
private:
    static QString formatMsg(
        const QString& format,
        const QString& msg, QtMsgType type,
        const QString& extCurFile,
        const QString& extCurLine,
        const QString& extCurFunc);
    static bool createNewLogFile(QFile& file, int backupFileMaxNum);
    static void outputMessage(QtMsgType type,
        const QMessageLogContext &context, const QString &msg);
    QFile* m_file;
    QString m_format;
    quint64 m_fileMaxSize;
    int m_fileMaxNum;
    QRecursiveMutex m_mutex;
};

#define _LOG_FIN_ __FILE__, QString::number(__LINE__), __FUNCTION__

#endif // QTSIMPLELOGGER_H
