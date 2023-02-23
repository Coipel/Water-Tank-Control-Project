#include "HX711.h"

// HX711 circuit wiring
const int LOADCELL_DOUT_PIN = 2;
const int LOADCELL_SCK_PIN = 3;

HX711 scale;

void setup() {
  Serial.begin(57600);
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale(2280.f);
  scale.tare();
  delay(100);
}

void loop() {  
  while (scale.is_ready() == 0) { }
  Serial.print("HX711 reading: ");
  Serial.println(scale.get_units(), 3);
}