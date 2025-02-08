#ifndef QTTOMLPARSER_H
#define QTTOMLPARSER_H

#include <QObject>
#include <QDateTime>
#include <QStack>
#include "toml.hpp"

//基于toml++实现的Toml文件解析器
class QtTomlParser : public QObject
{
    Q_OBJECT
public:
    //将toml字符串转为json字符串
    static QString tomlToJson(const QString& tomlString);
    //将json字符串转为toml字符串
    static QString jsonToToml(const QString& jsonString);
public:
    explicit QtTomlParser(QObject *parent = nullptr);
    //加载toml文件
    bool loadFile(const QString& tomlFile);
    //加载toml字符串
    bool loadText(const QString& tomlString);
    //进入节点
    bool into(const QString& key);
    //返回节点
    void outof();
    //获得当前节点数据
    bool getBool(const QString& key, bool defaultValue = false);
    int64_t getInt(const QString& key, int64_t defaultValue = 0);
    double getFloat(const QString& key, double defaultValue = 0.0);
    QString getString(const QString& key, const QString& defaultValue = QString());
    QDate getDate(const QString& key, const QDate& defaultValue = QDate());
    QTime getTime(const QString& key, const QTime& defaultValue = QTime());
    QDateTime getDateTime(const QString& key, const QDateTime& defaultValue = QDateTime());
    toml::table* getTable(const QString& key);
    toml::array* getArray(const QString& key);
    toml::node* getNode(const QString& key);
    //设置当前节点数据
    void setBool(const QString& key, const bool& value);
    void setInt(const QString& key, const int64_t& value);
    void setFloat(const QString& key, const double& value);
    void setString(const QString& key, const QString& value);
    void setData(const QString& key, const QDate& value);
    void setTime(const QString& key, const QTime& value);
    void setDateTime(const QString& key, const QDateTime& value);
    void setTable(const QString& key, const toml::table& value);
    void setArray(const QString& key, const toml::array& value);
    void setNode(const QString& key, const toml::node& value);
    //保存数据(文件名为空保存为当前打开文件)
    bool saveFile(const QString& tomlFile = QString());
    //获得当前Toml数据字符串
    QString getTomlString();
private:
    static void convertJsonToToml(
        const QJsonObject& jsonObject, toml::table& tomlTable);
    template<typename T>
    void setValue(const QString& key, const T &value);
    toml::table* getCurTable();
    toml::table m_rootTable;
    QStack<toml::table*> m_nodeStack;
    QString m_curPathFile;
};

template<typename T>
inline void QtTomlParser::setValue(const QString &key, const T &value)
{
    toml::table* curTable = getCurTable();
    QList<QString> parts = key.split('.', Qt::SkipEmptyParts);
    int count = parts.count();
    if(count == 0)
        return;
    //遍历键深入表
    for (int i = 0; i < count - 1; ++i)
    {
        QString part = parts.at(i);
        if (!curTable->contains(part.toStdString()) ||
            !curTable->at(part.toStdString()).is_table())
            curTable->insert_or_assign(part.toStdString(), toml::table());
        curTable = curTable->at(part.toStdString()).as_table();
    }
    //在最下层表中插入或更新值
    curTable->insert_or_assign(parts.last().toStdString(), value);
}

#endif // QTTOMLPARSER_H
