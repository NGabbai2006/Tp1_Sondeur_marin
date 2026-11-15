#pragma once
#pragma warning (disable : 4996)
#include <QtWidgets/QMainWindow>
#include "ui_InterfaceQt.h"
#include <QSerialPort>
#include <QSerialPortInfo>


class InterfaceQt : public QMainWindow
{
    Q_OBJECT

public:
    InterfaceQt(QWidget* parent = nullptr);
    ~InterfaceQt();
    void afficheTrame(QStringList Trame);

public slots:
    //arduino
    void onSerialPortReadyRead();
    void onSendMessageButtonClicked();
    void onOpenPortButtonClicked();


private:
    Ui::InterfaceQtClass ui;
    QSerialPort* port;
    QTimer* timer;

};



