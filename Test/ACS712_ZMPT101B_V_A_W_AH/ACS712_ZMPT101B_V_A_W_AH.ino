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

// Fungsi untuk memformat angka ke format rupiah
String formatRupiah(float value) {
  String result = String((int)value);
  int len = result.length();
  int insertPosition = len - 3;

  // Tambahkan titik setiap 3 digit dari belakang
  while (insertPosition > 0) {
    result = result.substring(0, insertPosition) + "." + result.substring(insertPosition);
    insertPosition -= 3;
  }

  // Tambahkan dua digit desimal
  int fraction = (value - (int)value) * 100;
  result += "." + String(fraction < 10 ? "0" : "") + String(fraction);

  return "Rp " + result;
}

void setup() {
  Serial.begin(115200);
  pinMode(PIN_TEGANGAN, INPUT);
  pinMode(PIN_ARUS, INPUT);

  Serial.println("Kalibrasi offset dimulai...");

  // Kalibrasi ulang beberapa kali untuk stabilitas
  for (int i = 0; i < 3; i++) {
    offsetTegangan = kalibrasiOffset(PIN_TEGANGAN);
    offsetArus = kalibrasiOffset(PIN_ARUS);
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
  float teganganAC = bacaNilai(PIN_TEGANGAN, offsetTegangan, teganganKalibrasi);
  float arusAC = bacaNilai(PIN_ARUS, offsetArus, arusKalibrasi);

  // Hitung daya aktif (Watt)
  float dayaAC = teganganAC * arusAC;

  // Hitung waktu berlalu dalam jam
  unsigned long currentMillis = millis();
  float elapsedHours = (currentMillis - lastUpdateMillis) / 3600000.0;
  lastUpdateMillis = currentMillis;

  // Tambahkan energi ke total (kWh)
  totalEnergyKWh += (dayaAC * elapsedHours) / 1000.0;

  // Hitung biaya energi (Rp)
  float biayaSaatIni = totalEnergyKWh * tarifPerKWh;

  // Hitung biaya berdasarkan konsumsi daya saat ini
  float estimasiBiayaBulanan = (dayaAC / 1000.0) * tarifPerKWh * 24 * 30; // 24 jam x 30 hari

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
  Serial.print("Energi Total: ");
  Serial.print(totalEnergyKWh);
  Serial.println(" kWh");
  Serial.print("Biaya Energi Saat Ini: ");
  Serial.println(formatRupiah(biayaSaatIni));
  Serial.print("Estimasi Biaya Bulanan Berdasarkan Konsumsi Saat Ini: ");
  Serial.println(formatRupiah(estimasiBiayaBulanan));
  Serial.println("--------------------------");

  delay(1000); // Delay antar tampilan hasil
}

float kalibrasiOffset(int pin) {
  float total = 0;
  for (int i = 0; i < sampleCount; i++) {
    total += analogRead(pin);
    delay(delayPerSample);
  }
  return total / sampleCount; // Nilai rata-rata ADC
}

float bacaNilai(int pin, float offset, float kalibrasi) {
  float total = 0;
  for (int i = 0; i < sampleCount; i++) {
    int adcValue = analogRead(pin);
    float nilai = ((adcValue - offset) / 4095.0) * 3.3 * kalibrasi;
    total += nilai > 0 ? nilai : 0; // Pastikan nilai positif
    delay(delayPerSample);
  }
  return total / sampleCount;
}
