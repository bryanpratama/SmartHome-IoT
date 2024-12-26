#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <EEPROM.h>
#define DECODE_NEC 
#include "PinDefinitionsAndMore.h"
#include <IRremote.hpp>
#include <DHT.h>

const char* ssid = "ESP32_AP";
const char* password = "12345678";

const int switchPins[] = {1, 21, 22, 23};
const int relayPins[] = {26, 27, 16, 17, 5, 18, 19, 14};
const int magneticDoorSwitchPin = 33;

#define DHTPIN 15
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

bool relayStates[8] = {true, true, true, true, true, true, true, true};
bool lastSwitchStates[4] = {HIGH, HIGH, HIGH, HIGH};
unsigned long lastDebounceTimes[4] = {0, 0, 0, 0};
const unsigned long debounceDelay = 50;

unsigned long lastIrTime = 0;
const unsigned long irDebounceDelay = 300;

bool relay2FeatureActive = false;
unsigned long relay2StartTime = 0;
const unsigned long relay2OnTime = 2 * 60 * 1000;
const unsigned long relay2OffTime = 5 * 60 * 1000;

unsigned long relay3StartTime = 0;
const unsigned long relay3ActiveDuration = 10 * 1000;
bool relay3Active = false;

WebServer server(80);

void setup() {
    Serial.begin(115200);
    while (!Serial);

    EEPROM.begin(512);
    for (int i = 0; i < 8; i++) {
        relayStates[i] = EEPROM.read(i);
        pinMode(relayPins[i], OUTPUT);
        digitalWrite(relayPins[i], HIGH);
    }

    for (int i = 0; i < 4; i++) {
        pinMode(switchPins[i], INPUT_PULLUP);
    }

    pinMode(magneticDoorSwitchPin, INPUT_PULLUP);

    IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);
    printActiveIRProtocols(&Serial);

    WiFi.softAP(ssid, password);
    Serial.println(WiFi.softAPIP());

    dht.begin();

    server.on("/", handleRoot);
    server.on("/status", handleStatus);
    server.on("/toggle", handleToggle);
    server.on("/relay2Feature", handleRelay2Feature);
    server.on("/dht", handleDHT);
    server.begin();
}

void loop() {
    server.handleClient();

    if (IrReceiver.decode()) {
        if ((millis() - lastIrTime) > irDebounceDelay) {
            lastIrTime = millis();
            if (IrReceiver.decodedIRData.protocol != UNKNOWN) {
                uint8_t command = IrReceiver.decodedIRData.command;
                String buttonName = mapCommandToButtonName(command);
                if (buttonName.toInt() >= 1 && buttonName.toInt() <= 8) {
                    int relayIndex = buttonName.toInt() - 1;
                    toggleRelay(relayIndex);
                } else if (command == 0x0D) {
                    toggleRelay2Feature();
                }
            }
        }
        IrReceiver.resume();
    }

    for (int i = 0; i < 4; i++) {
        int reading = digitalRead(switchPins[i]);
        if (reading != lastSwitchStates[i]) {
            lastDebounceTimes[i] = millis();
        }

        if ((millis() - lastDebounceTimes[i]) > debounceDelay) {
            if (reading != lastSwitchStates[i]) {
                lastSwitchStates[i] = reading;

                if (reading == LOW) {
                    toggleRelay(i);
                }
            }
        }
    }

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

    int doorState = digitalRead(magneticDoorSwitchPin);
    if (doorState == HIGH && !relay3Active) {
        relay3Active = true;
        relay3StartTime = millis();
        digitalWrite(relayPins[2], LOW);
    }

    if (relay3Active && (millis() - relay3StartTime >= relay3ActiveDuration)) {
        relay3Active = false;
        digitalWrite(relayPins[2], HIGH);
    }
}

void toggleRelay(int relayIndex) {
    relayStates[relayIndex] = !relayStates[relayIndex];
    digitalWrite(relayPins[relayIndex], relayStates[relayIndex] ? LOW : HIGH);
    EEPROM.write(relayIndex, relayStates[relayIndex]);
    EEPROM.commit();
}

void toggleRelay2Feature() {
    relay2FeatureActive = !relay2FeatureActive;
    if (relay2FeatureActive) {
        relay2StartTime = millis();
        digitalWrite(relayPins[1], LOW);
        relayStates[1] = true;
    } else {
        digitalWrite(relayPins[1], HIGH);
        relayStates[1] = false;
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
    float temperature = dht.readTemperature();
    float humidity = dht.readHumidity();

    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.println(" °C");
    Serial.print("Humidity: ");
    Serial.print(humidity);
    Serial.println(" %");

    String html = "<html><head>";
    html += "<title>ESP32 Relay Control</title>";
    html += "<script>";
    // Function to update DHT readings
    html += "function updateDHT() {";
    html += "  fetch('/dht')";
    html += "    .then(response => response.text())";
    html += "    .then(data => {";
    html += "      const [temp, hum] = data.split('\\n');";
    html += "      document.getElementById('temperature').innerHTML = temp;";
    html += "      document.getElementById('humidity').innerHTML = hum;";
    html += "    });";
    html += "}";
    // Update relay status
    html += "function updateRelayStatus() {";
    html += "  fetch('/status')";
    html += "    .then(response => response.text())";
    html += "    .then(data => {";
    html += "      const statusDiv = document.getElementById('relayStatus');";
    html += "      statusDiv.innerHTML = data.replace(/\\n/g, '<br>');";
    html += "    });";
    html += "}";
    // Toggle relay function
    html += "function toggleRelay(index) {";
    html += "  fetch('/toggle?relay=' + index)";
    html += "    .then(() => updateRelayStatus());";
    html += "}";
    // Toggle relay2 feature function
    html += "function toggleRelay2Feature() {";
    html += "  fetch('/relay2Feature')";
    html += "    .then(() => updateRelayStatus());";
    html += "}";
    // Set intervals for auto-update
    html += "setInterval(updateDHT, 2000);"; // Update every 2 seconds
    html += "setInterval(updateRelayStatus, 2000);";
    html += "</script></head><body>";
    
    html += "<h1>ESP32 Relay Control</h1>";
    html += "<div id='relayStatus'>";
    for (int i = 0; i < 8; i++) {
        html += "Relay " + String(i + 1) + ": " + String(relayStates[i] ? "ON" : "OFF") + "<br>";
    }
    html += "</div>";
    
    for (int i = 0; i < 8; i++) {
        html += "<button onclick=\"toggleRelay(" + String(i + 1) + ")\">Toggle Relay " + String(i + 1) + "</button>";
    }
    html += "<button onclick=\"toggleRelay2Feature()\">Toggle Relay 2 Feature</button>";

    html += "<div id='dhtReadings'>";
    html += "<p id='temperature'>Temperature: " + String(temperature) + " °C</p>";
    html += "<p id='humidity'>Humidity: " + String(humidity) + " %</p>";
    html += "</div>";

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

void handleDHT() {
    float temperature = dht.readTemperature();
    float humidity = dht.readHumidity();

    if (isnan(temperature) || isnan(humidity)) {
        server.send(200, "text/plain", "Failed to read from DHT sensor!");
        return;
    }

    String response = "Temperature: " + String(temperature) + "°C\n";
    response += "Humidity: " + String(humidity) + "%";
    server.send(200, "text/plain", response);
}
