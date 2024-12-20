#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#define DECODE_NEC 
#include "PinDefinitionsAndMore.h"
#include <IRremote.hpp>

// Konfigurasi Access Point
const char* ssid = "ESP32_AP";
const char* password = "12345678";

// Deklarasi pin
#define RelayPin 21
#define SwitchPin 25

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
    while (!Serial); // Wait for Serial
    
    Serial.println(F("START " __FILE__ " from " __DATE__ "\r\nUsing library version " VERSION_IRREMOTE));
    
    // Setup IR receiver
    IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);
    Serial.print(F("Ready to receive IR signals of protocols: "));
    printActiveIRProtocols(&Serial);
    Serial.println(F("at pin " STR(IR_RECEIVE_PIN)));
    
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
    
    // Handle IR Remote
    if (IrReceiver.decode()) {
        if (IrReceiver.decodedIRData.protocol == UNKNOWN) {
            Serial.println(F("Received noise or an unknown protocol"));
            IrReceiver.printIRResultRawFormatted(&Serial, true);
        } else {
            // Map command to specific button names
            String buttonName = mapCommandToButtonName(IrReceiver.decodedIRData.command);
            Serial.print(F("Tombol yang ditekan: "));
            Serial.println(buttonName);
            
            // Handle relay control with remote
            if(buttonName == "1") {
                // Toggle relay dengan tombol 1
                relayState = !relayState;
                digitalWrite(RelayPin, relayState ? LOW : HIGH);
                Serial.println("Remote toggled - Relay: " + String(relayState ? "ON" : "OFF"));
            }
        }
        IrReceiver.resume(); // Enable receiving of the next IR frame
        Serial.println();
    }
    
    // Handle Switch Button
    int reading = digitalRead(SwitchPin);

    if (reading != lastSwitchState) {
        lastDebounceTime = millis();
    }

    if ((millis() - lastDebounceTime) > debounceDelay) {
        if (reading != currentSwitchState) {
            currentSwitchState = reading;
            
            if (currentSwitchState != lastButtonState) {
                if (relayState) {
                    relayState = false;
                    digitalWrite(RelayPin, HIGH);
                    Serial.println("Switch toggled - Relay turned OFF");
                } else {
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

String mapCommandToButtonName(uint8_t command) {
    switch (command) {
        case 0x45:
            return "1";
        case 0x46:
            return "2";
        case 0x47:
            return "3";
        case 0x44:
            return "4";
        case 0x40:
            return "5";
        case 0x43:
            return "6";
        case 0x07:
            return "7";
        case 0x15:
            return "8";
        case 0x09:
            return "9";
        case 0x16:
            return "*";
        case 0x19:
            return "0";
        case 0x0D:
            return "#";
        case 0x18:
            return "Up";
        case 0x08:
            return "Left";
        case 0x5A:
            return "Right";
        case 0x52:
            return "Bottom";
        case 0x1C:
            return "OK";
        default:
            Serial.print(F("Tombol tidak dikenal ditekan dengan kode: "));
            Serial.println(command, HEX);
            return "Tombol tidak dikenal";
    }
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
    relayState = !relayState;
    digitalWrite(RelayPin, relayState ? LOW : HIGH);
    Serial.println("Web toggled - Relay: " + String(relayState ? "ON" : "OFF"));
    server.send(200, "text/plain", "OK");
}
