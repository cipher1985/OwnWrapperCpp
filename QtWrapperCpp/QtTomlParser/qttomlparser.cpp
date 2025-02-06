#include "qttomlparser.h"

#include <QFile>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <sstream>

QString QtTomlParser::tomlToJson(const QString &tomlString)
{
    try {
        auto tomlData = toml::parse(tomlString.toUtf8().data());
        std::ostringstream oss;
        oss << toml::json_formatter(tomlData);
        return QString::fromStdString(oss.str());
    } catch(...){
        return QString();
    }
}

QString QtTomlParser::jsonToToml(const QString &jsonString)
{
    //解析JSON文本
    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonString.toUtf8());
    if (!jsonDoc.isObject())
        return QString();
    //将JSON对象转换为TOML表
    toml::table tomlTable;
    convertJsonToToml(jsonDoc.object(), tomlTable);
    //将TOML表转换为字符串
    std::ostringstream oss;
    oss << tomlTable;
    return QString::fromStdString(oss.str());
}

void QtTomlParser::convertJsonToToml(
    const QJsonObject& jsonObject, toml::table& tomlTable)
{
    for (auto it = jsonObject.begin(); it != jsonObject.end(); ++it) {
        const QString key = it.key();
        const QJsonValue value = it.value();

        if (value.isBool()) {
            tomlTable.insert_or_assign(key.toStdString(), value.toBool());
        } else if (value.isDouble()) {
            tomlTable.insert_or_assign(key.toStdString(), value.toDouble());
        } else if (value.isString()) {
            tomlTable.insert_or_assign(key.toStdString(), value.toString().toStdString());
        } else if (value.isArray()) {
            toml::array array;
            const QJsonArray jsonArray = value.toArray();
            for (const QJsonValue& item : jsonArray) {
                if (item.isBool()) {
                    array.push_back(item.toBool());
                }
                else if (item.isDouble()) {
                    array.push_back(item.toDouble());
                }
                else if (item.isString()) {
                    array.push_back(item.toString().toStdString());
                }
                else if (item.isObject()) {
                    toml::table subTable;
                    convertJsonToToml(item.toObject(), subTable);
                    array.push_back(subTable);
                }
            }
            tomlTable.insert_or_assign(key.toStdString(), array);
        }
        else if (value.isObject())
        {
            toml::table subTable;
            convertJsonToToml(value.toObject(), subTable);
            tomlTable.insert_or_assign(key.toStdString(), subTable);
        }
    }
}

QtTomlParser::QtTomlParser(QObject *parent)
    : QObject{parent}{}

bool QtTomlParser::loadFile(const QString &tomlFile)
{
    QFile file(tomlFile);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;
    const QString text = file.readAll();
    bool ret = loadText(text);
    if(ret)
        m_curPathFile = tomlFile;
    return ret;
}

bool QtTomlParser::loadText(const QString &tomlString)
{
    try {
        std::istringstream is(tomlString.toUtf8().data());
        m_rootTable = toml::parse(is);
        m_curPathFile.clear();
        while(m_nodeStack.size() > 1)
            m_nodeStack.pop();
    } catch (...) {
        return false;
    }
    return true;
}

bool QtTomlParser::into(const QString &key)
{
    auto* current = getCurTable();
    auto parts = key.split('.', Qt::SkipEmptyParts);

    for(auto& part : parts) {
        auto node = current->get(part.toUtf8().data());
        if(!node || !node->is_table())
            return false;
        current = node->as_table();
        m_nodeStack.push(current);
    }
    return true;
}

void QtTomlParser::outof()
{
    if(m_nodeStack.size() > 1)
        m_nodeStack.pop();
}

bool QtTomlParser::getBool(const QString &key, bool defaultValue)
{
    toml::node* node = getNode(key);
    if(node && node->is_boolean())
        return node->as_boolean()->value_or(defaultValue);
    return defaultValue;
}

int64_t QtTomlParser::getInt(const QString &key, int64_t defaultValue)
{
    toml::node* node = getNode(key);
    if(node && (node->is_floating_point() || node->is_integer()))
        return node->as_integer()->value_or(defaultValue);
    return defaultValue;
}

double QtTomlParser::getFloat(const QString &key, double defaultValue)
{
    toml::node* node = getNode(key);
    if(node && (node->is_floating_point() || node->is_integer()))
        return node->as_floating_point()->value_or(defaultValue);
    return defaultValue;
}

