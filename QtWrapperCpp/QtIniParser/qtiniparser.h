#ifndef QT_INIPARSER_H
#define QT_INIPARSER_H

#include <QObject>
#include <QSettings>
//配置文件ini解析器
class QtIniParser : public QObject
{
	Q_OBJECT
public:
    QtIniParser(QObject* parent = nullptr);
    ~QtIniParser();
	//打开
	bool open(const QString& iniFile);
	//关闭
	void close();
	//设置当前节
	void setSection(const QString& section = QString());
	//获得当前节点数据
	bool getBool(const QString& key, bool defaultValue = false);
	int getInt(const QString& key, int defaultValue = 0);
	double getDouble(const QString& key, double defaultValue = 0.0);
	QString getString(const QString& key, QString defaultValue = QString());
	//设置当前节点数据
	void setBool(const QString& key, const bool& value);
	void setInt(const QString& key, const int& value);
	void setDouble(const QString& key, const double& value);
	void setString(const QString& key, const QString& value);
private:
	QSettings* m_pConfigIni{};
	QString m_curSection;
};
#endif //QT_INIPARSER_H
