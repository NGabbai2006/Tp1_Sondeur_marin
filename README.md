# Sondeur marin
 
Programme C++ (Qt, Visual Studio) qui communique avec un sondeur marin via le port série.
 
Il permet de :
- ouvrir le port série du sondeur
- lancer et arrêter l'acquisition
- afficher les trames reçues et les analyser
Informations affichées :
- profondeur sous le capteur
- profondeur totale
- température de l'eau
- latitude
- longitude

## Configuration du port série
 
Paramètres par défaut : 4800 bauds, 8 bits de données, pas de parité, 1 bit de stop.
 
```cpp
port->setBaudRate(4800);
port->setDataBits(QSerialPort::DataBits::Data8);
port->setParity(QSerialPort::Parity::NoParity);
port->setStopBits(QSerialPort::StopBits::OneStop);
```
 
Si votre sondeur utilise d'autres paramètres, modifiez-les dans `Sondeur_marin.cpp` (ligne 78).