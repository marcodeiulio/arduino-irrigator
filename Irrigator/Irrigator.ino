const int enterSwitchPin = 2;
const int minusSwitchPin = 3;
const int plusSwitchPin = 4;

const int redLedPin = 9;
const int greenLedPin = 10;
const int blueLedPin = 11;

const int ledOffValue = LOW;
const int ledOnValue = 10;

enum Switches { NO_PRESS = 0,
                ENTER = 1,
                MINUS = 2,
                PLUS = 3 };

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
}

void loop() {
  switch (readSwitch()) {

    default:
    case NO_PRESS:
      ledOff();
      break;

    case ENTER:
      blue();
      break;

    case PLUS:
      green();
      break;

    case MINUS:
      red();
      break;
  }
}

Switches readSwitch() {
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

void ledOff() {
  analogWrite(redLedPin, ledOffValue);
  analogWrite(greenLedPin, ledOffValue);
  analogWrite(blueLedPin, ledOffValue);
}
void red() {
  analogWrite(redLedPin, ledOnValue);
  analogWrite(greenLedPin, ledOffValue);
  analogWrite(blueLedPin, ledOffValue);
}
void green() {
  analogWrite(redLedPin, ledOffValue);
  analogWrite(greenLedPin, ledOnValue);
  analogWrite(blueLedPin, ledOffValue);
}
void blue() {
  analogWrite(redLedPin, ledOffValue);
  analogWrite(greenLedPin, ledOffValue);
  analogWrite(blueLedPin, ledOnValue);
}
