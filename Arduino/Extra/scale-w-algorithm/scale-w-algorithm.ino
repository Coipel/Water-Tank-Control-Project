#include "HX711.h"

// ---Sensor data filtering related variables--- [TODO] Make a library and turn this algorithm into a class with private/public members
bool ENABLE_ALGORITHM = false;

float difference;
float max_difference = 1;
int spike_sample_length = 1;

// Make a setter function that forces setting both these terms in one rather than being public and changed one by one.
float max_value = 100;
float min_value = 0;

int repetition_count = 0; // Make private
bool outside_range; // Make private

HX711 scale;

// Button and LED pins
const int BUTTON_PIN = 4;
const int BUILTIN_LED_PIN = 13;

// Loadcell pins
const int LOADCELL_DOUT_PIN = 2;
const int LOADCELL_SCK_PIN = 3;

// Manual (later setpoint setter) motor driver input via potentiometer
const int POTENTIOMETER_PIN = A0;

const int data_size = 10;
float strain[data_size];
unsigned int valve[data_size];

float prior_strain = 0;

unsigned long time_start;
unsigned long time_now;
unsigned long time[data_size];

bool process_started = false;
bool process_finished = false;
int last_index = 0;

void setup() {
  Serial.begin(57600);

  // ---Initalizing weight scale---
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale(2280.f);

  // ---Initalizing button related pins---
  pinMode(BUILTIN_LED_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT);

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
      scale.tare();
      delay(100);
      process_started = true;
      digitalWrite(BUILTIN_LED_PIN, HIGH);
      time_start = millis();
  }

  for (int i = 0; i < data_size; i++) {
    
    time[i] = millis() - time_start;

    // ---Strain Process and Data Sample---
    while (scale.is_ready() == 0) { } // This introduces a delay of around 100 ms to get strain sample
    
    strain[i] = scale.get_units();
    if (ENABLE_ALGORITHM == true) {
      // ---Strain Data Collection Algorithm---
      if (i == 0) {
        prior_strain = strain[last_index];
      }
      else {
        prior_strain = strain[i-1];
      }
      
      difference = fabs(strain[i] - prior_strain);
      outside_range = strain[i] < min_value || strain[i] > max_value;
      if (difference <= max_difference && !outside_range) {
        strain[i] = (prior_strain-strain[i])*(difference/max_difference) + strain[i];
        repetition_count = 0;
      }

      else if (outside_range) {
        strain[i] = prior_strain;
        repetition_count = 0;
      }

      else {
        if (repetition_count < spike_sample_length) {
          strain[i] = prior_strain;
          repetition_count += 1;        
        }
        else {
          repetition_count = 0;
        }
      }
    }

    // ---Valve Process and Data Sample---
    valve[i] = map(analogRead(POTENTIOMETER_PIN), 0, 1023, 0, 255);

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
  }

  Serial.println("END");
}