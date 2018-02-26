#include <Servo.h>
const int LIMIT_UPPER_ROTATION = 180;
const int LIMIT_LOWER_ROTATION = 2;
const int LIMIT_UPPER_INCLINATION = 138;
const int LIMIT_LOWER_INCLINATION = 0;

Servo servoRotation;
Servo servoInclination;
int setAngle = 0;

float angleRotation = 90;
float angleInclination = 90;
bool isRotation = true;

void setup(){
  servoRotation.attach(3);
  servoInclination.attach(6);
  Serial.begin(115200); // Speed of transmision
}

void loop(){
  char command[4];
  if(Serial.available()){
    Serial.readBytes(command, 4);
    setAngle = (String(command[1]) + String(command[2]) + String(command[3])).toInt();

    switch(command[0]) {
      case 'r': {
        if (setAngle <= LIMIT_UPPER_ROTATION && setAngle >= LIMIT_LOWER_ROTATION) {
          rotateServo(servoRotation, setAngle);
        }
      } break;
      case 'i': {
        if (setAngle <= LIMIT_UPPER_INCLINATION && setAngle >= LIMIT_LOWER_INCLINATION) {
          inclineServo(servoInclination, setAngle);
        }
      } break;
    }
  }
  delay(1);
}

void rotateServo(Servo servo, int angle) {
  angle=constrain(angle,LIMIT_LOWER_ROTATION,LIMIT_UPPER_ROTATION); // Establecemos limites por la limitacion fisica de los motores
  servo.write(angle);
  Serial.print("Angulo:");Serial.println(angle);
}

void inclineServo(Servo servo, int angle) {
  angle=constrain(angle,LIMIT_LOWER_INCLINATION,LIMIT_UPPER_INCLINATION); // Establecemos limites por la limitacion fisica de los motores
  servo.write(angle);
  Serial.print("Angulo:");Serial.println(angle);
}

