// Define pin numbers
#define BUTTON_PIN  36  // Pin untuk tombol
#define LED_BUILTIN 2   // Pin LED bawaan ESP32

// Variable untuk menyimpan status LED
bool ledState = false;

void setup() {
  // Inisialisasi pin untuk tombol sebagai input pull-up
  pinMode(BUTTON_PIN, INPUT);

  // Inisialisasi pin LED sebagai output
  pinMode(LED_BUILTIN, OUTPUT);

  // Matikan LED di awal
  digitalWrite(LED_BUILTIN, LOW);

  // Mulai Serial Monitor
  Serial.begin(115200);
}

void loop() {
  // Membaca status tombol
  static bool lastButtonState = HIGH;
  bool buttonState = digitalRead(BUTTON_PIN);

  // Deteksi perubahan status tombol
  if (buttonState == LOW && lastButtonState == HIGH) {
    // Tunda debounce
    delay(50);
    if (digitalRead(BUTTON_PIN) == LOW) {
      // Ubah status LED
      ledState = !ledState;
      digitalWrite(LED_BUILTIN, ledState ? HIGH : LOW);

      // Tampilkan pesan di Serial Monitor
      if (ledState) {
        Serial.println("Button di pencet: LED menyala");
      } else {
        Serial.println("Button di pencet: LED mati");
      }
    }
  }

  // Perbarui status tombol terakhir
  lastButtonState = buttonState;

  // Delay kecil untuk stabilitas
  delay(10);
}
