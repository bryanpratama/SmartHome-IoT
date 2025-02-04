// Definisi pin relay
const int relayPins[] = {2, 4, 16, 17, 5, 18, 19, 14};
const int relayCount = 8;

void setup() {
    Serial.begin(115200);
    for (int i = 0; i < relayCount; i++) {
        pinMode(relayPins[i], OUTPUT);
        digitalWrite(relayPins[i], HIGH); // Matikan relay saat startup (LOW untuk aktif)
    }
    Serial.println("Relay controller siap.");
}

void loop() {
    if (Serial.available()) {
        String command = Serial.readStringUntil('\n');
        command.trim();
        
        if (command.startsWith("ON")) {
            int relayNum = command.substring(2).toInt();
            if (relayNum >= 1 && relayNum <= relayCount) {
                digitalWrite(relayPins[relayNum - 1], LOW); // Aktifkan relay
                Serial.println("Relay " + String(relayNum) + " ON");
            }
        } else if (command.startsWith("OFF")) {
            int relayNum = command.substring(3).toInt();
            if (relayNum >= 1 && relayNum <= relayCount) {
                digitalWrite(relayPins[relayNum - 1], HIGH); // Matikan relay
                Serial.println("Relay " + String(relayNum) + " OFF");
            }
        } else {
            Serial.println("Perintah tidak dikenali. Gunakan ONx atau OFFx (contoh: ON1, OFF3)");
        }
    }
}
