#pragma once
#include <QRegularExpression>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QTimer>
#include <QObject>
#include <QDebug>

class Sondeur_marin : public QObject
{ 
	Q_OBJECT
public:
	Sondeur_marin(QObject* parent = nullptr);
	~Sondeur_marin();
	// recup et ouvre les ports
	QList<QSerialPortInfo> GetPort();
	bool LancerPort(QString portName);
	void onSerialPortReadyRead();
	// aquisition 
	void lancerAcquisition();
	void StopAcquisition();
	// analyse la trame
	void analyseTrame();
	QString convert(QString val);
	// recup pour l'affichage
	QList<QStringList> getTrame();
	QString getTrameBrut();
	// allege le code 
	void supprime();
	

private :
	QList<QStringList> GPGLL, SDMTW, SDDPT, SDDBT; // stock les trames qu'on affiche
	QSerialPort* port; // creer un port 
	QList<QSerialPortInfo> availablePorts; // stock les ports disponible
	QByteArray newTrame; // stock la trame recu du port
	QString ligne; // stock la trame a analyser
	QTimer* timer; // timer pour l'acquisition
	QString tramebrut; // stock la trame brute recu du port pour l'afficher

signals:
	void readyRead();
	void aquisitionFini();
	
};

