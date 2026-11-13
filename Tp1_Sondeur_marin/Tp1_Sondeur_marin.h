#pragma once
#pragma warning (disable : 4996)
#include <QtWidgets/QMainWindow>
#include "ui_Tp1_Sondeur_marin.h"
#include <QSerialPort>
#include <QSerialPortInfo>


class Tp1_Sondeur_marin : public QMainWindow
{
    Q_OBJECT

public:
    Tp1_Sondeur_marin(QWidget* parent = nullptr);
    ~Tp1_Sondeur_marin();

public slots:
    //arduino
    void onSerialPortReadyRead();
    void onSendMessageButtonClicked();
    void onOpenPortButtonClicked();


private:
    Ui::Tp1_Sondeur_marinClass ui;
    QSerialPort* port;
    QTimer* timer;

};



