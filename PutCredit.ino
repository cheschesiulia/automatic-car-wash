#include <Wire.h>
#include <Adafruit_PN532.h>

#define SCK_PIN  13
#define MOSI_PIN 11
#define SS_PIN   10
#define MISO_PIN 12

Adafruit_PN532 nfc(SCK_PIN, MISO_PIN, MOSI_PIN, SS_PIN);

void setup(void) {
  Serial.begin(9600);
  Serial.println("Hello!");

  nfc.begin();

  uint32_t versiondata = nfc.getFirmwareVersion();
  if (!versiondata) {
    Serial.print("Didn't find PN53x board");
    while (1);
  }

  nfc.SAMConfig();
  
  Serial.println("Waiting for an NFC tag...");
}

void loop(void) {
  uint8_t success;
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };
  uint8_t uidLength;

  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);

  if (success) {
    Serial.println("Found an NFC tag!");

    Serial.print("UID Length: ");Serial.print(uidLength, DEC);Serial.println(" bytes");
    Serial.print("UID Value: ");
    for (uint8_t i=0; i < uidLength; i++) {
      Serial.print(" 0x");Serial.print(uid[i], HEX);
    }
    Serial.println("");

    uint8_t key[] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

    success = nfc.mifareclassic_AuthenticateBlock(uid, uidLength, 5, 0, key);
    if (success) {
      Serial.println("Authentication successful for block 5");

      uint8_t dataBlock5[16] = {1, 20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
      success = nfc.mifareclassic_WriteDataBlock(5, dataBlock5);
      if (success) {
        Serial.println("User ID and credit written to block 5");
      } else {
        Serial.println("Failed to write user ID to block 5");
      }
    } else {
      Serial.println("Authentication failed for block 5");
    }

    delay(1000);
  }
}
