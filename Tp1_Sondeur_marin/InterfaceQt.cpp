#include "InterfaceQt.h"
#include "qtimer.h"
#include <QRegularExpression>
#include "Sondeur_marin.h"
#include <QDebug>

InterfaceQt::InterfaceQt(QWidget* parent)
    : QMainWindow(parent),sondeur()
{
    ui.setupUi(this);
    QList<QSerialPortInfo> info = sondeur.GetPort();
    for (int i = 0; i < info.size();i++) {
        ui.comboBox_port->addItem(info[i].portName(), QVariant(info[i].portName()));
    }
	timer = new QTimer(this);
	connect(timer, &QTimer::timeout, this, &InterfaceQt::TrameAnalysePause);
	 // pause de 15 secondes)
    
}

InterfaceQt::~InterfaceQt()
{ 
}

//====Trame====
//$GPAPB, V, A, 0.00, L, N, V, V, 22.6, T, , 22.6, T, , * 4F
//$GPGLL, 4952.6634, N, 00218.1741, E, 001924, V * 36\n$GPRMB, V, 0.00, L, , , 7517.5754, N, 06122.0368, E, 999.99, 22.6, 0.0, V * 23
//$GPRMC, 001924, V, 4952.6634, N, 00218.1741, E, 0.0, 0.0, 010200, 2.0, W * 79
//$GPGGA, 001924, 4952.6634, N, 00218.1741, E, 0, 0, 50.00, -47, M, , , , * 2E
//$GPGSA, A, 1, , , , , , , , , , , , , 50.00, 50.00, 50.00 * 35
//$GPGSV, 1, 1, 3, , , 0, * 4B\n$SDMTW, 25.2, C * 01
//$SDMTW,25.2,C*01
//$SDDPT, , * 57
//$SDDBT, , , , , , , * 45
// arduino

void InterfaceQt::onSerialPortReadyRead()
{
    // evite les bugs de valeurs en lisant ligne par ligne et pas tout d'un coup
        QByteArray data = port->readLine();
        data = "$GPAPB, V, A, 0.00, L, N, V, V, 22.6, T, , 22.6, T, , * 4F\n$GPGLL, 4952.6634, N, 00218.1741, E, 001924, V * 36\n$GPRMB, V, 0.00, L, , , 7517.5754, N, 06122.0368, E, 999.99, 22.6, 0.0, V * 23\n$GPRMC, 001924, V, 4952.6634, N, 00218.1741, E, 0.0, 0.0, 010200, 2.0, W * 79\n$GPGGA, 001924, 4952.6634, N, 00218.1741, E, 0, 0, 50.00, -47, M, , , , * 2E\n$GPGSA, A, 1, , , , , , , , , , , , , 50.00, 50.00, 50.00 * 35\n$GPGSV, 1, 1, 3, , , 0, * 4B\n$SDMTW, 25.2, C * 01\n$SDDPT, , * 57\n$SDDBT, , , , , , , * 45";
		ligne = QString::fromUtf8(data);

        // retire les espaces et les retours à la ligne
        ligne = ligne.trimmed();
        ui.textEdit_donnees->insertPlainText(ligne + "\n");
        QStringList data2 = ligne.split(QRegularExpression("[$\n]")); // recup tout les valeurs dans une liste 
		if (firstTrame) {
			firstTrame = false;
			TrameAnalysePause();
			return;
		}
        timer->start(15000);
}

void InterfaceQt::TrameAnalysePause()
{
    ui.textEdit_donnesanalyse->insertPlainText("\n=========Nouvelle Trame info clé=========\n");
    sondeur.analyseTrame(ligne);
    afficheTrame(sondeur.getTrame());
}

void InterfaceQt::afficheTrame(QList<QStringList> trames) {
	QStringList GPGLL = trames[0];
	QStringList SDMTW = trames[1];
	QStringList SDDPT = trames[2];
	QStringList SDDBT = trames[3];
    for (int i = 0; i < trames.size(); i++) {
        const QStringList trame = trames[i];
        ui.textEdit_donnesanalyse->insertPlainText("=========nom d'indentification =========\n");
        for (int j = 0; j < trame.size(); j++) {
            ui.textEdit_donnesanalyse->insertPlainText("Champ " + QString::number(j + 1) + " :  " + trame[j].trimmed() + "\n");
        }
    }
    ui.textEdit_info->insertPlainText("\n=======Info Trame======\n");
    if (SDDPT[1].isEmpty() || SDDPT[1]==" ") {
        SDDPT[1] = "N/A";
    }
    if (SDDBT[3].isEmpty() || SDDBT[3]==" ") {
        SDDBT[3] = "N/A";
    }
    ui.textEdit_info->insertPlainText("temparture : " + SDMTW[1] + "°" + GPGLL[2] + sondeur.convert(GPGLL[1]) + " " + GPGLL[4] + sondeur.convert(GPGLL[3]) + " Profondeur Sous sonde : " + SDDBT[3] + " m" + " Profondeur totale : " + SDDPT[1] + " m");
}


void InterfaceQt::onOpenPortButtonClicked()
{
    if (ui.comboBox_port->currentIndex() >= 0)
    {
        ui.comboBox_port->
        port = new QSerialPort(ui.comboBox_port->currentText());
        QObject::connect(port, &QSerialPort::readyRead, this, &InterfaceQt::onSerialPortReadyRead);
        port->setBaudRate(9600);
        port->setDataBits(QSerialPort::DataBits::Data8);
        port->setParity(QSerialPort::Parity::NoParity);
        port->setStopBits(QSerialPort::StopBits::OneStop);

        if (port->open(QIODevice::ReadWrite))
        {
            ui.label_status->setText("Status port : Ouvert");
            port->write("test");
        }
        else
        {
            ui.label_status->setText("Status port : Erreur d'ouverture");
        }
    }
}