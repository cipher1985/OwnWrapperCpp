#ifndef QTZIPWRAPPER_H
#define QTZIPWRAPPER_H

#include <QString>

class QZipWriter;
//基于QT实现的zip控制封装类(需要QT += gui-private)
class QtZipWrapper
{
public:
    QtZipWrapper(const QString& createZipFile = QString());
    ~QtZipWrapper();
    //解压到指定文件夹
    static bool UnZip(const QString& zipFile, const QString& extractPath = QString());
    //创建压缩文件
    void Create(const QString& zipFile);
    //向压缩文件中添加文件夹
    void AddFolder(const QString& pathFolder,
        bool includeRootFolder = true, QString zipLocate = QString());
    //向压缩文件中添加文件
    void AddFile(const QString& pathFile, QString zipLocate = QString());
    //关闭压缩文件
    void Close();
private:
    QZipWriter* m_pWriter{};
};

#endif // QTZIPWRAPPER_H
