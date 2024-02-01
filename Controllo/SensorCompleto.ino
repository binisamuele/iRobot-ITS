#include <Wire.h>
#include <DHT11.h>
#include <LiquidCrystal.h>


//costanti globali (più efficente con define)
#define SENSORS_NOMBER 4;
#define MAX_DISTANCE 700;
#define SPEED_OF_SOUND 0.0343;


//array di support per invio stringhe al seriale
char buffer[40];

//constanti gestione millis
const int fiveMinutes = 300000;
const int tenMinutes = 600000;

//!!da definire il funzionamento!!
DHT11 dht11(2);


//configurazione pin sensori distanza (vanno messi su pin in maniera crescente per esigenze del codice 
//                                      altrimenti sostituire i for del programma)
const int trigPinUp = 2;
const int echoPinUp = 3; 

const int trigPinDown = 4;
const int echoPinDown = 5; 

const int trigPinRh = 6;
const int echoPinRh = 7; 

const int trigPinLh = 8;
const int echoPinLh = 9; 




//pin sensori voltimetri
const int voltmeter1Pin = A0;
const int voltmeter2Pin = A1;


//constanti gestione millis
const int fiveMinutes = 300000;
const int tenMinutes = 600000;


// pin per sensore DHT 
//const int DHTPIN = 3;
//DHT dht(DHTPIN, DHTTYPE);
DHT11 dht11(3);


LiquidCrystal lcd(12, 11, 6, 5, 8, 7);

void emergencyManagement(){
  ///////////////////////////////loop di attesa //////////////////////////////////////////////////////
  int emergency; 
  int distanceTolerance=35;

  while(emergency<SENSORS_NOMBER){
    int trigPin=trigPinUp;
    int echoPin=echoPinUp;
    int incrementToNextPin = 2;
    
    emergency=0;
    for(int i=0;i<SENSORS_NOMBER;i++){
      
      if( measureDistance( trigPin, echoPin, distanceTolerance) ){
        emergency++;
      } else{
        continue;
      }
      trigPin += incrementToNextPin;
      echoPin +=incrementToNextPin;
    }
    delay(500);
  }
}

/* Verisione di Davide
int distanceManagement(){
  int trigPin=trigPinUp;
  int echoPin=echoPinUp;
  int distance;
  int reset=0;
  int incrementToNextPin = 2;
  int distanceTolerance=30;

  for(int i=0;i<SENSORS_NOMBER;i++){
    distance=measureDistance( trigPin, echoPin, distanceTolerance);
    if( distance ){
      sprintf(buffer, "distanzaUltraSuoni: %d cm", distance);
      Serial.println(buffer);
    } else{
      Serial.print("emergenza:");
      Serial.print(distance); //poi va messo a 1 per mandarlo all' altro arduino
      emergencyManagement();
      //i=reset;
    }
    trigPin += incrementToNextPin;
    echoPin +=incrementToNextPin;

  }

} */

void distanceManagement(){

}


int  measureDistance(int trigPin, int echoPin, int distanceTolerance) {

  int distance;

  NewPing sonar(trigPin, echoPin, MAX_DISTANCE);
  
  //distance = sonar.ping_cm();
  distance = (sonar.ping() / 2) * ;

  if (distance >= 0) {
    if (distance < distanceTolerance) {
      return error;
    } else {
        return distance;
    }
  } else {
      return -1;
  }

}

//funzione gestione Temperatura
int measureTemperature() {
  return dht11.readTemperature();
}
String printTemperature() {
  char buffer[40];
  sprintf(buffer, "Temperatura: %d °C \n", measureTemperature());
  return buffer;
}

//funzione gestione Umidità
int measureHumidity() {
  return dht11.readHumidity();
}
String printHumidity() {
  char buffer[40];
  sprintf(buffer, "Umidità: %d %% \n", measureHumidity());
  return buffer;
}

void measureVoltmeters() {
    // Misura tensione da voltmeter1Pin e voltmeter2Pin
    float voltage1 = analogRead(voltmeter1Pin) * (5.0 / 1023.0);
    //float voltage2 = analogRead(voltmeter2Pin) * (5.0 / 1023.0);

    // Stampa tensioni sulla porta seriale
    Serial.print("V1: ");
    Serial.print(voltage1, 2);
    Serial.print("V");
    //Serial.print(voltage2, 2);
    //Serial.println("V");

}

/* WIP da sistemare
void updateLCD() {
  lcd.setCursor(0, 1);
  lcd.print("distanzaUltraSuoni:");
  lcd.print(distanceCm);
  lcd.print(printTemperature());
  lcd.print(printHumidity());
}
*/

void setup() {
  Serial.begin(9600);       // Inizializza la comunicazione seriale a 9600 bps
  

  lcd.begin(16, 2);         // Inizializza il display LCD

  pinMode(trigPinUp, OUTPUT);
  pinMode(echoPinUp, INPUT);
  pinMode(trigPinDown, OUTPUT);
  pinMode(echoPinDown, INPUT);

  pinMode(trigPinRh, OUTPUT);
  pinMode(echoPinRh, INPUT);
  pinMode(trigPinLh, OUTPUT);
  pinMode(echoPinLh, INPUT);

}

void loop() {

  distanceManagement();
  
  
  // funzioni da eseguire ogni 5 minuti
  if (millis() % fiveMinutes == 0) {
    measureTemperatureAndHumidity();
    //updateLCD();
    measureVoltmeters();
  }
  if(millis() % tenMinutes == 0) {
  Serial.print(printTemperature);
  Serial.print(printHumidity());
  }
}