//*********************************************************************************************
//* Programme : Sondeur_marin.cpp                                                 Date : 20/09/2026
//*--------------------------------------------------------------------------------------------
//* Dernière mise à jour : 26/11/2026
//*
//* Programmeurs : Noah Gabbai                                            Classe : CIEL2
//*--------------------------------------------------------------------------------------------
//* But : Effectuer la communication avec le sondeur marin et recuperer les trames pour les analyser
//* Programmes associés : sondeur_marin.h
//*********************************************************************************************
#include "Sondeur_marin.h"

//---------------------------------------------------------------------------------------------
//* Constructeur : Sondeur_marin, permet d'initialiser les variables et de connecter les signaux et slots
//* Paramètres :
//*  - QObject* parent : le parent de l'objet
//*
//* Valeur de retour : aucune
//---------------------------------------------------------------------------------------------
Sondeur_marin::Sondeur_marin(QObject* parent) : QObject(parent), port(nullptr) {
    availablePorts = QSerialPortInfo::availablePorts();
    timer = new QTimer();
	timer->setSingleShot(true); // permet que le timer se lance une fois et donc a chaque fois que l'on lance l'acquisition il se relance
	QObject::connect(timer, &QTimer::timeout, this, &Sondeur_marin::StopAcquisition);
}

//---------------------------------------------------------------------------------------------
//* Destructeur : ~Sondeur_marin, permet de liberer la memoire et de fermer le port si il est ouvert
//* Paramètres :
//*  - aucun
//*
//* Valeur de retour : aucun
//---------------------------------------------------------------------------------------------
Sondeur_marin::~Sondeur_marin() {
    QObject::disconnect(timer, &QTimer::timeout, this, &Sondeur_marin::StopAcquisition);
    QObject::disconnect(port, &QSerialPort::readyRead, this, &Sondeur_marin::onSerialPortReadyRead);
    if (port != nullptr) {
        port->close();
        delete port;
    }
}

//---------------------------------------------------------------------------------------------
//* Fonction raccourci : permet de supprimer les anciennes trames pour ne pas garder les anciennes valeurs
//* Paramètres :
//*  - aucun
//*
//* Valeur de retour : aucune
//---------------------------------------------------------------------------------------------
void Sondeur_marin::supprime() {
    GPGLL.clear(); // supprime les listes et les anciennes trames  car sinon on garde les anciennes valeurs
    SDMTW.clear();
    SDDPT.clear();
    SDDBT.clear();
}

//---------------------------------------------------------------------------------------------
//* Fonction renvoyant le un nombre convertie en degres pour la latitude et la longitude
//* Paramètres :
//*  - QString nombre : le nombre a convertir
//*
//* Valeur de retour : QString : valeur convertie
//---------------------------------------------------------------------------------------------
QString Sondeur_marin::convert(QString nombre) {
    QStringList nb = nombre.split(QRegularExpression("[.]"));
    int nb1 = nb[0].toInt() / 100;
    int nb2 = nb[0].toInt() % 100;
    QString add = QString::number(nb1) + "°" + QString::number(nb2);
    nombre = add + "." + nb[1];
    return nombre;
}

//---------------------------------------------------------------------------------------------
//* Fonction permettant de recuperer la liste de tout les ports trouver.
//* Paramètres :
//*  - aucun
//*
//* Valeur de retour : QList<QSerialPortInfo> : liste des ports disponibles
//---------------------------------------------------------------------------------------------
QList<QSerialPortInfo> Sondeur_marin::GetPort() {
    return availablePorts;
}

