#pragma once
#include <QStringList>
#include <QRegularExpression>
#include <QString>
#include <QList>
#include <QSerialPort>
#include <QSerialPortInfo>

class Sondeur_marin
{

public:
	Sondeur_marin();
	~Sondeur_marin();
	QString convert(QString val);
	void analyseTrame(QString ligne);
	QList<QStringList> getTrame();
	QList<QSerialPortInfo> GetPort();

private :
	QStringList GPGLL, SDMTW, SDDPT, SDDBT;
	QSerialPort* port;
	QList<QSerialPortInfo> availablePorts;
};

