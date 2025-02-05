#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "narzo";
const char* password = "esp32cam";
WebServer server(80);

const int relayPins[] = {26, 27, 16, 17, 5, 18, 19, 14};
const int relayCount = 8;
bool relayStates[relayCount] = {false};

void handleRoot() {
    String html = "<html><head><style>"
                  "button { padding: 10px 20px; font-size: 16px; margin: 5px; cursor: pointer; }"
                  "</style></head><body><h1>Relay Controller</h1>";
    for (int i = 0; i < relayCount; i++) {
        html += "<form action='/toggle" + String(i + 1) + "' method='get'>";
        html += "<button type='submit'>Relay " + String(i + 1) + "</button></form>";
    }
    html += "</body></html>";
    server.send(200, "text/html", html);
}

void handleToggle() {
    int relayNum = server.uri().substring(7).toInt();
    if (relayNum >= 1 && relayNum <= relayCount) {
        relayStates[relayNum - 1] = !relayStates[relayNum - 1];
        digitalWrite(relayPins[relayNum - 1], relayStates[relayNum - 1] ? LOW : HIGH);
    }
    server.sendHeader("Location", "/");
    server.send(303);
}

void setup() {
    Serial.begin(115200);
    WiFi.begin(ssid, password);
    Serial.println("Connecting to WiFi...");
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.print(".");
    }
    Serial.println("\nConnected to WiFi");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
    
    for (int i = 0; i < relayCount; i++) {
        pinMode(relayPins[i], OUTPUT);
        digitalWrite(relayPins[i], HIGH);
    }
    
    server.on("/", handleRoot);
    for (int i = 1; i <= relayCount; i++) {
        server.on("/toggle" + String(i), handleToggle);
    }
    
    server.begin();
    Serial.println("Server started");
}

void loop() {
    server.handleClient();
}
