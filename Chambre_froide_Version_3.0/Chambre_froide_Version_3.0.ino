
// temperature range of the DS18b20 
// Temperature Range: -55°C to +125°C.
//D12, D13, D22 Eclairages
 
 
/* ESP & Blynk */
// Select your modem:
#define TINY_GSM_MODEM_SIM800
//#define TINY_GSM_MODEM_SIM900
//#define TINY_GSM_MODEM_M590
//#define TINY_GSM_MODEM_A6
//#define TINY_GSM_MODEM_A7
//#define TINY_GSM_MODEM_BG96
//#define TINY_GSM_MODEM_XBEE
#include <TinyGsmClient.h>
#include <BlynkSimpleSIM800.h>
#define SerialMon Serial
#include <SoftwareSerial.h>
#include <SimpleTimer.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#define BLYNK_PRINT Serial    // Comment this out to disable prints and save space
 // Global defines
#define T_TARGET_MIN 0          // Minimum target soil temperature (in degrees Celsius)
#define T_TARGET_MAX 50           // Maximum target soil temperature (in degrees Celsius)
#define COLOR_RED "#E3233F"       // Color of the red LED in Blynk app
#define COLOR_GREEN "#68FC79"     // Color of the green LED in Blynk app

char auth[] = "kZqly4FgGxGQz0ON37g7jBdBQZB-XLxI";
char apn[]  = "zeopdata";
char ssid[] = "ZEOP-c5c9d6_EXT2.4G";
char pass[] = "ZTEGC8538fa3";


// Valentin Dubuc : Ajout du coefficient Temporel  : Temps réellement écoulé en Seconde
  // Permet d'incrémenter de X secondes à chaque seconde dites par l'Arduino
  int TempsReelSeconde = 35;
  // ----------------------- Fin de l'ajout des secondes réels ------------------



int TempTarget0; // preset temperature. 
int TempTarget1; // preset temperature. 
int TempTarget2; // preset temperature. 

// Ajout (Dubuc Valentin) - Variables des timers des chambres et des portes

int TimeTarget0; // preset Timer 1
int TimeTarget1; // preset Timer 2 
int TimeTarget2; // preset Timer 3

int pmints_0; // preset Timer Porte 1
int pmints_1; // preset Timer Porte 2
int pmints_2; // preset Timer Porte 3

// Fin Ajout

SimpleTimer timer;

 
#define ONE_WIRE_BUS 4 // DS18B20 on arduino pin2 corresponds to D4 on physical board "D4 pin on the ndoemcu Module"
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature DS18B20(&oneWire);


// Dernière modification - Valentin Dubuc 
float temp_0; // pas un "int" car on veut la valeur décimale au centième
float temp_1; // pas un "int" car on veut la valeur décimale au centième
float temp_2; // pas un "int" car on veut la valeur décimale au centième
float temp_3; // pas un "int" car on veut la valeur décimale au centième
float temp;
// Fin Modification

float Fahrenheit = 0;


int Dsensor1 = 7; // Refrigerator door sensor
int Dsensor2 = 8; // Refrigerator door sensor
int Dsensor3 = 9; // Refrigerator door sensor
int Dsensor4 = 6; // Refrigerator door sensor

// Tracks the time since last event fired
unsigned long previousMillis=0;
unsigned long int previoussecs = 0; 
unsigned long int currentsecs = 0; 
unsigned long currentMillis = 0; 
int interval= 1; // 1 second interval


// Temps d'ouverture des portes

int secs_0 = 0; // current seconds
int secs_1 = 0; // current seconds
int secs_2 = 0; // current seconds
int secs_3 = 0; // current seconds

// Plus besoin des minutes dans la version précise, car on calcul directement en seconde
/*
int mints_0 = 0;  // current minutes 
int mints_1 = 0;  // current minutes 
int mints_2 = 0;  // current minutes 
int mints_3 = 0;  // current minutes 
*/

// Temps pendant lequel la température est trop élevée

int c_secs_0 = 0;
int c_secs_1 = 0;
int c_secs_2 = 0;
int c_secs_3 = 0; 

// Plus besoin des minutes dans la version précise, car on calcul directement en seconde
/*
int c_mints_0 = 0;
int c_mints_1 = 0;
int c_mints_2 = 0;
int c_mints_3 = 0;
*/


