#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <EEPROM.h>
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
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 50;

// Web server pada port 80
WebServer server(80);

void setup() {
    Serial.begin(115200);
    while (!Serial); // Wait for Serial
    
    EEPROM.begin(512); // Inisialisasi EEPROM
    relayState = EEPROM.read(0); // Membaca status relay dari EEPROM
    
    Serial.println(F("START " __FILE__ " from " __DATE__ "\r\nUsing library version " VERSION_IRREMOTE));
    
    // Setup IR receiver
    IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);
    Serial.print(F("Ready to receive IR signals of protocols: "));
    printActiveIRProtocols(&Serial);
    Serial.println(F("at pin " STR(IR_RECEIVE_PIN)));
    
    // Setup pin
    pinMode(RelayPin, OUTPUT);
    pinMode(SwitchPin, INPUT_PULLUP);
    digitalWrite(RelayPin, relayState ? LOW : HIGH); // Relay sesuai status terakhir
    
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
        if (IrReceiver.decodedIRData.protocol != UNKNOWN) {
            String buttonName = mapCommandToButtonName(IrReceiver.decodedIRData.command);
            Serial.print(F("Tombol yang ditekan: "));
            Serial.println(buttonName);

            // Handle relay control with remote
            if (buttonName == "1") {
                toggleRelay();
            } else if (buttonName == "2") {
                Serial.println(F("Tombol 2 ditekan."));
            } else if (buttonName == "3") {
                Serial.println(F("Tombol 3 ditekan."));
            }
        }
        IrReceiver.resume();
    }

    // Handle Switch Button
    int reading = digitalRead(SwitchPin);
    if (reading != lastSwitchState) {
        lastDebounceTime = millis();
    }

    if ((millis() - lastDebounceTime) > debounceDelay) {
        if (reading != currentSwitchState) {
            currentSwitchState = reading;

            if (currentSwitchState == LOW) {
                toggleRelay();
            }
        }
    }
    lastSwitchState = reading;
}

void toggleRelay() {
    relayState = !relayState;
    digitalWrite(RelayPin, relayState ? LOW : HIGH);
    EEPROM.write(0, relayState); // Menyimpan status relay ke EEPROM
    EEPROM.commit();
    Serial.println("Relay: " + String(relayState ? "ON" : "OFF"));
}

String mapCommandToButtonName(uint8_t command) {
    switch (command) {
        case 0x45: return "1";
        case 0x46: return "2";
        case 0x47: return "3";
        default:   return "Tombol tidak dikenal";
    }
}

void handleRoot() {
    String html = "<html><head><title>ESP32 Relay Control</title></head><body>";
    html += "<h1>ESP32 Relay Control</h1>";
    html += "<p>Status Relay: " + String(relayState ? "ON" : "OFF") + "</p>";
    html += "<button onclick=\"toggleRelay()\">Toggle Relay</button>";
    html += "<script>function toggleRelay() { fetch('/toggle').then(() => location.reload()); }</script>";
    html += "</body></html>";
    server.send(200, "text/html", html);
}

void handleStatus() {
    server.send(200, "text/plain", relayState ? "ON" : "OFF");
}

void handleToggle() {
    toggleRelay();
    server.send(200, "text/plain", "OK");
}
