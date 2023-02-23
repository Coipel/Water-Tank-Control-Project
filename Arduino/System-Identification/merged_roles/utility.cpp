#include <Arduino.h>
#include "utility.h"

// ------------------------------------------------------------------Class for having sampling occur like a Soft RTOS-------------------------------------------------------------------------
SamplingTimeEnforcer::SamplingTimeEnforcer(unsigned long sampling_time_ms) {
  SamplingTimeEnforcer::sampling_time_ms = sampling_time_ms;
  SamplingTimeEnforcer::sampling_regulator_ms = sampling_time_ms;
}

void SamplingTimeEnforcer::set_reference_time(unsigned long reference_time_ms) {
  SamplingTimeEnforcer::reference_time_ms = reference_time_ms;
}

unsigned long SamplingTimeEnforcer::elapsed_time_ms() {
  return millis() - SamplingTimeEnforcer::reference_time_ms;
}

unsigned long SamplingTimeEnforcer::regulate() {
  if (SamplingTimeEnforcer::elapsed_time_ms() != 0) {
    while (SamplingTimeEnforcer::elapsed_time_ms() < SamplingTimeEnforcer::sampling_regulator_ms) { }
    SamplingTimeEnforcer::sampling_regulator_ms += SamplingTimeEnforcer::sampling_time_ms;
  }
  return SamplingTimeEnforcer::elapsed_time_ms();
}
// -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


// ---------------------------------------------------------------------Class for handling motor control conveniently-------------------------------------------------------------------------
PumpControl::PumpControl(unsigned long stop_delay, unsigned long run_delay) {
  PumpControl::stop_delay = stop_delay;
  PumpControl::run_delay = run_delay;
}

void PumpControl::setup(const int motor_pin) {
  PumpControl::motor_pin = motor_pin;
  pinMode(motor_pin, OUTPUT);
  PumpControl::stop();
}

void PumpControl::stop() {
  analogWrite(PumpControl::motor_pin, 0);
  delay(PumpControl::stop_delay);
}

void PumpControl::run(int strength) {
  analogWrite(PumpControl::motor_pin, strength);
  delay(PumpControl::run_delay);
}
// -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------