#include <Arduino.h>

// Pin untuk modul ZMPT101B dan ACS712
#define PIN_TEGANGAN 36 // ZMPT101B
#define PIN_ARUS 34     // ACS712

// Variabel kalibrasi
float teganganKalibrasi = 100.0; // Kalibrasi ZMPT101B
float arusKalibrasi = 10.0;      // Kalibrasi ACS712

// Variabel untuk menyimpan offset
float offsetTegangan = 0;
float offsetArus = 0;

// Konfigurasi rata-rata pembacaan
const int sampleCount = 10; // Jumlah sampel untuk rata-rata
int delayPerSample = 100;   // Delay antar pembacaan (ms)

// Variabel untuk energi dan biaya
float totalEnergyKWh = 0.0; // Total energi dalam kWh
const float tarifPerKWh = 1352.0; // Tarif listrik (Rp/kWh)
unsigned long lastUpdateMillis = 0;

// Fungsi kalibrasi offset tegangan
float kalibrasiTegangan() {
  float total = 0;
  for (int i = 0; i < sampleCount; i++) {
    int adcValue = analogRead(PIN_TEGANGAN);
    total += adcValue;
    delay(delayPerSample);
  }
  return total / sampleCount; // Nilai rata-rata ADC
}

// Fungsi kalibrasi offset arus
float kalibrasiArus() {
  float total = 0;
  for (int i = 0; i < sampleCount; i++) {
    int adcValue = analogRead(PIN_ARUS);
    total += adcValue;
    delay(delayPerSample);
  }
  return total / sampleCount; // Nilai rata-rata ADC
}

// Fungsi pembacaan tegangan AC
float bacaTegangan() {
  float total = 0;
  for (int i = 0; i < sampleCount; i++) {
    int adcValue = analogRead(PIN_TEGANGAN);
    float tegangan = ((adcValue - offsetTegangan) / 4095.0) * 3.3 * teganganKalibrasi;
    total += tegangan > 0 ? tegangan : 0; // Pastikan nilai positif
    delay(delayPerSample);
  }
  return total / sampleCount;
}

// Fungsi pembacaan arus AC
float bacaArus() {
  float total = 0;
  for (int i = 0; i < sampleCount; i++) {
    int adcValue = analogRead(PIN_ARUS);
    float arus = ((adcValue - offsetArus) / 4095.0) * 3.3 * arusKalibrasi;
    total += arus > 0 ? arus : 0; // Pastikan nilai positif
    delay(delayPerSample);
  }
  return total / sampleCount;
}

void setup() {
  Serial.begin(115200);
  pinMode(PIN_TEGANGAN, INPUT);
  pinMode(PIN_ARUS, INPUT);

  Serial.println("Kalibrasi offset dimulai...");

  // Kalibrasi ulang beberapa kali untuk stabilitas
  for (int i = 0; i < 3; i++) {
    offsetTegangan = kalibrasiTegangan();
    offsetArus = kalibrasiArus();
    delay(1000); // Tunggu sebelum pengambilan ulang
  }

  Serial.println("Kalibrasi selesai!");
  Serial.print("Offset Tegangan: ");
  Serial.println(offsetTegangan);
  Serial.print("Offset Arus: ");
  Serial.println(offsetArus);

  lastUpdateMillis = millis();
}

void loop() {
  float teganganAC = bacaTegangan();
  float arusAC = bacaArus();

  // Hitung daya aktif (Watt)
  float dayaAC = teganganAC * arusAC;

  // Hitung daya semu (VA)
  float dayaSemu = teganganAC * arusAC;

  // Hitung waktu berlalu dalam jam
  unsigned long currentMillis = millis();
  float elapsedHours = (currentMillis - lastUpdateMillis) / 3600000.0;
  lastUpdateMillis = currentMillis;

  // Tambahkan energi ke total (kWh)
  totalEnergyKWh += (dayaAC * elapsedHours) / 1000.0;

  // Hitung biaya energi (Rp)
  float biayaSaatIni = totalEnergyKWh * tarifPerKWh;

  // Perkiraan biaya per bulan (30 hari)
  float biayaPerBulan = biayaSaatIni * 30 / elapsedHours;

  // Tampilkan hasil ke Serial Monitor
  Serial.println("--------------------------");
  Serial.print("Tegangan AC: ");
  Serial.print(teganganAC);
  Serial.println(" V");
  Serial.print("Arus AC: ");
  Serial.print(arusAC);
  Serial.println(" A");
  Serial.print("Daya Aktif: ");
  Serial.print(dayaAC);
  Serial.println(" W");
  Serial.print("Daya Semu: ");
  Serial.print(dayaSemu);
  Serial.println(" VA");
  Serial.print("Energi Total: ");
  Serial.print(totalEnergyKWh);
  Serial.println(" kWh");
  Serial.print("Biaya Energi Saat Ini: Rp ");
  Serial.print(biayaSaatIni);
  Serial.println();
  Serial.print("Perkiraan Biaya Bulanan: Rp ");
  Serial.print(biayaPerBulan);
  Serial.println();
  Serial.println("--------------------------");

  delay(1000); // Delay antar tampilan hasil
}
