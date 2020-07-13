//https://cuneyt.aliustaoglu.biz/en/managing-page-loop-for-u8glib-using-arduino-uno-oled-128x64-i2c/
// Anémomètre https://www.youtube.com/watch?v=Fk8GPtifyJ0
#include <SoftwareSerial.h>
#include <Wire.h>
#include "SPI.h"
#include <OneWire.h>
#include <DallasTemperature.h>
#include "Adafruit_Sensor.h"
#include "Adafruit_BMP280.h"
#include <Adafruit_BME280.h>
#include "U8glib.h"  //Download Library
U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NONE|U8G_I2C_OPT_DEV_0);
int p;
float b;
int buf[10],temp1;

const int trigPin = 8;
const int echoPin = 9;
#define ledalarm 32           // LED ALARM 
#define resetmaxpin 3            // remise a zero max Pin (button)
#define resetalarmpin 4          // remise a zero Alarm Pin (button)
#define SensorPin 2         // the pH meter Analog output is connected with the Arduino’s Analog
unsigned long int avgValue;  //Store the average value of the sensor feedback
long duration, distance; // Durée utilisée pour calculer la distance

// Pin capteur effet hall
#define REEDPIN 2// PIN D2 Pin capteur effet hall
// Interruptions matérielles pour le Reed-Pin

#define REEDINTERRUPT 0 //PIN A0
// Umfang in mm wurde im Windkanal mit dem Volkswindmesser ermittelt als Referenz
#define RADUMFANG 2200


const int buttonPin = 12;     // the number of the pushbutton pin
int EtatBoutton = 0;         // variable for reading the pushbutton status
int page = 0;

SoftwareSerial nodemcu(18,19);//RX D19 et TX D18
#include <Wire.h>

 #include "DHT.h"   // Librairie des capteurs DHT

#define DHTPIN 5    //  pin sur lequel est branché le DHT

// Dé-commentez la ligne qui correspond à votre capteur 
#define DHTTYPE DHT11     // DHT 11 
//#define DHTTYPE DHT22       // DHT 22  (AM2302)
//#define DHTTYPE DHT21     // DHT 21 (AM2301)




DHT dht(DHTPIN, DHTTYPE); 

#define ONE_WIRE_BUS 7
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature DS18B20(&oneWire);

//int pwm_back_light; 
int pwm_led1;  
long int data; /*
int relay1 = 13; 
int relay2 = 12; */
int gaz; 
float temp;
float Fahrenheit=0;
float tempf = 0; // temperature flag
int gaz_ppm = 0; 
float sdata1 = 0; // temperature data
long int sdata2 = 0; // pressure data
long int sdata3 = 0; // altitude data
 
String cdata; // complete data
 Adafruit_BMP280 bmp; // I2C //BMP 280 SCL SUR SCL et SDA SUR SDA
char str[10];

void drawTest(void) {
  u8g.setFont(u8g_font_unifont);
  u8g.drawStr( 0, 20, "  GESTION MAISON");

  
}
void setup()
{
   u8g.setFont(u8g_font_unifont);
  u8g.setColorIndex(1);
  p = 0;
Serial.begin(115200); 
  Serial.println("DHT11 test!");
  dht.begin();
nodemcu.begin(9600);

  // initialize the pushbutton pin as an input:
  pinMode(buttonPin, INPUT_PULLUP);
 

 
  pinMode(REEDPIN, INPUT_PULLUP); // Reedkontakt direkt und ohne Widerstand angeschlossen 
  attachInterrupt(REEDINTERRUPT, reedISR, FALLING);
 pinMode(resetmaxpin, INPUT_PULLUP); // Resetmax Taster direkt und ohne Widerstand angeschlossen (gegen ground)
pinMode(resetalarmpin, INPUT_PULLUP); // Resetalarm Taster direkt und ohne Widerstand angeschlossen (gegen ground) 

pinMode(trigPin, OUTPUT);
pinMode(echoPin, INPUT);

//pinMode(BUILTIN_LED, OUTPUT);
pinMode(32, OUTPUT);
pinMode(6, OUTPUT);

DS18B20.begin();
if (!bmp.begin(0x76)) {
   Serial.println(F("Could not find a valid BMP280 sensor, check wiring!"));
    while (1) {}
  }
 
}

