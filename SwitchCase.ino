  #include <MeAuriga.h>
  #include <MeColorSensor.h>
  #include <Wire.h>
  
  // ---------------------- VARIABILI GLOBALI ----------------------
  
  // PID base
  float kp = 75;          // FIX: aggiunto
  int velBase = 100;      // FIX: aggiunto
  float correction;                 // correzione traiettorie
  float error = 0.0;                // errore traiettori
  int pwmSX;                        // modulazione motore Sx
  int pwmDX;                        // modulazione motore Dx
  float Salita;
  float Discesa;
  float compSalita;
  float tempoRicerca;
  float durataRicerca;

  MeRGBLed led( 0, 12 );
  
  // stato macchina
  enum stato {
    linefollow,
    incrocioDritto,
    incrocioDx,
    incrocioSx,
    incrocioU,
    tratteggio,
    ricerca,
    salita,
    ostacolo,
    inizioArena,
    fineArena,
    End
  };
  
  int statoAttuale = linefollow;   // FIX: inizializzato
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
  MeUltrasonicSensor ultraFront(PORT_10);
  MeUltrasonicSensor ultraSide(PORT_9);
  
  // giroscopio
  MeGyro gyro;
  float angoloX, angoloY, angoloZ;
  
  // motori
  MeEncoderOnBoard motorDx(SLOT1);        // motore sx
  MeEncoderOnBoard motorSx(SLOT2);        // motore dx
  uint8_t motorSpeed = 100;
  
  // ---------------------- FUNZIONE SEGUI LINEA ----------------------
  
  
  
  void vaidritto(){
    seguilinea = lineFinder.readSensors();
    switch (seguilinea) {
      case S1_IN_S2_IN:
        led.setColor( 2, 40, 10, 40);     //accendo i 3 led frontali
        led.setColor( 3, 40, 10, 40);
        led.setColor( 4, 40, 10, 40);
        led.show();
        error = 0;
        if (colorSx == GREEN && excolorSx== WHITE){
          statoAttuale=incrocioSx;}
        else if (colorDx == GREEN && excolorDx== WHITE){
          statoAttuale=incrocioDx;}
        else if (colorSx == GREEN && colorDx == GREEN && excolorSx== WHITE && excolorDx==WHITE){
          statoAttuale=incrocioU;
        }
        break;
      case S1_IN_S2_OUT:
        led.setColor( 2, 40, 10, 40);     //accendo i 2 led frontali (SX)
        led.setColor( 3, 40, 10, 40);
        led.setColor( 4, 0, 0, 0);
        led.show();
        error = -1;
        break;
      case S1_OUT_S2_IN:
        led.setColor( 2, 0, 0, 0);     //accendo i 2 led frontali (DX)
        led.setColor( 3, 40, 10, 40);
        led.setColor( 4, 40, 10, 40);
        led.show();
        error = +1;
        break;
      case S1_OUT_S2_OUT:
        led.setColor( 2, 0, 0, 0);     //accendo 1 led frontali )
        led.setColor( 3, 40, 10, 40);
        led.setColor( 4, 0, 0, 0);
        led.show();
        statoOld = statoAttuale;
        statoAttuale = ricerca;  
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
    /*if (angoloY >= 10) pwmDX -= 50; pwmSX += 50;
      if (angoloY <= -10) pwmDX += 50; pwmSX -= 50;*/
  
    // ---------------------- MACCHINA A STATI ----------------------
  
    switch (statoAttuale) {        

      case linefollow:
        vaidritto;
      break;

      case ricerca:
        // FIX: condizione corretta
        unsigned long durataRicerca = millis() - tempoRicerca;
        motorDx.setMotorPwm(-velBase);
        motorSx.setMotorPwm(-velBase);
        delay(1000);
        motorDx.setMotorPwm(velBase);
        motorSx.setMotorPwm(velBase);
        delay(2000);
        motorDx.setMotorPwm(-velBase);
        motorSx.setMotorPwm(-velBase);
        delay(1000);
        
          if (seguilinea == S1_IN_S2_IN || seguilinea == S1_OUT_S2_IN || seguilinea == S1_IN_S2_OUT) {
            statoAttuale = linefollow;
          }
         statoAttuale=tratteggio;
        break;
      
      case incrocioDx:
        motorDx.setMotorPwm(-pwmDX);
        motorSx.setMotorPwm(-pwmSX);
        delay(500);
        if (seguilinea == S1_IN_S2_IN) statoAttuale = linefollow;
        break;
  
      case incrocioSx:
        motorDx.setMotorPwm(pwmDX);
        motorSx.setMotorPwm(pwmSX);
        delay(500);
        if (seguilinea == S1_IN_S2_IN) statoAttuale = linefollow;
        break;
  
      case incrocioU:
        motorDx.setMotorPwm(pwmDX);
        motorSx.setMotorPwm(pwmSX);
        delay(500);
        if (seguilinea == S1_IN_S2_IN) statoAttuale = linefollow;
        break;

      case tratteggio:
        motorDx.setMotorPwm(pwmDX);
        motorSx.setMotorPwm(pwmSX);
        delay(1000);
        statoAttuale = ricerca;
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
