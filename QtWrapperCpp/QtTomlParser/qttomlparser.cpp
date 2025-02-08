#include "qttomlparser.h"

#include <QFile>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>

#include <fstream>
#include <sstream>

QString QtTomlParser::tomlToJson(const QString &tomlString)
{
    try {
        auto tomlData = toml::parse(tomlString.toStdString());
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
        std::istringstream is(tomlString.toStdString().data());
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
        auto node = current->get(part.toStdString());
        if(!node || !node->is_table())
            return false;
        current = node->as_table();
        m_nodeStack.push(current);
    }
    return true;
}

void QtTomlParser::outof()
{
    if(!m_nodeStack.empty())
        m_nodeStack.pop();
}

bool QtTomlParser::getBool(const QString &key, bool defaultValue)
{
    toml::node* node = getNode(key);
    if(!node || !node->is_boolean())
        return defaultValue;
    return node->as_boolean()->value_or(defaultValue);
}

int64_t QtTomlParser::getInt(const QString &key, int64_t defaultValue)
{
    toml::node* node = getNode(key);
    if(!node)
        return defaultValue;

    if(node->is_integer()) {
        return node->as_integer()->value_or(defaultValue);
    } else if(node->is_floating_point()) {
        return (int64_t)node->as_floating_point()->value_or((double)defaultValue);
    } else if(node->is_string()) {
        std::string ret = node->as_string()->value_or(std::to_string(defaultValue));
        return (int64_t)std::stoll(ret);
    }
    return defaultValue;
}

double QtTomlParser::getFloat(const QString &key, double defaultValue)
{
    toml::node* node = getNode(key);
    if(!node)
        return defaultValue;

    if(node->is_integer()) {
        return (double)node->as_integer()->value_or(defaultValue);
    } else if(node->is_floating_point()) {
        return node->as_floating_point()->value_or(defaultValue);
    } else if(node->is_string()) {
        std::string ret = node->as_string()->value_or(std::to_string(defaultValue));
        return std::stod(ret);
    }
    return defaultValue;
}

QString QtTomlParser::getString(const QString &key, const QString& defaultValue)
{
    toml::node* node = getNode(key);
    if(!node || !node->is_string())
        return defaultValue;
    std::string ret = node->as_string()->value_or(defaultValue.toStdString());
    return QString::fromStdString(ret);
}

QDate QtTomlParser::getDate(const QString &key, const QDate& defaultValue)
{
    toml::date d(defaultValue.year(), defaultValue.month(), defaultValue.day());
    toml::node* node = getNode(key);
    if(!node || !node->is_date())
        return defaultValue;
    toml::date ret = node->as_date()->value_or(d);
    return QDate(ret.year, ret.month, ret.day);
}

QTime QtTomlParser::getTime(const QString &key, const QTime& defaultValue)
{
    toml::time t(defaultValue.hour(), defaultValue.minute(), defaultValue.second());
    toml::node* node = getNode(key);
    if(!node || !node->is_time())
        return defaultValue;
    toml::time ret = node->as_time()->value_or(t);
    return QTime(ret.hour, ret.minute, ret.second);
}

QDateTime QtTomlParser::getDateTime(const QString &key, const QDateTime& defaultValue)
{
    toml::date d(defaultValue.date().year(), defaultValue.date().month(), defaultValue.date().day());
    toml::time t(defaultValue.time().hour(), defaultValue.time().minute(), defaultValue.time().second());
    toml::date_time dt(d, t);
    toml::node* node = getNode(key);
    if(!node || !node->is_date_time())
        return defaultValue;
    toml::date_time ret = node->as_date_time()->value_or(dt);
    return QDateTime(
        QDate(ret.date.year, ret.date.month, ret.date.day),
        QTime(ret.time.hour, ret.time.minute, ret.time.second));
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
    QList<QString> parts = key.split('.', Qt::SkipEmptyParts);
    int count = parts.count();
    if(count == 0)
        return nullptr;
    QString curKey;
    for(int i = 0; i < count - 1; ++i) {
        curKey = parts.at(i);
        toml::node* node = curTable->get(curKey.toStdString());
        if(!node || !node->is_table())
            return nullptr;
        curTable = node->as_table();
    }
    curKey = parts.at(count - 1);
    toml::node* node = curTable->get(curKey.toStdString());
    return node;
}

void QtTomlParser::setBool(const QString &key, const bool &value)
{
    setValue<bool>(key, value);
}

void QtTomlParser::setInt(const QString &key, const int64_t &value)
{
    setValue<int64_t>(key, value);
}

void QtTomlParser::setFloat(const QString &key, const double &value)
{
    setValue<double>(key, value);
}

void QtTomlParser::setString(const QString &key, const QString &value)
{
    std::string data = value.toStdString();
    setValue<std::string>(key, data);
}

void QtTomlParser::setData(const QString &key, const QDate &value)
{
    toml::date data(value.year(), value.month(), value.day());
    setValue<toml::date>(key, data);
}

void QtTomlParser::setTime(const QString &key, const QTime &value)
{
    toml::time data(value.hour(), value.minute(), value.second());
    setValue<toml::time>(key, data);
}

void QtTomlParser::setDateTime(const QString &key, const QDateTime &value)
{
    toml::date d(value.date().year(), value.date().month(), value.date().day());
    toml::time t(value.time().hour(), value.time().minute(), value.time().second());
    toml::date_time dt(d, t);
    setValue<toml::date_time>(key, dt);
}

void QtTomlParser::setTable(const QString &key, const toml::table& value)
{
    setValue<toml::table>(key, value);
}

void QtTomlParser::setArray(const QString &key, const toml::array& value)
{
    setValue<toml::array>(key, value);
}

void QtTomlParser::setNode(const QString &key, const toml::node& value)
{
    setValue<toml::node>(key, value);
}

bool QtTomlParser::saveFile(const QString& tomlFile)
{
    try{
        if(getCurTable()->empty())
            return false;
        QString fileName = tomlFile;
        if (fileName.isEmpty())
            fileName = m_curPathFile;
        if (fileName.isEmpty())
            return false;
        std::ofstream ofs(fileName.toStdString());
        ofs << m_rootTable;
        return true;
    } catch(...) {
        return false;
    }
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
