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
const int switchPins[] = {1, 21, 22, 23}; // Pin untuk 4 saklar
const int relayPins[] = {26, 27, 16, 17, 5, 18, 19, 14}; // Pin untuk 8 relay

// Variabel status
bool relayStates[8] = {false, false, false, false, false, false, false, false}; // Status tiap relay
bool lastSwitchStates[4] = {HIGH, HIGH, HIGH, HIGH};
unsigned long lastDebounceTimes[4] = {0, 0, 0, 0};
const unsigned long debounceDelay = 50;

// Variabel IR debounce
unsigned long lastIrTime = 0;
const unsigned long irDebounceDelay = 300;

// Web server pada port 80
WebServer server(80);

void setup() {
    Serial.begin(115200);
    while (!Serial);

    EEPROM.begin(512); // Inisialisasi EEPROM
    for (int i = 0; i < 8; i++) {
        relayStates[i] = EEPROM.read(i);
        pinMode(relayPins[i], OUTPUT);
        digitalWrite(relayPins[i], relayStates[i] ? LOW : HIGH); // Relay sesuai status terakhir
    }

    for (int i = 0; i < 4; i++) {
        pinMode(switchPins[i], INPUT_PULLUP);
    }

    Serial.println(F("START " __FILE__ " from " __DATE__ "\r\nUsing library version " VERSION_IRREMOTE));

    // Setup IR receiver
    IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);
    Serial.print(F("Ready to receive IR signals of protocols: "));
    printActiveIRProtocols(&Serial);
    Serial.println(F("at pin " STR(IR_RECEIVE_PIN)));

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
        if ((millis() - lastIrTime) > irDebounceDelay) {
            lastIrTime = millis();
            if (IrReceiver.decodedIRData.protocol != UNKNOWN) {
                String buttonName = mapCommandToButtonName(IrReceiver.decodedIRData.command);
                Serial.print(F("Tombol yang ditekan: "));
                Serial.println(buttonName);

                if (buttonName.toInt() >= 1 && buttonName.toInt() <= 8) {
                    int relayIndex = buttonName.toInt() - 1;
                    toggleRelay(relayIndex);
                }
            }
        }
        IrReceiver.resume();
    }

    // Handle Switch Buttons
    for (int i = 0; i < 4; i++) {
        int reading = digitalRead(switchPins[i]);
        if (reading != lastSwitchStates[i]) {
            lastDebounceTimes[i] = millis();
        }

        if ((millis() - lastDebounceTimes[i]) > debounceDelay) {
            if (reading != lastSwitchStates[i]) {
                lastSwitchStates[i] = reading;

                if (reading == LOW) {
                    toggleRelay(i); // Toggle relay terkait tombol
                }
            }
        }
    }
}

void toggleRelay(int relayIndex) {
    relayStates[relayIndex] = !relayStates[relayIndex];
    digitalWrite(relayPins[relayIndex], relayStates[relayIndex] ? LOW : HIGH);
    EEPROM.write(relayIndex, relayStates[relayIndex]); // Menyimpan status relay ke EEPROM
    EEPROM.commit();
    Serial.println("Relay " + String(relayIndex + 1) + ": " + String(relayStates[relayIndex] ? "ON" : "OFF"));
}

String mapCommandToButtonName(uint8_t command) {
    switch (command) {
        case 0x45: return "1";
        case 0x46: return "2";
        case 0x47: return "3";
        case 0x44: return "4";
        case 0x40: return "5";
        case 0x43: return "6";
        case 0x07: return "7";
        case 0x15: return "8";
        default:   return "Tombol tidak dikenal";
    }
}

void handleRoot() {
    String html = "<html><head><title>ESP32 Relay Control</title></head><body>";
    html += "<h1>ESP32 Relay Control</h1>";
    for (int i = 0; i < 8; i++) {
        html += "<p>Relay " + String(i + 1) + ": " + String(relayStates[i] ? "ON" : "OFF") + "</p>";
        html += "<button onclick=\"toggleRelay(" + String(i + 1) + ")\">Toggle Relay " + String(i + 1) + "</button>";
    }
    html += "<script>function toggleRelay(index) { fetch('/toggle?relay=' + index).then(() => location.reload()); }</script>";
    html += "</body></html>";
    server.send(200, "text/html", html);
}

void handleStatus() {
    String status = "";
    for (int i = 0; i < 8; i++) {
        status += "Relay " + String(i + 1) + ": " + String(relayStates[i] ? "ON" : "OFF") + "\n";
    }
    server.send(200, "text/plain", status);
}

void handleToggle() {
    if (server.hasArg("relay")) {
        int relayIndex = server.arg("relay").toInt() - 1;
        if (relayIndex >= 0 && relayIndex < 8) {
            toggleRelay(relayIndex);
        }
    }
    server.send(200, "text/plain", "OK");
}
