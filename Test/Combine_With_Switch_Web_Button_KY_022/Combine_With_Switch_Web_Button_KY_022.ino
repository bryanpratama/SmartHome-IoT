#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <EEPROM.h>
#define DECODE_NEC 
#include "PinDefinitionsAndMore.h"
#include <IRremote.hpp>
#include <DHT.h>

// WiFi settings
const char* ssid = "ESP32_AP";
const char* password = "12345678";

// Pin definitions
const int switchPins[] = {1, 21, 22, 23};
const int relayPins[] = {26, 27, 16, 17, 5, 18, 19, 14};
const int magneticDoorSwitchPin = 33;

// DHT22 settings
#define DHTPIN 15
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

// Power monitoring pins
#define PIN_TEGANGAN 36 // ZMPT101B
#define PIN_ARUS 34     // ACS712

// Power monitoring variables
float teganganKalibrasi = 234.3; // Adjusted for 220V AC
float arusKalibrasi = 30.0;      // Adjusted for ACS712 30A version
float offsetTegangan = 0;
float offsetArus = 1650;         // Center point for ACS712
const int sampleCount = 10;
int delayPerSample = 100;
float totalEnergyKWh = 0.0;
const float tarifPerKWh = 1352.0;
unsigned long lastUpdateMillis = 0;

// Relay states and timing
bool relayStates[8] = {true, true, true, true, true, true, true, true};
bool lastSwitchStates[4] = {HIGH, HIGH, HIGH, HIGH};
unsigned long lastDebounceTimes[4] = {0, 0, 0, 0};
const unsigned long debounceDelay = 50;

// IR remote timing
unsigned long lastIrTime = 0;
const unsigned long irDebounceDelay = 300;

// Relay 2 feature timing
bool relay2FeatureActive = false;
unsigned long relay2StartTime = 0;
const unsigned long relay2OnTime = 2 * 60 * 1000;
const unsigned long relay2OffTime = 5 * 60 * 1000;

// Relay 3 timing
unsigned long relay3StartTime = 0;
const unsigned long relay3ActiveDuration = 10 * 1000;
bool relay3Active = false;

WebServer server(80);

// Power monitoring functions
float kalibrasiOffset(int pin) {
    float total = 0;
    for (int i = 0; i < sampleCount; i++) {
        total += analogRead(pin);
        delay(delayPerSample);
    }
    return total / sampleCount;
}

float bacaNilai(int pin, float offset, float kalibrasi) {
    float total = 0;
    float maxValue = 0;
    
    // Take more samples for better accuracy
    for (int i = 0; i < sampleCount; i++) {
        int adcValue = analogRead(pin);
        float nilai = abs(((adcValue - offset) / 4095.0) * 3.3 * kalibrasi);
        
        // Keep maximum value for current measurement
        if (nilai > maxValue) {
            maxValue = nilai;
        }
        
        total += nilai;
        delay(1);
    }
    
    // Use RMS for voltage, max value for current
    if (pin == PIN_TEGANGAN) {
        return (total / sampleCount) * 0.707; // RMS conversion
    } else {
        return maxValue > 0.2 ? maxValue : 0; // Apply threshold for noise reduction
    }
}

void setup() {
    Serial.begin(115200);
    while (!Serial);

    // Initialize EEPROM and relays
    EEPROM.begin(512);
    for (int i = 0; i < 8; i++) {
        relayStates[i] = EEPROM.read(i);
        pinMode(relayPins[i], OUTPUT);
        digitalWrite(relayPins[i], HIGH);
    }

    // Initialize switches
    for (int i = 0; i < 4; i++) {
        pinMode(switchPins[i], INPUT_PULLUP);
    }

    pinMode(magneticDoorSwitchPin, INPUT_PULLUP);

    // Initialize IR receiver
    IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);
    printActiveIRProtocols(&Serial);

    // Initialize power monitoring
    pinMode(PIN_TEGANGAN, INPUT);
    pinMode(PIN_ARUS, INPUT);
    
    Serial.println("Kalibrasi offset dimulai...");
    for (int i = 0; i < 3; i++) {
        offsetTegangan = kalibrasiOffset(PIN_TEGANGAN);
        offsetArus = kalibrasiOffset(PIN_ARUS);
        delay(1000);
    }
    lastUpdateMillis = millis();

    // Start WiFi AP
    WiFi.softAP(ssid, password);
    Serial.println(WiFi.softAPIP());

    // Initialize DHT
    dht.begin();

    // Setup web server endpoints
    server.on("/", handleRoot);
    server.on("/status", handleStatus);
    server.on("/toggle", handleToggle);
    server.on("/relay2Feature", handleRelay2Feature);
    server.on("/dht", handleDHT);
    server.on("/power", handlePower);
    server.begin();
}

