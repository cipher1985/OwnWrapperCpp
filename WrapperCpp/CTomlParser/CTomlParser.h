#ifndef CTOMLPARSER_H
#define CTOMLPARSER_H

#include <stack>
#include "toml.hpp"

//基于toml++实现的Toml文件解析器
class CTomlParser
{
public:
    //将toml字符串转为json字符串
    static std::string tomlToJson(const std::string& tomlString);
    //指定字符分割字符串
    static std::vector<std::string> split(
        const std::string& s, const std::string& delimiter, bool skipEmpty = false);
public:
    //加载toml文件
    bool loadFile(const std::string& tomlFile);
    //加载toml字符串
    bool loadText(const std::string& tomlString);
    //进入节点
    bool into(const std::string& key);
    //返回节点
    void outof();
    //获得当前节点数据
    bool getBool(const std::string& key, bool defaultValue = false);
    int64_t getInt(const std::string& key, int64_t defaultValue = 0);
    double getFloat(const std::string& key, double defaultValue = 0.0);
    std::string getString(const std::string& key, const std::string& defaultValue = std::string());
    toml::date getDate(const std::string& key, const toml::date& defaultValue = toml::date());
    toml::time getTime(const std::string& key, const toml::time& defaultValue = toml::time());
    toml::date_time getDateTime(const std::string& key, const toml::date_time& defaultValue = toml::date_time());
    toml::table* getTable(const std::string& key);
    toml::array* getArray(const std::string& key);
    toml::node* getNode(const std::string& key);
    //设置当前节点数据
    void setBool(const std::string& key, const bool& value);
    void setInt(const std::string& key, const int64_t& value);
    void setFloat(const std::string& key, const double& value);
    void setString(const std::string& key, const std::string& value);
    void setData(const std::string& key, const toml::date& value);
    void setTime(const std::string& key, const toml::time& value);
    void setDateTime(const std::string& key, const toml::date_time& value);
    void setTable(const std::string& key, const toml::table& value);
    void setArray(const std::string& key, const toml::array& value);
    void setNode(const std::string& key, const toml::node& value);
    //保存数据(文件名为空保存为当前打开文件)
    bool saveFile(const std::string& tomlFile = std::string());
    //获得当前Toml数据字符串
    std::string getTomlString();
private:
    template<typename T>
    void setValue(const std::string& key, const T &value);
    toml::table* getCurTable();
    toml::table m_rootTable;
    std::stack<toml::table*> m_nodeStack;
    std::string m_curPathFile;
};

template<typename T>
inline void CTomlParser::setValue(const std::string &key, const T &value)
{
    toml::table* curTable = getCurTable();
    std::vector<std::string> parts = split(key, ".", true);
    int count = (int)parts.size();
    if(count == 0)
        return;
    //遍历键深入表
    for (int i = 0; i < count - 1; ++i)
    {
        std::string part = parts[i];
        if (!curTable->contains(part) ||
            !curTable->at(part).is_table())
            curTable->insert_or_assign(part, toml::table());
        curTable = curTable->at(part).as_table();
    }
    //在最下层表中插入或更新值
    curTable->insert_or_assign(parts[count - 1], value);
}

#endif // CTOMLPARSER_H
