#include <ezButton.h>
#include <LCD_I2C.h>
#include <SoftwareSerial.h>

#define LED_PIN_1 13
#define LED_PIN_2 12

#define STEP_INCREMENT 0.05  // Initial radian increment for searching limits

SoftwareSerial serialMotor0(8, 9); // RX, TX
SoftwareSerial serialMotor1(10, 11);

float minLimitM0 = -15.0;
float maxLimitM0 = 15.0;

float minLimitM1 = -15.0;
float maxLimitM1 = 15.0;

long last = 0;

LCD_I2C lcd(0x3F, 16, 2); // address can change, check wiring on github issue for connecting screen
ezButton button1(7);  // next or player 1
ezButton button2(6); // enter or player 2

int curr;
String states[] = {"automated", "single player", "two player"};
bool modeSelected = false;
bool player1Ready = false;
bool player2Ready = false;
bool gameEnd = false;
bool restart = false;
int lScore= 0;
int rScore = 0;
int computerID = 0;


void setup()
{
    lcd.begin(); 
    lcd.backlight();
    button1.setDebounceTime(50);
    button2.setDebounceTime(50);
    pinMode(LED_PIN_1, OUTPUT);
    digitalWrite(LED_PIN_1, LOW);
    pinMode(LED_PIN_2, OUTPUT);
    digitalWrite(LED_PIN_2, LOW);
    button1.setCountMode(COUNT_FALLING);
    Serial.begin(115200);
    serialMotor0.begin(115200);
    serialMotor1.begin(115200);

    last = millis();

    lcd.print("Waiting for");
    lcd.setCursor(0, 1);
    lcd.print("calibration");
    lcd.setCursor(0, 0);

    while (!Serial){
      ;
      //Implement a manual calibration
    }
    
    calibrateMotors();
    // lcd.clear();
}

int potentiometerValue0 = 0;
int potentiometerValue1 = 1;
int prevValue0 = 0;
int prevValue1 = 1;

void loop()
{
  button1.loop(); 
  button2.loop();

  // game mode selection
  if (!modeSelected) {
    curr = button1.getCount() % 3;
    lcd.print(states[curr]+"       ");
    lcd.setCursor(0, 0);
  } 


  if (button2.isPressed()) {
    modeSelected = true;
    lcd.print(states[curr]);
    lcd.setCursor(0,1);
    lcd.print("selected. Ready?");
  }

  // players confirm ready
  if (modeSelected) {
    if (curr == 0) {
      player2Ready = true;
      player1Ready = true;
    }
  }

  while (modeSelected && (!player1Ready || !player2Ready) ) {
    button1.loop();
    button2.loop();

    if (button1.isPressed()) {
      player1Ready = true;
      if (curr == 1) {
        player2Ready = true;
        computerID = 1;
      }
    } 
    if (player1Ready) {
      digitalWrite(LED_PIN_1, LOW);
    } else {
      digitalWrite(LED_PIN_1, HIGH);
    }
    if (button2.isPressed()) {
      player2Ready = true;
      if (curr == 1) {
        player1Ready = true;
        computerID = 0;
      }
    } 
    if (player2Ready) {
      digitalWrite(LED_PIN_2, LOW);
    } else {
      digitalWrite(LED_PIN_2, HIGH);
    }
  }
  
  // game countdown
  if (player1Ready && player2Ready) {
    for (int i = 3; i >= 1; i--) {
      lcd.clear();
      digitalWrite(LED_PIN_1, LOW);
      digitalWrite(LED_PIN_2, LOW);
      delay(750);
      lcd.print(String(i)+"...");
      digitalWrite(LED_PIN_1, HIGH);
      digitalWrite(LED_PIN_2, HIGH);
      delay(750);
    }
    lcd.clear();
    lcd.print(String(lScore) + "-" + String(rScore));
    digitalWrite(LED_PIN_1, LOW);
    digitalWrite(LED_PIN_2, LOW);
  }

  // gameplay
  while (player1Ready && player2Ready && lScore < 5 && rScore < 5) {
    String command_received = readCommand();
    if (curr == 2) {
      twoPlayer(command_received);
    } else if (curr == 1) {
      singlePlayer(command_received);
    } else {
      automated(command_received);
    }
    
  }
  
  // ending sequence
  if (lScore == 5) {
    gameEnd = true;
    lcd.setCursor(0,0);
    lcd.print("Player two wins");
    lcd.setCursor(0,1);
    lcd.print("Continue?");
    for (int i = 0; i <10; i++) {
        digitalWrite(LED_PIN_2, HIGH);
        delay(100);
        digitalWrite(LED_PIN_2, LOW);
        delay(100);
    }
  } else if (rScore == 5) {
    gameEnd = true;
    lcd.setCursor(0,0);
    lcd.print("Player one wins");
    lcd.setCursor(0,1);
    lcd.print("Continue?");
    for (int i = 0; i < 10; i++) {
      digitalWrite(LED_PIN_1, HIGH);
      delay(100);
      digitalWrite(LED_PIN_1, LOW);
      delay(100);
    }
  }

  while (gameEnd && !restart ) {
      button2.loop();
      if (button2.isPressed()) {
        restart = true;
      }
  }

  if (restart) {
    modeSelected = false;
    player1Ready = false;
    player2Ready = false;
    gameEnd = false;
    restart = false;
    lcd.clear();
    lScore = 0;
    rScore = 0;
    moveMotor(0, 512, 200);
    moveMotor(1, 512, 200);
  }
}

