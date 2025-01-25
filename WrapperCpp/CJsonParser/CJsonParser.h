/********************************************************
*
* �ļ���: CJsonParser.h
* ��Ȩ:   ChinaEdu Co. Ltd. Copyright 2022 All Rights Reserved.
* ����:   ����jsoncpp��json���ݽ�����
*
* �޸���:     ������
* �޸�����:
* �汾:       1.0
* �޸�ʱ��:   2023-04-13
*
********************************************************/
#ifndef CJSON_PARSER_H
#define CJSON_PARSER_H

#include "jsoncpp/json.h"
#include <list>
class CJsonParser
{
public:
	//�ַ���ת��ΪJson����
	static Json::Value String2Json(const Json::String& jsonString, Json::String* err = nullptr);
	//��Json����ת��Ϊ�ַ���(�����ʽindentedȡֵtrue.����ģʽfalse.����ģʽ)
	static Json::String Json2String(const Json::Value& json, bool indented = true);
	//�����ļ�
	static bool SaveJson(const Json::Value& json, 
		const Json::String& saveFile, bool indented = true);
public:
	//����Json�ļ�
	bool OpenFile(const Json::String& jsonFile);
	//����Json�ַ���
	bool OpenString(const Json::String& jsonString);
	//����ڵ�
	bool Into(const Json::String& key);
	//���ؽڵ�
	void Outof();
	//��õ�ǰ�ڵ�����
	bool GetBool(const Json::String& key, bool defaultValue = false);
	int GetInt(const Json::String& key, int defaultValue = 0);
	double GetDouble(const Json::String& key, double defaultValue = 0.0);
	Json::String GetString(const Json::String& key, Json::String defaultValue = Json::String());
	Json::Value GetArray(const Json::String& key, Json::Value defaultValue = Json::Value());
	Json::Value GetValue(const Json::String& key, Json::Value defaultValue = Json::Value());
	//���õ�ǰ�ڵ����ݣ�����setStringFormatǿ��ת��Ϊ�ַ�����ʽ����json��
	void SetBool(const Json::String& key, const bool& value, bool setStringFormat = false);
	void SetInt(const Json::String& key, const int& value, bool setStringFormat = false);
	void SetDouble(const Json::String& key, const double& value, bool setStringFormat = false);
	void SetString(const Json::String& key, const Json::String& value);
	void SetArray(const Json::String& key, const Json::Value& value);
	void SetValue(const Json::String& key, const Json::Value& value);
	//��������(�ļ���Ϊ�ձ���Ϊ��ǰ���ļ�)
	bool SaveFile(Json::String jsonFile = Json::String(), bool indented = true);
	//��õ�ǰJSON�����ַ���
	Json::String GetJsonString(bool indented = true);
	//��ô�����Ϣ
	Json::String GetErrorInfo();
private:
	//�������нڵ�����
	void UpdateData();
	//���ݽڵ��¼
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