//---------------------------------------------------------------------------------------------
//* Fonction qui permet d'ouvrir le port selectionner par l'utilisateur et de connecter le signal readyRead du port au slot onSerialPortReadyRead
//* Paramètres :
//*  - QString portName : le nom du port a ouvrir
//*
//* Valeur de retour : bool : true si le port est ouvert, false sinon
//---------------------------------------------------------------------------------------------
bool Sondeur_marin::LancerPort(QString portName) { //regarde tout les ports disponibles/actifs
    if (port != nullptr) {
        port->close();
        delete port;
    }
    port = new QSerialPort();
    port->setPortName(portName);
	QObject::connect(port, &QSerialPort::readyRead, this, &Sondeur_marin::onSerialPortReadyRead);
	port->setBaudRate(4800);
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

//---------------------------------------------------------------------------------------------
//* Fonction qui permet de lancer l'acquisition des trames du sondeur marin et de lancer le timer pour arreter l'acquisition apres 15 secondes
//* Paramètres :
//*  - aucun
//*
//* Valeur de retour : aucune
//---------------------------------------------------------------------------------------------
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

//---------------------------------------------------------------------------------------------
//* Fonction qui permet d'arreter la lecture du port et d'envoyer un signal pour dire que l'acquisition est fini
//* Paramètres :
//*  - aucun
//*
//* Valeur de retour : aucune
//---------------------------------------------------------------------------------------------
void Sondeur_marin::StopAcquisition() {
    port->blockSignals(true);
	emit aquisitionFini(); // signal pour dire que l'acquisition est fini
}

//---------------------------------------------------------------------------------------------
//* Fonction qui lit les trames recu du sondeur marin et les analyse pour recuperer les informations voulues
//* Paramètres :
//*  - aucun
//*
//* Valeur de retour : aucune
//---------------------------------------------------------------------------------------------
void Sondeur_marin::onSerialPortReadyRead() {
	newTrame += port->readAll(); // permet de recuperer la trame petit a petit
    while (newTrame.contains('\n')) {
        int i = newTrame.indexOf('\n'); // recup la premiere occurence du \n
		tramebrut = QString::fromUtf8(newTrame.left(i)); // recupere la trame jusqu'au retour a la ligne pour l'afficher apres
		ligne = QString::fromUtf8(newTrame.left(i)); // trame a analyser
		newTrame.remove(0, i + 1); // supprime la trame deja recupere car plus besoin
		analyseTrame(); // Appel pour analyser la trame
    }
}

//---------------------------------------------------------------------------------------------
//* Fonction qui analyse la trame recu du sondeur marin et la stock dans les variables correspondantes pour l'affichage
//* Paramètres :
//*  - aucun
//*
//* Valeur de retour : aucune
//---------------------------------------------------------------------------------------------
void Sondeur_marin::analyseTrame() {
	QString ligneverifie = ligne.trimmed(); // permet de manipuler la trame et de la verifier
    if (ligneverifie.isEmpty()) {
		return; // permet de passser a la trame suivante si par malchance la trame est mal decouper avec seulement un espace vide
    }
    if (!ligneverifie.contains('$')) {
        return;
    }
    ligneverifie = ligneverifie.section('*', 0, 1).trimmed(); // recup tout la trame verifié
    ligneverifie.replace("$", ""); // supprime le $
    QStringList data = ligneverifie.split(QRegularExpression("[,]")); // coupe la trame en plusieurs bout
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

//---------------------------------------------------------------------------------------------
//* Fonction renvoyant la trame brute recu du sondeur marin pour l'affichage
//* Paramètres :
//*  - aucun
//*
//* Valeur de retour : QString : trame brute recu
//---------------------------------------------------------------------------------------------
QString Sondeur_marin::getTrameBrut() {
	return tramebrut;
}

//---------------------------------------------------------------------------------------------
//* Fonction renvoyant les trames analysees du sondeur marin pour l'affichage
//* Paramètres :
//*  - aucun
//*
//* Valeur de retour : QList<QStringList> : trames analysees
//---------------------------------------------------------------------------------------------
QList<QStringList> Sondeur_marin::getTrame() {
    QList<QStringList> trames;
    QStringList tempo;
    int i = 0;
    int indice = GPGLL.size();
    trames.append(GPGLL);
    trames.append(SDMTW);
    trames.append(SDDPT);
    trames.append(SDDBT);
    if (!GPGLL.isEmpty() && !SDMTW.isEmpty() && !SDDPT.isEmpty() && !SDDBT.isEmpty()) {
        supprime();
    }
    return trames;
// ============== probleme d'index dedans est possiblement inutile au final =======================
/*
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
*/
}