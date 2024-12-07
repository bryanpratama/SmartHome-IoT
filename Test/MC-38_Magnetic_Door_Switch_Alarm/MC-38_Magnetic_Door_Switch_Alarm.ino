const int doorSwitchPin = 33; // Pin untuk MC-38 Magnetic Door Switch
const int ledPin = 2;        // Pin LED internal ESP32 (GPIO 2)

int previousState = HIGH; // Menyimpan status sebelumnya (default: HIGH karena INPUT_PULLUP)

void setup() {
  // Inisialisasi Serial Monitor
  Serial.begin(115200);

  // Konfigurasi pin
  pinMode(doorSwitchPin, INPUT_PULLUP); // Door switch aktif LOW
  pinMode(ledPin, OUTPUT);

  // Pastikan LED mati saat awal
  digitalWrite(ledPin, LOW);
}

void loop() {
  // Baca status door switch
  int currentState = digitalRead(doorSwitchPin);

  // Jika status pintu berubah
  if (currentState != previousState) {
    previousState = currentState; // Perbarui status sebelumnya

    if (currentState == HIGH) { // Pintu terbuka
      Serial.println("Pintu terbuka - LED menyala selama 10 detik");
      digitalWrite(ledPin, HIGH); // Nyalakan LED
      delay(10000);               // Tunggu 10 detik
      digitalWrite(ledPin, LOW);  // Matikan LED
      Serial.println("LED mati setelah 10 detik");
    } else { // Pintu tertutup
      Serial.println("Pintu tertutup - LED mati");
      digitalWrite(ledPin, LOW); // Pastikan LED tetap mati
    }
  }

  delay(100); // Tambahkan sedikit delay untuk stabilitas pembacaan
}
