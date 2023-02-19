const int PWMA_PIN = 9;
const int AI2_PIN = 8;
const int AI1_PIN = 7;
//const int STBY_PIN = 5; // This used to be pin 2, now it is 5 so that weight sensor doesn't need changes from default pins [Not Tested]

const int POTENTIOMETER_PIN = A0;

void setup() {
  Serial.begin(57600);
  pinMode(PWMA_PIN, OUTPUT);
  pinMode(AI2_PIN, OUTPUT);
  pinMode(AI1_PIN, OUTPUT);
  //pinMode(STBY_PIN, OUTPUT);

  //digitalWrite(STBY_PIN, HIGH);

  // Forward rotation
  digitalWrite(AI1_PIN, HIGH);
  digitalWrite(AI2_PIN, LOW);
}

void loop() {
  int raw_input = analogRead(POTENTIOMETER_PIN);
  Serial.print("raw_input");
  Serial.println(raw_input);
  int pump_input = map(raw_input, 0, 1023, 0, 255); // [NOTE] Using 3.3 V on the potentiometer rather than 5 V so max value around 700 not 1023.
  Serial.println(pump_input);

  analogWrite(PWMA_PIN, pump_input);
}