// Fin de la modification

// Déclaration d'un "tempf" pour chaque chambre, afin de vérifier lesquels sont corrects ou non

int tempf = 0; // Indicateur général de température
int tempf_0 = 0; // indicateur de température (0 = OK, 1 = Trop chaud)
int tempf_1 = 0; // indicateur de température (0 = OK, 1 = Trop chaud)
int tempf_2 = 0; // indicateur de température (0 = OK, 1 = Trop chaud)
int tempf_3 = 0; // indicateur de température (0 = OK, 1 = Trop chaud)

// Fin déclaration
 
SoftwareSerial SerialAT(10,11); // (RX PIN D3, TX PIN D2) pour NANO et UNO /RX PIN D11 et TX PIN D10 POUR MEGA
TinyGsm modem(SerialAT);




 // ------------------------FUNCTIONS------------------------------------------------------

// TempTarget étant une température, on ne la multiplie pas par 60 !
 
BLYNK_WRITE(V8)
// This function will be calLED every time Slider Widget
// in Blynk app writes values to the Virtual Pin V8
{
  TempTarget0 = param.asInt();  // Assignation de la valeur entrante de la broche V8 à la variable
}

BLYNK_WRITE(V9)
// This function will be calLED every time Slider Widget
// in Blynk app writes values to the Virtual Pin V9
{
  TempTarget1 = param.asInt();  // Assignation de la valeur entrante de la broche V8 à la variable

}
BLYNK_WRITE(V11)
// This function will be calLED every time Slider Widget
// in Blynk app writes values to the Virtual Pin V11
{
  TempTarget2 = param.asInt();  // Assignation de la valeur entrante de la broche V11 à la variable

}
BLYNK_WRITE(V12)
// This function will be calLED every time Slider Widget
// in Blynk app writes values to the Virtual Pin V12
{
  TempTarget3 = param.asInt();  // Assignation de la valeur entrante de la broche V8 à la variable

}

// IL EST POSSIBLE D'AMELIORER LES TEMPS EN RESTANT EN SECONDES
// ENSUITE, IL SUFFIT DE CONSIDERER LES VALEURS DU TIMERS param.asInt(); comme valant 60 * param.asInt()
// Ainsi, il définira un réel temps en minutes, mais pmints_ et TimeTarget seront bien écrit en secondes, 
// et le temps en secondes de c_secs_  ou secs_ pourront être comparés à TimeTarget et pmints_



// Ajout (Dubuc Valentin) - Pour régler les 3 Timers des chambres, on va agir de la même façon qu'en haut

BLYNK_WRITE(V25)
{
  TimeTarget0 = 60 * param.asInt();  // Calcul avec la valeur * 60 pour bien la considérer en secondes!

}
BLYNK_WRITE(V26)
{
  TimeTarget1 = 60 * param.asInt();  // Calcul avec la valeur * 60 pour bien la considérer en secondes!
  
}
BLYNK_WRITE(V27)
{
  TimeTarget2 = 60 * param.asInt();  // Calcul avec la valeur * 60 pour bien la considérer en secondes!

}

// Fin Ajout


// Ajout (Dubuc Valentin) - Pour régler les 4 Timers des Portes, on va agir de la même façon qu'en haut

BLYNK_WRITE(V28)
{
  pmints_0 = 60 * param.asInt();  // Calcul avec la valeur * 60 pour bien la considérer en secondes!

}

BLYNK_WRITE(V29)
{
  pmints_1 = 60 * param.asInt();  // Calcul avec la valeur * 60 pour bien la considérer en secondes!

}

BLYNK_WRITE(V30)

{
  pmints_2 = 60 * param.asInt();  // Calcul avec la valeur * 60 pour bien la considérer en secondes!

}

BLYNK_WRITE(V31)

{
  pmints_3 = 60 * param.asInt();  // Calcul avec la valeur * 60 pour bien la considérer en secondes!

}

// Fin Ajout

// ---------------------------------- FIN FONCTION ------------------







