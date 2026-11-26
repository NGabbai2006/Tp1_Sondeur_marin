//*********************************************************************************************
//* Programme : InterfaceQt.cpp                                                 Date : 20/09/2026
//*--------------------------------------------------------------------------------------------
//* Dernière mise à jour : 26/11/2026
//*
//* Programmeurs : Noah Gabbai                                            Classe : CIEL2
//*--------------------------------------------------------------------------------------------
//* But : Tout les programmes sont executer a partir de ce fichier
//* Programmes associés : sondeur_marin.h,InterfaceQt.h
//*********************************************************************************************
#include "InterfaceQt.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    InterfaceQt window;
    window.show();
    return app.exec();
}
