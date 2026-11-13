#include "Tp1_Sondeur_marin.h"
#include <QDesktopServices>
#include <QDir>
#include <QCoreApplication>
#include "qtimer.h"
#include <QRegularExpression>

Tp1_Sondeur_marin::Tp1_Sondeur_marin(QWidget* parent)
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

Tp1_Sondeur_marin::~Tp1_Sondeur_marin()
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
void Tp1_Sondeur_marin::onSerialPortReadyRead()
{
    qDebug() << "test test";
    // evite les bugs de valeurs en lisant ligne par ligne et pas tout d'un coup
        QByteArray data = port->readLine();
        data = "$GPAPB, V, A, 0.00, L, N, V, V, 22.6, T, , 22.6, T, , * 4F\n$GPGLL, 4952.6634, N, 00218.1741, E, 001924, V * 36\n$GPRMB, V, 0.00, L, , , 7517.5754, N, 06122.0368, E, 999.99, 22.6, 0.0, V * 23\n$GPRMC, 001924, V, 4952.6634, N, 00218.1741, E, 0.0, 0.0, 010200, 2.0, W * 79\n$GPGGA, 001924, 4952.6634, N, 00218.1741, E, 0, 0, 50.00, -47, M, , , , * 2E\n$GPGSA, A, 1, , , , , , , , , , , , , 50.00, 50.00, 50.00 * 35\n$GPGSV, 1, 1, 3, , , 0, * 4B\n$SDMTW, 25.2, C * 01\n$SDDPT, , * 57\n$SDDBT, , , , , , , * 45";
        QString ligne(data);

        // retire les espaces et les retours à la ligne
        ligne = ligne.trimmed();
        QStringList data2 = ligne.split(QRegularExpression("[,\n]")); // recup tout les valeurs dans une liste 
        ui.textEdit_donnees->insertPlainText(ligne + "\n");
        int j=0;
        QStringList analyst;
        int x = 0;
        int indice = 0;
        int tab[10];
        while (data2.size() > j) {
            qDebug() << data2[j];
            if (data2[j] == "$GPGLL" || data2[j] == "$SDMTW" || data2[j] == "$SDDPT") {
                tab[indice]= x;
                indice++;
                while (j < data.size() && data2[j] != "$GPRMC" && data2[j] == "$SDDPT" && data2[j] != "$SDDBT") {
                    analyst.append(data2[j]);
                    j++;
                    x++;
                }
            }
            else if (data2[j] == "$SDDBT") {
                while (j < data2.size()) {
                    analyst.append(data2[j]);
                    j++;
                    x++;
                }
            }
            else {
            j++;
            }
        }
        for (int i = 0; i < analyst.size(); i++)
        { 
            if (i == 0) {
                ui.textEdit_donnesanalyse->insertPlainText("=========Nouvelle Trame=========\n");
            }
            analyst[i] = analyst[i].trimmed();
            ui.textEdit_donnesanalyse->insertPlainText("Champ "+QString::number(i + 1) + " :  " + analyst[i] + "\n");
        }
        
        ui.textEdit_info->insertPlainText("=======Info Trame======\n");
        ui.textEdit_info->insertPlainText("temparture : "+analyst[tab[0] + 1]);
        qDebug() << "ENVOYE :" << ligne;
}



void Tp1_Sondeur_marin::onSendMessageButtonClicked()
{
    if (port->isOpen())
    {
        port->write(ui.lineEdit_envoi->text().toStdString().c_str());
        //QByteArray donnees = port->readAll();
        //ui.textEdit_donnees->insertPlainText("Message envoye : " +donnees+ "\n");
        //ui.lineEdit_envoi->clear(); 
    }
   
}

void Tp1_Sondeur_marin::onOpenPortButtonClicked()
{
    
    if (ui.comboBox_port->currentIndex() >= 0)
    {
        port = new QSerialPort(ui.comboBox_port->currentText());
        QObject::connect(port, &QSerialPort::readyRead, this, &Tp1_Sondeur_marin::onSerialPortReadyRead);
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