void loop() {
    server.handleClient();

    // Handle IR remote
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

    // Handle switches
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

    // Handle relay2 feature
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

    // Handle magnetic door switch
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

void handlePower() {
    float teganganAC = bacaNilai(PIN_TEGANGAN, offsetTegangan, teganganKalibrasi);
    float arusAC = bacaNilai(PIN_ARUS, offsetArus, arusKalibrasi);
    
    // Apply minimal thresholds
    teganganAC = teganganAC < 1.0 ? 0 : teganganAC;
    arusAC = arusAC < 0.1 ? 0 : arusAC;
    
    float dayaAC = teganganAC * arusAC;
    
    unsigned long currentMillis = millis();
    float elapsedHours = (currentMillis - lastUpdateMillis) / 3600000.0;
    lastUpdateMillis = currentMillis;
    
    if (dayaAC > 0) {
        totalEnergyKWh += (dayaAC * elapsedHours) / 1000.0;
    }
    
    float biayaSaatIni = totalEnergyKWh * tarifPerKWh;
    float estimasiBiayaBulanan = (dayaAC / 1000.0) * tarifPerKWh * 24 * 30;

    // Format currency with thousand separators
    char biayaStr[32];
    char estimasiStr[32];
    sprintf(biayaStr, "%'d", (int)biayaSaatIni);
    sprintf(estimasiStr, "%'d", (int)estimasiBiayaBulanan);

    String response = "Tegangan: " + String(teganganAC, 1) + " V\n";
    response += "Arus: " + String(arusAC, 2) + " A\n";
    response += "Daya: " + String(dayaAC, 1) + " W\n";
    response += "Energi Total: " + String(totalEnergyKWh, 3) + " kWh\n";
    response += "Biaya: Rp " + String(biayaStr) + "\n";
    response += "Estimasi Biaya Bulanan: Rp " + String(estimasiStr);
    
    server.send(200, "text/plain", response);
}

void handleRoot() {
    float temperature = dht.readTemperature();
    float humidity = dht.readHumidity();

    String html = "<html><head>";
    html += "<title>ESP32 IoT Control</title>";
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
    
    // Function to update power readings
    html += "function updatePower() {";
    html += "  fetch('/power')";
    html += "    .then(response => response.text())";
    html += "    .then(data => {";
    html += "      const lines = data.split('\\n');";
    html += "      const powerDiv = document.getElementById('powerReadings');";
    html += "      powerDiv.innerHTML = lines.join('<br>');";
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
    html += "setInterval(updateDHT, 2000);";
    html += "setInterval(updatePower, 2000);";
    html += "setInterval(updateRelayStatus, 2000);";
    html += "</script></head><body>";
    
    html += "<h1>ESP32 IoT Control</h1>";
    
    html += "<div id='relayStatus'>";
    for (int i = 0; i < 8; i++) {
        html += "Relay " + String(i + 1) + ": " + String(relayStates[i] ? "ON" : "OFF") + "<br>";
    }
    html += "</div>";
    
    for (int i = 0; i < 8; i++) {
        html += "<button onclick=\"toggleRelay(" + String(i + 1) + ")\">Toggle Relay " + String(i + 1) + "</button>";
    }
    html += "<button onclick=\"toggleRelay2Feature()\">Toggle Relay 2 Feature</button>";

    html += "<h2>Sensor Readings</h2>";
    html += "<div id='dhtReadings'>";
    html += "<p id='temperature'>Temperature: " + String(temperature) + " °C</p>";
    html += "<p id='humidity'>Humidity: " + String(humidity) + " %</p>";
    html += "</div>";

    html += "<h2>Power Monitoring</h2>";
    html += "<div id='powerReadings'></div>";

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
