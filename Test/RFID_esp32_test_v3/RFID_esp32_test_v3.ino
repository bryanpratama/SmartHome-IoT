#include <MFRC522v2.h>
#include <MFRC522DriverSPI.h>
#include <MFRC522DriverPinSimple.h>
#include <MFRC522Debug.h>

MFRC522DriverPinSimple ss_1_pin(5);  // Pin SPI SS
MFRC522DriverPinSimple ss_2_pin(8);  // Pin SPI SS 2

MFRC522DriverSPI driver_1{ss_1_pin};
MFRC522DriverSPI driver_2{ss_2_pin};

MFRC522 readers[]{driver_1, driver_2};  // Create MFRC522 instance.

void setup() {
  Serial.begin(115200);  // Initialize serial communications with the PC for debugging.
  while (!Serial);       // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4).

  for (MFRC522 reader : readers) {
    reader.PCD_Init(); // Init each MFRC522 card.
    Serial.print(F("Reader "));
    static uint8_t i = 0;
    i++;
    Serial.print(i);
    Serial.print(F(": "));
    MFRC522Debug::PCD_DumpVersionToSerial(reader, Serial);
  }
}

void loop() {
  // Look for new cards.
  for (MFRC522 reader : readers) {
    if (reader.PICC_IsNewCardPresent() && reader.PICC_ReadCardSerial()) {
      Serial.print(F("Reader "));
      static uint8_t i = 0;
      i++;
      Serial.print(i);

      // Show some details of the PICC (that is: the tag/card).
      Serial.print(F(": Card UID:"));
      MFRC522Debug::PrintUID(Serial, reader.uid);
      Serial.println();

      Serial.print(F("PICC type: "));
      MFRC522::PICC_Type piccType = reader.PICC_GetType(reader.uid.sak);
      Serial.println(MFRC522Debug::PICC_GetTypeName(piccType));

      // Halt PICC.
      reader.PICC_HaltA();
      // Stop encryption on PCD.
      reader.PCD_StopCrypto1();
    }
  }
}
