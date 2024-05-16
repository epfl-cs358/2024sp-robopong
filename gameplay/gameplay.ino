#include <ezButton.h>
#include <LCD_I2C.h>
#include <SoftwareSerial.h>

#define LED_PIN_1 12
#define LED_PIN_2 13

#define STEP_INCREMENT 1.0  // Initial radian increment for searching limits

SoftwareSerial serialMotor0(8, 9); // RX, TX
SoftwareSerial serialMotor1(10, 11);

float minLimitM0 = -15.0;
float maxLimitM0 = 15.0;

float minLimitM1 = -15.0;
float maxLimitM1 = 15.0;

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
        delay(5);
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
  if (id == 1){
    float range = maxLimitM1 - minLimitM1;
    float targetPosition = minLimitM1 + (position / 1023.0) * range;
    serialMotor1.println("M" + String(targetPosition, 2) + speedString);
  }
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
