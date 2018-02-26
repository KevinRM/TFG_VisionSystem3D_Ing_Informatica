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
  unsigned char command;
  if(Serial.available()){
    command = Serial.read();
    
    switch(command) {
      case 'r': {
        //rotateServo(servoRotation, setAngle);
        if (angleRotation < 180) {
          angleRotation += 1;
          rotateServo(servoRotation, (int)angleRotation);
        }
      } break;
      case 'l': {
        //rotateServo(servoRotation, setAngle);
        if (angleRotation > 2) {
          angleRotation -= 1;
          rotateServo(servoRotation, (int)angleRotation);
        }
      } break;
      case 'u': {
        if (angleInclination < 138) {
          angleInclination += 0.5;
          inclineServo(servoInclination, (int)angleInclination);
        }
      } break;
      case 'd': {
        if (angleInclination > 0) {
          angleInclination -= 0.5;
          inclineServo(servoInclination, (int)angleInclination);
        }
      } break;
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

