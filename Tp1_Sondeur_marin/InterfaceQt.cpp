#include "InterfaceQt.h"
#include "Sondeur_marin.h"

InterfaceQt::InterfaceQt(QWidget* parent)
    : QMainWindow(parent),sondeur()
{
    ui.setupUi(this);
    QList<QSerialPortInfo> info = sondeur.GetPort();
    for (int i = 0; i < info.size();i++) {
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

//====Trame====
//$GPAPB, V, A, 0.00, L, N, V, V, 22.6, T, , 22.6, T, , * 4F
//$GPGLL, 4952.6634, N, 00218.1741, E, 001924, V * 36\n$GPRMB, V, 0.00, L, , , 7517.5754, N, 06122.0368, E, 999.99, 22.6, 0.0, V * 23
//$GPRMC, 001924, V, 4952.6634, N, 00218.1741, E, 0.0, 0.0, 010200, 2.0, W * 79
//$GPGGA, 001924, 4952.6634, N, 00218.1741, E, 0, 0, 50.00， -47， M， , , , * 2E
//$GPGSA, A, 1, , , , , , , , , , , , , 50.00, 50.00, 50.00 * 35
//$GPGSV, 1, 1, 3, , , 0, * 4B\n$SDMTW, 25.2, C * 01
//$SDMTW,25.2,C*01
//$SDDPT, , * 57
//$SDDBT, , , , , , , * 45


void InterfaceQt::afficheTrame(QList<QStringList> trames) {
    if (trames.isEmpty()) {
        return;
    }
    for (int i = 0; i < trames.size(); i++) {
        const QStringList trame = trames[i];
        ui.textEdit_donnesanalyse->insertPlainText("=========nom d'indentification =========\n");
        for (int j = 0; j < trame.size(); j++) {
            ui.textEdit_donnesanalyse->insertPlainText("Champ " + QString::number(j + 1) + " :  " + trame[j].trimmed() + "\n");
        }
    }
    QStringList GPGLL, SDMTW, SDDPT, SDDBT;
    for (int i = 0; i < trames.size(); i++) {
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
	if (!GPGLL.isEmpty() && !SDMTW.isEmpty() && !SDDPT.isEmpty() && !SDDBT.isEmpty()) {
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


void InterfaceQt::onOpenPortButtonClicked()
{
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
    QList<QStringList> trames = sondeur.getTrame();
    ui.textEdit_donnees->insertPlainText(sondeur.getTrameBrut() + "\n");
    afficheTrame(trames);
}
void InterfaceQt::OnAquisitionFini() {
	ui.textEdit_donnees->insertPlainText("=================Acquisition terminée.=================\n");
}

void InterfaceQt::onButtonAquisitionClicked() {
	qDebug() << "Bouton Acquisition cliqué";
	sondeur.lancerAcquisition();
	ui.textEdit_donnees->clear();
	ui.textEdit_donnesanalyse->clear();
	ui.textEdit_info->clear();
}