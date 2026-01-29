#include <MeAuriga.h>
#include <MeColorSensor.h>
#include <Wire.h>

// ---------------------- VARIABILI GLOBALI ----------------------

// PID base
float kp = 65;          // FIX: aggiunto
int velBase = 120;      // FIX: aggiunto
float correction;                 // correzione traiettorie
float error = 0.0;                // errore traiettori
int pwmSX;                        // modulazione motore Sx
int pwmDX;                        // modulazione motore Dx

// stato macchina
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

int statoAttuale = dritto;   // FIX: inizializzato
int statoOld;

// sensore di linea
MeLineFollower lineFinder(PORT_6);
int seguilinea;

// sensori colore
MeColorSensor colorSensorDx(PORT_7);
MeColorSensor colorSensorSx(PORT_8);
uint8_t colorDx, colorSx;
uint8_t excolorDx = WHITE;   // FIX: aggiunti
uint8_t excolorSx = WHITE;

// ultrasuoni
MeUltrasonicSensor ultraFront(PORT_9);
MeUltrasonicSensor ultraSide(PORT_10);

// giroscopio
MeGyro gyro;
float angoloX, angoloY, angoloZ;

// motori
MeEncoderOnBoard motorDx(SLOT1);        // motore sx
MeEncoderOnBoard motorSx(SLOT2);        // motore dx
uint8_t motorSpeed = 100;

// ---------------------- FUNZIONE SEGUI LINEA ----------------------

void vaidritto() {   // FIX: ora è corretta
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
      //statoAttuale = ricerca;                      // se tutto bianco è tratteggio
      break;
    default: break;
  }
  correction = kp * error;
  pwmSX = constrain(int(velBase + correction), 0, 255);
  pwmDX = constrain(int(velBase - correction), 0, 255);
  motorDx.setMotorPwm(-pwmDX);
  motorSx.setMotorPwm(pwmSX);
}
// ---------------------- SETUP ----------------------

void setup() {
  Serial.begin(9600);
  gyro.begin();
}

// ---------------------- LOOP PRINCIPALE ----------------------

void loop() {
 Serial.print(statoAttuale);
  // FIX: chiamata corretta
  vaidritto();

  // lettura sensori
  colorSx = colorSensorSx.ColorIdentify();
  colorDx = colorSensorDx.ColorIdentify();

  float distFront = ultraFront.distanceCm();
  float distSide = ultraSide.distanceCm();

  seguilinea = lineFinder.readSensors();

  angoloX = gyro.getAngleX();
  angoloY = gyro.getAngleY();
  angoloZ = gyro.getAngleZ();

  // gestione salita/discesa
  if (angoloY >= 10) pwmDX -= 50; pwmSX += 50;
  if (angoloY <= -10) pwmDX += 50; pwmSX -= 50;

  // ---------------------- MACCHINA A STATI ----------------------

  switch (statoAttuale) {

    case dritto:
      vaidritto();

      if (seguilinea == S1_IN_S2_OUT) {
        statoAttuale = curvaSx;

      } else if (seguilinea == S1_OUT_S2_IN) {
        statoAttuale = curvaDx;

      } else if (seguilinea == S1_OUT_S2_OUT && colorDx == WHITE && colorSx == WHITE &&
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

      } else if (distFront <= 20) {
        statoAttuale = ostacolo;

      } else if (colorDx == RED && colorSx == RED) {
        statoAttuale = End;
      }
      break;

    case curvaDx:
      motorDx.setMotorPwm(-pwmDX);
      motorSx.setMotorPwm(-pwmSX);
      if (seguilinea == S1_IN_S2_IN) statoAttuale = dritto;
      break;

    case curvaSx:
      motorDx.setMotorPwm(pwmDX);
      motorSx.setMotorPwm(pwmSX);
      if (seguilinea == S1_IN_S2_IN) statoAttuale = dritto;
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
      motorDx.setMotorPwm(-pwmDX);
      motorSx.setMotorPwm(pwmSX);
      delay(1000);

      // FIX: condizione corretta

/*
      
      if (seguilinea == S1_IN_S2_IN || seguilinea == S1_OUT_S2_IN || seguilinea == S1_IN_S2_OUT){
        statoAttuale = dritto;
      }else{
      motor1.run(20);
      motor2.run(-20);
      delay(2000);}
      
      if (seguilinea == S1_IN_S2_IN ||
          seguilinea == S1_OUT_S2_IN ||
          seguilinea == S1_IN_S2_OUT) {
        statoAttuale = dritto;
      } else {
       if (millis()-tempoRicerca<=0){
      motor1.run(-20);
      motor2.run(20);
    else{
      motor1.run(20);
      motor2.run(-20);
    }
      }*/
      break;

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
  excolorDx = colorDx;
  excolorSx = colorSx;
}
