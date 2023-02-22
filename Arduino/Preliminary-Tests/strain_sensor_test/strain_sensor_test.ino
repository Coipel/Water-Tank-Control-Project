#include "HX711.h"

// HX711 circuit wiring
const int LOADCELL_DOUT_PIN = 2;
const int LOADCELL_SCK_PIN = 3;

unsigned long time_before;
unsigned long waiting_time;

HX711 scale;
//float reading;

void setup() {
  Serial.begin(57600);
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale(2280.f);
  scale.tare();
}

void loop() {
  time_before = millis();  
  while (scale.is_ready() == 0) { }
  waiting_time = millis() - time_before;
  Serial.print("waiting_time: ");
  Serial.println(waiting_time); 

  Serial.print("HX711 reading: ");
  Serial.println(scale.get_units(), 3);
}