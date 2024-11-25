#define THERMISTOR_PIN 36  // Pin ADC ESP32
#define SERIES_RESISTOR 10000  // Resistor tetap 10k ohm
#define NOMINAL_RESISTANCE 10000  // Resistansi thermistor pada 25°C (10k ohm)
#define NOMINAL_TEMPERATURE 25  // Temperatur nominal (25°C)
#define B_COEFFICIENT 3950  // Koefisien B untuk NTC 3950
#define ADC_MAX 4095  // Resolusi ADC ESP32 (12-bit)
#define SUPPLY_VOLTAGE 3.3  // Tegangan suplai ESP32 (3.3V)

void setup() {
  Serial.begin(115200);
  analogReadResolution(12);  // Set resolusi ADC ESP32
  Serial.println("Thermistor NTC 3950 siap...");
}

void loop() {
  int adcValue = analogRead(THERMISTOR_PIN);  // Baca nilai ADC
  float voltage = adcValue * (SUPPLY_VOLTAGE / ADC_MAX);  // Hitung tegangan
  float resistance = SERIES_RESISTOR * (SUPPLY_VOLTAGE / voltage - 1);  // Hitung resistansi

  // Hitung suhu menggunakan persamaan Steinhart-Hart sederhana
  float steinhart;
  steinhart = resistance / NOMINAL_RESISTANCE;  // R / R0
  steinhart = log(steinhart);  // ln(R / R0)
  steinhart /= B_COEFFICIENT;  // 1/B * ln(R / R0)
  steinhart += 1.0 / (NOMINAL_TEMPERATURE + 273.15);  // + (1/T0)
  steinhart = 1.0 / steinhart;  // Invers
  steinhart -= 273.15;  // Konversi ke Celsius

  Serial.print("Suhu: ");
  Serial.print(steinhart);
  Serial.println(" °C");

  delay(1000);  // Tunggu 1 detik sebelum membaca ulang
}
