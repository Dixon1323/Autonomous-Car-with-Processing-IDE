const float Vref = 3.3;  // ESP32 reference voltage
const int ADCmax = 4095; // 12-bit ADC resolution
const float R_fixed = 10000.0;  // Fixed resistor value in ohms (assumed 10kΩ)

// Steinhart-Hart coefficients (example values, replace with actual ones)
const float A = 0.001129148;
const float B = 0.000234125;
const float C = 0.0000000876741;

void setup() {
    Serial.begin(9600);
    // analogReadResolution(10);  // Set ADC to 12-bit resolution
    analogSetAttenuation(ADC_11db);  // Allows full 0-3.3V range for ADC
}

void loop() {
    int raw_temp1 = analogRead(35);  // Read raw ADC value
    int raw_temp2 = analogRead(35);  // Read raw ADC value
    float voltage_temp1 = (raw_temp1 / (float)ADCmax) * Vref;  // Convert to voltage
    float voltage_temp2 = (raw_temp2 / (float)ADCmax) * Vref;  // Convert to voltage
    // Calculate thermistor resistance
    float R_temp1 = R_fixed * (voltage_temp1 / (Vref - voltage_temp1));
    float R_temp2 = R_fixed * (voltage_temp2 / (Vref - voltage_temp2));
    // Apply Steinhart-Hart equation
    float logR = log(R_temp1);
    float tempKelvin = 1.0 / (A + (B * logR) + (C * logR * logR * logR));
    float temp1 = tempKelvin - 273.15;  // Convert Kelvin to Celsius

    logR = log(R_temp2);
    tempKelvin = 1.0 / (A + (B * logR) + (C * logR * logR * logR));
    float temp2 = tempKelvin - 273.15;  // Convert Kelvin to Celsius

    Serial.print("ADC Value: ");
    Serial.print(raw_temp1);
    Serial.print(" | Voltage: ");
    Serial.print(voltage_temp1, 2);
    Serial.print("V | Temperature 1:  ");
    Serial.print(temp1);
    Serial.println("°C");
    Serial.print("ADC Value: ");
    Serial.print(raw_temp2);
    Serial.print(" | Voltage: ");
    Serial.print(voltage_temp2, 2);
    Serial.print("V | Temperature 2: ");
    Serial.print(temp2);
    Serial.println("°C");

    delay(1000);
}