void setup() 
{
  SerialMon.begin(9600);
  delay(10);
  SerialAT.begin(9600);
  delay(2000); // delay(3000);

 
  SerialMon.println(F("Initializing modem..."));
  modem.restart();
  Blynk.begin(auth, modem, apn, ssid, pass);

  DS18B20.begin();
  pinMode(Dsensor1, INPUT_PULLUP);
  pinMode(Dsensor2, INPUT_PULLUP);
  pinMode(Dsensor3, INPUT_PULLUP);
 // pinMode(Dsensor4, INPUT_PULLUP);


  timer.setInterval(1000L, getSendData);
  timer.setInterval(1000L, DoorSensor1);
  timer.setInterval(1000L, DoorSensor2);
  timer.setInterval(1000L, DoorSensor3);
  timer.setInterval(1000L, DoorSensor4);

  
  //timer.setInterval(100L, getSendData);
  //timer.setInterval(100L, DoorSensor1);
  //timer.setInterval(100L, DoorSensor2); // 100L au lieu de 1000L
  //timer.setInterval(100L, DoorSensor3);
  //timer.setInterval(100L, DoorSensor4);

// Ajout du timer sur les fonctions des chambres

  timer.setInterval(1000L, Chambre1);
  timer.setInterval(1000L, Chambre2);
  timer.setInterval(1000L, Chambre3);
  
 // timer.setInterval(100L, Chambre1);
 // timer.setInterval(100L, Chambre2); // 100L au lieu de 1000L
 // timer.setInterval(100L, Chambre3);
  
  
}
 
void loop() 
{

  //getSendData();

  //DoorSensor1();
  //DoorSensor2();
  //DoorSensor3();
  //DoorSensor4();

  // Chambre1();
  // Chambre2();
  // Chambre3();

  timer.run(); // Initiates SimpleTimer T°
  Blynk.run();
}
 void UpdateBlynk()
// Write device data to Blynk app via virtual pins
{

  Blynk.virtualWrite(V8, TempTarget0);//CURSEUR T°1
  Blynk.virtualWrite(V9, TempTarget1);//CURSEUR T°2
  Blynk.virtualWrite(V11, TempTarget2);//CURSEUR T°3
  Blynk.virtualWrite(V12, TempTarget3);//CURSEUR T°4

  // Ajout (Dubuc Valentin) - Ajout des curseurs pour les Timers (chambres puis portes)

  // Test d'effectuer la division /60 pour afficher la bonne valeur dans le virtualWrite
  // Puisque sinon, le curseur serait en seconde, hors, on le veut en minute
  
  Blynk.virtualWrite(V25, TimeTarget0 / 60);//CURSEUR Timer Chambre 1
  Blynk.virtualWrite(V26, TimeTarget1 / 60);//CURSEUR Timer Chambre 2
  Blynk.virtualWrite(V27, TimeTarget2/60);//CURSEUR Timer Chambre 3

  
  Blynk.virtualWrite(V28, pmints_0/60);//CURSEUR Timer Porte 1
  Blynk.virtualWrite(V29, pmints_1/60);//CURSEUR Timer Porte 2
  Blynk.virtualWrite(V30, pmints_2/60);//CURSEUR Timer Porte 3
  Blynk.virtualWrite(V31, pmints_3/60);//CURSEUR Timer Porte 4
 // Fin Ajout
 

}
/***************************************************
 * Send Sensor data to Blynk
 **************************************************/
