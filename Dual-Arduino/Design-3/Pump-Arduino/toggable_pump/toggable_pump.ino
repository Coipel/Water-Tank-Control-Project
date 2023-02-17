const int PWMA_PIN = 9;
const int POTENTIOMETER_PIN = A0;
const int PUMP_ON_PIN = 8;

void setup() {
  Serial.begin(57600);
  pinMode(PWMA_PIN, OUTPUT);
  pinMode(PUMP_ON_PIN, INPUT);
}

void loop() {
  
  int pump_input;
  if (digitalRead(PUMP_ON_PIN) == HIGH) {
    pump_input = map(analogRead(POTENTIOMETER_PIN), 0, 1023, 0, 255);
  }
  else {
    pump_input = 0;
  }

  Serial.println(pump_input);
  analogWrite(PWMA_PIN, pump_input);
}
