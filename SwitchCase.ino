#include "MeAuriga.h"
#include <wire.h> 

motorspeed = 50;

enum stato {
  dritto, 
  curvaDx,
  curvaSx,
  incrocioDritto,
  incrocioDx,
  incrocioSx,
  incrocioU,
  gomitoDx,
  gomitoSx,
  tratteggio,
  ricerca,
  salita,
  discesa,
  ostacolo,
  inizioArena,
  cercaUscita,
  fineArena,
  End
};

// variabile per controllare la situazione attuale
Stato statoAttuale;


// variabili
// sensore di linea
MeLineFollower lineFinder(PORT_3);
int seguilinea;

// sensore di colore dx
// sensore di colore sx
MeColorSensor colorsensorDx(PORT_8);
MeColorSensor colorsensorSx(PORT_9);

// sensore ultrasuoni
MeUltrasonicSensor ultraFront(PORT_8);
MeUltrasonicSensor ultraSide(PORT_9);

// giroscopio
MeGyro gyro;
float angoloX;
float angoloY;
float angoloZ;

//motori
MeDCMotor motor1(PORT_1);
MeDCMotor motor2(PORT_2);
MeEncoderOnBoard motorLeft(M1);
MeEncoderOnBoard motorRight(M2);
uint8_t motorSpeed = 100; 

// intervallo sensore colori
long tempoColore = millis();
uint8_t colorDx, colorSx;

void setup() {
  Serial.begin(9600);
  gyro.begin();

 /*int n = 0b00100;
  bool res = n & 4;
  Serial.println(n);
  Serial.println (res);*/
  
}

void loop() {
  
  int colorSx = colorSensorSx.ColorIdentify();//ColorIdentify oppure colorresult
  int colorDx = colorSensorDx.ColorIdentify();
  
  float distFront = ultraFront.distanceCm();
  float distSide = ultraSide.distanceCm();
  
  int seguilinea = lineFinder.readSensors();
  
  float angoloX = gyro.getAngleX();
  float angoloY = gyro.getAngleY();
  float angoloZ = gyro.getAngleZ();

  switch (statoAttuale){
    case dritto:
      
      motor1.run(motorspeed);
      motor2.run(motorspeed);

      if (seguilinea == S1_IN_S2_OUT){
        statoAttuale = curvaSx;
      }
      else if (seguilinea == S1_OUT_S2_IN){
        statoAttuale = curvaDx;
      }
      else if (seguilinea == S1_OUT_S2_OUT){
        statoAttuale = ricerca;
      }
      else if (distFront <= 10) {
        statoAttuale=ostacolo;
      }
      else if (colordx== VERDE){
        statoAttuale=incrocioDx;
      }
      else if (colorsx== VERDE){
        statoAttuale=incrocioSx;
      }
      else if (colordx== VERDE & colorsx== VERDE){
        statoAttuale=incrocioU;
      }
      else if (seguilinea == S1_OUT_S2_OUT){
        statoAttuale=ricerca;
      }
      else if (float angoloY==20) {
        statoAttuale=salita;
      }
      else if (float angoloY==-20) {
        statoAttuale=discesa;
      }
      else if (distFront <= 20) {
        statoAttuale = ostacolo;
      }
      else if (colordx== GRIGIO & colorsx== GRIGIO){
        statoAttuale=inizioArena
      }
      else if (colordx== VERDE & colorsx== VERDE){
          statoAttuale=End;
      }
      
      break;
    case curvaDx:
      break;
  
    case curvaSx:
      break;
  
    case incrocioDritto:
      break;
  
    case incrocioDx:
      break;
  
    case incrocioSx:
      break;
  
    case incrocioU:
      break;
  
    case gomitoDx:
      break;
  
    case gomitosx:
      break;
  
    case tratteggio:
      break;
  
    case ricerca:
      break;
  
    case salita:
      break;
  
    case discesa:
      break;
  
    case ostacolo:
      break;
  
    case inizioArena:
      break;
  
    case cercaUscita:
      break;
  
    case fineArena:
      break;
  
    case End:
      break;
    
  }
}
