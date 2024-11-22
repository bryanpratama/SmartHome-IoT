#include <WiFi.h>
#include <WebServer.h>

// Konfigurasi nama Wi-Fi dan password untuk Access Point
const char* ssid = "ESP32_AP";      // Nama Wi-Fi
const char* password = "12345678";  // Password Wi-Fi

// Web server di port 80
WebServer server(80);

// Pin LED
const int ledPin = 2; // Pin GPIO 2 (LED bawaan ESP32)

// Status LED
bool ledStatus = false;

// Fungsi untuk menangani halaman utama
void handleRoot() {
  String html = "<h1>Kontrol LED ESP32</h1>";
  html += "<p>Status LED: " + String(ledStatus ? "Nyala" : "Mati") + "</p>";
  html += "<button onclick=\"location.href='/nyala'\">Nyalakan LED</button>";
  html += "<button onclick=\"location.href='/mati'\">Matikan LED</button>";
  server.send(200, "text/html", html);
}

// Fungsi untuk menyalakan LED
void handleNyala() {
  ledStatus = true;
  digitalWrite(ledPin, HIGH); // Nyalakan LED
  server.send(200, "text/html", "<h1>LED Nyala!</h1><a href='/'>Kembali</a>");
}

// Fungsi untuk mematikan LED
void handleMati() {
  ledStatus = false;
  digitalWrite(ledPin, LOW); // Matikan LED
  server.send(200, "text/html", "<h1>LED Mati!</h1><a href='/'>Kembali</a>");
}

void setup() {
  // Serial Monitor untuk debugging
  Serial.begin(115200);

  // Menyiapkan pin LED sebagai output
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW); // Matikan LED saat awal

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
  server.on("/nyala", handleNyala); // Halaman untuk menyalakan LED
  server.on("/mati", handleMati);   // Halaman untuk mematikan LED
  server.begin();                   // Mulai server
  Serial.println("Server dimulai");
}

void loop() {
  server.handleClient(); // Menangani permintaan dari klien
}
