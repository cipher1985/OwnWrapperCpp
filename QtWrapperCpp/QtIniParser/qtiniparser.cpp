#include "QtIniParser.h"

QtIniParser::QtIniParser(QObject* parent)
	: QObject(parent) {}

QtIniParser::~QtIniParser()
{
	close();
}

bool QtIniParser::open(const QString& iniFile)
{
	close();
	m_pConfigIni = new QSettings(iniFile, QSettings::IniFormat);
	return m_pConfigIni;
}

void QtIniParser::close()
{
	if (m_pConfigIni) {
		setSection();
		m_pConfigIni->deleteLater();
		m_pConfigIni = nullptr;
	}
}

void QtIniParser::setSection(const QString& section)
{
	if (m_pConfigIni) {
		if (!m_curSection.isEmpty()) {
			m_pConfigIni->endGroup();
			m_curSection.clear();
		}
		if (!section.isEmpty()) {
			m_pConfigIni->beginGroup(section);
			m_curSection = section;
		}
	}
}

bool QtIniParser::getBool(const QString& key, bool defaultValue)
{
	if (m_pConfigIni)
		return m_pConfigIni->value(key, defaultValue).toBool();
	return defaultValue;
}

int QtIniParser::getInt(const QString& key, int defaultValue)
{
	if (m_pConfigIni)
		return m_pConfigIni->value(key, defaultValue).toInt();
	return defaultValue;
}

double QtIniParser::getDouble(const QString& key, double defaultValue)
{
	if (m_pConfigIni)
		return m_pConfigIni->value(key, defaultValue).toDouble();
	return defaultValue;
}

QString QtIniParser::getString(const QString& key, QString defaultValue)
{
	if (m_pConfigIni)
		return m_pConfigIni->value(key, defaultValue).toString();
	return defaultValue;
}

void QtIniParser::setBool(const QString& key, const bool& value)
{
	if (m_pConfigIni)
		m_pConfigIni->setValue(key, value);
}

void QtIniParser::setInt(const QString& key, const int& value)
{
	if (m_pConfigIni)
		m_pConfigIni->setValue(key, value);
}

void QtIniParser::setDouble(const QString& key, const double& value)
{
	if (m_pConfigIni)
		m_pConfigIni->setValue(key, value);
}

void QtIniParser::setString(const QString& key, const QString& value)
{
	if (m_pConfigIni)
		m_pConfigIni->setValue(key, value);
}