float ms2; //globale Variable Speed in m/s
float msmax = 0;  //Maximalwert
unsigned long umin; // U/min

int alarm; // Limite d’alarme en m/s 


volatile byte reedCountSum;
volatile long reedMillisSum;

unsigned long lastReedMillis;

void reedISR()
{
  if (millis()-lastReedMillis>=5)   // 25 ms correspond au maximum à 40 tours par seconde (pour les anémomètres populaires, le temps de rebondissement est suffisant)
  {
    reedCountSum++;                 // compter un tour de roue
    reedMillisSum+=millis()-lastReedMillis;   // Zeit addieren
    lastReedMillis=millis();       // Zeit merken
  }
}

unsigned long gesamtUmdrehungen = 0; //4545300;

void tachoAnzeige()
{
  byte umdrehungen;
  unsigned long zeit;
  float kph, kilometer, ms;
  char buffer[10];
  noInterrupts();            // Interrupts sperren
    umdrehungen=reedCountSum;// Zählvariable umkopieren
    reedCountSum=0;          // Zählvariable auf 0 zurücksetzen
    zeit=reedMillisSum;      // Zeitzähler umkopieren
    reedMillisSum=0;         // Zeitzähler auf 0 zurücksetzen
  interrupts();              // Interrupts wieder zulassen
    gesamtUmdrehungen+= umdrehungen; // Aufsummieren aller Radumdrehungen
    kilometer=(float)gesamtUmdrehungen*(float)RADUMFANG/1000000.0; // Kilometerzähler 

  if (umdrehungen>0){
    kph=float(RADUMFANG)*(float)umdrehungen/(float)zeit*3.6;  
    ms=float(RADUMFANG)*(float)umdrehungen/(float)zeit;  
    umin=umdrehungen*60000/zeit;  }
  else {
    kph=0.0;      //Null setzen wenn keine Umdrehung gefunden wurde
    umin = 0; }   //Null setzen wenn keine Umdrehung gefunden wurde
    
    
    // ms = kph/3.6; //aus km/h mach m/s - kmh/3.6
    ms2 = kph; //variable ms in ms2 global kopieren

    sprintf(buffer,"%4i T/min ",umin);    // mit Nullen - sprintf(buffer,"%04i U/min ",umin); 4-Stellen Lagerichtig :-).

    dtostrf(msmax,5,2,buffer);            // Insgesamt 5 Stellen 12.45 (Dezimalpunkt ist auch eine Stelle)

    dtostrf(kph,5,1,buffer);
   
    dtostrf(ms,5,2,buffer);

    //maximal mémorisées
    if (ms2 > msmax) {
    msmax = ms2;   }
   // Serial.println(msmax);

    //umin 0 setzen wenn ms2 0 ist.
   // if (ms2 == 0.0) {
   // umin = 0;  }

   //Serial.println(gesamtUmdrehungen);
  

  
  }

unsigned long interval;           // update LCD/LED interval (milliseconds)
unsigned long previousMillis = 0; 
unsigned long intervalalarm = 200;           // update Alarm Anzeige interval (milliseconds)
unsigned long previousMillisalarm = 0; 

unsigned long currentMillis;

