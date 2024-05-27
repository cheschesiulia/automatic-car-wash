#include <Keypad.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <Adafruit_PN532.h>
#include <Servo.h>

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

// NFC reader
#define SCK_PIN  13
#define MOSI_PIN 11
#define SS_PIN   10
#define MISO_PIN 12

Adafruit_PN532 nfc(SCK_PIN, MISO_PIN, MOSI_PIN, SS_PIN);

//Servo
Servo servoMotor;

// Logic variables
bool choose_program = true;
bool pay = false;
bool start_wash = false;

long totalMillis = 0;
int display_secs_left = 0;

void setup() {
  Serial.begin(9600);

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Press * to begin");

  nfc.begin();
  uint32_t versiondata = nfc.getFirmwareVersion();
  if (!versiondata) {
    Serial.print("Didn't find PN53x board");
  }

  nfc.SAMConfig();

  servoMotor.attach(9);
}

void loop() {
  if (choose_program) {
    char key = keypad.getKey();

    if (key == '*') {
      number = "";
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Choose program");
      lcd.setCursor(0, 1);
      lcd.print("and press #");

      while(true) {
        char key = keypad.getKey();
    
        if (key != NO_KEY) {
          if (key != '#') {
            if (key != '*') {
              number += key;
              Serial.print(key);
            }
          } else {
            break;
          }
        }
      }

      pay = true;

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("For program " + number);
      lcd.setCursor(0, 1);
      lcd.print("Pay " + number + "RON");
    }
  }

  if (pay) {
    uint8_t success;
    uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };
    uint8_t uidLength;

    success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength, 100);

    if (success) {
      uint8_t key[] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

      success = nfc.mifareclassic_AuthenticateBlock(uid, uidLength, 5, 0, key);
      if (success) {
        uint8_t dataBlock5[16];
        success = nfc.mifareclassic_ReadDataBlock(5, dataBlock5);
        if (success) {
          Serial.print("Client: ");
          Serial.println(dataBlock5[0]);
          Serial.print("Credit: ");
          Serial.println(dataBlock5[1]);

          if (dataBlock5[1] >= number.toInt()) {
            Serial.println("Enough credit. Paid");

            choose_program = false;
            pay = false;

            uint8_t writeBlock5[16] = {dataBlock5[0], dataBlock5[1] - number.toInt(), 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
            success = nfc.mifareclassic_WriteDataBlock(5, writeBlock5);
            if (success) {
              Serial.println("User ID and credit written to block 5");
            } else {
              Serial.println("Failed to write user ID to block 5");
            }

            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("OK");

            totalMillis = number.toInt() * 1000;
            pay = false;
            choose_program = false;
            start_wash = true;

            delay(3000);
          } else {
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Not enough");
            lcd.setCursor(0, 1);
            lcd.print("credit");
          }
        } else {
          Serial.println("Failed to read");
        }
      } else {
        Serial.println("Authentication failed");
      }
    }
  }

  if (start_wash) {
    display_secs_left = totalMillis/1000;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Washing...");
    lcd.setCursor(0, 1);
    lcd.print(String(display_secs_left) + " seconds left");

    servoMotor.write(180);
    delay(500);
    totalMillis = totalMillis - 500;
    servoMotor.write(0);
    delay(500);
    totalMillis = totalMillis - 500;

    if (totalMillis <= 0) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Done");

      delay(3000);

      choose_program = true;
      pay = false;
      start_wash = false;

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Press * to start");
    }
  }
}