//*********************************************************************************************
//* Programme : InterfaceQt.cpp                                                 Date : 20/09/2026
//*--------------------------------------------------------------------------------------------
//* Dernière mise à jour : 26/11/2026
//*
//* Programmeurs : Noah Gabbai                                            Classe : CIEL2
//*--------------------------------------------------------------------------------------------
//* But : Afficher les trames recu du sondeur marin et les analyser pour afficher les informations interressante
//* Programmes associés : sondeur_marin.h,InterfaceQt.h
//*********************************************************************************************
#include "InterfaceQt.h"

//---------------------------------------------------------------------------------------------
//* Constructeur : InterfaceQt, permet d'initialiser les variables et de connecter les signaux et slots
//* Paramètres :
//*  - QWidget* parent : pointeur vers le widget parent
//*
//* Valeur de retour : aucune
//---------------------------------------------------------------------------------------------
InterfaceQt::InterfaceQt(QWidget* parent)
    : QMainWindow(parent),sondeur()
{
    ui.setupUi(this);
    QList<QSerialPortInfo> info = sondeur.GetPort();
    for (int i = 0; i < info.size();i++) { // Permet d'ajouter les ports dans l'affiche ui
        ui.comboBox_port->addItem(info[i].portName(), QVariant(info[i].portName()));
    }    
    QObject::connect(&sondeur, &Sondeur_marin::readyRead, this, &InterfaceQt::OnAquisition);
    QObject::connect(&sondeur, &Sondeur_marin::aquisitionFini, this, &InterfaceQt::OnAquisitionFini);
}

//---------------------------------------------------------------------------------------------
//* Destructeur : InterfaceQt, permet de liberer la memoire et de deconnecter les signaux et slots
//* Paramètres :
//*  - aucun
//*
//* Valeur de retour : aucune
//---------------------------------------------------------------------------------------------
InterfaceQt::~InterfaceQt()
{ 
	QObject::disconnect(&sondeur, &Sondeur_marin::readyRead, this, &InterfaceQt::OnAquisition);
	QObject::disconnect(&sondeur, &Sondeur_marin::aquisitionFini, this, &InterfaceQt::OnAquisitionFini);
}

//---------------------------------------------------------------------------------------------
//* Fonction afficheTrame, permet d'afficher les trames recu du sondeur marin et les analyser pour afficher les informations interressante
//* Paramètres :
//*  - QList<QStringList> trames : liste des trames recu du sondeur marin
//*
//* Valeur de retour : aucune
//---------------------------------------------------------------------------------------------
void InterfaceQt::afficheTrame(QList<QStringList> trames) { // On s'assure que la trame contienne des infos
    if (trames.isEmpty()) {
        return;
    }
    for (int i = 0; i < trames.size(); i++) { // Affiche la trame traitée
        const QStringList trame = trames[i];
        ui.textEdit_donnesanalyse->insertPlainText("=========nom d'indentification =========\n");
        for (int j = 0; j < trame.size(); j++) {
            ui.textEdit_donnesanalyse->insertPlainText("Champ " + QString::number(j + 1) + " :  " + trame[j].trimmed() + "\n");
        }
    }
    QStringList GPGLL, SDMTW, SDDPT, SDDBT;
    for (int i = 0; i < trames.size(); i++) { // Recupère les trames dans variables qui correspondent
        if (trames[i].isEmpty()){ 
            continue; 
        }
        if (trames[i][0] == "GPGLL") {
            GPGLL = trames[i];
        }
        else if (trames[i][0] == "SDMTW") {
            SDMTW = trames[i];
        }
        else if (trames[i][0] == "SDDPT") {
            SDDPT = trames[i];
        }
        else if (trames[i][0] == "SDDBT") {
            SDDBT = trames[i];
        }
    }
	if (!GPGLL.isEmpty() && !SDMTW.isEmpty() && !SDDPT.isEmpty() && !SDDBT.isEmpty()) { // Si il manque une partie de la tramme je n'affiche pas les informations interressante / j'attends
        qDebug() << GPGLL;
        ui.textEdit_info->insertPlainText("=======Info Trame======\n");
        if (SDDPT[1].isEmpty() || SDDPT[1] == " ") {
            SDDPT[1] = "N/A";
        }
        if (SDDBT[3].isEmpty() || SDDBT[3] == " ") {
            SDDBT[3] = "N/A";
        }
       ui.textEdit_info->insertPlainText("temparture : " + SDMTW[1] + "°C " + GPGLL[2] + sondeur.convert(GPGLL[1]) + "  " + GPGLL[4] + sondeur.convert(GPGLL[3]) + " Profondeur Sous sonde : " + SDDBT[3] + " m" + " Profondeur totale : " + (SDDPT[1]+ SDDPT[2]) + " m\n");
	}
}

//---------------------------------------------------------------------------------------------
//* Fonction qui permet d'appeler la methode pour ouvrir le port en renvoyant le nom du port selectionner par l'utilisateur et d'afficher si le port est ouvert ou non
//* Paramètres :
//*  - aucun
//*
//* Valeur de retour : aucune
//---------------------------------------------------------------------------------------------
void InterfaceQt::onOpenPortButtonClicked() { // Permet d'appeler la methode pour ouvrir le port
		QString portName = ui.comboBox_port->currentText();
		if (portName.isEmpty()) {
			ui.label_status->setText("Status port : Aucun port sélectionné");
			return;
		}
		if (sondeur.LancerPort(portName)) {
			ui.label_status->setText("Status port : Ouvert");
		} else {
			ui.label_status->setText("Status port : Erreur d'ouverture");
		}
}

//---------------------------------------------------------------------------------------------
//* Fonction qui permet d'afficher les trames recu du sondeur marin et les analyser pour afficher les informations interressante lorsqu'il recoit le signal analyseTrame()
//* Paramètres :
//*  - aucun
//*
//* Valeur de retour : aucune
//---------------------------------------------------------------------------------------------
void InterfaceQt::OnAquisition() {
    QList<QStringList> trames = sondeur.getTrame(); // Recupère la trame
    ui.textEdit_donnees->insertPlainText(sondeur.getTrameBrut() + "\n"); // Affiche la trame recue avant traitement
    afficheTrame(trames); 
}

//---------------------------------------------------------------------------------------------
//* Fonction qui stoppe l'acquisition lorsqu'il recoit le signal aquisitionFini()
//* Paramètres :
//*  - 
//*
//* Valeur de retour : 
//---------------------------------------------------------------------------------------------
void InterfaceQt::OnAquisitionFini() {
	ui.textEdit_donnees->insertPlainText("=================Acquisition terminée.=================\n");
}

//---------------------------------------------------------------------------------------------
//* Fonction qui lance l'acquisition lorsqu'on clique sur le bouton "Lancer Acquisition" et vide les textEdit pour ne pas garder les anciennes valeurs
//* Paramètres :
//*  - aucun
//*
//* Valeur de retour : aucune
//---------------------------------------------------------------------------------------------
void InterfaceQt::onButtonAquisitionClicked() { // Bouton qui lance l'acquisition 
	sondeur.lancerAcquisition(); // Appele la methode lancer aquisition qui lance tout le programme et le timer
    // Vide tout a chaque fois
	ui.textEdit_donnees->clear();
	ui.textEdit_donnesanalyse->clear();
	ui.textEdit_info->clear();
}