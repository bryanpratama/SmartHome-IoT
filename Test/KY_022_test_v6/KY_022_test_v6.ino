#include <Arduino.h>
#define DECODE_NEC 
#include "PinDefinitionsAndMore.h"
#include <IRremote.hpp>

void setup() {
    Serial.begin(115200);
    while (!Serial)
        ; // Wait for Serial to become available. Is optimized away for some cores.

    // Just to know which program is running on my Arduino
    Serial.println(F("START " __FILE__ " from " __DATE__ "\r\nUsing library version " VERSION_IRREMOTE));

    // Start the receiver and if not 3rd parameter specified, take LED_BUILTIN pin from the internal boards definition as default feedback LED
    IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);

    Serial.print(F("Ready to receive IR signals of protocols: "));
    printActiveIRProtocols(&Serial);
    Serial.println(F("at pin " STR(IR_RECEIVE_PIN)));
}

void loop() {
    if (IrReceiver.decode()) {
        if (IrReceiver.decodedIRData.protocol == UNKNOWN) {
            Serial.println(F("Received noise or an unknown (or not yet enabled) protocol"));
            IrReceiver.printIRResultRawFormatted(&Serial, true);
            IrReceiver.resume();
        } else {
            // Map command to specific button names
            String buttonName = mapCommandToButtonName(IrReceiver.decodedIRData.command);
            Serial.print(F("Tombol yang ditekan: "));
            Serial.println(buttonName);

            IrReceiver.resume(); // Enable receiving of the next IR frame
        }
        Serial.println();
    }
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
            return "Tombol tidak dikenal";
    }
}
