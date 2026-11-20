#include "InterfaceQt.h"
#include "Sondeur_marin.h"

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

InterfaceQt::~InterfaceQt()
{ 
	QObject::disconnect(&sondeur, &Sondeur_marin::readyRead, this, &InterfaceQt::OnAquisition);
	QObject::disconnect(&sondeur, &Sondeur_marin::aquisitionFini, this, &InterfaceQt::OnAquisitionFini);
}

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
			qDebug() << "Trame GPGLL : " << trames[i];
            GPGLL = trames[i];
        }
        else if (trames[i][0] == "SDMTW") {
			qDebug() << "Trame SDMTW : " << trames[i];
            SDMTW = trames[i];
        }
        else if (trames[i][0] == "SDDPT") {
			qDebug() << "Trame SDDPT : " << trames[i];
            SDDPT = trames[i];
        }
        else if (trames[i][0] == "SDDBT") {
			qDebug() << "Trame SDDBT : " << trames[i];
            SDDBT = trames[i];
        }
    }
	if (!GPGLL.isEmpty() && !SDMTW.isEmpty() && !SDDPT.isEmpty() && !SDDBT.isEmpty()) { // Si il manque une partie de la tramme je n'affiche pas les informations interressante / j'attends
        ui.textEdit_info->insertPlainText("=======Info Trame======\n");
        if (SDDPT[1].isEmpty() || SDDPT[1] == " ") {
            SDDPT[1] = "N/A";
        }
        if (SDDBT[3].isEmpty() || SDDBT[3] == " ") {
            SDDBT[3] = "N/A";
        }
        ui.textEdit_info->insertPlainText("temparture : " + SDMTW[1] + "°" + GPGLL[2] + sondeur.convert(GPGLL[1]) + " " + GPGLL[4] + sondeur.convert(GPGLL[3]) + " Profondeur Sous sonde : " + SDDBT[3] + " m" + " Profondeur totale : " + SDDPT[1] + " m\n");
	}
}


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

void InterfaceQt::OnAquisition() {
    QList<QStringList> trames = sondeur.getTrame(); // Recupère la trame
    ui.textEdit_donnees->insertPlainText(sondeur.getTrameBrut() + "\n"); // Affiche la trame recue avant traitement
    afficheTrame(trames); 
}
void InterfaceQt::OnAquisitionFini() {
	ui.textEdit_donnees->insertPlainText("=================Acquisition terminée.=================\n");
}

void InterfaceQt::onButtonAquisitionClicked() { // Bouton qui lance l'acquisition 
	qDebug() << "Bouton Acquisition cliqué";
	sondeur.lancerAcquisition(); // Appele la methode lancer aquisition qui lance tout le programme et le timer
    // Vide tout a chaque fois
	ui.textEdit_donnees->clear();
	ui.textEdit_donnesanalyse->clear();
	ui.textEdit_info->clear();
}