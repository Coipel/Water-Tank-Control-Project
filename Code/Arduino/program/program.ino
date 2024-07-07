#include "HX711.h"

const int VCC_PIN = 13; 
const int LOADCELL_DOUT_PIN = 2;
const int LOADCELL_SCK_PIN = 15;
const int MOTOR_PIN = 4;

HX711 scale;

void setup() {
  Serial.begin(57600);
  Serial.setTimeout(10); // Prevents String character search from hanging program too long

  // Powering strain guage and motor driver reference
  pinMode(VCC_PIN, OUTPUT);
  digitalWrite(VCC_PIN, HIGH);

  // Motor PWM signal setup
  ledcAttach(MOTOR_PIN, 50, 8);

  // Strain guage calibration
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale(2280.f);
  scale.tare();
  delay(1000);
}


void loop() {
  if (Serial.available()) {
    String name = Serial.readStringUntil(':');
    String data = Serial.readStringUntil('\n');

    if (name == "power")
      ledcWrite(MOTOR_PIN, data.toInt());
    
    else if (name == "arduino") {
      if (data == "strain") {
        while (scale.is_ready() == 0) { }
        Serial.print("strain:");
        Serial.println(scale.get_units(), 3);
      }
      
      else { 
        Serial.print("unknown command, ");
        Serial.print(name);
        Serial.print(":");
        Serial.println(data);
      }
    }

    else {
      Serial.print("unknown command, ");
      Serial.print(name);
      Serial.print(":");
      Serial.println(data);
    }
  }
}