void twoPlayer(String command_received) {
  if(millis()-last > 10){
    potentiometerValue0 = analogRead(A1);
    potentiometerValue1 = analogRead(A0);

    if (abs(prevValue0 - potentiometerValue0) > 0){
      moveMotor(0, potentiometerValue0, 200);
    }
    delay(10);
    if (abs(prevValue1 - potentiometerValue1) > 0){
      moveMotor(1, potentiometerValue1, 200);
    }
  }
  checkGoal(command_received);
}

void singlePlayer(String command_received){
  checkGoal(command_received);

  if (command_received.startsWith("game ")){
    delay(20);
      int motor = command_received.substring(5,6).toInt();
      if (motor == computerID) {
        float command = command_received.substring(7).toFloat();
        moveMotor(motor, command, 200);
      }
  } else if(millis()-last > 10){
    if (computerID == 1) {
      potentiometerValue0 = analogRead(A1);
      if (abs(prevValue0 - potentiometerValue0) > 0){
        moveMotor(0, potentiometerValue0, 200);
      }
    } else {
      potentiometerValue1 = analogRead(A0);
      if (abs(prevValue1 - potentiometerValue1) > 0){
        moveMotor(1, potentiometerValue1, 200);
      }
    }
  }
}

void automated(String command_received) {
  if (command_received.startsWith("game ")){
      int motor = command_received.substring(5,6).toInt();
      float command = command_received.substring(7).toFloat();
      moveMotor(motor, command, 200);
  }
  checkGoal(command_received);
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

  if(id == 0){
    float range = maxLimitM0 - minLimitM0;
    float targetPosition = minLimitM0 + (position / 1023.0) * range;
    sendCommandToMotor(getMotorSerial(0), moveMotorCommand(targetPosition, speed));
  }
  if (id == 1){
    float range = maxLimitM1 - minLimitM1;
    float targetPosition = minLimitM1 + (position / 1023.0) * range;
    sendCommandToMotor(getMotorSerial(1), moveMotorCommand(targetPosition, speed));
  }
}

void SerialBridge(){
  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');
    
    serialMotor0.println(input);
  }

  if (serialMotor0.available() > 0) {
    String messageReceived = serialMotor0.readStringUntil('\n');
    
    Serial.print("Received: ");
    Serial.println(messageReceived);
  }
}

