//Fonctionne aussi sur ESP01
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <SoftwareSerial.h>
#include <SimpleTimer.h>
 
int pinValue1;
int pinValue2;
 
char auth[] = "VSFNI564xH3YAOSXGErJ6GbBmHHgkNl0";

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "ZEOP-c5c9d6_EXT2.4G";
char pass[] = "ZTEGC8538fa3";
 
SimpleTimer timer;
 
String myString; // complete message from arduino, which consistors of snesors data
char rdata; // received charactors
 
//for temperature , pressure and altitude
float firstVal, secondVal,thirdVal, fourVal, fiveVal, 
sixVal,sevenVal,eightVal,nineVal,tenVal, elevenVal ; // 
// This function sends Arduino's up time every second to Virtual Pin (1).
// In the app, Widget's reading frequency should be set to PUSH. This means
// that you define how often to send data to Blynk App.
void myTimerEvent()
{
  // You can send any value at any time.
  // Please don't send more that 10 values per second.
  Blynk.virtualWrite(V1, millis() / 1000);
  
}
 

long duration, distance; // Duration used to calculate distance
 
void setup()
{
  // Debug console
  Serial.begin(9600);
 
  Blynk.begin(auth, ssid, pass);
 
     timer.setInterval(2000L,sensorvalue1); 
     timer.setInterval(2000L,sensorvalue2); 
     timer.setInterval(2000L,sensorvalue3);
     timer.setInterval(2000L,sensorvalue4);
     timer.setInterval(2000L,sensorvalue5);
     timer.setInterval(2000L,sensorvalue6);
     timer.setInterval(2000L,sensorvalue7);
     timer.setInterval(2000L,sensorvalue8);
     timer.setInterval(2000L,sensorvalue9);
     timer.setInterval(2000L,sensorvalue10);
      timer.setInterval(2000L,sensorvalue11);
     timer.setInterval(2000L,getSendData);
 
}
 
void loop()
{
   if (Serial.available() == 0 ) 
   {
  Blynk.run();
  timer.run(); // Initiates BlynkTimer
   }
   
  if (Serial.available() > 0 ) 
  {
    rdata = Serial.read(); 
    myString = myString+ rdata; 
   // Serial.print(rdata);
    if( rdata == '\n')
    {
   //  Serial.println(myString); 
  // Serial.println("fahad");
// new code
String TempDS18B20  = getValue(myString, ',', 0);
String Pressure = getValue(myString, ',', 1);
String Altitude = getValue(myString, ',', 2);
String Gaz = getValue(myString, ',', 3); 
String TempDHT = getValue(myString, ',', 4);
String Humid = getValue(myString, ',', 5); 
String Vitesseventreel = getValue(myString, ',', 6);   
String Vitesseventmemo = getValue(myString, ',', 7);   
String Niveau= getValue(myString, ',', 8);  
String ValPH = getValue(myString, ',', 9);     
String Alarm = getValue(myString, ',', 10);    
 
firstVal = TempDS18B20.toFloat();
//maValeurAMoi =  TempDS18B20.toFloat();
secondVal = Pressure.toFloat();
thirdVal = Altitude.toInt();
fourVal = Gaz.toInt();
fiveVal = TempDHT.toFloat();
sixVal = Humid.toInt();
sevenVal = Vitesseventreel.toInt();
eightVal = Vitesseventmemo.toInt();
nineVal = Niveau.toInt();
tenVal = ValPH.toFloat();
elevenVal = Alarm.toInt();

/* dht.valeur = 14,3, String DHTVAL = position4,  valeur = DHTVAL.toFloat(), float sdata5 = valeur
 Serialprint(sdata5) --> 14,00

 RAPPEL : cdata vaut sdata1 + "," + sdata2 + ...
 cdata est envoyé par le software serial à cette car-là
 cdata est une variable qui contient une série de valeur séparé par des virgules
 Si on crée une variable qui vaut une variable chiffré (int) + du texte (string)
 int + string = string
 ex : cdata = 14, 25, 13
 cdata est bien un String

 Explication de la fonction getValue :
 cette fonction va lire un string, chaque élément est séparé par une virgule
 Ensuite, chaque élément du coup à une position qui lui est popre, un index

 */
  myString = "";
// end new code
    }
  }
 
}
 
void sensorvalue1()
{
float sdata = firstVal;
  // You can send any value at any time.
  // Please don't send more that 10 values per second.
  Blynk.virtualWrite(V2, sdata);//T° DS
 
}
void sensorvalue2()
{
float sdata = secondVal;
  // You can send any value at any time.
  // Please don't send more that 10 values per second.
  Blynk.virtualWrite(V3, sdata);//PRESSION ATMO
 
}
 
void sensorvalue3()
{
int sdata = thirdVal;
  // You can send any value at any time.
  // Please don't send more that 10 values per second.
  Blynk.virtualWrite(V8, sdata);//ALTITUDE
 
}

