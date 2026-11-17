#include "Sondeur_marin.h"

Sondeur_marin::Sondeur_marin() {

    availablePorts = QSerialPortInfo::availablePorts();
    
}
Sondeur_marin::~Sondeur_marin() {
}

// permet de recuperer la liste de tout les ports trouver. 
QList<QSerialPortInfo> Sondeur_marin::GetPort() {
    return availablePorts;
}

// permet de convertir la longitude et la latitude
QString Sondeur_marin::convert(QString nombre) {
    QStringList nb = nombre.split(QRegularExpression("[.]"));
    int nb1 = nb[0].toInt() / 100;
    int nb2 = nb[0].toInt() % 100;
    QString add = QString::number(nb1) += "°" + QString::number(nb2);
    nombre = add + "." + nb[1];
    return nombre;
}


// coupe la trame en plusieurs bout 
void Sondeur_marin::analyseTrame(QString ligne) {
    QStringList data2 = ligne.split(QRegularExpression("[$\n]")); // recup tout les valeurs dans une liste 
    
    for (int i = 0; i < data2.size(); i++) {
        if (data2[i].startsWith("GPGLL")) {
            GPGLL = data2[i].split(QRegularExpression("[,]"));
        }
        else if (data2[i].startsWith("SDMTW")) {
            SDMTW = data2[i].split(QRegularExpression("[,]"));
        }
        else if (data2[i].startsWith("SDDPT")) {
            SDDPT = data2[i].split(QRegularExpression("[,]"));
        }
        else if (data2[i].startsWith("SDDBT")) {
            SDDBT = data2[i].split(QRegularExpression("[,]"));
        }

    }
}

// permet de recuper les parties de la trame qui nous interesse dans une liste 
QList<QStringList> Sondeur_marin::getTrame() {
	QList<QStringList> trames;
	trames.append(GPGLL);
	trames.append(SDMTW);
	trames.append(SDDPT);
	trames.append(SDDBT);
	return trames;
}