QString QtTomlParser::getString(const QString &key, QString defaultValue)
{
    toml::node* node = getNode(key);
    if(!node || !node->is_string())
        return defaultValue;
    std::string ret = node->as_string()->
        value_or(defaultValue.toStdString());
    return QString::fromStdString(ret);
}

QDate QtTomlParser::getDate(const QString &key, QDate defaultValue)
{
    toml::node* node = getNode(key);
    if(!node || !node->is_date())
        return defaultValue;
    toml::date& d = node->as_date()->get();
    return QDate(d.year, d.month, d.day);
}

QTime QtTomlParser::getTime(const QString &key, QTime defaultValue)
{
    toml::node* node = getNode(key);
    if(!node || !node->is_time())
        return defaultValue;
    toml::time& t = node->as_time()->get();
    return QTime(t.hour, t.minute, t.second);
}

QDateTime QtTomlParser::getDateTime(const QString &key, QDateTime defaultValue)
{
    toml::node* node = getNode(key);
    if(!node || !node->is_date_time())
        return defaultValue;
    toml::date_time& dt = node->as_date_time()->get();
    return QDateTime(
        QDate(dt.date.year, dt.date.month, dt.date.day),
        QTime(dt.time.hour, dt.time.minute, dt.time.second));
}

toml::table *QtTomlParser::getTable(const QString &key)
{
    toml::node* node = getNode(key);
    if(!node || !node->is_table())
        return nullptr;
    return node->as_table();
}

toml::array *QtTomlParser::getArray(const QString &key)
{
    toml::node* node = getNode(key);
    if(!node || !node->is_array())
        return nullptr;
    return node->as_array();
}

toml::node *QtTomlParser::getNode(const QString &key)
{
    toml::table* curTable = getCurTable();
    if(curTable->contains(key.toUtf8().data())) {
        toml::node* node = curTable->get(key.toUtf8().data());
        return node;
    }
    return nullptr;
}

void QtTomlParser::setBool(const QString &key, const bool &value)
{
    toml::table* curTable = getCurTable();
    curTable->insert_or_assign(key.toStdString(), value);
}

void QtTomlParser::setInt(const QString &key, const int64_t &value)
{
    toml::table* curTable = getCurTable();
    curTable->insert_or_assign(key.toStdString(), value);
}

void QtTomlParser::setFloat(const QString &key, const double &value)
{
    toml::table* curTable = getCurTable();
    curTable->insert_or_assign(key.toStdString(), value);
}

void QtTomlParser::setString(const QString &key, const QString &value)
{
    toml::table* curTable = getCurTable();
    curTable->insert_or_assign(key.toStdString(), value.toUtf8().data());
}

void QtTomlParser::setData(const QString &key, const QDate &value)
{
    toml::table* curTable = getCurTable();
    toml::date d(value.year(), value.month(), value.day());
    curTable->insert_or_assign(key.toStdString(), d);
}

void QtTomlParser::setTime(const QString &key, const QTime &value)
{
    toml::table* curTable = getCurTable();
    toml::time t(value.hour(), value.minute(), value.second());
    curTable->insert_or_assign(key.toStdString(), t);
}

void QtTomlParser::setDateTime(const QString &key, const QDateTime &value)
{
    toml::table* curTable = getCurTable();
    toml::date d(value.date().year(), value.date().month(), value.date().day());
    toml::time t(value.time().hour(), value.time().minute(), value.time().second());
    toml::date_time dt(d, t);
    curTable->insert_or_assign(key.toStdString(), dt);
}

void QtTomlParser::setTable(const QString &key, const toml::table *value)
{
    toml::table* curTable = getCurTable();
    curTable->insert_or_assign(key.toStdString(), *value);
}

void QtTomlParser::setArray(const QString &key, const toml::array *value)
{
    toml::table* curTable = getCurTable();
    curTable->insert_or_assign(key.toStdString(), *value);
}

void QtTomlParser::setNode(const QString &key, const toml::node *value)
{
    toml::table* curTable = getCurTable();
    curTable->insert_or_assign(key.toStdString(), *value);
}

bool QtTomlParser::saveFile(QString tomlFile)
{
    if (tomlFile.isEmpty())
        tomlFile = m_curPathFile;
    if (tomlFile.isEmpty())
        return false;

    QFile file(tomlFile);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;

    const QString text = getTomlString();
    file.write(text.toUtf8());

    return true;
}

QString QtTomlParser::getTomlString()
{
    std::ostringstream oss;
    oss << m_rootTable;
    return QString::fromStdString(oss.str());
}

toml::table *QtTomlParser::getCurTable()
{
    return m_nodeStack.empty() ?
        &m_rootTable : m_nodeStack.top();
}