void getSendData()
{

  DS18B20.requestTemperatures(); 
  temp_0 = DS18B20.getTempCByIndex(0); // Sensor 1 
  temp_1 = DS18B20.getTempCByIndex(1); // Sensor 2
  temp_2 = DS18B20.getTempCByIndex(2); // Sensor 3 
  temp_3 = DS18B20.getTempCByIndex(3); // Sensor 4 
   Fahrenheit = DS18B20.toFahrenheit(temp); // Fahrenheit
   
  Serial.println("");
   Serial.print("temp0:  ");
  Serial.print(temp_0);
  Serial.print("    temp1:  ");
  Serial.print(temp_1);
  Serial.print("    temp2: ");
  Serial.println(temp_2);
  Serial.println("");
  // Lecture sur les Portes

// Inutile d'afficher les minutes puisque dans la version précise, elles n'existent pas vraiment

    Serial.print("SECONDE Porte 1  ");
  Serial.print(secs_0); 
  /*
    Serial.print("  minute Porte 1  ");
  Serial.print(mints_0); */
        Serial.print("  Temps de Seuil 1  ");
  Serial.println(pmints_0);  


      Serial.print("SECONDE Porte 2  ");
  Serial.print(secs_1);
  /*
    Serial.print("  minute Porte 2  ");
  Serial.print(mints_1); */
        Serial.print("  Temps de Seuil 2  ");
  Serial.println(pmints_1);  


      Serial.print("SECONDE Porte 3  ");
  Serial.print(secs_2);
  /*
    Serial.print("  minute Porte 3  ");
  Serial.print(mints_2); */
        Serial.print("  Temps de Seuil 3  ");
  Serial.println(pmints_2);  
  
  Serial.println("");
  
      Serial.println("----- Chambre 2 et 3-------");
      
      Serial.print("SECONDE Chambre 2  ");
  Serial.print(c_secs_1);
  /*
    Serial.print("  minute Porte 2  ");
  Serial.print(c_mints_1); */
    Serial.print("  Temperature de Seuil Chambre 2  ");
  Serial.print(TempTarget1);
        Serial.print("  Temps de Seuil Chambre 2  ");
  Serial.println(TimeTarget1);
   
      Serial.print("SECONDE Chambre 3  ");
  Serial.print(c_secs_2);
  /*
    Serial.print("  minute Porte 3  ");
  Serial.print(c_mints_2); */
  
    Serial.print("  Temperature de Seuil Chambre 3  ");
  Serial.print(TempTarget2);
        Serial.print("  Temps de Seuil Chambre 3  ");
  Serial.println(TimeTarget2);  


  

  
//  Serial.println(Fahrenheit);
//  Serial.println(TempTarget0);
  
 Blynk.virtualWrite(V2, temp_0); //AFFICHAGE TEMPERATURE CHAMBRES FROIDES 1
 Blynk.virtualWrite(V3, temp_1); //AFFICHAGE TEMPERATURE CHAMBRES FROIDES 2
 Blynk.virtualWrite(V4, temp_2); //AFFICHAGE TEMPERATURE CHAMBRES FROIDES 3
 Blynk.virtualWrite(V5, temp_3); //AFFICHAGE TEMPERATURE CHAMBRES FROIDES 4
    Blynk.virtualWrite(V8, TempTarget0);
    Blynk.virtualWrite(V9, TempTarget1);
    Blynk.virtualWrite(V11, TempTarget2);
    Blynk.virtualWrite(V12, TempTarget3);
    
  // Ajout (Dubuc Valentin) - Ajout des curseurs pour les Timers (chambres puis portes)

  // Test d'effectuer la division /60 pour afficher la bonne valeur dans le virtualWrite
  // Puisque sinon, le curseur serait en seconde, hors, on le veut en minute

  Blynk.virtualWrite(25, TimeTarget0/60);//CURSEUR Timer Chambre 1
  Blynk.virtualWrite(26, TimeTarget1/60);//CURSEUR Timer Chambre 2
  Blynk.virtualWrite(27, TimeTarget2/60);//CURSEUR Timer Chambre 3

  
  Blynk.virtualWrite(28, pmints_0/60);//CURSEUR Timer Porte 1
  Blynk.virtualWrite(29, pmints_1/60);//CURSEUR Timer Porte 2
  Blynk.virtualWrite(30, pmints_2/60);//CURSEUR Timer Porte 3
  Blynk.virtualWrite(31, pmints_3/60);//CURSEUR Timer Porte 4
 // Fin Ajout
}

//  ---------- Vérification des Chambres ------------ //
    
