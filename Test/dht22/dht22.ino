#include <WiFi.h>
#include <WebServer.h>
#include "DHT.h"

// Konfigurasi DHT22
#define DHTPIN 15       // Pin data DHT22
#define DHTTYPE DHT22   // Jenis sensor DHT22
DHT dht(DHTPIN, DHTTYPE);

// Konfigurasi nama Wi-Fi dan password untuk Access Point
const char* ssid = "ESP32_AP";      // Nama Wi-Fi
const char* password = "12345678";  // Password Wi-Fi

// Web server di port 80
WebServer server(80);

// Fungsi untuk menangani halaman utama
void handleRoot() {
  String html = R"rawliteral(
    <!DOCTYPE html>
    <html>
    <head>
      <title>Monitoring DHT22</title>
      <script>
        function updateData() {
          fetch('/data')
            .then(response => response.text())
            .then(data => {
              if (data === "error") {
                document.getElementById("suhu").innerHTML = "Error";
                document.getElementById("kelembapan").innerHTML = "Error";
              } else {
                const [suhu, kelembapan] = data.split(",");
                document.getElementById("suhu").innerHTML = suhu + " &deg;C";
                document.getElementById("kelembapan").innerHTML = kelembapan + " %";
              }
            })
            .catch(error => {
              console.error("Error:", error);
            });
        }

        setInterval(updateData, 10000); // Update data setiap 10 detik
      </script>
    </head>
    <body>
      <h1>Monitoring DHT22</h1>
      <p>Suhu: <span id="suhu">--</span></p>
      <p>Kelembapan: <span id="kelembapan">--</span></p>
    </body>
    </html>
  )rawliteral";

  server.send(200, "text/html", html);
}

// Fungsi untuk menangani data DHT22
void handleData() {
  float suhu = dht.readTemperature();
  float kelembapan = dht.readHumidity();

  if (isnan(suhu) || isnan(kelembapan)) {
    server.send(200, "text/plain", "error");
  } else {
    String data = String(suhu) + "," + String(kelembapan);
    server.send(200, "text/plain", data);
  }
}

void setup() {
  // Serial Monitor untuk debugging
  Serial.begin(115200);

  // Inisialisasi DHT22
  dht.begin();

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
  server.on("/data", handleData); // Endpoint untuk data suhu dan kelembapan
  server.begin();              // Mulai server
  Serial.println("Server dimulai");
}

void loop() {
  server.handleClient(); // Menangani permintaan dari klien
}
