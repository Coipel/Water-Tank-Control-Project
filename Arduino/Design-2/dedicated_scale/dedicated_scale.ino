#include "HX711.h"

HX711 scale;

bool ENABLE_ALGORITHM = false;

float difference;
float max_difference = 1;
int spike_sample_length = 1;

float max_value = 100;
float min_value = 0;

int repetition_count = 0; // Make private
//bool outside_range; // Make private

float prior_strain = 0;
float strain;

// Loadcell pins
const int LOADCELL_DOUT_PIN = 2;
const int LOADCELL_SCK_PIN = 3;

// Output pins
const int STRAIN_PIN = 9;

// Pin used for debugging
const int PWM_PIN = 8;

void setup() {
  Serial.begin(57600);

  // ---Initalizing weight scale---
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale(2280.f);
  scale.tare();

  pinMode(STRAIN_PIN, OUTPUT);
  pinMode(PWM_PIN, INPUT);
}

void loop() {
  while (scale.is_ready() == 0) { }

  strain = scale.get_units();
  bool outside_range = strain < min_value || strain > max_value;

  if (ENABLE_ALGORITHM == true) {
    difference = fabs(strain - prior_strain);
    if (difference <= max_difference && !outside_range) {
      strain = (prior_strain-strain)*(difference/max_difference) + strain;
      repetition_count = 0;
    }

    else if (outside_range) {
      strain = prior_strain;
      repetition_count = 0;
    }

    else {
      if (repetition_count < spike_sample_length) {
        strain = prior_strain;
        repetition_count += 1;        
      }
      else {
        repetition_count = 0;
      }
    }

    prior_strain = strain;
  }

  Serial.print("strain: ");
  Serial.print(strain);
  Serial.print(" ");
  int massaged_strain;
  if (!outside_range) {
    massaged_strain = map(strain, min_value, max_value, 0, 242);
  }

  else if (strain < min_value) {
    massaged_strain = 0;
  }

  else {
    massaged_strain = 242; // Note: the value 255 isn't used as to make it unambiquous in PWM duty cycle reading whether something is 0% or 100% new duty cycle range is 0% ~ 95% (actually ~90% empirically).
  }
  
  Serial.print("massaged_strain: ");
  Serial.println(massaged_strain);
  analogWrite(STRAIN_PIN, massaged_strain);

  /*
  float frequency = 490; // 490 Hz
  float period = 1000000/frequency; // 1/490 Hz, but scaled to be for mircoseconds period
  //unsigned long time_start = micros();
  
  float duty_cycle = (pulseIn(PWM_PIN, HIGH, 3*period)*100)/period;
  //unsigned long time_difference = micros() - time_start;

  Serial.print(" ");
  //Serial.print("time_difference: ");
  //Serial.print(time_difference);
  //Serial.print(" ");

  Serial.print("duty_cycle: ");
  Serial.print(duty_cycle);
  Serial.print(" ");
  float rx_strain = map(duty_cycle, 0, 90, min_value, max_value);

  Serial.print("rx_strain: ");
  Serial.println(rx_strain);
  */

}
