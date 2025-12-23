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

  int melody[] = {
    // Flagpole (24 note)
    2637, 2349, 2217, 2093,
    1976, 1865, 1760, 1661,
    1568, 1480, 1397, 1319,
    1245, 1175, 1109, 1047,
    988, 932, 880, 831,
    784, 740, 698, 659,
  
    // Course Clear
    1047, 1319, 1568,
    2093, 1568, 1319, 1047,
    784
  };
  
  };
  int noteDuration[] = {
    // Flagpole
    8,8,8,8,
    8,8,8,8,
    8,8,8,8,
    8,8,8,8,
    8,8,8,8,
    8,8,8,8,
  
    // Course Clear
    4,4,4,
    4,4,4,4,
    2
  };
  }


// variabile per controllare la situazione attuale
Stato statoAttuale;


// variabili
// sensore di linea
MeLineFollower lineFinder(PORT_3);
int seguilinea;

MeBuzzer buzzer;

// sensore di colore dx
// sensore di colore sx
MeColorSensor colorsensorDx(PORT_8);
MeColorSensor colorsensorSx(PORT_9);

// sensore ultrasuoni
MeUltrasonicSensor ultraFront(PORT_6);
MeUltrasonicSensor ultraSide(PORT_7);

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
      else if (float angoloY=>20) {
        statoAttuale=salita;
      }
      else if (float angoloY=<-20) {
        statoAttuale=discesa;
      }
      else if (distFront <= 20) {
        statoAttuale = ostacolo;
      }
      else if (colordx== GRIGIO & colorsx== GRIGIO){
        statoAttuale=inizioArena
      }
      else if (colordx== RED & colorsx== RED){
          statoAttuale=End;
      }
      
      break;
    case curvaDx:
      motor1.run(20);
      motor2.run(80);
      if (seguilinea == S1_IN_S2_IN){
        statoAttuale = dritto;
      }
      break;
  
    case curvaSx:
      motor1.run(80);
      motor2.run(20);
      if (seguilinea == S1_IN_S2_IN){
        statoAttuale = dritto;
      }
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
      motor1.run(-20);
      motor2.run(20);
      delay(1000);
      motor1.run(20);
      motor2.run(-20);
      if (seguilinea == S1_IN_S2_IN || S1_OUT_S2_IN || S1_IN_S2_OUT || S1_OUT_S2_OUT){
        statoAttuale = dritto;
      break;
  
    case salita:
      motor1.run(100);
      motor2.run(100);
      break;
  
    case discesa:
      motor1.run(20);
      motor2.run(20);
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
      motor1.run(0);
      motor2.run(0);
      play;
      break;
    
  }
}







void play()
{
    for (int thisNote = 0; thisNote < 32; thisNote++) {

    // to calculate the note duration, take one second 
    // divided by the note type.
    //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
    int noteDuration = 1000/noteDurations[thisNote];
    tone(8, melody[thisNote],noteDuration);

    // to distinguish the notes, set a minimum time between them.
    // the note's duration + 30% seems to work well:
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    // stop the tone playing:
    noTone(8);
  }
}
