# Arduino-Projects
Liste de projet en Arduino réalisé avec mes élèves.

Les projets ont été réalisé sur des cartes Arduino UNO, MEGA, ESP et NodeMCU.

Ce repository contient actuellement 2 projets.

## Affichage des températures d'une chambre Froide sur Blynk et système d'alerte
Le premier est celui d'une chambre froide contenant plusieurs salles.
  
Le programme permet de savoir en temps réel la température de chaque chambre, ainsi que d'envoyer une alerte via Blynk à l'utilisateur si jamais la température d'une des chambres est trop élevé depuis une certaine durée.

La température de seuil (à partir de laquelle l'alerte peut se déclencher) est directement paramétrée via Blynk, ainsi que la durée tolérée avant d'activer une alerte.

## Affichage d'une série de capteurs sur écran OLED (Organic Light-Emitting Diode) et sur application Blynk
Ce deuxième programme permet de lire la valeur de plusieurs capteurs et les affiches sur deux supports :

- Un petit écran OLED, qui contiendra 4 affichages possibles, le changement du numéro de page se réalise à l'aide d'un bouton poussoir.

- Une application Blynk, application très utilisée en Arduino permettant d'envoyer et de recevoir directement des informations d'une carte Arduino.

Voici la liste exhaustive des 7 capteurs présents et utilisés :

  - Capteur/Sonde de température - DS18B20
  - Capteur de pression barométrique et d'altitude - BMP280
  - Capteur d'humidité et de température pour piscine - DHT11
  - Capteur de gaz - MQ (valeur en ppm)
  - Anémomètre sur Pergola (lecture de la vitesse du vent, et de la plus grande vitesse relevée)
  - Capteur de PH
  - Capteur Ultrason HC-SR04
  
## Conclusion
Ces programmes ont été réalisées d'Avril à Juillet 2020, dans le cadre d'une série de cours-projets en Arduino afin d'aider les élèves à apprendre à utiliser ce langage.
