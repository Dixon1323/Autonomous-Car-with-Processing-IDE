#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>


#define REF_VOLTAGE 3.3
#define ADC_RESOLUTION 4096.0
#define R1 30000.0
#define R2 7500.0

#define BLYNK_PRINT Serial
#define Auth "7Fgai9CLViNTH83yEr5j7HZ4DmhZNGZ5"

char ssid[] = "EDUREKA";
char pass[] = "Edureka@123456";


const float Vref = 3.3;
const int ADCmax = 4095;
const float R_fixed = 10000.0;

const float A = 0.001129148;
const float B = 0.000234125;
const float C = 0.0000000876741;

void setup() {
  Serial.begin(9600);
  Blynk.begin(Auth, ssid, pass);
  analogSetAttenuation(ADC_11db);
  pinMode(32,OUTPUT);
  digitalWrite(32,HIGH);
}

void loop() {
  int raw_voltage_1 = analogRead(39);
  int raw_voltage_2 = analogRead(36);
  int raw_temp1 = analogRead(35);
  int raw_temp2 = analogRead(34);
  float raw_voltage1_adc = ((float)raw_voltage_1 * REF_VOLTAGE) / ADC_RESOLUTION;
  float voltage_1 = raw_voltage1_adc * (R1 + R2) / R2;
  float raw_voltage2_adc = ((float)raw_voltage_2 * REF_VOLTAGE) / ADC_RESOLUTION;
  float voltage_2 = raw_voltage2_adc * (R1 + R2) / R2;
  float voltage_temp1 = (raw_temp1 / (float)ADCmax) * Vref;
  float voltage_temp2 = (raw_temp2 / (float)ADCmax) * Vref;
  float R_temp1 = R_fixed * (voltage_temp1 / (Vref - voltage_temp1));
  float R_temp2 = R_fixed * (voltage_temp2 / (Vref - voltage_temp2));
  float logR = log(R_temp1);
  float tempKelvin = 1.0 / (A + (B * logR) + (C * logR * logR * logR));
  float temp1 = tempKelvin - 273.15;
  logR = log(R_temp2);
  tempKelvin = 1.0 / (A + (B * logR) + (C * logR * logR * logR));
  float temp2 = tempKelvin - 273.15;
  Blynk.virtualWrite(V0,voltage_1);
  Blynk.virtualWrite(V1,voltage_2);
  Blynk.virtualWrite(V2,temp1);
  Blynk.virtualWrite(V3,temp2);
  if(voltage_2<=12){
    digitalWrite(32,HIGH);
  }
  else {
  digitalWrite(32,LOW);
  }
  Serial.print("Battery Volt = ");
  Serial.print(voltage_1, 2);
  Serial.print("|  Scondary volt  = ");
  Serial.println(voltage_2, 2);
  Serial.print("Temperature 1:  ");
  Serial.print(temp1);
  Serial.print("°C");
  Serial.print("|. Temperature 2: ");
  Serial.print(temp2);
  Serial.println("°C");
  Blynk.run();
  delay(500);
}
