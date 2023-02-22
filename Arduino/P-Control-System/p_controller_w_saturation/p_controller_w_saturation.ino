#include "HX711.h"
#include "utility.h"

// Button and LED pins
const int BUTTON_PIN = 4;
const int BUILTIN_LED_PIN = 13;
const int LOADCELL_DOUT_PIN = 2;
const int LOADCELL_SCK_PIN = 3;
const int POTENTIOMETER_PIN = A0;
const int MOTOR_PIN = 9;

HX711 scale;
SamplingTimeEnforcer sampling_time_enforcer(2000);
PumpControl pump(500, 1000);
P_Controller p_controller(226.779, 0, 137);

// [TODO] Use a class and develop a "DataManager" to more cleanly interface with the arrays of data I use. (Need to study more on mixing arrays with functions/methods)
const int data_size = 10;
float strain[data_size];
unsigned int valve[data_size];
unsigned long time[data_size];
float setpoint[data_size];

bool transmitted_data = false;

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

  // ---Initalizing motor driver standby pin---
  pump.setup(MOTOR_PIN);

  // ---Initalizing indicator LED to inform on the state of the program---
  digitalWrite(BUILTIN_LED_PIN, LOW);
}

void loop() {
  while (process_finished == true) {
    // Builtin LED blinks when the program is over
    digitalWrite(BUILTIN_LED_PIN, LOW);
    delay(100);
    digitalWrite(BUILTIN_LED_PIN, HIGH);
    delay(100);
   }

  // Button must be pushed for the system to start
  if (process_started == false) {  
    while (digitalRead(BUTTON_PIN) == LOW) { } 
      while (digitalRead(BUTTON_PIN) == HIGH) { }
      scale.tare();
      delay(100);
      process_started = true;
      digitalWrite(BUILTIN_LED_PIN, HIGH);
      sampling_time_enforcer.set_reference_time(millis());
  }

  for (int i = 0; i < data_size; i++) {
    // ---Regulates the sampling so it occurs at the designated sampling_time_ms (Simulates a soft real time operating system (Soft RTOS) behavior)---
    time[i] = sampling_time_enforcer.regulate();

    // Checks if a data transmission just occured as to not double poweroff the motor and wait needlessly (wasted time and confuses setting a sampling time)
    if (transmitted_data == false) {
      pump.stop();
    }
    else {
      transmitted_data = false;
    }

    while (scale.is_ready() == 0) { }
    strain[i] = scale.get_units();
    
    setpoint[i] = map(analogRead(POTENTIOMETER_PIN), 0, 1023, 0, 40);
    float error = setpoint[i] - strain[i]; 
    valve[i] = round(p_controller.accuation(error));
    pump.run(map(valve[i], 0, 137, 118, 255));

    // This serves somewhat as an interupt which ceases data collection at the current point and begins motor shutting off and data transmission
    last_index = i;
    if (digitalRead(BUTTON_PIN) == HIGH) {break;}
  }
  
  // ---Turn off pump motor before data transmission---
  pump.stop();

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
  Serial.println("SP");
  for (int i = 0; i <= last_index; i++) {
    Serial.println(setpoint[i], 3);
  }

  if (digitalRead(BUTTON_PIN) == HIGH) {  // Must hold the button down so that the system wraps up what it is doing and stops
    process_finished = true;
    Serial.println("OVER");
    digitalWrite(BUILTIN_LED_PIN, LOW);
  }

  Serial.println("END");
  transmitted_data = true;
}