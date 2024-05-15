#include <SoftwareSerial.h>

#define STEP_INCREMENT 1.0  // Initial radian increment for searching limits

SoftwareSerial serialMotor0(8, 9); // RX, TX
float minLimitM0 = -15.0;
float maxLimitM0 = 15.0;

long last=0;

void setup() {
  Serial.begin(115200);
  serialMotor0.begin(115200);

  last = millis();
}

int potentiometerValue0 = 0;
int prevValue0 = 0;

void loop() {

  if(millis()-last > 10){
    potentiometerValue0 = analogRead(A0);
    if (abs(prevValue0 - potentiometerValue0) > 0){
      moveMotor(0, potentiometerValue0, -1);
    }
  }

  SerialBridge();

}

void SerialBridge(){
  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');
    
    serialMotor0.println(input);
    //Serial1.println(input);
  }

  if (serialMotor0.available() > 0) {
  //if (Serial1.available() > 0) {
    String messageReceived = serialMotor0.readStringUntil('\n');
    //String messageReceived = Serial1.readStringUntil('\n');
    
    Serial.print("Received: ");
    Serial.println(messageReceived);
  }
}

/*
 * id         : which motor we want to control, 0 or 1
 * position   : position between 0 and 100 that we want the motor to goes to 
 * speed      : speed in rad/s that we want to move
*/
void moveMotor(int id, float position, float speed){
  if (position < 0 || position >1023 || millis()-last < 10 ){
    return;
  }
  last=millis();

  String speedString = (speed == -1) ? "" : " " + String(speed, 2);

  if(id == 0){
    float range = maxLimitM0 - minLimitM0;
    float targetPosition = minLimitM0 + (position / 1023.0) * range;
    serialMotor0.println("M" + String(targetPosition, 2) + speedString);
  }
  /*if (id == 1){
    float range = maxLimitM1 - minLimitM1;
    float targetPosition = minLimitM1 + (position / 100.0) * range;
    serialMotor1.println("M" + String(targetPosition, 2) + speedString);
  }*/
}

