#include "InterfaceQt.h"
#include <QDesktopServices>
#include <QDir>
#include <QCoreApplication>
#include "qtimer.h"
#include <QRegularExpression>

InterfaceQt::InterfaceQt(QWidget* parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

    QList<QSerialPortInfo> availablePorts = QSerialPortInfo::availablePorts();

    for (int i = 0; i < availablePorts.size(); ++i) {
        QSerialPortInfo info = availablePorts[i];
        ui.comboBox_port->addItem(info.portName(), QVariant(info.portName()));

    }
    timer = new QTimer(this);



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

QString convert(QString nombre) {
    QStringList nb = nombre.split(QRegularExpression("[.]"));
    int nb1 = nb[0].toInt() / 100;
    int nb2 = nb[0].toInt() % 100;
    QString add = QString::number(nb1) += "°"+QString::number(nb2);
    nombre = add + "." + nb[1];
    return nombre;
}

void InterfaceQt::onSerialPortReadyRead()
{
    // evite les bugs de valeurs en lisant ligne par ligne et pas tout d'un coup
        QByteArray data = port->readLine();
        data = "$GPAPB, V, A, 0.00, L, N, V, V, 22.6, T, , 22.6, T, , * 4F\n$GPGLL, 4952.6634, N, 00218.1741, E, 001924, V * 36\n$GPRMB, V, 0.00, L, , , 7517.5754, N, 06122.0368, E, 999.99, 22.6, 0.0, V * 23\n$GPRMC, 001924, V, 4952.6634, N, 00218.1741, E, 0.0, 0.0, 010200, 2.0, W * 79\n$GPGGA, 001924, 4952.6634, N, 00218.1741, E, 0, 0, 50.00, -47, M, , , , * 2E\n$GPGSA, A, 1, , , , , , , , , , , , , 50.00, 50.00, 50.00 * 35\n$GPGSV, 1, 1, 3, , , 0, * 4B\n$SDMTW, 25.2, C * 01\n$SDDPT, , * 57\n$SDDBT, , , , , , , * 45";
        QString ligne(data);

        // retire les espaces et les retours à la ligne
        ligne = ligne.trimmed();
        ui.textEdit_donnees->insertPlainText(ligne + "\n");
        QStringList data2 = ligne.split(QRegularExpression("[$\n]")); // recup tout les valeurs dans une liste 
        QStringList GPGLL, SDMTW,SDDPT,SDDBT;
        ui.textEdit_donnesanalyse->insertPlainText("=========Nouvelle Trame info clé=========\n");
        for (int i = 0; i < data2.size(); i++) {
            if (data2[i].startsWith("GPGLL")) {
                GPGLL = data2[i].split(QRegularExpression("[,]"));
                afficheTrame(GPGLL);
            }
            else if (data2[i].startsWith("SDMTW")) {
                SDMTW = data2[i].split(QRegularExpression("[,]"));
                afficheTrame(SDMTW);
            }
            else if (data2[i].startsWith("SDDPT")){
                SDDPT = data2[i].split(QRegularExpression("[,]"));
                afficheTrame(SDDPT);
            }
            else if (data2[i].startsWith("SDDBT")) {
                SDDBT = data2[i].split(QRegularExpression("[,]"));
                afficheTrame(SDDBT);
            }

        }
        
        ui.textEdit_info->insertPlainText("=======Info Trame======\n");
        if (SDDPT[1].isEmpty()) {
            SDDPT[1] = "N/A";
        }
        if (SDDBT[3].isEmpty()) {
            SDDBT[3] = "N/A";
        }
        ui.textEdit_info->insertPlainText("temparture : "+ SDMTW[1]+"°"+ GPGLL[2]+convert(GPGLL[1])+ " "+ GPGLL[4]+ convert(GPGLL[3])+ " Profondeur Sous sonde : "+ SDDBT[3]+"m" + " Profondeur totale : " + SDDPT[1]+"m");
        
}

void InterfaceQt::afficheTrame(QStringList Trame) {
    ui.textEdit_donnesanalyse->insertPlainText("=========nom d'indentification =========\n");
    for (int i = 0; i < Trame.size(); i++)
    {
        Trame[i] = Trame[i].trimmed();
        ui.textEdit_donnesanalyse->insertPlainText("Champ " + QString::number(i + 1) + " :  " + Trame[i] + "\n");
    }
}

void InterfaceQt::onSendMessageButtonClicked()
{
    if (port->isOpen())
    {
        port->write(ui.lineEdit_envoi->text().toStdString().c_str());
        //QByteArray donnees = port->readAll();
        //ui.textEdit_donnees->insertPlainText("Message envoye : " +donnees+ "\n");
        //ui.lineEdit_envoi->clear(); 
    }
   
}

void InterfaceQt::onOpenPortButtonClicked()
{
    
    if (ui.comboBox_port->currentIndex() >= 0)
    {
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