void displayWeather(){ //Création d’une fonction pour lire et afficher la température et l’humidité sur l’écran OLED
  // Read humidité as %
  float sdata1 = dht.readHumidity();
  // Read temperature as Celsius
  float sdata2 = dht.readTemperature();
  // Read temperature as Celsius
  float sdata3 = DS18B20.getTempCByIndex(0);
  // Read temperature as Fahrenheit
  //float f = dht.readTemperature(true);
  int gaz_ppm = analogRead(A2);
  
  // Vérifier si des lectures ont échoué et  réessayer.
  if (isnan(sdata1) || isnan(sdata2) ){
    //display.clear(); // clearing the display
    u8g.drawStr(5,0, "PROBLEME DHT");

    return;
  }
      if ( isnan(sdata3)){
   
    u8g.drawStr(0, 15, "PROBLEME DS");
   
    return;
      }
         if ( isnan(gaz_ppm)){
   
    u8g.drawStr(0, 15, "PROBLEME GAZ");
    
    return;
         }

}
int potpin = 0;  // A0 broche analogique utilisée pour connecter le potentiomètre réglage alarme vent
int val;    // variable pour lire la valeur à partir de la broche analogique
 
void loop()
{

   long duration, distance;
  digitalWrite(trigPin, LOW);  
  delayMicroseconds(2); 

  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10); 
  
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = (duration/2) / 29.1;
  Serial.print(distance);
  Serial.println("   Centimetres:");
 //Adapter l’entrée d’évaluation en fonction du régime
  if (ms2 < 1) {
    interval = 4000;  }
  if (ms2 >= 1 && ms2 < 2) {
    interval = 2100;  }  
  if (ms2 >= 2 && ms2 < 3) {
    interval = 1100;  } 
  if (ms2 >= 3 && ms2 < 4) {
    interval = 800;  } 
  if (ms2 >= 4 && ms2 < 6 ) {
    interval = 600;  }   
  if (ms2 > 6) {
    interval = 400;  }  

  
  // Anzeige wird nach dem eingestellten inteval aktualisiert
  currentMillis = millis();
  if (currentMillis - previousMillis >= interval)
  {
    tachoAnzeige();
    previousMillis = currentMillis;
  }
    
  // Mise à jour de l’affichage d’alarame après l’interval défini
  if (currentMillis - previousMillisalarm >= intervalalarm)
  {
    val = analogRead(potpin);            // reads the value of the potentiometer (value between 0 and 1023)
    alarm = map(val, 0, 1023, 0, 99);     // LE CHIFFRE SUR LCD CORRESPOND A LA VITESSE EN KMH
  Serial.println("Réglage alarme vent");
  Serial.println( val);
// lcd.setCursor(18, 1);
//    if(alarm < 10) lcd.print("0"); // un espace ou aussi un nombre zéro f(zahl <10) print(" "); // un espace ou aussi un nombre zéro
//    lcd.print(alarm);
   
     //Allumer l’alarme LED lorsque le vent > la valeur d’alarme
    if (ms2 > alarm) {
    digitalWrite(ledalarm, HIGH);
   // lcd.setCursor(0, 1);
  //  lcd.print("*** A L A R M ***");    }
Serial.println("*** A L A R M ***");
   previousMillisalarm = currentMillis;
  }
    // Réinitialiser l’alarme LED si le vent est > 5 ms ou si le bouton de réinitialisation est enfoncé
    // if (ms2 < 2 && digitalRead(ledalarm) == HIGH || digitalRead(resetmaxpin) == LOW)
    if (digitalRead(resetalarmpin) == LOW) {
    digitalWrite(ledalarm, LOW);

    }  
    
    //Suppression des 5 premières rotations ou réinitialisation du bouton pour rétablir la valeur maximale
    if (gesamtUmdrehungen < 10 || digitalRead(resetmaxpin) == LOW ) {  
    msmax = 0;  }
        
    //Réinitialiser les rotations globales avant que le présentoir ne déborde
    if (gesamtUmdrehungen > 4545430) {
    gesamtUmdrehungen = 0;  }
 
  for(int i=0;i<10;i++)       //Get 10 sample value from the sensor for smooth the value
  { 
    buf[i]=analogRead(SensorPin);
    delay(10);
  }
  for(int i=0;i<9;i++)        //sort the analog from small to large
  {
    for(int j=i+1;j<10;j++)
    {
      if(buf[i]>buf[j])
      {
        temp=buf[i];
        buf[i]=buf[j];
        buf[j]=temp;
      }
    }
  }
  avgValue=0;
  for(int i=2;i<8;i++)                      //take the average value of 6 center sample
    avgValue+=buf[i];
  float phValue=(float)avgValue*5.0/1024/6; //convert the analog into millivolt
  phValue=2.0*phValue;                      //convert the millivolt into pH value
  Serial.print("    pH:");  
  Serial.print(phValue,2);
  Serial.println(" ");
  
digitalWrite(6, 1); 
Serial.println("test gaz");

  gaz_ppm=analogRead(A1);
  
  Serial.println(gaz_ppm);
   
  if(gaz_ppm>300)
  {
     digitalWrite(4,LOW);
    digitalWrite(5,HIGH);   
  }
  
  if(gaz_ppm<250)
  {
      digitalWrite(4,HIGH);
      digitalWrite(5,LOW);
  }

  DS18B20.requestTemperatures(); 
   float temp = DS18B20.getTempCByIndex(0);

      Serial.print("Temperature DS18B20 = ");
    Serial.print(temp);
    sdata1 = temp;
    Serial.println(" *C");
    
Serial.println(nodemcu.available());
  
if(nodemcu.available() == 0 )
{
    
    Serial.print("Pressure = ");
    Serial.print(bmp.readPressure());
    sdata2 =( bmp.readPressure()/100)+10;// pour ajuster la pression
    Serial.println(" Pa");
    Serial.print("ma_variable");
    Serial.println("hPa");
    
    // Calculate altitude assuming 'standard' barometric
    // pressure of 1013.25 millibar = 101325 Pascal
    Serial.print("Altitude = ");
    Serial.print(bmp.readAltitude());
    int ma_variable_2 = bmp.readAltitude();
    sdata3 = (bmp.readAltitude())+90;// Pour ajuster l'altitude
    Serial.println(" metres");  
    Serial.println(sdata3);

  
  float sdata4 = dht.readHumidity();
  float sdata5 = dht.readTemperature();

   cdata = cdata + sdata1 + "," + sdata2 + "," + sdata3 + "," + gaz_ppm + "," + sdata4 + "," 
     + sdata5 + "," + ms2+ "," + msmax + "," + distance + "," + phValue + "," + alarm ; 


Serial.println("On affiche cdata: ");
   Serial.println(cdata); 
   nodemcu.println(cdata);
   delay(1000); // 100 milli seconds
   cdata = ""; 
}

if ( nodemcu.available() > 0 ) 
{
  data = nodemcu.parseInt();
  delay(100); 
  Serial.println(data); 

}
// Délai de 2 secondes entre chaque mesure. La lecture prend 250 millisecondes
  delay(2000);

  // Lecture du taux d'humidité
  float h = dht.readHumidity();
  // Lecture de la température en Celcius
  float t = dht.readTemperature();
  // Pour lire la température en Fahrenheit
  float f = dht.readTemperature(true);
  
  // Stop le programme et renvoie un message d'erreur si le capteur ne renvoie aucune mesure
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println("Echec de lecture !");
    return;
  }

  // Calcul la température ressentie. Il calcul est effectué à partir de la température en Fahrenheit
  // On fait la conversion en Celcius dans la foulée
  float hi = dht.computeHeatIndex(t, h);
  

  Serial.print("Humidite: "); 
  Serial.print(h);
  Serial.print(" *C");
  Serial.print("Temperature: "); 
  Serial.print(t);
  Serial.print(" *C ");
  Serial.print("Temperature ressentie: ");
  Serial.print(dht.convertFtoC(hi));
  Serial.println(" *C");
  // picture loop



  // read the state of the pushbutton value:
  EtatBoutton = digitalRead(buttonPin);

  
  if ( EtatBoutton == LOW)// low ou high pour faire défiler le oled ou pas
   { 
    page = page +1; // 0, 1, 2 et 3
    if (page == 4)
    {
      page = 0; 
    }
    //delay(1);
   }

 if (page == 0)
 {    
    u8g.firstPage();  
    do {
        //Taille police
        //u8g.setFont(u8g_font_5x8);
       // u8g.setFont(u8g_font_6x10);
          u8g.setFont(u8g_font_helvB08);
        
        u8g.drawStr( 0, 10, "T Piscine:");//chiffre N°1 déplacement du titre vertical chiffre N°2 déplacement du titre horizontal 
        u8g.drawStr( 70, 10, dtostrf(sdata1, 5, 2, str));//chiffre N°1 déplacement de la valeur vertical chiffre N°2 déplacement de la valeurhorizontal 
        u8g.drawStr( 105, 10, "*C");//chiffre N°1 déplacement du "C" chiffre N°2 déplacement du "C" horizontal 
        
        u8g.drawStr( 0, 25, "T Exterieur:");
        u8g.drawStr( 70, 25, dtostrf(t, 5, 2, str));
        u8g.drawStr( 105,25, "*C");

        u8g.drawStr( 0, 40, "Humidite:");
        u8g.drawStr( 70, 40, dtostrf(h, 5, 2, str));
        u8g.drawStr( 105,40, "%");

        u8g.drawStr( 0, 55, "T ressentie");
        u8g.drawStr( 70, 55, dtostrf(hi, 5, 2, str));
        u8g.drawStr( 105, 55, "*C");
        
        } while( u8g.nextPage() );
      delay(100);// Temps d'affichage de l'écran
 }

 if (page == 1)
 {
   u8g.firstPage();  
    do {

      u8g.drawStr( 0, 10, "Anemometre pergola");
      
      u8g.drawStr( 0, 30, "Vent Reel");
      u8g.drawStr( 70, 30, dtostrf(ms2, 5, 2, str));
      u8g.drawStr( 105, 30, "km/h");
        
      u8g.drawStr( 0, 45, " Vent Max");
      u8g.drawStr( 70, 45, dtostrf(msmax, 5, 2, str));
      u8g.drawStr( 105, 45, "km/h");

      u8g.drawStr( 0, 60, "Reg Alarme");
      u8g.drawStr( 70, 60, dtostrf(alarm, 5, 2, str));
      u8g.drawStr( 105, 60, "km/h");
        
      } while( u8g.nextPage() );
      delay(100);// Temps d'affichage de l'écran
 }  

 if (page == 2)
 {
   u8g.firstPage();  
    do {
        u8g.drawStr( 0, 10, "Centimetres");
        u8g.drawStr( 70, 10, dtostrf(distance, 5, 2, str));
        u8g.drawStr( 105, 10, "cm");
       
        u8g.drawStr( 0, 25, "Gaz");
        u8g.drawStr( 70,25, dtostrf(gaz_ppm, 5, 2, str));
        u8g.drawStr( 105, 25, "%");
        
        u8g.drawStr( 0, 42, "Pression");
        u8g.drawStr( 65,42, dtostrf(sdata2, 5, 2, str));
        u8g.drawStr( 105,42, "hpa");

        
        u8g.drawStr( 0,60, "Altitude");
        u8g.drawStr( 70,60, dtostrf(sdata3, 5, 2, str));
        u8g.drawStr( 105,60, "m");
      
      
      } while( u8g.nextPage() );
      delay(100);// Temps d'affichage de l'écran
 }

   
      
      if (page == 3)
     {
        u8g.firstPage();  
        do {
        u8g.drawStr( 0, 10, "Valeur Ph");
        u8g.drawStr( 70, 10, dtostrf(phValue, 5, 2, str));
        u8g.drawStr( 105, 10, "");
           
         
          } while( u8g.nextPage() );
          delay(100);// Temps d'affichage de l'écran
     }
  }
}
 
