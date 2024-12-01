#include <Arduino.h>

// Pin untuk modul ZMPT101B dan ACS712
#define PIN_TEGANGAN 36 // ZMPT101B
#define PIN_ARUS 34     // ACS712

// Variabel kalibrasi
float teganganKalibrasi = 100.0; // Ubah sesuai kalibrasi ZMPT101B
float arusKalibrasi = 10.0;      // Ubah sesuai kalibrasi ACS712

// Konfigurasi rata-rata pembacaan
const int sampleCount = 10; // Jumlah sampel untuk rata-rata
int delayPerSample = 100;   // Delay antar pembacaan (ms)

// Fungsi pembacaan tegangan AC
float bacaTegangan() {
  float total = 0;
  for (int i = 0; i < sampleCount; i++) {
    int adcValue = analogRead(PIN_TEGANGAN);
    float tegangan = (adcValue / 4095.0) * 3.3 * teganganKalibrasi;
    total += tegangan;
    delay(delayPerSample);
  }
  return total / sampleCount;
}

// Fungsi pembacaan arus AC
float bacaArus() {
  float total = 0;
  for (int i = 0; i < sampleCount; i++) {
    int adcValue = analogRead(PIN_ARUS);
    float arus = ((adcValue / 4095.0) * 3.3 - 2.5) * arusKalibrasi;
    total += abs(arus); // Pastikan nilai positif
    delay(delayPerSample);
  }
  return total / sampleCount;
}

void setup() {
  Serial.begin(115200);
  pinMode(PIN_TEGANGAN, INPUT);
  pinMode(PIN_ARUS, INPUT);
}

void loop() {
  float teganganAC = bacaTegangan();
  float arusAC = bacaArus();

  Serial.println("--------------------------");
  Serial.print("Tegangan AC: ");
  Serial.print(teganganAC);
  Serial.println(" V");
  Serial.print("Arus AC: ");
  Serial.print(arusAC);
  Serial.println(" A");
  Serial.println("--------------------------");

  delay(1000); // Delay antar tampilan hasil
}
