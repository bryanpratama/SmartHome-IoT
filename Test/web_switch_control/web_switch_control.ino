#include <WiFi.h>
#include <WebServer.h>

// Konfigurasi Access Point
const char* ssid = "ESP32_AP";
const char* password = "12345678";

// Deklarasi pin
#define RelayPin 2
#define SwitchPin 32

// Variabel status
bool relayState = false;           // Status relay (ON/OFF)
bool lastSwitchState = HIGH;       // Status terakhir switch
bool currentSwitchState = HIGH;    // Status switch saat ini
bool lastButtonState;              // Untuk mendeteksi perubahan status switch
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 50;

// Web server pada port 80
WebServer server(80);

void setup() {
  Serial.begin(115200);
  
  // Setup pin
  pinMode(RelayPin, OUTPUT);
  pinMode(SwitchPin, INPUT_PULLUP);
  digitalWrite(RelayPin, HIGH); // Relay awal OFF
  
  // Baca status awal switch
  lastButtonState = digitalRead(SwitchPin);
  
  // Setup Access Point
  WiFi.softAP(ssid, password);
  Serial.println("Access Point Started");
  Serial.print("IP Address: ");
  Serial.println(WiFi.softAPIP());
  
  // Setup route untuk web server
  server.on("/", handleRoot);
  server.on("/status", handleStatus);
  server.on("/toggle", handleToggle);
  server.begin();
  Serial.println("Web server started");
}

void loop() {
  server.handleClient();
  
  // Baca status switch
  int reading = digitalRead(SwitchPin);

  // Reset timer debounce jika pembacaan berubah
  if (reading != lastSwitchState) {
    lastDebounceTime = millis();
  }

  // Jika sudah melewati waktu debounce
  if ((millis() - lastDebounceTime) > debounceDelay) {
    // Jika status switch berubah
    if (reading != currentSwitchState) {
      currentSwitchState = reading;
      
      // Deteksi perubahan status switch
      if (currentSwitchState != lastButtonState) {
        // Switch berubah dari OFF ke ON atau ON ke OFF
        if (relayState) {
          // Jika relay ON, switch akan mematikan
          relayState = false;
          digitalWrite(RelayPin, HIGH);
          Serial.println("Switch toggled - Relay turned OFF");
        } else {
          // Jika relay OFF, switch akan menyalakan
          relayState = true;
          digitalWrite(RelayPin, LOW);
          Serial.println("Switch toggled - Relay turned ON");
        }
        lastButtonState = currentSwitchState;
      }
    }
  }
  
  lastSwitchState = reading;
}

void handleRoot() {
  String html = "<html><head>";
  html += "<title>ESP32 Relay Control</title>";
  html += "<style>";
  html += "body { font-family: Arial, sans-serif; margin: 20px; text-align: center; }";
  html += "button { padding: 15px 30px; font-size: 18px; cursor: pointer; ";
  html += "background-color: #4CAF50; color: white; border: none; border-radius: 5px; }";
  html += "button:hover { background-color: #45a049; }";
  html += ".status { font-size: 24px; margin: 20px 0; }";
  html += "</style>";
  html += "</head><body>";
  html += "<h1>ESP32 Relay Control</h1>";
  html += "<div class='status'>Status Relay: <span id='relayState'>Loading...</span></div>";
  html += "<button onclick=\"toggleRelay()\">Toggle Relay</button>";
  html += "<script>";
  html += "function updateRelayState() {";
  html += "  fetch('/status').then(response => response.text()).then(state => {";
  html += "    document.getElementById('relayState').innerText = state;";
  html += "  });";
  html += "}";
  html += "function toggleRelay() {";
  html += "  fetch('/toggle').then(() => setTimeout(updateRelayState, 100));";
  html += "}";
  html += "setInterval(updateRelayState, 300);";
  html += "updateRelayState();";
  html += "</script>";
  html += "</body></html>";
  server.send(200, "text/html", html);
}

void handleStatus() {
  String state = relayState ? "ON" : "OFF";
  server.send(200, "text/plain", state);
}

void handleToggle() {
  // Web toggle selalu membalik status relay
  relayState = !relayState;
  digitalWrite(RelayPin, relayState ? LOW : HIGH);
  Serial.println("Web toggled - Relay: " + String(relayState ? "ON" : "OFF"));
  server.send(200, "text/plain", "OK");
}