void Chambre1(){

   Blynk.virtualWrite(V18,0);
  Blynk.virtualWrite(V19,255);
  
 // SI la température dans la chambre 1 est trop élevé :
// Modification (Dubuc Valentin) - transformation des secs_ en c_secs (timer des chambres et pas des portes)

if (temp_0 < TempTarget0) 
{
 c_secs_0 = 0;
 //c_mints_0 = 0;  
}
  
  if(temp_0 >= TempTarget0)   
 {
  Blynk.virtualWrite(V18,255);
  Blynk.virtualWrite(V19,0);
        currentMillis = millis();
   currentsecs = currentMillis / 1000; 
    if ((unsigned long)(currentsecs - previoussecs) >= interval) {
      c_secs_0 = c_secs_0 + TempsReelSeconde; // Temps réel en seconde Arduino

// SUPPRESSION DU CONCEPT DE MINUTE
    /*  
        if (c_secs_0 >= 60 ) // Modification (Dubuc Valentin) - secs_0 >= 60, car une minute correspond à 60s et pas 50s
      {
        c_secs_0 = 0; 
   c_mints_0 = c_mints_0 + 1; // Modification (Dubuc Valentin) - la variable mints_0 était mal incrémenté, en effet le code était : "mints_0 = mints_1 + 1;"  or mints_1 n'est pas la bonne variable
*/
if(c_secs_0 >= TimeTarget0 ) // Modification (Dubuc Valentin) - la variable n'est pas pmints_ mais TimeTarget
{
  c_secs_0 = 0; 
  //c_mints_0 = 0;
  Blynk.notify("Température N° 1 trop élevée  !!!");  
}
    
      previoussecs = currentsecs;
      }
   }
 }

void Chambre2() {
   
   Blynk.virtualWrite(V20,0);
  Blynk.virtualWrite(V24,255);

 // SI la température dans la chambre 2 est trop élevé :
// Modification (Dubuc Valentin) - transformation des secs_ en c_secs (timer des chambres et pas des portes)

if (temp_1 < TempTarget1) 
{
 c_secs_1 = 0;
 //c_mints_1 = 0;  
}
  
  if(temp_1 > TempTarget1)
 {
  Blynk.virtualWrite(V20,255);
  Blynk.virtualWrite(V24,0);
        currentMillis = millis();
   currentsecs = currentMillis / 1000; 
    if ((unsigned long)(currentsecs - previoussecs) >= interval) {
      
      c_secs_1 = c_secs_1 +  TempsReelSeconde; // Temps réel en seconde Arduino 
// SUPPRESSION DU CONCEPT DE MINUTE
    /*     
      if (c_secs_1 >= 60 ) // Modification (Dubuc Valentin) - secs_0 >= 60, car une minute correspond à 60s et pas 50s 
      {
        c_secs_1 = 0; 
   c_mints_1 = c_mints_1 + 1; 
   */ 
if(c_secs_1 >= TimeTarget1 ) // Modification (Dubuc Valentin) - la variable n'est pas pmints_ mais TimeTarget
{
  c_secs_1 = 0; 
  //c_mints_1 = 0;
  Blynk.notify("Température N° 2 trop élevée  !!!");  
}
 
      previoussecs = currentsecs;
      }   
   }
 }

void Chambre3() {
 
   Blynk.virtualWrite(V22,0);
  Blynk.virtualWrite(V23,255);

     // SI la température dans la chambre 1 est trop élevé :
// Modification (Dubuc Valentin) - transformation des secs_ en c_secs (timer des chambres et pas des portes)

if (temp_2 < TempTarget2) 
{
 c_secs_2 = 0;
 //c_mints_2 = 0;  
}
 
  if(temp_2 > TempTarget2) // TempTarget2 et pas TempTarget0
 {
  Blynk.virtualWrite(V22,255);
  Blynk.virtualWrite(V23,0);
        currentMillis = millis();
   currentsecs = currentMillis / 1000; 
    if ((unsigned long)(currentsecs - previoussecs) >= interval) {
      
      c_secs_2= c_secs_2 +  TempsReelSeconde; // Temps réel en seconde Arduino
// SUPPRESSION DU CONCEPT DE MINUTE
    /*     
      if (c_secs_2 >= 60 ) // Modification (Dubuc Valentin) - secs_0 >= 60, car une minute correspond à 60s et pas 50s 
      {
        c_secs_2 = 0; 
   c_mints_2 = c_mints_2 + 1; 
   */ 
if(c_secs_2 >= TimeTarget2 )
{
  c_secs_2 = 0; 
  //c_mints_2 = 0;
  Blynk.notify("Température N° 3 trop élevée  !!!");  
}
 
      previoussecs = currentsecs;
      }   
   }
 }



//  ---------- Vérification des Portes ------------ //


