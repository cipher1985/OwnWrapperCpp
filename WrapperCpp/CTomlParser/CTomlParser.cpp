#include "ctomlparser.h"

#include <iostream>
#include <sstream>

std::string CTomlParser::tomlToJson(const std::string &tomlString)
{
    try {
        auto tomlData = toml::parse(tomlString);
        std::ostringstream oss;
        oss << toml::json_formatter(tomlData);
        return oss.str();
    } catch(...){
        return std::string();
    }
}

bool CTomlParser::loadFile(const std::string &tomlFile)
{
    try {
        m_rootTable = toml::parse_file(tomlFile);
        m_curPathFile = tomlFile;
        while(m_nodeStack.size() > 1)
            m_nodeStack.pop();
    } catch (...) {
        return false;
    }
    return true;
}

bool CTomlParser::loadText(const std::string &tomlString)
{
    try {
        m_rootTable = toml::parse(tomlString);
        m_curPathFile.clear();
        while(m_nodeStack.size() > 1)
            m_nodeStack.pop();
    } catch (...) {
        return false;
    }
    return true;
}

bool CTomlParser::into(const std::string &key)
{
    toml::table* curTable = getCurTable();
    if (!curTable || !curTable->contains(key))
        return false;
    auto node = curTable->get(key);
    if(!node || !node->is_table())
        return false;
    curTable = node->as_table();
    m_nodeStack.push(curTable);
    return true;
}

void CTomlParser::outof()
{
    if(!m_nodeStack.empty())
        m_nodeStack.pop();
}

bool CTomlParser::getBool(const std::string &key, bool defaultValue)
{
    try {
        toml::node* node = getNode(key);
        return node->as_boolean()->value_or(defaultValue);
    } catch(...) {
        return defaultValue;
    }
}

int64_t CTomlParser::getInt(const std::string &key, int64_t defaultValue)
{
    try {
        toml::node* node = getNode(key);
        return node->as_boolean()->value_or(defaultValue);
    } catch(...) {
        return defaultValue;
    }
}

double CTomlParser::getFloat(const std::string &key, double defaultValue)
{
    try {
        toml::node* node = getNode(key);
        return node->as_floating_point()->value_or(defaultValue);
    } catch(...) {
        return defaultValue;
    }
}

std::string CTomlParser::getString(const std::string &key,
    const std::string& defaultValue)
{
    try {
        toml::node* node = getNode(key);
        return node->as_string()->value_or(defaultValue);
    } catch(...) {
        return defaultValue;
    }
}

toml::date CTomlParser::getDate(const std::string &key,
    const toml::date& defaultValue)
{
    try {
        toml::node* node = getNode(key);
        return node->as_date()->value_or(defaultValue);
    } catch(...) {
        return defaultValue;
    }
}

toml::time CTomlParser::getTime(const std::string &key,
    const toml::time& defaultValue)
{
    try {
        toml::node* node = getNode(key);
        return node->as_time()->value_or(defaultValue);
    } catch(...) {
        return defaultValue;
    }
}

toml::date_time CTomlParser::getDateTime(const std::string &key,
    const toml::date_time& defaultValue)
{
    try {
        toml::node* node = getNode(key);
        return node->as_date_time()->value_or(defaultValue);
    } catch(...) {
        return defaultValue;
    }
}

toml::table *CTomlParser::getTable(const std::string &key)
{
    toml::node* node = getNode(key);
    if(!node || !node->is_table())
        return nullptr;
    return node->as_table();
}

toml::array *CTomlParser::getArray(const std::string &key)
{
    toml::node* node = getNode(key);
    if(!node || !node->is_array())
        return nullptr;
    return node->as_array();
}

toml::node *CTomlParser::getNode(const std::string &key)
{
    toml::table* curTable = getCurTable();
    if(curTable->contains(key)) {
        toml::node* node = curTable->get(key);
        return node;
    }
    return nullptr;
}

void CTomlParser::setBool(const std::string &key, const bool &value)
{
    setValue<bool>(key, value);
}

void CTomlParser::setInt(const std::string &key, const int64_t &value)
{
    setValue<int64_t>(key, value);
}

void CTomlParser::setFloat(const std::string &key, const double &value)
{
    setValue<double>(key, value);
}

void CTomlParser::setString(const std::string &key, const std::string &value)
{
    setValue<std::string>(key, value);
}

void CTomlParser::setData(const std::string &key, const toml::date &value)
{
    setValue<toml::date>(key, value);
}

void CTomlParser::setTime(const std::string &key, const toml::time &value)
{
    setValue<toml::time>(key, value);
}

void CTomlParser::setDateTime(const std::string &key, const toml::date_time &value)
{
    setValue<toml::date_time>(key, value);
}

void CTomlParser::setTable(const std::string &key, const toml::table& value)
{
    setValue<toml::table>(key, value);
}

void CTomlParser::setArray(const std::string &key, const toml::array& value)
{
    setValue<toml::array>(key, value);
}

void CTomlParser::setNode(const std::string &key, const toml::node& value)
{
    setValue<toml::node>(key, value);
}

bool CTomlParser::saveFile(const std::string& tomlFile)
{
    try{
        if(getCurTable()->empty())
            return false;
        std::string fileName = tomlFile;
        if (fileName.empty())
            fileName = m_curPathFile;
        if (fileName.empty())
            return false;
        std::ofstream ofs(fileName);
        ofs << m_rootTable;
        return true;
    } catch(...) {
        return false;
    }
}

std::string CTomlParser::getTomlString()
{
    std::ostringstream oss;
    oss << m_rootTable;
    return oss.str();
}

toml::table *CTomlParser::getCurTable()
{
    return m_nodeStack.empty() ?
        &m_rootTable : m_nodeStack.top();
}


