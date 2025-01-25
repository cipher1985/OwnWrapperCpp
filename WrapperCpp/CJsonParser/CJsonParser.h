/********************************************************
*
* 文件名: CJsonParser.h
* 版权:   ChinaEdu Co. Ltd. Copyright 2022 All Rights Reserved.
* 描述:   基于jsoncpp的json数据解析器
*
* 修改人:     傅祯勇
* 修改内容:
* 版本:       1.0
* 修改时间:   2023-04-13
*
********************************************************/
#ifndef CJSON_PARSER_H
#define CJSON_PARSER_H

#include "jsoncpp/json.h"
#include <list>
class CJsonParser
{
public:
	//字符串转换为Json对象
	static Json::Value String2Json(const Json::String& jsonString, Json::String* err = nullptr);
	//将Json对象转换为字符串(输出格式indented取值true.缩进模式false.紧凑模式)
	static Json::String Json2String(const Json::Value& json, bool indented = true);
	//保存文件
	static bool SaveJson(const Json::Value& json, 
		const Json::String& saveFile, bool indented = true);
public:
	//加载Json文件
	bool OpenFile(const Json::String& jsonFile);
	//加载Json字符串
	bool OpenString(const Json::String& jsonString);
	//进入节点
	bool Into(const Json::String& key);
	//返回节点
	void Outof();
	//获得当前节点数据
	bool GetBool(const Json::String& key, bool defaultValue = false);
	int GetInt(const Json::String& key, int defaultValue = 0);
	double GetDouble(const Json::String& key, double defaultValue = 0.0);
	Json::String GetString(const Json::String& key, Json::String defaultValue = Json::String());
	Json::Value GetArray(const Json::String& key, Json::Value defaultValue = Json::Value());
	Json::Value GetValue(const Json::String& key, Json::Value defaultValue = Json::Value());
	//设置当前节点数据（参数setStringFormat强制转换为字符串格式生成json）
	void SetBool(const Json::String& key, const bool& value, bool setStringFormat = false);
	void SetInt(const Json::String& key, const int& value, bool setStringFormat = false);
	void SetDouble(const Json::String& key, const double& value, bool setStringFormat = false);
	void SetString(const Json::String& key, const Json::String& value);
	void SetArray(const Json::String& key, const Json::Value& value);
	void SetValue(const Json::String& key, const Json::Value& value);
	//保存数据(文件名为空保存为当前打开文件)
	bool SaveFile(Json::String jsonFile = Json::String(), bool indented = true);
	//获得当前JSON数据字符串
	Json::String GetJsonString(bool indented = true);
	//获得错误信息
	Json::String GetErrorInfo();
private:
	//更新所有节点数据
	void UpdateData();
	//数据节点记录
	struct node
	{
		Json::String key;
		Json::Value obj;
	};
	std::list<node>  m_nodes;
	//Json::Value m_root;
	Json::String m_errInfo;
};

#endif	//CJSON_PARSER_H