void DoorSensor1()
{
 
 if(digitalRead(Dsensor1) == LOW)
 {
  Blynk.virtualWrite(V6,0);
  Blynk.virtualWrite(V7,255);
 
   secs_0 = 0; 
   //mints_0 = 0; 
 }
 
  if(digitalRead(Dsensor1) == HIGH)
 {
  Blynk.virtualWrite(V6,255);
  Blynk.virtualWrite(V7,0);
        currentMillis = millis();
   currentsecs = currentMillis / 1000; 
    if ((unsigned long)(currentsecs - previoussecs) >= interval) {
      secs_0 = secs_0 +  TempsReelSeconde; // Temps réel en seconde Arduino
// SUPPRESSION DU CONCEPT DE MINUTE
    /*     
      if (secs_0 >= 60 ) // Modification (Dubuc Valentin) - secs_0 >= 60, car une minute correspond à 60s et pas 0s  
      {
        secs_0 = 0; 
   mints_0 = mints_0 + 1;  
   */
if(secs_0 >= pmints_0 )
{
  secs_0 = 0; 
  //mints_0 = 0;
  Blynk.notify("Porte N°1 ouverte depuis trop longtemps !!!");  
}
 
      previoussecs = currentsecs;
      }   
   }
 
 Blynk.virtualWrite(V10, "Seconds");
Blynk.virtualWrite(V10, secs_0);  
Blynk.virtualWrite(V10,"*********");
 
}

void DoorSensor2()
{
 
 if(digitalRead(Dsensor2) == LOW)
 {
  Blynk.virtualWrite(V13,0);
  Blynk.virtualWrite(V14,255);
 
   secs_1 = 0; 
   //mints_1 = 0; 
 }
 
  if(digitalRead(Dsensor2) == HIGH)
 {
  Blynk.virtualWrite(V13,255);
  Blynk.virtualWrite(V14,0);
        currentMillis = millis();
   currentsecs = currentMillis / 1000; 
    if ((unsigned long)(currentsecs - previoussecs) >= interval) {
      secs_1 = secs_1 +  TempsReelSeconde; // Temps réel en seconde Arduino 
// SUPPRESSION DU CONCEPT DE MINUTE
    /*     
      if (secs_1 >= 60 ) // Modification (Dubuc Valentin) - secs_0 >= 60, car une minute correspond à 60s et pas 0s  
      {
        secs_1 = 0; 
   mints_1 = mints_1 + 1; 
   */ 
if(secs_1 >= pmints_1 )
{
  secs_1 = 0; 
  //mints_1 = 0;
  Blynk.notify("Porte N°2 ouverte depuis trop longtemps !!!");  
}
 
      previoussecs = currentsecs;
      }   
   }
 
 Blynk.virtualWrite(V15, "Seconds");
Blynk.virtualWrite(V15, secs_1);  
Blynk.virtualWrite(V15,"*********");
 
}
void DoorSensor3()
{
 
 if(digitalRead(Dsensor3) == LOW)
 {
  Blynk.virtualWrite(V16,0);
  Blynk.virtualWrite(V17,255);
 
   secs_2 = 0; 
   //mints_2 = 0; 
 }
 
  if(digitalRead(Dsensor3) == HIGH)
 {
  Blynk.virtualWrite(V16,255);
  Blynk.virtualWrite(V17,0);
        currentMillis = millis();
   currentsecs = currentMillis / 1000; 
    if ((unsigned long)(currentsecs - previoussecs) >= interval) {
      secs_2 = secs_2 + TempsReelSeconde; // Temps réel en seconde Arduino 
// SUPPRESSION DU CONCEPT DE MINUTE
    /*     
      if (secs_2 >= 60 ) // Modification (Dubuc Valentin) - secs_0 >= 60, car une minute correspond à 60s et pas 0s  
      {
        secs_2 = 0; 
   mints_2 = mints_2 + 1;  
   */
if(secs_2 >= pmints_2 )
{
  secs_2 = 0; 
  //mints_2 = 0;
  Blynk.notify("Porte N°3 ouverte depuis trop longtemps !!!");  
}
 
      previoussecs = currentsecs;
      }   
   }
 
 Blynk.virtualWrite(V18, "Seconds");
Blynk.virtualWrite(V18, secs_2);   
Blynk.virtualWrite(V18,"*********");
 
}
