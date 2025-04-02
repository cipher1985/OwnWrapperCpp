#include "CJsonParser.h"

#include <iostream>
#include <fstream>
Json::Value CJsonParser::String2Json(const Json::String& jsonString, Json::String* err)
{
	//创建工厂对象
	Json::CharReaderBuilder ReaderBuilder;
	//设置utf8支持
	ReaderBuilder["emitUTF8"] = true;
	//创建json读取器对象
	std::unique_ptr<Json::CharReader> charread(ReaderBuilder.newCharReader());
	//返回json对象
	Json::Value root;
	//生成json对象
	Json::String strerr;
	bool isok = charread->parse(jsonString.c_str(),
		jsonString.c_str() + jsonString.size(), &root, &strerr);
	//获得错误信息
	if (!isok || strerr.size() != 0) 
	{
		if (err)
			*err = strerr;
	}
	return root;
}
Json::String CJsonParser::Json2String(
	const Json::Value& json, bool indented)
{
	//创建Json写入器对象
	Json::StreamWriterBuilder writebuild;
	//设置utf8支持
	writebuild["emitUTF8"] = true;
	//输出格式
	if(indented)
		writebuild.settings_["indentation"] = "";
	//把json对象转变为字符串
	Json::String document = Json::writeString(writebuild, json);
	return document;
}
bool CJsonParser::SaveJson(const Json::Value& json,
	const Json::String& saveFile, bool indented)
{
	//把json对象转变为字符串
	Json::String document = Json2String(json, indented);
	//保存数据
	std::ofstream ofile(saveFile);
	if (!ofile.is_open())
		return false;
	ofile << document;
	ofile.close();
	return true;
}
bool CJsonParser::OpenFile(const Json::String& jsonFile)
{
	//打开文件
	std::ifstream ifile;
	ifile.open(jsonFile);
	//创建json读取器对象
	Json::CharReaderBuilder ReaderBuilder;
	//设置utf8支持
	ReaderBuilder["emitUTF8"] = true;
	//把文件转变为json对象
	Json::Value root;
	bool ok = Json::parseFromStream(ReaderBuilder,
		ifile, &root, &m_errInfo);
	if (!ok) 
		return false;
	m_nodes.push_back(node{ jsonFile, root });
	return true;
}

bool CJsonParser::OpenString(const Json::String& jsonString)
{
	Json::Value root = String2Json(jsonString, &m_errInfo);
	if (root.isNull() || m_errInfo.size() != 0)
		return false;
	m_nodes.push_back(node{ "", root });
	return true;
}

bool CJsonParser::Into(const Json::String& key)
{
	if (m_nodes.size() == 0)
		return false;
	Json::Value& obj = m_nodes.rbegin()->obj;
	if (obj.isNull() || obj.isMember(key))
		return false;
	Json::Value v = obj[key];
	if (!v.isObject())
		return false;
	m_nodes.push_back(node{ key, v });
	return true;
}

void CJsonParser::Outof()
{
	int count = m_nodes.size();
	if (count <= 1)
		return;
	node back = *m_nodes.rbegin();
	m_nodes.pop_back();
	m_nodes.rbegin()->obj[back.key] = back.obj;
}

Json::String CJsonParser::GetErrorInfo()
{
	return m_errInfo;
}
void CJsonParser::UpdateData()
{
	for (auto itor = m_nodes.rbegin(); itor != m_nodes.rend(); ++itor)
	{
		auto subItor1 = itor;
		--subItor1;
		auto subItor2 = subItor1;
		--subItor2;
		node& newData = *subItor1;
		subItor2->obj[newData.key] = newData.obj;
	}
}
//////////////////////////////////////////////////////////////////////////
Json::Value CJsonParser::GetValue(const Json::String& key, Json::Value defaultValue)
{
	if (m_nodes.size() == 0)
		return defaultValue;
	Json::Value obj = m_nodes.rbegin()->obj;
	if (obj.isNull() || !obj.isMember(key))
		return defaultValue;
	return obj[key];
}

bool CJsonParser::GetBool(const Json::String& key, bool defaultValue)
{
	Json::Value v = GetValue(key);
	if (v.isNull())
		return defaultValue;
	return v.asBool();
}

int CJsonParser::GetInt(const Json::String& key, int defaultValue)
{
	Json::Value v = GetValue(key);
	if (v.isNull())
		return defaultValue;
	return v.asInt();
}

double CJsonParser::GetDouble(const Json::String& key, double defaultValue)
{
	Json::Value v = GetValue(key);
	if (v.isNull())
		return defaultValue;
	return v.asDouble();
}

Json::String CJsonParser::GetString(const Json::String& key, Json::String defaultValue)
{
	Json::Value v = GetValue(key);
	if (v.isNull())
		return defaultValue;
	return v.asCString();
}
Json::Value CJsonParser::GetArray(const Json::String& key, Json::Value defaultValue)
{
	Json::Value v = GetValue(key);
	if (v.isNull() || !v.isArray())
		return defaultValue;
	return v;
}
//////////////////////////////////////////////////////////////////////////
void CJsonParser::SetValue(const Json::String& key, const Json::Value& value)
{
	if (value.isNull())
		return;
	if (m_nodes.size() == 0)
	{
		Json::Value obj;
		obj[key] = value;
		m_nodes.push_back(node{ "", obj });
		return;
	}
	Json::Value& obj = m_nodes.rbegin()->obj;
	obj[key] = value;
}
void CJsonParser::SetArray(const Json::String& key, const Json::Value& value)
{
	if (value.isNull() || !value.isArray())
		return;
	SetValue(key, value);
}
void CJsonParser::SetBool(const Json::String& key, const bool& value, bool setStringFormat)
{
	if (setStringFormat)
		SetValue(key, value ? "true" : "false");
	else
		SetValue(key, value);
}
void CJsonParser::SetInt(const Json::String& key, const int& value, bool setStringFormat)
{
	if (setStringFormat)
		SetValue(key, std::to_string(value));
	else
		SetValue(key, value);
}
void CJsonParser::SetDouble(const Json::String& key, const double& value, bool setStringFormat)
{
	if (setStringFormat)
		SetValue(key, std::to_string(value));
	else
		SetValue(key, value);
}
void CJsonParser::SetString(const Json::String& key, const Json::String& value)
{
	SetValue(key, value);
}


bool CJsonParser::SaveFile(Json::String jsonFile, bool indented)
{
	UpdateData();
	if (m_nodes.size() <= 0)
		return false;
	node& content = *m_nodes.begin();
	if (jsonFile.empty())
		jsonFile = content.key;
	if (jsonFile.empty() || content.obj.isNull())
		return false;
	return SaveJson(content.obj, jsonFile, indented);
}
Json::String CJsonParser::GetJsonString(bool indented)
{
	UpdateData();
	Json::String ret;
	if (m_nodes.size() <= 0)
		return ret;
	node& content = *m_nodes.begin();
	if (content.obj.isNull())
		return ret;
	ret = Json2String(content.obj, indented);
	return ret;
}