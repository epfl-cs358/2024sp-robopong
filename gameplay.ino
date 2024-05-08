#include <ezButton.h>
#include <LCD_I2C.h>
#define LED_PIN_1 12
#define LED_PIN_2 13

LCD_I2C lcd(0x3F, 16, 2); // address can change, check wiring on github issue for connecting screen
ezButton button1(6);  // next
ezButton button2(7); // enter

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
    Serial.begin(9600);
}

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
    playGame();
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
