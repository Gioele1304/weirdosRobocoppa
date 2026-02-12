/*---------------------LIBRERIE---------------------------*/
#include "MeAuriga.h"   //librerie Makeblock
#include <Wire.h>
//#include <cmath.h>
/*--------------------------------------------------------*/

/*-------------------VALORI DA IMPOSTARE------------------*/

//HARDWARE
MeEncoderOnBoard motorDx(SLOT1);        // motore sx
MeEncoderOnBoard motorSx(SLOT2);        // motore dx
MeLineFollower lineFinder(PORT_6);      // sensore di linea
MeColorSensor colorsensorDx(PORT_7);    // sensore di colore dx
MeColorSensor colorsensorSx(PORT_8);    // sensore di colore sx
MeUltrasonicSensor ultraFront(PORT_9);  // sensore ultrasuoni frontale
MeUltrasonicSensor ultraSide(PORT_10);  // sensore ultrasuoni laterale

//SOFTWARE
#define velocitaMotore 90                 //velocità base del motore
#define correggiTraiettoria 60         //valore coefficiente correttivo traiettoria
#define numeroStatoCorrente 0             //stato iniziale
#define numeroStatoOld 0                  //stato precedente iniziale

/*--------------------------------------------------------*/

/*------------------DICHIARAZIONI VARIABILI---------------*/
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
  ostacolo,
  inizioArena,
  cercaUscita,
  fineArena,
  End
};

stato statoAttuale = numeroStatoCorrente;  // variabile per controllare la situazione attuale
stato statoOld = numeroStatoOld;            // variabile per ricordare il vecchio stato

// MOTORE
int velBase = velocitaMotore;     // velocità base motore
float kp = correggiTraiettoria;   // coefficiente correttivo traiettoria
float correction;                 // correzione traiettorie
float error = 0.0;                // errore traiettori
int pwmSX;                        // modulazione motore Sx
int pwmDX;                        // modulazione motore Dx
float Salita;
float Discesa;
float compSalita;
float tempoRicerca;
float durataRicerca;

// TEMPO E GIROSCOPIO
long time0, time;             // tempo iniziale e attuale
MeGyro gyro;                  // variabile giroscopio
float asseX0, asseY0, asseZ0; // posizione assi giroscopio all'accensione
float asseX, asseY, asseZ;    // posizione assi giroscopio
float rollX, pitchY, yawZ;    // rotazioni lungo assi

// SEGUILINEA
int seguilinea;           // variabile per seguilinea (S1_IN_S2_IN / S1_IN_S2_OUT / S1_OUT_S2_IN / S1_OUT_S2_OUT)

// SENSORI COLORE DX & SX
uint8_t excolorDx, excolorSx;
uint8_t colorDx, colorSx;   //  Risultati sensori colore
uint16_t redvalueDX = 0, greenvalueDX = 0, bluevalueDX = 0, colorvalueDX = 0;
uint16_t redvalueSX = 0, greenvalueSX = 0, bluevalueSX = 0, colorvalueSX = 0;
long colorcodeDX = 0, colorcodeSX = 0;

// ULTRASUONI
float distFronte, distLato; // distanza sensori ultrasuoni

/*--------------------------------------------------------*/


/*--------------------------------------------------------------------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------------------------TEST SENSORI----------------------------*/
/*--------------------------------------------------------------------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------------------TEST GIROSCOPIO--------------------------------*/
void testgyro()
{
  time = millis() - time0;             //tempo attuale

  gyro.update();
  asseX = gyro.getAngleX();   // posizione asse X iniziale
  asseY = gyro.getAngleY();   // posizione asse Y iniziale
  asseZ = gyro.getAngleZ();   // posizione asse Z iniziale
  rollX = asseX - asseX0;     // rotazione asse X
  pitchY = asseY - asseY0;    // rotazione asse Y
  yawZ = asseZ - asseZ0;      // rotazione asse Z

  Serial.print(time);
  Serial.println("  -------GIROSCOPIO--------");
  Serial.print("X:");
  Serial.print(asseX);
  Serial.print(" Y:");
  Serial.print(asseY);
  Serial.print(" Z:");
  Serial.println(asseZ);
  Serial.print("RollX:");
  Serial.print(rollX);
  Serial.print(" PitchY:");
  Serial.print(pitchY);
  Serial.print(" YawZ:");
  Serial.println(yawZ);

  asseX0 = asseX;               // posizione asse X precedente
  asseY0 = asseY;               // posizione asse Y precedente
  asseZ0 = asseZ;               // posizione asse Z precedente
}

void vaidritto() {
  switch (seguilinea) {
    case S1_IN_S2_IN:
      error = 0;
      break;
    case S1_IN_S2_OUT:
      error = -1;
      break;
    case S1_OUT_S2_IN:
      error = +1;
      break;
    case S1_OUT_S2_OUT:
      statoOld = statoAttuale;
      statoAttuale = ricerca;                      // se tutto bianco è tratteggio
      break;
    default: break;
  }
  correction = kp * error;
  pwmSX = constrain(int(velBase + correction), 0, 255);
  pwmDX = constrain(int(velBase - correction), 0, 255);
  motorDx.setMotorPwm(-pwmDX);
  motorSx.setMotorPwm(pwmSX);
}


