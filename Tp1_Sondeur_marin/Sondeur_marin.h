#pragma once
#include <QStringList>
#include <QRegularExpression>
#include <QString>
#include <QList>
class Sondeur_marin
{

public:
	Sondeur_marin();
	~Sondeur_marin();
	QString convert(QString val);
	void analyseTrame(QString ligne);
	QList<QStringList> getTrame();

private :
	QStringList GPGLL, SDMTW, SDDPT, SDDBT;
};

