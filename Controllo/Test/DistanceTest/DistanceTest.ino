#include <NewPing.h>

// COSTANTI PINS
const int DX_FORWARD = 5, DX_BACKWARD = 4, DX_FORWARD_EN = 27, DX_BACKWARD_EN = 26;   // Motore DX
const int SX_FORWARD = 7, SX_BACKWARD = 6, SX_FORWARD_EN = 22, SX_BACKWARD_EN = 23;   // Motore SX

const int BUMPERS = 3;                  // pin dei bumpers
const int BUTTONS = 2;                  // pin dei bottoni
const int EMERGENCY_PIN;                // pin di emergenza
const int ARDUINO_EMERGENCIES = 8;      // pin per le emergenze degli altri Arduino

const int KEY = 9;                      // pin della chiave
const int START_FROM_APP;               // pin collegato all'app per l'accensione
const int COMMUNICATION_PIN;            // pin per la rispota ai messaggi

// SENSORI PROSSIMITA'
// frontale
const int TRIG_PIN_U = 32;     
const int ECHO_PIN_U = 33;
const int SENSOR_U_INDEX = 0;     
// frontale destro
const int TRIG_PIN_UR = 34;
const int ECHO_PIN_UR = 35;
const int SENSOR_UR_INDEX = 1;    
// frontale sinistro
const int TRIG_PIN_UL = 36;  
const int ECHO_PIN_UL = 37;
const int SENSOR_UL_INDEX = 2;     

// posteriore
const int TRIG_PIN_D = 38;
const int ECHO_PIN_D = 39;
const int SENSOR_D_INDEX = 3;    
// posteriore destro
const int TRIG_PIN_DR = 40;
const int ECHO_PIN_DR = 41;
const int SENSOR_DR_INDEX = 4;    
// posteriore sinistro
const int TRIG_PIN_DL = 42;
const int ECHO_PIN_DL = 43;
const int SENSOR_DL_INDEX = 5;        

// COSTANTI
const int SENSORS_NUMBER = 6;
const int MAX_DISTANCE = 200;
const int INTERVAL = 1000;
const int SONAR_INTERVAL = 50;

const int MAX_SPEED = 150;
const int MIN_SPEED = -100;
const int SPEED_GAIN = 2;
const int LOW_SPEED = 35;   //da testare

const int TANGO_SIZE = 20;
const int EMERGENCY_DISTANCE = 40;
const int SECURITY_DISTANCE = 60;
const float SPEED_OF_SOUND = 0.0343;

// VARIABILI
unsigned long startTime;
unsigned long currentTime;
unsigned long commTime;

int speed = 0;
int movementInt = 0;
int brakingTime = 0;
int sensorIndex = 0;

bool emergency = true;
bool forwardDir = true;
bool isRotating = false;

String serialString = "";

// Array inizializzazione sensori di prossimità
NewPing sonar[SENSORS_NUMBER] = {   
    NewPing(TRIG_PIN_U, ECHO_PIN_U, MAX_DISTANCE),      // sensore frontale
    NewPing(TRIG_PIN_UR, ECHO_PIN_UR, MAX_DISTANCE),    // sensore frontale destro
    NewPing(TRIG_PIN_UL, ECHO_PIN_UL, MAX_DISTANCE),    // sensore frontale sinistro
    NewPing(TRIG_PIN_D, ECHO_PIN_D, MAX_DISTANCE),      // sensore posteriore 
    NewPing(TRIG_PIN_DR, ECHO_PIN_DR, MAX_DISTANCE),    // sensore posteriore destro
    NewPing(TRIG_PIN_DL, ECHO_PIN_DL, MAX_DISTANCE)     // sensore posteriore sinistro
}


