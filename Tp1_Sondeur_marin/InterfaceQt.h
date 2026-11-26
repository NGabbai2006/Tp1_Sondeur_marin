//*********************************************************************************************
//* Programme : InterfaceQt.h                                                 Date : 20/09/2026
//*--------------------------------------------------------------------------------------------
//* Dernière mise à jour : 26/11/2026
//*
//* Programmeurs : Noah Gabbai                                            Classe : CIEL2
//*--------------------------------------------------------------------------------------------
//* But : Afficher les trames recu du sondeur marin et les analyser pour afficher les informations interressante
//* Programmes associés : sondeur_marin.h,InterfaceQt.h
//*********************************************************************************************
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



