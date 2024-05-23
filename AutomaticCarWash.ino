#include <Keypad.h>
#include <LiquidCrystal_I2C.h>

// Keypad
const byte ROWS = 4;
const byte COLS = 3;
char keys[ROWS][COLS] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};

byte rowPins[ROWS] = {8, 7, 6, 5};
byte colPins[COLS] = {4, 3, 2};

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

bool on = false;

// LCD
LiquidCrystal_I2C lcd(0x27, 20, 4);

void setup() {
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Press * to begin");
}

void loop() {
  char key = keypad.getKey();

  if (key == '*') {
    on = true;
  } else if (key && starPressed) {
    Serial.println(key);
  }
}
#include <Keypad.h>
#include <LiquidCrystal_I2C.h>

// Keypad
const byte ROWS = 4;
const byte COLS = 3;
char keys[ROWS][COLS] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};

byte rowPins[ROWS] = {8, 7, 6, 5};
byte colPins[COLS] = {4, 3, 2};

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

String number = "";

// LCD
LiquidCrystal_I2C lcd(0x27, 20, 4);

void setup() {
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Press * to begin");
}

void loop() {
  char key = keypad.getKey();

  if (key == '*') {
    number = "";
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Choose program");

    while(true) {
      char key = keypad.getKey();
  
      if (key != NO_KEY) {
        if (key != '#') {
          number += key;
          Serial.print(key);
        } else {
          break;
        }
      }
    }

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Program " + number);
    lcd.setCursor(0, 1);
    lcd.print("chosen");
  }
}
