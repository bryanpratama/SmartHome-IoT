#include <WiFi.h>
#include <WebServer.h>

// Konfigurasi nama Wi-Fi dan password untuk Access Point
const char* ssid = "ESP32_AP";      // Nama Wi-Fi
const char* password = "12345678";  // Password Wi-Fi

// Web server di port 80
WebServer server(80);

// Pin modul relay
const int relayPins[] = {2, 4, 16, 17, 5, 18, 19, 21}; // Pin GPIO untuk IN1-IN8
bool relayStatus[8] = {false, false, false, false, false, false, false, false}; // Status setiap relay

// Pin untuk MC-38 Magnetic Door Switch dan LED
const int doorSwitchPin = 33; // Pin untuk MC-38 Magnetic Door Switch
const int ledPin = 22;        // Pin LED tambahan untuk MC-38
int previousState = HIGH;     // Menyimpan status sebelumnya

// Fungsi untuk menangani halaman utama
void handleRoot() {
  String html = "<h1>Kontrol Relay 8-Channel</h1>";
  for (int i = 0; i < 8; i++) {
    html += "<p>Relay " + String(i + 1) + ": " + (relayStatus[i] ? "Nyala" : "Mati") + "</p>";
    html += "<button onclick=\"location.href='/toggle?relay=" + String(i) + "'\">Toggle Relay " + String(i + 1) + "</button><br>";
  }
  server.send(200, "text/html", html);
}

// Fungsi untuk toggle relay
void handleToggle() {
  if (server.hasArg("relay")) {
    int relayIndex = server.arg("relay").toInt();
    if (relayIndex >= 0 && relayIndex < 8) {
      relayStatus[relayIndex] = !relayStatus[relayIndex]; // Toggle status
      digitalWrite(relayPins[relayIndex], relayStatus[relayIndex] ? LOW : HIGH); // Relay aktif LOW
    }
  }
  server.sendHeader("Location", "/"); // Redirect ke halaman utama
  server.send(303); // Status HTTP untuk redirect
}

void setup() {
  // Serial Monitor untuk debugging
  Serial.begin(115200);

  // Menyiapkan pin relay sebagai output dan mematikannya di awal
  for (int i = 0; i < 8; i++) {
    pinMode(relayPins[i], OUTPUT);
    digitalWrite(relayPins[i], HIGH); // Relay aktif LOW, jadi default HIGH (mati)
  }

  // Konfigurasi pin untuk MC-38 dan LED
  pinMode(doorSwitchPin, INPUT_PULLUP); // Door switch aktif LOW
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW); // Pastikan LED mati saat awal

  // Menyiapkan ESP32 sebagai Access Point
  WiFi.softAP(ssid, password);
  Serial.println("Access Point dimulai");
  Serial.print("Nama Wi-Fi: ");
  Serial.println(ssid);
  Serial.print("Password: ");
  Serial.println(password);

  // Tampilkan alamat IP Access Point
  IPAddress IP = WiFi.softAPIP();
  Serial.print("Alamat IP Access Point: ");
  Serial.println(IP);

  // Konfigurasi server
  server.on("/", handleRoot);  // Halaman utama
  server.on("/toggle", handleToggle); // Toggle relay
  server.begin();                      // Mulai server
  Serial.println("Server dimulai");
}

void loop() {
  server.handleClient(); // Menangani permintaan dari klien

  // Logika MC-38 Magnetic Door Switch
  int currentState = digitalRead(doorSwitchPin); // Membaca status pintu

  if (currentState != previousState) { // Jika status pintu berubah
    previousState = currentState; // Perbarui status sebelumnya

    if (currentState == HIGH) { // Pintu terbuka
      Serial.println("Pintu terbuka - Relay  menyala selama 10 detik");
//      digitalWrite(ledPin, HIGH); // Nyalakan LED
      pinMode(relayPins[1], OUTPUT);
    digitalWrite(relayPins[1], LOW);
      delay(10000);               // Tunggu 10 detik
//      digitalWrite(ledPin, LOW);  // Matikan LED
      pinMode(relayPins[1], OUTPUT);
    digitalWrite(relayPins[1], HIGH);
      Serial.println("Relay  mati setelah 10 detik");
    } else { // Pintu tertutup
      Serial.println("Pintu tertutup - Relay mati");
//      digitalWrite(ledPin, LOW); // Pastikan LED tetap mati
      pinMode(relayPins[1], OUTPUT);
    digitalWrite(relayPins[1], HIGH);
    }
  }
}
