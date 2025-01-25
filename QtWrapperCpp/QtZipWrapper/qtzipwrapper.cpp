#include "qtzipwrapper.h"
#include <QDir>
#include <private/qzipwriter_p.h>
#include <private/qzipreader_p.h>

QtZipWrapper::QtZipWrapper(const QString &createZipFile)
{
    Create(createZipFile);
}

QtZipWrapper::~QtZipWrapper()
{
    Close();
}
bool QtZipWrapper::UnZip(const QString &zipFile, const QString &extractPath)
{
    QZipReader reader(QDir().absoluteFilePath(zipFile));
    return reader.extractAll(QDir().absoluteFilePath(extractPath));
}

void QtZipWrapper::Create(const QString &zipFile)
{
    Close();
    if(zipFile.isEmpty())
    {
        return;
    }
    m_pWriter = new QZipWriter(QDir().absoluteFilePath(zipFile));
}

void QtZipWrapper::AddFolder(const QString &pathFolder,
    bool includeRootFolder, QString zipLocate)
{
    // 判断路径是否存在
    QDir dir(pathFolder);
    if(!dir.exists())
    {
        return;
    }
    if(!zipLocate.isEmpty())
    {
        zipLocate.replace("/","\\");
        if(!zipLocate.endsWith("\\"))
        {
            zipLocate.append('\\');
        }
        zipLocate = QDir::toNativeSeparators(zipLocate);
    }
    if(includeRootFolder)
    {
        zipLocate.append(QFileInfo(pathFolder).fileName());
        zipLocate.append('\\');
        zipLocate = QDir::toNativeSeparators(zipLocate);
    }
    dir.setFilter(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot);
    QFileInfoList list = dir.entryInfoList();
    // 遍历文件列表
    foreach (QFileInfo fileInfo, list)
    {
        if(fileInfo.isDir())
        {
            m_pWriter->addDirectory(zipLocate + fileInfo.fileName());
            AddFolder(fileInfo.absoluteFilePath(), false, zipLocate + fileInfo.fileName());
        }
        //添加需要压缩的文件
        AddFile(fileInfo.absoluteFilePath(), zipLocate);
    }
}

void QtZipWrapper::AddFile(const QString &pathFile, QString zipLocate)
{
    QFileInfo fileInfo(pathFile);
    if(!fileInfo.isFile())
    {
        return;
    }
    QFile file(fileInfo.absoluteFilePath());
    if(!file.open(QIODevice::ReadOnly))
    {
        return;
    }
    if(zipLocate.isEmpty())
    {
        zipLocate = fileInfo.fileName();
    }else
    {
        zipLocate.replace('/','\\');
        if(zipLocate.endsWith("\\"))
        {
            zipLocate.append(fileInfo.fileName());
        }
        zipLocate = QDir::toNativeSeparators(zipLocate);
    }
    m_pWriter->addFile(zipLocate, file.readAll());
}

void QtZipWrapper::Close()
{
    if(!m_pWriter)
    {
        return;
    }
    m_pWriter->close();
    delete m_pWriter;
    m_pWriter = nullptr;
}
