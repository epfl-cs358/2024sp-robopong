#include <ezButton.h>
#include <LCD_I2C.h>
#include <SoftwareSerial.h>

#define LED_PIN_1 12
#define LED_PIN_2 13

#define STEP_INCREMENT 0.1  // Initial radian increment for searching limits

SoftwareSerial serialMotor0(8, 9); // RX, TX
SoftwareSerial serialMotor1(10, 11);

float minLimitM0, minLimitM1 = -15.0;
float maxLimitM0, maxLimitM1 = 15.0;

long last = 0;

LCD_I2C lcd(0x3F, 16, 2); // address can change, check wiring on github issue for connecting screen
ezButton button1(6);  // next / player 1
ezButton button2(7); // enter / player 2

int curr;
String states[] = {"automated", "single player", "two player"};
bool modeSelected = false;
// for single player mode, player1 always human and player2 always computer
bool player1Ready = false;
bool player2Ready = false;
bool gameEnd = false;
bool restart = false;
int left = 0;
int right = 0;


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

  // calibrate here?


  if (button2.isPressed()) {
    modeSelected = true;
    lcd.print(states[curr]);
    lcd.setCursor(0,1);
    lcd.print("selected. Ready?");
  }

  // players confirm ready
  if (modeSelected) {
    if (curr == 1) {
      player2Ready = true;
    } else if (curr == 0) {
      player2Ready = true;
      player1Ready = true;
    }
  }

  while (modeSelected && !player1Ready) {
    button1.loop();
    digitalWrite(LED_PIN_1, HIGH);
    if (button1.isPressed()) {
      player1Ready = true;
    }
  }
  
  while (modeSelected && !player2Ready) {
    button2.loop();
    digitalWrite(LED_PIN_1, LOW);
    digitalWrite(LED_PIN_2, HIGH);

      if (button2.isPressed()) {
        player2Ready = true;
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
    lcd.print(String(left) + "-" + String(right));
    digitalWrite(LED_PIN_1, LOW);
    digitalWrite(LED_PIN_2, LOW);
  }

  // gameplay
  while (player1Ready && player2Ready && left < 5 && right < 5) {
    if (curr == 2) {
      if(millis()-last > 10){
        potentiometerValue0 = analogRead(A0);
        potentiometerValue1 = analogRead(A1);
        
        if (potentiometerValue0 == 1000) {
          left += 1;
          for (int i = 0; i < 5; i++) {
            digitalWrite(LED_PIN_1, HIGH);
            delay(100);
            digitalWrite(LED_PIN_1, LOW);
            delay(100);
          }
          lcd.setCursor(0,0);
          lcd.print(String(left));
        }
        if (potentiometerValue1 == 1000) {
          right += 1;
          for (int i = 0; i < 5; i++) {
            digitalWrite(LED_PIN_2, HIGH);
            delay(100);
            digitalWrite(LED_PIN_2, LOW);
            delay(100);
          }
          lcd.setCursor(2,0);
          lcd.print(String(right));
        }

        if (abs(prevValue0 - potentiometerValue0) > 0){
          moveMotor(0, potentiometerValue0, -1);
        }
        delay(10);
        if (abs(prevValue1 - potentiometerValue1) > 0){
          moveMotor(1, potentiometerValue1, -1);
        }
      }

      SerialBridge();
    } else{
      playGame();
    }
  }
  
  // ending sequence
  if (left == 5) {
    gameEnd = true;
    lcd.setCursor(0,0);
    lcd.print("Player one wins");
    lcd.setCursor(0,1);
    lcd.print("Continue?");
    for (int i = 0; i <10; i++) {
        digitalWrite(LED_PIN_1, HIGH);
        delay(100);
        digitalWrite(LED_PIN_1, LOW);
        delay(100);
    }
  } else if (right == 5) {
    gameEnd = true;
    lcd.setCursor(0,0);
    lcd.print("Player two wins");
    lcd.setCursor(0,1);
    lcd.print("Continue?");
    for (int i = 0; i < 10; i++) {
      digitalWrite(LED_PIN_2, HIGH);
      delay(100);
      digitalWrite(LED_PIN_2, LOW);
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
    left = 0;
    right = 0;
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


    if (side.equals("l")) {
      left += 1;
      for (int i = 0; i < 5; i++) {
        digitalWrite(LED_PIN_1, HIGH);
        delay(100);
        digitalWrite(LED_PIN_1, LOW);
        delay(100);
      }
      lcd.setCursor(0,0);
      lcd.print(String(left));
    } 
    if (side.equals("r")) {
      right += 1;
      for (int i = 0; i < 5; i++) {
        digitalWrite(LED_PIN_2, HIGH);
        delay(100);
        digitalWrite(LED_PIN_2, LOW);
        delay(100);
      }
      lcd.setCursor(2,0);
      lcd.print(String(right));
    }
}

void calibrateMotors(){
  bool m0Down, m0Up, m1Down, m1Up = false;
  String command_received = "";
  String command = "";
  int motor = -1;
  float position0, position1 = 0;

  lcd.clear();
  lcd.print("Calibrating");
  lcd.setCursor(0, 1);
  lcd.print("motors");
  lcd.setCursor(0, 0);

  while(m0Down && m0Up && m1Down && m1Up){
    command_received = readCommand();

    if (command_received.startsWith("cal ")){
      motor = command_received.substring(4, 5).toInt();
      command = command_received.substring(6);
    }

    if(command.equals("up")){
      if (motor == 0) {
        position0 += STEP_INCREMENT;
        sendCommandToMotor(getMotorSerial(motor), moveMotorCommand(position0, 40));
      }
      else if (motor == 1) {
        position1 += STEP_INCREMENT;
        sendCommandToMotor(getMotorSerial(motor), moveMotorCommand(position1, 40));
      }
      command_received = "";
      command = "";
    }
    else if (command.equals("down")){
      if (motor == 0) {
        position0 -= STEP_INCREMENT;
        sendCommandToMotor(getMotorSerial(motor), moveMotorCommand(position0, 40));
      }
      else if (motor == 1) {
        position1 -= STEP_INCREMENT;
        sendCommandToMotor(getMotorSerial(motor), moveMotorCommand(position1, 40));
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

  moveMotor(0, 512, -1);
  moveMotor(1, 512, -1);
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


void CVPlayerExample(){
  String command_received = readCommand();
  if (command_received.startsWith("game ")){
      int motor = command_received.substring(5,6).toInt();
      float command = command_received.substring(7).toFloat();
      moveMotor(motor, command, -1);
  }
}