void setup() {
    Serial1.begin(9600);    // collegamento all'arduino di comunicazione

    startTime = millis();
    /*
    pinMode(DX_FORWARD, OUTPUT);
    pinMode(DX_BACKWARD, OUTPUT);
    pinMode(DX_FORWARD_EN, OUTPUT);
    pinMode(DX_BACKWARD_EN, OUTPUT);

    pinMode(SX_FORWARD, OUTPUT);
    pinMode(SX_BACKWARD, OUTPUT);
    pinMode(SX_FORWARD_EN, OUTPUT);
    pinMode(SX_BACKWARD_EN, OUTPUT);

    pinMode(EMERGENCY_PIN, OUTPUT);
    pinMode(COMMUNICATION_PIN, OUTPUT);

    pinMode(KEY, INPUT_PULLUP);

    pinMode(BUTTONS, INPUT_PULLUP);         // emergenza bottoni
    pinMode(BUMPERS, INPUT_PULLUP);         // emergenza bumper
    pinMode(ARDUINO_EMERGENCIES, INPUT);    // emergenza arduino
    */
    pinMode(TRIG_PIN_U, OUTPUT);
    pinMode(ECHO_PIN_U, INPUT);

    pinMode(TRIG_PIN_UR, OUTPUT);
    pinMode(ECHO_PIN_UR, INPUT);

    pinMode(TRIG_PIN_UL, OUTPUT);
    pinMode(ECHO_PIN_UL, INPUT);

    pinMode(TRIG_PIN_D, OUTPUT);
    pinMode(ECHO_PIN_D, INPUT);
    
    pinMode(TRIG_PIN_DR, OUTPUT);
    pinMode(ECHO_PIN_DR, INPUT);
    
    pinMode(TRIG_PIN_DL, OUTPUT);
    pinMode(ECHO_PIN_DL, INPUT);
}


void loop() {

    currentTime = millis();

    measureDistance(0);
    //measureDistance(1);
    /*
    for (int i = 0; i < 1; i++)
    {
        measureDistance(i);
    }
    */
    //distanceManagement();
}

// funzione stato emergenza
void emergencyState() {
    if (!emergency){            // ferma la macchina e manda un messaggio di emergenza agli altri arduino
        emergency = true;
        emergencyStop();
    }

    while (emergency || !digitalRead(key))      		// rimane nel loop finché non viene girata la chiave o viene mandato un messaggio dall'app
    {
        if (!digitalRead(key)){
            emergency = false;
        }
    }
}

// segnale di arresto del motore (potrebbe essere non necessaria)
void emergencyStop() {
    stopMotor();
    resetVariables();
}

// reset delle variabili
void resetVariables() {
    speed = 0;
    movementInt = 0;
}

// funzione con le opzioni di movimento
void movement(){
    switch (movementInt) {
        // andare avanti
        case 1:
            rotationCheck();

            if (speed < 0) {                                   // se la velocità era negativa, rallentiamo i motori
                decelerate();
                break;
            }

            accelerate();
            break;
        // andare indietro
        case 2:
            rotationCheck();

            if (speed > 0) {                                   // se la velocità era positiva, rallentiamo i motori
                decelerate();
                break;
            }

            accelerate();
            break;
		// frenata
        case 5:
            rotationCheck();

            decelerate();
            break;
        // nessun comando di movimento
        default:
            rotationCheck();

            decelerate();
            break;
    }
}

// controllo della velocità vicino a zero
void speedControl(){
    if (speed < speedGain && speed > -speedGain) speed = 0;
}

// controllo della rotazione
void rotationCheck(){
    if (isRotating){
        stopMotor();
        isRotating = false;
    }
}

// funzione per fermare i motori
void stopMotor(){
    analogWrite(dxForward, 0);
    analogWrite(dxBackward, 0);

    analogWrite(sxForward, 0);
    analogWrite(sxBackward, 0);

    currentTime = startTime;
}

// funzione per muoversi avanti o indietro
void driveMotor(int motor1, int motor2, int spd) {
    if (spd < 0) spd = -spd;

    analogWrite(motor1, spd);
    analogWrite(motor2, spd);
}

