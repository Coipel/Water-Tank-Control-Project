const int MOTOR_PIN = 4;

void setup() {
  Serial.begin(57600);
  
  // Motor PWM signal setup
  ledcAttach(MOTOR_PIN, 50, 8);
}

void loop() {
  for (int i=0; i<256; i++) {
    ledcWrite(MOTOR_PIN, i);
    Serial.print("power:");
    Serial.println(i);
    delay(100);
  }

  for (int i=1; i<256; i++) {
    ledcWrite(MOTOR_PIN, 255-i);
    Serial.print("power:");
    Serial.println(255-i);
    delay(100);
  }
}
