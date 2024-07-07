#include "HX711.h"

const int VCC_PIN = 13;
// HX711 circuit wiring
const int LOADCELL_DOUT_PIN = 2;
const int LOADCELL_SCK_PIN = 15;

HX711 scale;

void setup() {
  Serial.begin(57600);
  
  // Powering strain guage and motor driver reference
  pinMode(VCC_PIN, OUTPUT);
  digitalWrite(VCC_PIN, HIGH);
  
  // Strain guage calibration
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale(2280.f);
  scale.tare();
  delay(1000);
}

void loop() {  
  while (scale.is_ready() == 0) { }
  Serial.print("strain:");
  Serial.println(scale.get_units(), 3);
}