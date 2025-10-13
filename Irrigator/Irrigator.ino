#include <LiquidCrystal.h>

LiquidCrystal lcd(12, 11, 6, 7, 8, 9);

int lcdColumns = 16;
int lcdRows = 2;

const int sensor1Pin = A0;
const int sensor2Pin = A1;

bool readingStatus = false;

// ============ SWITCHES MANAGEMENT ============

const int enterSwitchPin = 2;
const int minusSwitchPin = 3;
const int plusSwitchPin = 4;

enum SwitchState { NO_PRESS = 0,
                   ENTER = 1,
                   MINUS = 2,
                   PLUS = 3 };
SwitchState currValue = NO_PRESS;
SwitchState prevValue = NO_PRESS;

SwitchState readSwitch() {
  if (digitalRead(enterSwitchPin) == HIGH) {
    // short delay to avoid accidental multiple press
    delay(100);
    return ENTER;
  }
  if (digitalRead(minusSwitchPin) == HIGH) {
    delay(100);
    return MINUS;
  }
  if (digitalRead(plusSwitchPin) == HIGH) {
    delay(100);
    return PLUS;
  }
  return NO_PRESS;
}

// ============ MENU SYSTEM ============

enum MenuItemType {
  MENU_ITEM_SUBMENU,
  MENU_ITEM_ACTION,
  MENU_ITEM_BACK
};

struct MenuItem;
typedef void (*ActionCallback)();

struct MenuItem {
  const char* label;
  MenuItemType type;
  MenuItem** children;
  int childCount;
  ActionCallback action;
  MenuItem* parent;
};

class MenuNavigator {
private:
  MenuItem* currentMenu;
  int currentIndex;
  int scrollOffset;
  const int displayRows;

public:
  MenuNavigator(MenuItem* root, int rows = 2)
    : currentMenu(root), currentIndex(0), scrollOffset(0), displayRows(rows) {}

  void navigate(int direction) {
    if (!currentMenu || currentMenu->childCount == 0) return;

    currentIndex += direction;

    if (currentIndex < 0) {
      currentIndex = currentMenu->childCount - 1;
    } else if (currentIndex >= currentMenu->childCount) {
      currentIndex = 0;
    }

    if (currentIndex < scrollOffset) {
      scrollOffset = currentIndex;
    } else if (currentIndex >= scrollOffset + displayRows) {
      scrollOffset = currentIndex - displayRows + 1;
    }
  }

  void select() {
    if (!currentMenu || currentMenu->childCount == 0) return;

    MenuItem* selected = currentMenu->children[currentIndex];

    switch (selected->type) {
      case MENU_ITEM_SUBMENU:

        if (selected->action) {
          selected->action();
        }

        currentMenu = selected;
        currentIndex = 0;
        scrollOffset = 0;
        break;

      case MENU_ITEM_ACTION:
        if (selected->action) selected->action();
        break;

      case MENU_ITEM_BACK:
        if (currentMenu && currentMenu->parent) {
          currentMenu = currentMenu->parent;
          currentIndex = 0;
          scrollOffset = 0;
        }
        break;
    }
  }

  void getVisibleItems(MenuItem** items, int& count) {
    count = 0;
    if (!currentMenu || currentMenu->childCount == 0) return;

    for (int i = scrollOffset; i < scrollOffset + displayRows && i < currentMenu->childCount; i++) {
      items[count++] = currentMenu->children[i];
    }
  }

  int getCurrentIndex() {
    return currentIndex;
  }
  int getScrollOffset() {
    return scrollOffset;
  }
};

// ============ MENU CALLBACKS ============


// ============ SENSORS MANAGEMENT ============

int sensor1HighValue = 1023;
int sensor2HighValue = 1023;
int sensor1LowValue = 0;
int sensor2LowValue = 0;

char s1PrintedValues[15] = "null";
char s2PrintedValues[15] = "null";

void calibrateMenuPrintedValues() {
  sprintf(s1PrintedValues, "S1 %d | %d", sensor1HighValue, sensor1LowValue);
  sprintf(s2PrintedValues, "S2 %d | %d", sensor2HighValue, sensor2LowValue);
}

int readSensor(int sensor, bool returnRaw = false) {
  if (!readingStatus || !sensor)
    return 0;

  String sensorNumber = (String)sensor;
  int pin;
  int highVal;
  int lowVal;
  switch (sensor) {
    default:
      return 0;
      break;
    case 1:
      pin = sensor1Pin;
      highVal = sensor1HighValue;
      lowVal = sensor1LowValue;
      break;
    case 2:
      pin = sensor2Pin;
      highVal = sensor2HighValue;
      lowVal = sensor2LowValue;
      break;
  };

  int val = analogRead(pin);

  if (returnRaw)
    return val;

  int mappedValue;
  mappedValue = map(val, lowVal, highVal, 0, 100);
  return mappedValue;
}

