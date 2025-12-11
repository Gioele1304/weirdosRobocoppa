#include "MeAuriga.h"
#include <wire.h>

MeLineFollower lineFinder(PORT_3);
MeColorSensor cSensorSx(PORT_6);
MeColorSensor colorSensorDx(PORT_7);
MeUltrasonicSensor ultraFront(PORT_8);
MeUltrasonicSensor ultraSide(PORT_9);
MeGyro gyro;
MeEncoderOnBoard motorLeft(M1);
MeEncoderOnBoard motorRight(M2);  


enum stato {
  dritto,
  curvadx,
  curvaSXl,
  incrociod,
  incrociodx,
  incrociosx,
  incrociou,
  gomitodx,
  gomitosx,
  tratteggio,
  cerca,
  salita,
  discesa,
  ostacolo,
  inizioarena,
  cercauscita,
  finearena,
  End
};

stato statoAttule;

void setup() {
  Serial.begin(9600);
  gyro.begin();

 /*int n = 0b00100;
  bool res = n & 4;
  Serial.println(n);
  Serial.println (res);*/
  
}

void loop() {
  gyro.getAngleX();
  int colorSx = colorSensorSx.ColorIdentify();//ColorIdentify oppure colorresult
  int colorDx = colorSensorDx.ColorIdentify();
  float distFront = ultraFront.distanceCm();
  float distSide = ultraSide.distanceCm();

  switch (statoAttuale){
    case dritto:
      break;
    case curvadx:
      break;
  
    case curvaSXl:
      break;
  
    case incrociod:
      break;
  
    case incrociodx:
      break;
  
    case incrociosx:
      break;
  
    case incrociou:
      break;
  
    case gomitodx:
      break;
  
    case gomitosx:
      break;
  
    case tratteggio:
      break;
  
    case cerca:
      break;
  
    case salita:
      break;
  
    case discesa:
      break;
  
    case ostacolo:
      break;
  
    case inizioarena:
      break;
  
    case cercauscita:
      break;
  
    case finearena:
      break;
  
    case End:
      break;
    
  }
}
