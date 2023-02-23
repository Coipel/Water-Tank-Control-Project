const int MOTOR_PIN = 9;  // Pin that sends the PWM signal to the motor driver
const int POTENTIOMETER_PIN = A0;

int pump_input;

void setup() {
  Serial.begin(57600);
  pinMode(MOTOR_PIN, OUTPUT);
}

void loop() {
  pump_input = map(analogRead(POTENTIOMETER_PIN), 0, 1023, 0, 255);
  Serial.println(pump_input);
  analogWrite(MOTOR_PIN, pump_input);
}
