Ce code permet d'afficher les trames recu du sondeur marin , de les analyser et d'afficher les informations suivantes : 
-la profondeur sous le capteur
-la profondeur total
-la temperature de l'eau
-la longitude
-la latitude. 
Il permet aussi de lancer l'acquisition et de l'arreter. Il permet aussi d'ouvrir le port serie du sondeur marin pour pouvoir communiquer avec lui.
Ce code s'utilise donc avec un sondeur marin. Si votre sondeur marin n'est pas en Baudrate 4800, No Parity, 8 DataBits et 1 StopBit, il faudra modifier le code dans le fichier Sondeur_marin.cpp ligne 78.
Voici les parametres du port serie du sondeur marin :
	port->setBaudRate(4800);
	port->setDataBits(QSerialPort::DataBits::Data8);
	port->setParity(QSerialPort::Parity::NoParity);
	port->setStopBits(QSerialPort::StopBits::OneStop);