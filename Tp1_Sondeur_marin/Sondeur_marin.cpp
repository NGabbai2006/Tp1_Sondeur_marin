#include "Sondeur_marin.h"

Sondeur_marin::Sondeur_marin(QObject* parent) : QObject(parent), port(nullptr) {
    availablePorts = QSerialPortInfo::availablePorts();
    timer = new QTimer();
	timer->setSingleShot(true); // permet que le timer se lance une fois et donc a chaque fois que l'on lance l'acquisition il se relance
	QObject::connect(timer, &QTimer::timeout, this, &Sondeur_marin::StopAcquisition);
}

Sondeur_marin::~Sondeur_marin() {
    QObject::disconnect(timer, &QTimer::timeout, this, &Sondeur_marin::StopAcquisition);
    QObject::disconnect(port, &QSerialPort::readyRead, this, &Sondeur_marin::onSerialPortReadyRead);
    if (port != nullptr) {
        port->close();
        delete port;
    }
}

void Sondeur_marin::supprime() {
    GPGLL.clear(); // supprime les listes et les anciennes trames  car sinon on garde les anciennes valeurs
    SDMTW.clear();
    SDDPT.clear();
    SDDBT.clear();
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

// permet de recuperer la liste de tout les ports trouver. 
QList<QSerialPortInfo> Sondeur_marin::GetPort() {
    return availablePorts;
}

bool Sondeur_marin::LancerPort(QString portName) {
    if (port != nullptr) {
        port->close();
        delete port;
    }
    port = new QSerialPort();
    port->setPortName(portName);
	QObject::connect(port, &QSerialPort::readyRead, this, &Sondeur_marin::onSerialPortReadyRead);
	port->setBaudRate(9600);
	port->setDataBits(QSerialPort::DataBits::Data8);
	port->setParity(QSerialPort::Parity::NoParity);
	port->setStopBits(QSerialPort::StopBits::OneStop);
	if (port->open(QIODevice::ReadWrite))
	{
        port->blockSignals(true);
        return true;
	}
	else
	{
        return false;
	}
}

void Sondeur_marin::lancerAcquisition() {
    if (!port || !port->isOpen()) {
        return;
    }
    supprime();
    newTrame.clear();
    ligne.clear();
    port->clear();
    port->blockSignals(false);
	timer->start(15000); // Démarre le timer pour 15 secondes
}

void Sondeur_marin::StopAcquisition() {
    port->blockSignals(true);
	emit aquisitionFini(); // signal pour dire que l'acquisition est fini
}

void Sondeur_marin::onSerialPortReadyRead() {
	newTrame += port->readAll(); // permet de recuperer la trame petit a petit
    qDebug() << QString::fromUtf8(newTrame);
    while (newTrame.contains('\n')) {
        int i = newTrame.indexOf('\n'); // recup la premiere occurence du \n
		tramebrut = QString::fromUtf8(newTrame.left(i)); // recupere la trame jusqu'au retour a la ligne pour l'afficher apres
		ligne = QString::fromUtf8(newTrame.left(i)); // trame a analyser
		qDebug() << "Trame reçue : " << ligne;
		newTrame.remove(0, i + 1); // supprime la trame deja recupere car plus besoin
		qDebug() << "Trame restante : " << QString::fromUtf8(newTrame);
		analyseTrame(); // Appel pour analyser la trame
    }
}

// coupe la trame en plusieurs bout 
void Sondeur_marin::analyseTrame() {
	QString ligneverifie = ligne.trimmed(); // permet de manipuler la trame et de la verifier
    ligne.clear();
    qDebug() << "ligne vidé :" << ligne ;
	qDebug() << "ligne verifié :" << ligneverifie;
    if (ligneverifie.isEmpty()) {
		return; // permet de passser a la trame suivante si par malchance la trame est mal decouper avec seulement un espace vide
    }
    bool ok;
    int checksum = ligneverifie.section('*', 1, 1).toInt(&ok, 16); // récupère la valeur de checksum et verif et le met en hexa
    QString calcule = ligneverifie.split("*")[0].trimmed(); // permet de recup tout avant le *
    calcule.replace("$", ""); // supprime le $ 
    if (!ok) { // faire /remplacer par le calcul du checksum 
        // La trame est invalide
        return;
    }
    ligneverifie = ligneverifie.section('*', 0, 1).trimmed(); // recup tout la trame verifié
    ligneverifie.replace("$", ""); // supprime le $
    QStringList data = ligneverifie.split(QRegularExpression("[,]")); // coupe la trame en plusieurs bout
    qDebug() << "old Trame : " << ligneverifie;
	qDebug() << "Trame analysée : " << data;
    if (data[0].startsWith("GPGLL")) {
        GPGLL.append(data);
    }
    else if (data[0].startsWith("SDMTW")) {
        SDMTW.append(data);
    }
    else if (data[0].startsWith("SDDPT")) {
        SDDPT.append(data);
    }
    else if (data[0].startsWith("SDDBT")) {
        SDDBT.append(data);
    }
	emit readyRead(); // envoie un signal pour dire que la trame est prete
}

QString Sondeur_marin::getTrameBrut() {
	return tramebrut;
}
// permet de recuper les parties de la trame qui nous interesse dans une liste 
QList<QStringList> Sondeur_marin::getTrame() {
    QList<QStringList> trames;
    QStringList tempo;
    int i = 0;
    int indice = GPGLL.size();
	qDebug() << "Trames temporaires : " << trames;

	while (indice + 1 < SDMTW.size() || indice + 1 < SDDPT.size() || indice + 1 < SDDBT.size()) { // permet de verifier que tous les trames soit recu
        tempo.append({ "GPGLL", "N/A", "N/A", "N/A", "N/A" });
        GPGLL.append(tempo);
		tempo.clear();  
        indice++;
    }
	indice = SDMTW.size();
    while (indice + 1 < GPGLL.size() || indice + 1 < SDDPT.size() || indice + 1 < SDDBT.size()) { // meme chose
		tempo.append({ "SDMTW", "N/A" });
        SDMTW.append(tempo);
        tempo.clear();
        indice++;
    }
	indice = SDDPT.size();
    while (indice + 1 < GPGLL.size() || indice + 1 < SDMTW.size() || indice + 1 < SDDBT.size()) { // meme chose
		tempo.append({ "SDDPT", "N/A" });
        SDDPT.append(tempo);
        tempo.clear();
		indice++;
    }
	indice = SDDBT.size();
    while (indice + 1 < GPGLL.size() || indice + 1 < SDMTW.size() || indice + 1 < SDDPT.size()) { // meme chose
		tempo.append({ "SDDBT", "N/A", "N/A", "N/A" });
        SDDBT.append(tempo);
        tempo.clear();
        indice++;
    }
    if (!GPGLL.isEmpty()) {
        trames.append(GPGLL[0]);
    }
	if (!SDMTW.isEmpty()) {
        trames.append(SDMTW[0]);
	}
	if (!SDDPT.isEmpty()) {
        trames.append(SDDPT[0]);
	}
	if (!SDDBT.isEmpty()) {
        trames.append(SDDBT[0]);
	}
	if (!GPGLL.isEmpty() && !SDMTW.isEmpty() && !SDDPT.isEmpty() && !SDDBT.isEmpty()) {
        supprime();
	}
    qDebug() << "Trames finales : " << trames;
    return trames;
}