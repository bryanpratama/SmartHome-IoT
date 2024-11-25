#define PIR_PIN 23  // Pin yang digunakan untuk menghubungkan sensor AM312 ke ESP32

void setup() {
  Serial.begin(115200);  // Inisialisasi Serial Monitor
  pinMode(PIR_PIN, INPUT);  // Atur pin PIR sebagai input
  Serial.println("Sensor PIR AM312 siap digunakan...");
}

void loop() {
  int sensorValue = digitalRead(PIR_PIN);  // Membaca status dari sensor PIR

  if (sensorValue == HIGH) {
    Serial.println("Gerakan terdeteksi!");
  } else {
    Serial.println("Tidak ada gerakan.");
  }
  
  delay(1000);  // Tunggu 1 detik sebelum membaca ulang
}
