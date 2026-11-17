#pragma once
#pragma warning (disable : 4996)
#include <QtWidgets/QMainWindow>
#include "ui_InterfaceQt.h"
#include <QTimer>
#include "Sondeur_marin.h"

class InterfaceQt : public QMainWindow
{
    Q_OBJECT

public:
    InterfaceQt(QWidget* parent = nullptr);
    ~InterfaceQt();
    void afficheTrame(QList<QStringList> trames);
	void TrameAnalysePause();

public slots:
    //arduino
    void onSerialPortReadyRead();
    void onOpenPortButtonClicked();

private:
    Ui::InterfaceQtClass ui;
    Sondeur_marin sondeur;
	QTimer*timer;
	QString ligne;
    bool firstTrame = true;
};