/*---------------------------------------------------------------------------------------------SETUP------------------------------------*/
void setup() {
  Serial.begin(115200);
  time0 = millis();            // inizializzo tempo
  gyro.begin();                // giroscopio
  asseX0 = gyro.getAngleX();   // posizione asse X iniziale
  asseY0 = gyro.getAngleY();   // posizione asse Y iniziale
  asseZ0 = gyro.getAngleZ();   // posizione asse Z iniziale

  /*MAGIA NERA MOTORI*/
  //Set PWM 8KHz
  TCCR1A = _BV(WGM10);
  TCCR1B = _BV(CS11) | _BV(WGM12);
  TCCR2A = _BV(WGM21) | _BV(WGM20);
  TCCR2B = _BV(CS21);
  motorDx.setPulse(9);
  motorSx.setPulse(9);
  /*FINE MAGIA NERA MOTORI*/

  // IMPOSTAZIONI INIZIALI SENSORI COLORI
  colorsensorDx.SensorInit();
  colorsensorSx.SensorInit();



}
/*---------------------------------------------------------------------------------------------LOOP-------------------------------------*/
void loop() {

  // lettura sensori
  colorSx = colorsensorSx.ColorIdentify();
  colorDx = colorsensorDx.ColorIdentify();

  float distFront = ultraFront.distanceCm();
  float distSide = ultraSide.distanceCm();

  seguilinea = lineFinder.readSensors();

  asseX0 = gyro.getAngleX();   // posizione asse X iniziale
  asseY0 = gyro.getAngleY();   // posizione asse Y iniziale
  asseZ0 = gyro.getAngleZ();

  switch (statoAttuale) {
    case dritto:
      vaidritto();

      if (seguilinea == S1_OUT_S2_OUT && colorDx == WHITE && colorSx == WHITE &&
          excolorDx == WHITE && excolorSx == BLACK) {
        statoAttuale = gomitoSx;

      } else if (seguilinea == S1_OUT_S2_OUT && colorDx == WHITE && colorSx == WHITE &&
                 excolorDx == BLACK && excolorSx == WHITE) {
        statoAttuale = gomitoDx;

      } else if (distFront <= 10) {
        statoAttuale = ostacolo;

      } else if (colorDx == GREEN && excolorDx == WHITE) {
        statoAttuale = incrocioDx;

      } else if (colorSx == GREEN && excolorSx == WHITE) {
        statoAttuale = incrocioSx;

      } else if (colorDx == GREEN && colorSx == GREEN &&
                 excolorDx == WHITE && excolorSx == WHITE) {
        statoAttuale = incrocioU;

      } else if (seguilinea == S1_OUT_S2_OUT) {
        statoAttuale = ricerca;
       
      float asseX = gyro.getAngleX();   // posizione asse X iniziale
      float asseY = gyro.getAngleY();   // posizione asse Y iniziale
      float asseZ = gyro.getAngleZ();
     
      } else if ( asseY > asseY0 || asseY > asseY0) {
        statoAttuale = salita;

      } else if (colorDx == RED && colorSx == RED) {
        statoAttuale = End;
      }
      break;

    case incrocioDx:
      motorDx.setMotorPwm(-pwmDX);
      motorSx.setMotorPwm(-pwmSX);
      delay(500);
      if (seguilinea == S1_IN_S2_IN) statoAttuale = dritto;
      break;

    case incrocioSx:
      motorDx.setMotorPwm(pwmDX);
      motorSx.setMotorPwm(pwmSX);
      delay(500);
      if (seguilinea == S1_IN_S2_IN) statoAttuale = dritto;
      break;

    case incrocioU:
      motorDx.setMotorPwm(pwmDX);
      motorSx.setMotorPwm(pwmSX);
      delay(500);
      if (seguilinea == S1_IN_S2_IN) statoAttuale = dritto;
      break;

    case gomitoDx:
      motorDx.setMotorPwm(-pwmDX);
      motorSx.setMotorPwm(-pwmSX);
      delay(500);
      if (seguilinea == S1_IN_S2_IN) statoAttuale = dritto;
      break;

    case gomitoSx:
      motorDx.setMotorPwm(pwmDX);
      motorSx.setMotorPwm(pwmSX);
      delay(500);
      if (seguilinea == S1_IN_S2_IN) statoAttuale = dritto;
      break;

    case tratteggio:
      motorDx.setMotorPwm(pwmDX);
      motorSx.setMotorPwm(pwmSX);
      delay(1000);
      statoAttuale = ricerca;
      break;

    case ricerca:
      // FIX: condizione corretta
      unsigned long durataRicerca = millis() - tempoRicerca;
      if (durataRicerca < 10000 ) {
        motorDx.setMotorPwm(-pwmDX);
        motorSx.setMotorPwm(pwmSX);
        if (seguilinea == S1_IN_S2_IN || seguilinea == S1_OUT_S2_IN || seguilinea == S1_IN_S2_OUT) {
          statoAttuale = dritto;
        }
      } else if (durataRicerca >= 10000) {
        statoAttuale = tratteggio;
      }
      break;

    case salita:
      float asseX = gyro.getAngleX();   // posizione asse X iniziale
      float asseY = gyro.getAngleY();   // posizione asse Y iniziale
      float asseZ = gyro.getAngleZ();
      if (asseY > asseY0){
        pwmSX = pwmSX+20 && pwmDX+20;
      }else if (asseY < asseY0){
        pwmSX = pwmSX-20 && pwmDX-20;
      }

    case ostacolo:
      motorDx.setMotorPwm(pwmDX);
      motorSx.setMotorPwm(pwmSX);
      break;

    case End:
      motorDx.setMotorPwm(0);
      motorSx.setMotorPwm(0);
      break;

  }

  // FIX: aggiorno colori precedenti
  uint8_t excolorDx = colorDx;
  uint8_t excolorSx = colorSx;
}