void sensorvalue4()
{
int sdata = fourVal;
  // You can send any value at any time.
  // Please don't send more that 10 values per second.
  Blynk.virtualWrite(V9, sdata);//GAZ
  if(fourVal>= 320)
  {
  Blynk.virtualWrite(V1, 255);
  }
  if(fourVal< 300)
  {
  Blynk.virtualWrite(V1, 0);
  }
}

void sensorvalue5()
{
int sdata = fiveVal;
  // You can send any value at any time.
  // Please don't send more that 10 values per second.
  Blynk.virtualWrite(V12, sdata);//HUMIDITE DHT
 
}

void sensorvalue6()
{
int sdata = sixVal;
  // You can send any value at any time.
  // Please don't send more that 10 values per second.
  Blynk.virtualWrite(V13, sdata);//T° DHT
 
}
void sensorvalue7()
{
int sdata = sevenVal;
  // You can send any value at any time.
  // Please don't send more that 10 values per second.
  Blynk.virtualWrite(V14, sdata);//VITESSE VENT REEL km/h
  if(sevenVal>= 60)
  {
  Blynk.virtualWrite(V23, 255);
  }
  if(sevenVal<= 50)
  {
  Blynk.virtualWrite(V23, 0);
  }
} 
void sensorvalue8()
{
int sdata = eightVal;
  // You can send any value at any time.
  // Please don't send more that 10 values per second.
  Blynk.virtualWrite(V15, sdata);//VITESSE VENT MEMO km/h
   if(eightVal>= 60)
  {
  Blynk.virtualWrite(V26, 255);
  }
  if(eightVal<= 50)
  {
  Blynk.virtualWrite(V26, 0);
  }
}
void sensorvalue9()
{
int sdata = nineVal;
  // You can send any value at any time.
  // Please don't send more that 10 values per second.
  Blynk.virtualWrite(V16, sdata);//NIVEAU EAU PISCINE cm
  Serial.print(nineVal);
 Serial.println("NIVEAU EAU PISCINE");
 
}
void sensorvalue10()
{
int sdata = tenVal;
  // You can send any value at any time.
  // Please don't send more that 10 values per second.
  Blynk.virtualWrite(V24, sdata);//Valeur PH
    Serial.print(tenVal);
 Serial.println("sensorvaluePH");
}
void sensorvalue11()
{
int sdata = elevenVal;
  // You can send any value at any time.
  // Please don't send more that 10 values per second.
  Blynk.virtualWrite(V4, sdata);//Alarm
}
String getValue(String data, char separator, int index)
{
    int found = 0;
    int strIndex[] = { 0, -1 };
    int maxIndex = data.length() - 1;
 
    for (int i = 0; i <= maxIndex && found <= index; i++) {
        if (data.charAt(i) == separator || i == maxIndex) {
            found++;
            strIndex[0] = strIndex[1] + 1;
            strIndex[1] = (i == maxIndex) ? i+1 : i;
        }
    }
    return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}


 
// dans l’application Blynk écrit des valeurs sur la broche virtuelle 10
BLYNK_WRITE(V10)
{
   pinValue1 = param.asInt(); // assigning incoming value from pin V10 to a variable
 
  Serial.print(pinValue1);
 
}
 
 
// in Blynk app writes values to the Virtual Pin 11
BLYNK_WRITE(V11)
{
   pinValue2 = param.asInt(); // assigning incoming value from pin V10 to a variable
 
  Serial.print(pinValue2);
 
}
/***************************************************
 * Send Sensor data to Blynk
 **************************************************/
void getSendData()
{
 /*
  Ptemp = map(Ptemp, 0, 1023, -40,40); 
  DS18B20.requestTemperatures(); 
  temp = DS18B20.getTempCByIndex(0); // Celcius
  Fahrenheit = DS18B20.toFahrenheit(temp); // Fahrenheit
  Blynk.virtualWrite(V7,Ptemp);
  Blynk.virtualWrite(V8, temp); //virtual pin V8
 
  long duration, distance;
  digitalWrite(trigPin, LOW);  
  delayMicroseconds(2); 
  
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10); 
  
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = (duration/2) / 29.1;
 */
   if (distance <=5) {
    Blynk.virtualWrite(V10,0 );
}
  else {
    Blynk.virtualWrite(V10, 255);
  }

 if (distance <= 10) {
    Blynk.virtualWrite(V11, 0);
}
  else {
    Blynk.virtualWrite(V11, 255);
  }

   if (distance <= 15) {
    Blynk.virtualWrite(V16, 0);
}
  else {
    Blynk.virtualWrite(V16, 255);
  }

   if (distance <= 20) {
    Blynk.virtualWrite(V18, 0);
}
  else {
    Blynk.virtualWrite(V18, 255);
  }

   if (distance <= 15) {
    Blynk.virtualWrite(V17, 0);
}
  else {
    Blynk.virtualWrite(V17, 255);
  }

  
  /*
  Serial.print(distance);
  Serial.println("Centimeter:");
  Blynk.virtualWrite(V5, distance);
  delay(200);
  Blynk.run();*/

 Serial.print(distance);
 //Serial.println("Centimetres:   ");
  Blynk.virtualWrite(V16, nineVal);
 delay(10000);
  Blynk.run();
}
