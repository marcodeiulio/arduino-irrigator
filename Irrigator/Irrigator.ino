#include <LiquidCrystal.h>
LiquidCrystal lcd(12, 11, 6, 7, 8, 9);

const int enterSwitchPin = 2;
const int minusSwitchPin = 3;
const int plusSwitchPin = 4;

const int redLedPin = 9;
const int greenLedPin = 10;
const int blueLedPin = 11;

const int ledOffValue = LOW;
const int ledOnValue = 10;

enum SwitchState { NO_PRESS = 0,
                   ENTER = 1,
                   MINUS = 2,
                   PLUS = 3 };

SwitchState currValue = NO_PRESS;
SwitchState prevValue = NO_PRESS;

void setup() {
  pinMode(enterSwitchPin, INPUT);
  pinMode(plusSwitchPin, INPUT);
  pinMode(minusSwitchPin, INPUT);

  pinMode(redLedPin, OUTPUT);
  pinMode(greenLedPin, OUTPUT);
  pinMode(blueLedPin, OUTPUT);

  analogWrite(redLedPin, ledOffValue);
  analogWrite(greenLedPin, ledOffValue);
  analogWrite(blueLedPin, ledOffValue);

  lcd.begin(16, 2);
  updateScreen("Adjust contrast", "as preferred");
  delay(2000);
  updateScreen("Please", "press a button");
}

void loop() {

  currValue = readSwitch();

  if (currValue == prevValue) {
    return;
  }

  switch (currValue) {

    default:
    case NO_PRESS:
      updateScreen("Please", "press a button");
      break;

    case ENTER:
      updateScreen("You pressed", "ENTER");
      break;

    case PLUS:
      updateScreen("You pressed", "PLUS");
      break;

    case MINUS:
      updateScreen("You pressed", "MINUS");
      break;
  }

  prevValue = currValue;
}

void updateScreen(String firstRow, String secondRow) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(firstRow);
  lcd.setCursor(0, 1);
  lcd.print(secondRow);
}

SwitchState readSwitch() {
  if (digitalRead(enterSwitchPin) == HIGH) {
    return ENTER;

  } else if (digitalRead(minusSwitchPin) == HIGH) {
    return MINUS;

  } else if (digitalRead(plusSwitchPin) == HIGH) {
    return PLUS;

  } else {
    return NO_PRESS;
  }
}