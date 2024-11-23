#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRutils.h>

// Pin untuk modul KY-022
const uint16_t RECV_PIN = 35; // Ganti sesuai pin yang Anda gunakan

// Objek IR receiver
IRrecv irrecv(RECV_PIN);
decode_results results;

void setup() {
  // Inisialisasi Serial Monitor
  Serial.begin(115200);
  Serial.println("Inisialisasi KY-022 Receiver...");

  // Mulai IR receiver
  irrecv.enableIRIn();
  Serial.println("IR Receiver siap menerima sinyal.");
}

void loop() {
  // Periksa apakah ada data IR yang diterima
  if (irrecv.decode(&results)) {
    Serial.println("Sinyal diterima!");

    // Tampilkan data sinyal dalam bentuk yang mudah dibaca
    Serial.println(resultToHumanReadableBasic(&results));

    // Opsional: Tampilkan data mentah
    // Serial.println(resultToSourceCode(&results));

    // Reset untuk membaca sinyal berikutnya
    irrecv.resume();
  }
}