// remapHighOrLow -> 1 to remap High, 0 to remap Low
void remapSensor(int sensor, int remapHighOrLow) {

  Serial.print("Entering remapSensor \n");
  Serial.print("remapHighOrLow = " + (String)remapHighOrLow + "\n");

  if (!readingStatus || !sensor)
    return 0;

  String sensorNumber = (String)sensor;
  int pin;
  int* highValPtr;
  int* lowValPtr;
  switch (sensor) {
    case 1:
      pin = sensor1Pin;
      highValPtr = &sensor1HighValue;
      lowValPtr = &sensor1LowValue;
      break;
    case 2:
      pin = sensor2Pin;
      highValPtr = &sensor2HighValue;
      lowValPtr = &sensor2LowValue;
      break;
    default:
      return;
  };

  int val = analogRead(pin);

  if (remapHighOrLow == 1) {
    *highValPtr = val;
    Serial.print("Remapped High S" + sensorNumber + " -> " + (String)*highValPtr + "\n");
    Serial.print("highValPtr = " + (String)*highValPtr + "; sensor1HighValue = " + (String)sensor1HighValue + "\n");
  } else if (remapHighOrLow == 0) {
    *lowValPtr = val;
    Serial.print("Remapped Low S" + sensorNumber + " -> " + (String)*lowValPtr + "\n");
    Serial.print("lowValPtr = " + (String)*lowValPtr + "; sensor1LowValue = " + (String)sensor1LowValue + "\n");
  } else {
    Serial.print("Invalid remapHighOrLow value: " + (String)remapHighOrLow + " \n");
  }

  Serial.print("Exiting remapSensor \n");

  return;
}

void startAll() {
  readingStatus = true;
  int s1Val;
  int s2Val;

  do {
    s1Val = readSensor(1);
    s2Val = readSensor(2);

    lcd.clear();
    lcd.print("S1 " + (String)s1Val + "% S2 " + (String)s2Val + "%");
    lcd.setCursor(0, 1);
    lcd.print("Press to exit");

    delay(1000);
  } while (readSwitch() == NO_PRESS);

  readingStatus = false;
}

// remapHighOrLow -> 1 to remap High, 0 to remap Low
bool stepCalibrate(int sensor = 1, int remapHighOrLow = 1) {

  Serial.print("Entering calibrate \n");
  Serial.print("remapHighOrLow = " + (String)remapHighOrLow + "\n");

  readingStatus = true;
  int val;
  String perc;

  switch (remapHighOrLow) {
    default:
    case 1:
      perc = "100%";
      break;
    case 0:
      perc = "0%";
      break;
  }

  SwitchState switchState;
  do {
    val = readSensor(sensor, true);
    Serial.print("S" + (String)sensor + " reading:" + (String)val + "\n");

    lcd.clear();
    lcd.print("S" + (String)sensor + " " + perc + "-> " + (String)val);
    lcd.setCursor(0, 1);
    lcd.print("E->Save M->cancel");

    delay(500);
    switchState = readSwitch();
  } while (switchState == NO_PRESS);

  if (switchState != ENTER) {
    return false;
  }
  // only remap if ENTER is pressed
  remapSensor(sensor, remapHighOrLow);

  readingStatus = false;

  Serial.print("Exiting calibrate \n");
  return true;
}

void fullCalibrate(int sensor) {
  bool ok = false;
  // Calibrate High Value (100%)
  ok = stepCalibrate(sensor, 1);
  if (ok) {
    // Calibrate Low Value (0%)
    stepCalibrate(sensor, 0);
  }
  calibrateMenuPrintedValues();
  return;
}

void startS1Calibration() {
  fullCalibrate(1);
}

void startS2Calibration() {
  fullCalibrate(2);
}

void viewSensor1() {
  readingStatus = true;
  int val = readSensor(1);

  lcd.clear();
  lcd.print("Sensor 1: " + (String)val + "%");
  delay(2000);
  readingStatus = false;
}

void viewSensor2() {
  readingStatus = true;
  int val = readSensor(2);

  lcd.clear();
  lcd.print("Sensor 2: " + (String)val + "%");
  delay(2000);
  readingStatus = false;
}

void viewSensor3() {
  readingStatus = true;
  int val = readSensor(3);

  lcd.clear();
  lcd.print("Sensor 3: " + (String)val + "%");
  delay(2000);
  readingStatus = false;
}

void viewSensor4() {
  readingStatus = true;
  int val = readSensor(4);

  lcd.clear();
  lcd.print("Sensor 4: " + (String)val + "%");
  delay(2000);
  readingStatus = false;
}

