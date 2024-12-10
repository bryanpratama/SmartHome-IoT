#include <Arduino.h>
#include "PinDefinitionsAndMore.h"
#include <IRremote.hpp> // include the library

void setup() {
    pinMode(LED_BUILTIN, OUTPUT);

    Serial.begin(115200);
    while (!Serial)
        ; // Wait for Serial to become available. Is optimized away for some cores.

    Serial.println(F("START SimpleSender"));
    Serial.println(F("Ketik angka di Serial Monitor untuk mengirim sinyal:"));
    Serial.println(F("1 = Merah, 2 = Hijau, 3 = Biru, 4 = Putih"));

    IrSender.begin(); // Initialize IR sender
    disableLEDFeedback(); // Disable feedback LED
}

void loop() {
    if (Serial.available()) {
        String input = Serial.readStringUntil('\n'); // Baca input dari Serial Monitor
        input.trim(); // Hapus spasi atau karakter newline yang tidak diinginkan
        uint8_t command = 0; // Default command

        if (input == "1") {
            Serial.println(F("Mengirim warna: Merah"));
            command = 0x58; // Kode Merah
        } else if (input == "2") {
            Serial.println(F("Mengirim warna: Hijau"));
            command = 0x59; // Kode Hijau
        } else if (input == "3") {
            Serial.println(F("Mengirim warna: Biru"));
            command = 0x45; // Kode Biru
        } else if (input == "4") {
            Serial.println(F("Mengirim warna: Putih"));
            command = 0x44; // Kode Putih
        } else {
            Serial.println(F("Input tidak valid! Ketik 1, 2, 3, atau 4."));
            return;
        }

        // Kirim sinyal IR dengan kode yang sesuai
        IrSender.sendNEC(0x00, command, 0);
        delay(500); // Tambahkan delay untuk memastikan sinyal selesai dikirim
    }
}