void checkGoal(String command_received) {
  if (command_received.startsWith("goal ")){
    int id = command_received.substring(5,6).toInt();
    if (id == 0) {
      rScore += 1;
      for (int i = 0; i < 5; i++) {
        digitalWrite(LED_PIN_1, HIGH);
        delay(100);
        digitalWrite(LED_PIN_1, LOW);
        delay(100);
      }
      lcd.setCursor(0,0);
      lcd.print(String(rScore));
    } else {
      lScore += 1;
      for (int i = 0; i < 5; i++) {
        digitalWrite(LED_PIN_2, HIGH);
        delay(100);
        digitalWrite(LED_PIN_2, LOW);
        delay(100);
      }
      lcd.setCursor(2,0);
      lcd.print(String(lScore));
    }
  }
}
SoftwareSerial getMotorSerial(int id){
  if(id == 0){
    return serialMotor0;
  }
  else if(id == 1){
    return serialMotor1;
  }
}

void sendCommandToMotor(SoftwareSerial serial, String command) {
  serial.println(command);
}

String moveMotorCommand(float position, float speed){
  String speedString = (speed == -1) ? "" : " " + String(speed, 2);
  return ("M" + String(position, 2) + speedString);
}

void playGame() {
  while (Serial.available() == 0) {}
  String side = Serial.readString();
  side.trim();
  Serial.println(side);
  if (side.equals("l")) {
    lScore += 1;
    for (int i = 0; i < 5; i++) {
      digitalWrite(LED_PIN_1, HIGH);
      delay(100);
      digitalWrite(LED_PIN_1, LOW);
      delay(100);
    }
    lcd.setCursor(0,0);
    lcd.print(String(lScore));
  } 
  if (side.equals("r")) {
    rScore += 1;
    for (int i = 0; i < 5; i++) {
      digitalWrite(LED_PIN_2, HIGH);
      delay(100);
      digitalWrite(LED_PIN_2, LOW);
      delay(100);
    }
    lcd.setCursor(2,0);
    lcd.print(String(rScore));
  }
}

void calibrateMotors(){
  bool m0Down = false;
  bool m0Up = false;
  bool m1Down = false;
  bool m1Up = false;
  String command_received = "";
  String command = "";
  int motor = -1;
  float position0 = 0;
  float position1 = 0;

  lcd.clear();
  lcd.print("Calibrating");
  lcd.setCursor(0, 1);
  lcd.print("motors");
  lcd.setCursor(0, 0);

  while(!(m0Down && m0Up && m1Down && m1Up)){
    command_received = readCommand();

    if (command_received.startsWith("cal ")){
      motor = command_received.substring(4, 5).toInt();
      command = command_received.substring(6);
    }
    float calibration_speed = 200;

    if(command.equals("up")){
      if (motor == 0) {
        position0 += STEP_INCREMENT;
        sendCommandToMotor(getMotorSerial(motor), moveMotorCommand(position0, calibration_speed));
      }
      else if (motor == 1) {
        position1 += STEP_INCREMENT;
        sendCommandToMotor(getMotorSerial(motor), moveMotorCommand(position1, calibration_speed));
      }
      command_received = "";
      command = "";
    }
    else if (command.equals("down")){
      if (motor == 0) {
        position0 -= STEP_INCREMENT;
        sendCommandToMotor(getMotorSerial(motor), moveMotorCommand(position0, calibration_speed));
      }
      else if (motor == 1) {
        position1 -= STEP_INCREMENT;
        sendCommandToMotor(getMotorSerial(motor), moveMotorCommand(position1, calibration_speed));
      }
      command_received = "";
      command = "";
    }
    else if (command.equals("done up")){
      if (motor == 0) {
        m0Up = true;
        maxLimitM0 = position0;
      }
      else if (motor == 1) {
        m1Up = true;
        maxLimitM1 = position1;
      }
      command_received = "";
      command = "";
    }
    else if (command.equals("done down")){
      if (motor == 0) {
        m0Down = true;
        minLimitM0 = position0;
      }
      else if (motor == 1) {
        m1Down = true;
        minLimitM1 = position1;
      }
      command_received = "";
      command = "";
    }
  }

  moveMotor(0, 512, 200);
  delay(10);
  moveMotor(1, 512, 200);
  lcd.clear();
}

String readCommand(){
  String command_received = "";
  if (Serial.available() > 0) {
      command_received = Serial.readStringUntil('\n');
      command_received.trim();
    }
    return command_received;
}