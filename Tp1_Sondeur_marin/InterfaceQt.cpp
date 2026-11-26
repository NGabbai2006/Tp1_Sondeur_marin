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
		if (trame.isEmpty()) {
			continue;
		}
        ui.textEdit_donnesanalyse->insertPlainText("=========nom d'indentification =========\n");
        for (int j = 0; j < trame.size(); j++) {
			if (trame[j].isEmpty()) {
                ui.textEdit_donnesanalyse->insertPlainText("Champ " + QString::number(j + 1) + " :  (vide)\n");
			}
			else {
				ui.textEdit_donnesanalyse->insertPlainText("Champ " + QString::number(j + 1) + " :  " + trame[j].trimmed() + "\n");
			}
        }
    }
    QStringList GPGLL, SDMTW, SDDPT, SDDBT;
	int s_GLL = 0;
	int s_MTW = 0;
	int s_DPT = 0;
	int s_DBT = 0;

    for (int i = 0; i < trames.size(); i++) { // Recupère les trames dans variables qui correspondent
        if (trames[i].isEmpty()){ 
            continue; 
        }
        if (trames[i][0] == "GPGLL") {
            GPGLL = trames[i];
            s_GLL = trames[i].size();
        }
        else if (trames[i][0] == "SDMTW") {
            SDMTW = trames[i];
            s_MTW = trames[i].size();
        }
        else if (trames[i][0] == "SDDPT") {
            SDDPT = trames[i];
			s_DPT = trames[i].size();
        }
        else if (trames[i][0] == "SDDBT") {
            SDDBT = trames[i];
			s_DBT = trames[i].size();
        }
    }
    if (s_GLL >= 5) {
		if (!GPGLL[3].isEmpty() && !GPGLL[4].isEmpty()) {
			ui.label_longitude->setText("Longitude : " + sondeur.convert(GPGLL[3]) + " " + GPGLL[4]);
		}
    }
	if (s_GLL >= 3) {
		if (!GPGLL[1].isEmpty() && !GPGLL[2].isEmpty()) {
            ui.label_latitude->setText("Latitude : " + sondeur.convert(GPGLL[1]) + " " + GPGLL[2]);
		}
	}
	if (s_MTW >= 2) {
		if (!SDMTW[1].isEmpty()) {
			ui.label_temp->setText("Température : " + SDMTW[1] + "°C");
		}
	}
	if (s_DBT >= 4) {
		if (!SDDBT[3].isEmpty()) {
			ui.label_sous_cpt->setText("Profondeur sous sonde : " + SDDBT[3] + " m");
		}
	}
	if (s_DPT >= 3) {
		if (!SDDPT[1].isEmpty() && !SDDPT[2].isEmpty()) {
			float nb = SDDPT[1].toFloat() + SDDPT[2].toFloat();
			QString totalDepth = QString::number(nb);
			ui.label_profon_total->setText("Profondeur totale : " + totalDepth + " m");
		}
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
	ui.label_latitude->clear();
	ui.label_longitude->clear();
	ui.label_temp->clear();
	ui.label_sous_cpt->clear();
	ui.label_profon_total->clear();
}