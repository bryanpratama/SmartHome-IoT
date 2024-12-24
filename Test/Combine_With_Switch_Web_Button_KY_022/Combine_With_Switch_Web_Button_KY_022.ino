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
const int magneticDoorSwitchPin = 33; // Pin untuk MC-38 Magnetic Door Switch

// Variabel status
bool relayStates[8] = {true, true, true, true, true, true, true, true}; // Status tiap relay
bool lastSwitchStates[4] = {HIGH, HIGH, HIGH, HIGH};
unsigned long lastDebounceTimes[4] = {0, 0, 0, 0};
const unsigned long debounceDelay = 50;

// Variabel IR debounce
unsigned long lastIrTime = 0;
const unsigned long irDebounceDelay = 300;

// Variabel untuk fitur relay 2 (nyala 2 menit, mati 5 menit)
bool relay2FeatureActive = false;
unsigned long relay2StartTime = 0;
const unsigned long relay2OnTime = 2 * 60 * 1000; // 2 menit
const unsigned long relay2OffTime = 5 * 60 * 1000; // 5 menit

// Variabel untuk fitur MC-38 Magnetic Door Switch
unsigned long relay3StartTime = 0;
const unsigned long relay3ActiveDuration = 10 * 1000; // 10 detik
bool relay3Active = false;

// Web server pada port 80
WebServer server(80);

void setup() {
    Serial.begin(115200);
    while (!Serial);

    EEPROM.begin(512); // Inisialisasi EEPROM
    for (int i = 0; i < 8; i++) {
        relayStates[i] = EEPROM.read(i);
        pinMode(relayPins[i], OUTPUT);
        //digitalWrite(relayPins[i], relayStates[i] ? LOW : HIGH); // Relay sesuai status terakhir
        digitalWrite(relayPins[i], HIGH);
    }

    for (int i = 0; i < 4; i++) {
        pinMode(switchPins[i], INPUT_PULLUP);
    }

    pinMode(magneticDoorSwitchPin, INPUT_PULLUP); // Konfigurasi pin MC-38 Magnetic Door Switch

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
    server.on("/relay2Feature", handleRelay2Feature);
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
                uint8_t command = IrReceiver.decodedIRData.command;
                String buttonName = mapCommandToButtonName(command);
                Serial.print(F("Tombol yang ditekan: "));
                Serial.println(buttonName);

                if (buttonName.toInt() >= 1 && buttonName.toInt() <= 8) {
                    int relayIndex = buttonName.toInt() - 1;
                    toggleRelay(relayIndex);
                } else if (command == 0x0D) { // ID IR untuk fitur relay 2
                    toggleRelay2Feature();
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

    // Handle fitur relay 2 (2 menit nyala, 5 menit mati)
    if (relay2FeatureActive) {
        unsigned long elapsedTime = millis() - relay2StartTime;
        if (relayStates[1] && elapsedTime >= relay2OnTime) {
            digitalWrite(relayPins[1], HIGH);
            relayStates[1] = false;
            relay2StartTime = millis();
        } else if (!relayStates[1] && elapsedTime >= relay2OffTime) {
            digitalWrite(relayPins[1], LOW);
            relayStates[1] = true;
            relay2StartTime = millis();
        }
    }

    // Handle fitur MC-38 Magnetic Door Switch
    int doorState = digitalRead(magneticDoorSwitchPin);
    if (doorState == HIGH && !relay3Active) { // Pintu terbuka
        relay3Active = true;
        relay3StartTime = millis();
        digitalWrite(relayPins[2], LOW); // Aktifkan relay 3
    }

    if (relay3Active && (millis() - relay3StartTime >= relay3ActiveDuration)) {
        relay3Active = false;
        digitalWrite(relayPins[2], HIGH); // Matikan relay 3
    }
}

void toggleRelay(int relayIndex) {
    relayStates[relayIndex] = !relayStates[relayIndex];
    digitalWrite(relayPins[relayIndex], relayStates[relayIndex] ? LOW : HIGH);
    EEPROM.write(relayIndex, relayStates[relayIndex]); // Menyimpan status relay ke EEPROM
    EEPROM.commit();
    Serial.println("Relay " + String(relayIndex + 1) + ": " + String(relayStates[relayIndex] ? "ON" : "OFF"));
}

void toggleRelay2Feature() {
    relay2FeatureActive = !relay2FeatureActive;
    if (relay2FeatureActive) {
        relay2StartTime = millis();
        digitalWrite(relayPins[1], LOW);
        relayStates[1] = true;
        Serial.println("Fitur Relay 2: Aktif");
    } else {
        digitalWrite(relayPins[1], HIGH);
        relayStates[1] = false;
        Serial.println("Fitur Relay 2: Nonaktif");
    }
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
    html += "<button onclick=\"toggleRelay2Feature()\">Toggle Relay 2 Feature</button>";
    html += "<script>function toggleRelay(index) { fetch('/toggle?relay=' + index).then(() => location.reload()); }";
    html += "function toggleRelay2Feature() { fetch('/relay2Feature').then(() => location.reload()); }</script>";
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

void handleRelay2Feature() {
    toggleRelay2Feature();
    server.send(200, "text/plain", "OK");
}
