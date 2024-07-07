#include "HX711.h"

const int VCC_PIN = 13; 
// HX711 circuit wiring
const int LOADCELL_DOUT_PIN = 2;
const int LOADCELL_SCK_PIN = 15;
const int MOTOR_PIN = 4;

HX711 scale;

void setup() {
  Serial.begin(57600);

  // Powering strain guage and motor driver reference
  pinMode(VCC_PIN, OUTPUT);
  digitalWrite(VCC_PIN, HIGH);

  ledcAttach(MOTOR_PIN, 50, 8);

  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale(2280.f);
  scale.tare();
  delay(1000);
}

void loop() {  
  for (int i=0; i<256; i++) {
    ledcWrite(MOTOR_PIN, i);
    Serial.print("power:");
    Serial.println(i);
    
    while (scale.is_ready() == 0) { }
    Serial.print("strain:");
    Serial.println(scale.get_units(), 3);
    
    delay(100);
  }

  for (int i=1; i<256; i++) {
    ledcWrite(MOTOR_PIN, 255-i);
    Serial.print("power:");
    Serial.println(255-i);

    while (scale.is_ready() == 0) { }
    Serial.print("strain:");
    Serial.println(scale.get_units(), 3);

    delay(100);
  }
}