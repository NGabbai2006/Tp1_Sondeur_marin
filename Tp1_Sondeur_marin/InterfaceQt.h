#pragma once
#pragma warning (disable : 4996)
#include <QtWidgets/QMainWindow>
#include "ui_InterfaceQt.h"
#include "Sondeur_marin.h"

class InterfaceQt : public QMainWindow
{
    Q_OBJECT

public:
    InterfaceQt(QWidget* parent = nullptr);
    ~InterfaceQt();
    void afficheTrame(QList<QStringList> trames);
    void OnAquisition();
	void OnAquisitionFini();

public slots:
    void onOpenPortButtonClicked();
	void onButtonAquisitionClicked();

private:
    Ui::InterfaceQtClass ui;
    Sondeur_marin sondeur;
};