// funzione per accelerare
void accelerate(){
    if (currentTime - startTime >= interval){

        startTime = currentTime;
        Serial.println(speed);
        if (speed >= 0) {
            if (speed >= maxSpeed) return;      // se la velocità è al massimo, non fare niente

            speed += speedGain;
            driveMotor(dxForward, sxForward, speed);
            return;
        }

        if (speed <= 0) {
            if (speed <= minSpeed) return;      // se la velocità è al minimo, non fare niente

            speed -= speedGain;
            driveMotor(dxBackward, sxBackward, speed);
            return;
        }
    }
}

// funzione per decelerare e lentamente fermarsi
void decelerate(){
    if (currentTime - startTime >= interval){

        startTime = currentTime;
        Serial.println(speed);
        if (speed < 0) {
            speed += speedGain;
            speedControl();
            driveMotor(dxBackward, sxBackward, speed);
            return;
        }

        if (speed > 0) {
            speed -= speedGain;
            speedControl();
            driveMotor(dxForward, sxForward, speed);
            return;
        }
    }
}

// MISURA DISTANZA           
///////////////////////////////////////////////////////////////////////////////
// funzioni per la gestione della distanza
void measureDistance(int sonarNum) {

    float distance = (sonar[sonarNum].ping() / 2) * SPEED_OF_SOUND;
    serial.print(printDistance(distance)); //DEBUG da cancellare

    if (distance < TANGO_SIZE) return;

    if (distance < (EMERGENCY_DISTANCE + TANGO_SIZE) && (speed > LOW_SPEED || speed < -LOW_SPEED)) emergency = true;
    
}

//funzione di debug
String printDistance(float distance) {  
    
    if (sensorIndex == SENSOR_U_INDEX)  Serial.print("\n S ");
    if (sensorIndex == SENSOR_UR_INDEX) Serial.print("\n S1 ");
    if (sensorIndex == SENSOR_UL_INDEX) Serial.print("\n S2 ");
    if (sensorIndex == SENSOR_D_INDEX)  Serial.print("\n S3 ");
    if (sensorIndex == SENSOR_DR_INDEX) Serial.print("\n S4 ");
    if (sensorIndex == SENSOR_DL_INDEX) Serial.print("\n S5 ");

    return String("Distanza: " + (String)distance +"cm");
        
}

//founzione per la gestione dei sensori a ultrasuoni
void distanceManagement() {

    // ciclo non bloccante ogni 50 ms + 450 a fini di debug
    if (currentTime - startTime >= SONAR_INTERVAL + 450){

        // rotazione su se stesso
        if (isRotating) {

            if (sensorIndex < SENSORS_NUMBER) {

                measureDistance(sensorIndex);
                sensorIndex++;
            
            } else {
                sensorIndex = 0;
            }
        // movimento in avanti
        } else if (forwardDir) {

            switch (sensorIndex)
            {
            case SENSOR_U_INDEX:

                measureDistance(sensorIndex);
                sensorIndex = SENSOR_UR_INDEX;
                break;

            case SENSOR_UR_INDEX:

                measureDistance(sensorIndex);
                sensorIndex = SENSOR_UL_INDEX;
                break;
            
            case SENSOR_UL_INDEX:

                measureDistance(sensorIndex);
                sensorIndex = SENSOR_U_INDEX;
                break;
            
            default:
                sensorIndex = SENSOR_U_INDEX;
                break;
            }
        // retromarcia
        } else if (!forwardDir) {
            
            switch (sensorIndex)
            {
            case SENSOR_D_INDEX:

                measureDistance(sensorIndex);
                sensorIndex = SENSOR_DR_INDEX;
                break;

            case SENSOR_DR_INDEX:

                measureDistance(sensorIndex);
                sensorIndex = SENSOR_DL_INDEX;
                break;
            
            case SENSOR_DL_INDEX:

                measureDistance(sensorIndex);
                sensorIndex = SENSOR_D_INDEX;
                break;
            
            default:
                sensorIndex = SENSOR_D_INDEX;
                break;
            }
        }

        startTime = currentTime;    //set del tempo per ciclo successivo
    } 
}