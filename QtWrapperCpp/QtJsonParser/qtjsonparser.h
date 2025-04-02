/********************************************************
*
* 文件名: qtjsonparser.h
* 版权:   ChinaEdu Co. Ltd. Copyright 2022 All Rights Reserved.
* 描述:   基于QT的json数据解析器
*
* 修改人:     傅祯勇
* 修改内容:
* 版本:       1.0
* 修改时间:    2024-12-20
*
* 版本:       1.1
* 修改时间:    2025-04-02
*
********************************************************/
#ifndef QJSON_PARSER_H
#define QJSON_PARSER_H

#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

class QtJsonParser
{
public:
    //QJsonObject对象转换为字符串
    static QString jsonToText(const QJsonObject& obj,
        QJsonDocument::JsonFormat format = QJsonDocument::Compact);
    //QJsonArray对象转换为字符串
    static QString jsonToText(const QJsonArray& arr,
        QJsonDocument::JsonFormat format = QJsonDocument::Compact);
    //字符串转换为QJsonObject对象
    static QJsonObject textToJsonObject(const QString& jsonText);
    //字符串转换为QJsonArray对象
    static QJsonArray textToJsonArray(const QString& jsonText);
public:
    //加载Json文件
    bool loadFile(const QString& jsonFile);
    //加载json字符串
    bool loadText(const QString& jsonString);
    //加载json对象
    bool loadJson(const QJsonObject& jsonObj);
    //进入节点
    bool into(const QString& key);
    //返回节点
    void outof();
    //获得当前节点数据
    bool getBool(const QString& key, bool defaultValue = false);
    int getInt(const QString& key, int defaultValue = 0);
    double getDouble(const QString& key, double defaultValue = 0.0);
    QString getString(const QString& key, QString defaultValue = QString());
    QJsonArray getArray(const QString& key, QJsonArray defaultValue = QJsonArray());
    QJsonValue getValue(const QString& key, QJsonValue defaultValue = QJsonValue());
    //设置当前节点数据
    void setBool(const QString& key, const bool& value);
    void setInt(const QString& key, const int& value);
    void setDouble(const QString& key, const double& value);
    void setString(const QString& key, const QString& value);
    void setArray(const QString& key, const QJsonArray& value);
    void setValue(const QString& key, const QJsonValue& value);
    //保存数据(文件名为空保存为当前打开文件)
    bool saveFile(QString jsonFile = QString(),
        QJsonDocument::JsonFormat format = QJsonDocument::Indented);
    //获得当前JSON数据字符串
    QString getJsonString(QJsonDocument::JsonFormat format = QJsonDocument::Compact);
private:
    //更新所有节点数据
    void updateData();
    //数据节点记录
    struct node
    {
        QString key;
        QJsonObject obj;
    };
    QList<node>  m_nodes;
};

#endif // QJSON_PARSER_H