// ============ MENU STRUCTURE ============

// Main Menu
MenuItem mainMenu = { "Main Menu", MENU_ITEM_SUBMENU, nullptr, 0, nullptr, nullptr };

// Main menu children
MenuItem startMenu = { "Start", MENU_ITEM_ACTION, nullptr, 0, startAll, &mainMenu };
MenuItem sensorsMenu = { "Sensors", MENU_ITEM_SUBMENU, nullptr, 0, nullptr, &mainMenu };
MenuItem settingsMenu = { "Settings", MENU_ITEM_SUBMENU, nullptr, 0, nullptr, &mainMenu };

// Sensors submenu children
MenuItem sensor1Item = { "Sensor 1", MENU_ITEM_ACTION, nullptr, 0, viewSensor1, &sensorsMenu };
MenuItem sensor2Item = { "Sensor 2", MENU_ITEM_ACTION, nullptr, 0, viewSensor2, &sensorsMenu };
MenuItem sensor3Item = { "Sensor 3", MENU_ITEM_ACTION, nullptr, 0, viewSensor3, &sensorsMenu };
MenuItem sensor4Item = { "Sensor 4", MENU_ITEM_ACTION, nullptr, 0, viewSensor4, &sensorsMenu };
MenuItem sensorBackItem = { "< Back", MENU_ITEM_BACK, nullptr, 0, nullptr, &sensorsMenu };

// Settings submenu children
MenuItem settingCalibrateMenu = { "Calibrate", MENU_ITEM_SUBMENU, nullptr, 3, calibrateMenuPrintedValues, &settingsMenu };
MenuItem settingThresholdsItem = { "Thresholds", MENU_ITEM_ACTION, nullptr, 0, nullptr, &settingsMenu };
MenuItem settingBackItem = { "< Back", MENU_ITEM_BACK, nullptr, 0, nullptr, &settingsMenu };

// Calibrate submenu children
MenuItem calibrateS1Item = { s1PrintedValues, MENU_ITEM_ACTION, nullptr, 0, startS1Calibration, &settingCalibrateMenu };
MenuItem calibrateS2Item = { s2PrintedValues, MENU_ITEM_ACTION, nullptr, 0, startS2Calibration, &settingCalibrateMenu };
MenuItem calibrateBackItem = { "< Back", MENU_ITEM_BACK, nullptr, 0, nullptr, &settingsMenu };


// Is there a workaround for this?
void setupMenu() {
  // Build main menu
  static MenuItem* mainChildren[] = { &startMenu, &sensorsMenu, &settingsMenu };
  mainMenu.children = mainChildren;
  mainMenu.childCount = 3;

  // Build sensors submenu
  static MenuItem* sensorChildren[] = { &sensor1Item, &sensor2Item, &sensor3Item, &sensor4Item, &sensorBackItem };
  sensorsMenu.children = sensorChildren;
  sensorsMenu.childCount = 5;

  // Build settings submenu
  static MenuItem* settingsChildren[] = { &settingCalibrateMenu, &settingThresholdsItem, &settingBackItem };
  settingsMenu.children = settingsChildren;
  settingsMenu.childCount = 3;

  // Build calibrate submenu
  static MenuItem* calibrateChildren[] = { &calibrateS1Item, &calibrateS2Item, &calibrateBackItem };
  settingCalibrateMenu.children = calibrateChildren;
  settingCalibrateMenu.childCount = 3;
}

MenuNavigator nav(&mainMenu, lcdRows);


// ============ ARDUINO ============

void setup() {

  Serial.begin(9600);

  pinMode(sensor1Pin, INPUT);

  pinMode(enterSwitchPin, INPUT);
  pinMode(plusSwitchPin, INPUT);
  pinMode(minusSwitchPin, INPUT);

  lcd.begin(lcdColumns, lcdRows);

  setupMenu();

  updateDisplay();
}

void loop() {
  currValue = readSwitch();
  if (currValue == prevValue || readingStatus)
    return;

  switch (currValue) {
    case ENTER:
      nav.select();
      updateDisplay();
      break;

    case PLUS:
      nav.navigate(1);
      updateDisplay();
      break;

    case MINUS:
      nav.navigate(-1);
      updateDisplay();
      break;

    default:
      break;
  }

  prevValue = currValue;
}

void updateDisplay() {
  MenuItem* items[lcdRows];
  int count;
  nav.getVisibleItems(items, count);

  lcd.clear();
  for (int i = 0; i < count; i++) {
    lcd.setCursor(0, i);
    lcd.print((i + nav.getScrollOffset() == nav.getCurrentIndex()) ? ">" : " ");
    lcd.print(items[i]->label);
  }
}