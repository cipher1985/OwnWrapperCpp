#include "qtjsonparser.h"

#include <QFile>

QString QtJsonParser::jsonToText(const QJsonObject& obj,
        QJsonDocument::JsonFormat format)
{
    QJsonDocument jsonDoc(obj);
    return jsonDoc.toJson(format);
}

QString QtJsonParser::jsonToText(const QJsonArray& arr,
        QJsonDocument::JsonFormat format)
{
    QJsonDocument jsonDoc(arr);
    return jsonDoc.toJson(format);
}

QJsonObject QtJsonParser::textToJsonObject(const QString &jsonText)
{
    QJsonObject obj;
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(jsonText.toUtf8(), &error);
    if (error.error == QJsonParseError::NoError)
        obj = doc.object();
    return obj;
}
QJsonArray QtJsonParser::textToJsonArray(const QString &jsonText)
{
    QJsonArray arr;
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(jsonText.toUtf8(), &error);
    if (error.error == QJsonParseError::NoError)
        arr = doc.array();
    return arr;
}
//////////////////////////////////////////////////////////////////////////////////////
bool QtJsonParser::loadFile(const QString& jsonFile)
{
    m_nodes.clear();
    QJsonObject obj;
    QFile file(jsonFile);
    if(file.open(QIODevice::ReadOnly)) {
        QByteArray ba = file.readAll();
        obj = textToJsonObject(ba);
    }
    if(!obj.isEmpty()) {
        m_nodes.push_back(node{jsonFile, obj});
        return true;
    }
    return false;
}

bool QtJsonParser::loadText(const QString& jsonString)
{
    QJsonObject obj = textToJsonObject(jsonString);
    if(obj.isEmpty())
        return false;

    m_nodes.push_back(node{"", obj});
    return true;
}

bool QtJsonParser::into(const QString &key)
{
    if(m_nodes.count() == 0)
        return false;

    QJsonObject& obj = m_nodes.back().obj;
    if(obj.isEmpty() || !obj.contains(key))
        return false;

    QJsonValue v = obj.value(key);
    if(!v.isObject())
        return false;

    m_nodes.push_back(node{key, obj});
    return true;
}
void QtJsonParser::outof()
{
    int count = m_nodes.count();
    if(count <= 1)
        return;

    node back = m_nodes.back();
    m_nodes.pop_back();
    m_nodes.back().obj[back.key] = back.obj;
}
//////////////////////////////////////////////////////////////////////////////////////
QJsonValue QtJsonParser::getValue(const QString& key, QJsonValue defaultValue)
{
    QJsonValue v = defaultValue;
    if(m_nodes.count() == 0)
        return v;

    QJsonObject& obj = m_nodes.back().obj;
    if(obj.isEmpty() || !obj.contains(key))
        return v;

    v = obj.value(key);
    return v;
}

bool QtJsonParser::getBool(const QString &key, bool defaultValue)
{
    QJsonValue v = getValue(key);
    if(v.isNull())
        return defaultValue;
    return v.toBool(defaultValue);
}

int QtJsonParser::getInt(const QString &key, int defaultValue)
{
    QJsonValue v = getValue(key);
    if(v.isNull())
        return defaultValue;
    return v.toInt(defaultValue);
}

double QtJsonParser::getDouble(const QString &key, double defaultValue)
{
    QJsonValue v = getValue(key);
    if(v.isNull())
        return defaultValue;
    return v.toDouble(defaultValue);
}

QString QtJsonParser::getString(const QString &key, QString defaultValue)
{
    QJsonValue v = getValue(key);
    if(v.isNull())
        return defaultValue;
    return v.toString(defaultValue);
}

QJsonArray QtJsonParser::getArray(const QString &key, QJsonArray defaultValue)
{
    QJsonValue v = getValue(key);
    if(v.isNull())
        return defaultValue;
    return v.toArray(defaultValue);
}
//////////////////////////////////////////////////////////////////////////////////////
void QtJsonParser::setValue(const QString &key, const QJsonValue &value)
{
    if(m_nodes.count() == 0) {
        QJsonObject obj;
        obj.insert(key, value);
        m_nodes.push_back(node{"", obj});
        return;
    }
    QJsonObject& obj = m_nodes.back().obj;
    obj.insert(key, value);
}
void QtJsonParser::setArray(const QString &key, const QJsonArray &value)
{
    setValue(key, value);
}

void QtJsonParser::setBool(const QString &key, const bool &value)
{
    setValue(key, value);
}

void QtJsonParser::setInt(const QString &key, const int &value)
{
    setValue(key, value);
}

void QtJsonParser::setDouble(const QString &key, const double &value)
{
    setValue(key, value);
}

void QtJsonParser::setString(const QString &key, const QString &value)
{
    setValue(key, value);
}
//////////////////////////////////////////////////////////////////////////////////////
bool QtJsonParser::saveFile(QString jsonFile,
    QJsonDocument::JsonFormat format)
{
    updateData();
    if(m_nodes.count() <= 0)
        return false;

    if(jsonFile.isEmpty())
        jsonFile = m_nodes[0].key;
    if(jsonFile.isEmpty() || m_nodes[0].obj.isEmpty())
        return false;
    QFile file(jsonFile);
    if (!file.open(QIODevice::WriteOnly))
        return false;
    QJsonDocument saveDoc(m_nodes[0].obj);
    file.write(saveDoc.toJson(format));
    m_nodes[0].key = jsonFile;
    return true;
}

QString QtJsonParser::getJsonString(QJsonDocument::JsonFormat format)
{
    updateData();
    if(m_nodes.count() <= 0)
        return QString();
    return jsonToText(m_nodes[0].obj, format);
}

void QtJsonParser::updateData()
{
    for(int i = m_nodes.count() - 1;i > 0; --i) {
        node& newData = m_nodes[i];
        m_nodes[i - 1].obj[newData.key] = newData.obj;
    }
}
