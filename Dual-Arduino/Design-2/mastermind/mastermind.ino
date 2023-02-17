const int PWMA_PIN = 9;

// Button and LED pins
const int BUTTON_PIN = 4;
const int BUILTIN_LED_PIN = 13;

// Manual (later setpoint setter) motor driver input via potentiometer
const int POTENTIOMETER_PIN = A0;

const int data_size = 10;
float strain[data_size];
unsigned int valve[data_size];

unsigned long time_start;
unsigned long time_now;
unsigned long time[data_size];

bool process_started = false;
bool process_finished = false;
int last_index = 0;

// Input pins
const int STRAIN_PIN = 9;
float max_value = 100;
float min_value = 0;

const int PWM_PIN = 8;

void setup() {
  Serial.begin(57600);

  // ---Initalizing button related pins---
  pinMode(BUILTIN_LED_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT);

  pinMode(PWMA_PIN, OUTPUT);
  pinMode(PWM_PIN, INPUT);

  digitalWrite(BUILTIN_LED_PIN, LOW);
}

void loop() {
  while (process_finished == true) {
    digitalWrite(BUILTIN_LED_PIN, LOW);
    delay(100);
    digitalWrite(BUILTIN_LED_PIN, HIGH);
    delay(100);
   }

  if (process_started == false) {  // Button must be pushed for the system to start
    while (digitalRead(BUTTON_PIN) == LOW) { } 
      while (digitalRead(BUTTON_PIN) == HIGH) { }
      process_started = true;
      digitalWrite(BUILTIN_LED_PIN, HIGH);
      time_start = millis();
  }

  for (int i = 0; i < data_size; i++) {
    delay(100);
    time[i] = millis() - time_start;

    // ---Strain Process and Data Sample---
    float frequency = 490; // 490 Hz
    float period = 1000000/frequency; // 1/490 Hz, but scaled to be for mircoseconds period
    float duty_cycle = (pulseIn(PWM_PIN, HIGH, 3*period)*100)/period;
    strain[i] = map(duty_cycle, 0, 90, min_value, max_value);

    // ---Valve Process and Data Sample---
    valve[i] = map(analogRead(POTENTIOMETER_PIN), 0, 1023, 0, 255);
    analogWrite(PWMA_PIN, valve[i]);

    // This serves somewhat as an interupt which ceases data collection at the current point and begins transmission followed by cleanup
    last_index = i;
    if (digitalRead(BUTTON_PIN) == HIGH) {break;}
  }

  Serial.println("SD");
  for (int i = 0; i <= last_index; i++) {
    Serial.println(strain[i], 3);
  }

  Serial.println("VD");
  for (int i = 0; i <= last_index; i++) {
    Serial.println(valve[i]);
  }
  Serial.println("TD");
  for (int i = 0; i <= last_index; i++) {
    Serial.println(time[i]);
  }

  if (digitalRead(BUTTON_PIN) == HIGH) {  // Must hold the button down so that the system wraps up what it is doing and stops
    process_finished = true;
    Serial.println("OVER");
    digitalWrite(BUILTIN_LED_PIN, LOW);

    analogWrite(PWMA_PIN, 0);
  }

  Serial.println("END");
}
