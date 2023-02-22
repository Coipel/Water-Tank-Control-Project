// ---Class for handling having sampling occur like a Soft RTOS---
class SamplingTimeEnforcer {
  private:
    unsigned long reference_time_ms;
    unsigned long time_elapsed_ms;
    unsigned long sampling_time_ms;
    unsigned long sampling_regulator_ms;

    public:
      SamplingTimeEnforcer(unsigned long sampling_time_ms);
      void set_reference_time(unsigned long reference_time_ms); // reference_time_ms != object creation time, hence why isn't set in constructor.
      unsigned long elapsed_time_ms();
      unsigned long regulate();
};

// ---Class for handling motor control conveniently---
class PumpControl {
  private:
    unsigned long stop_delay;
    unsigned long run_delay; //powered prefered over poweron since powered state is not a single value
    int motor_pin;

  public:
    PumpControl(unsigned long stop_delay, unsigned long run_delay);
    void setup(const int motor_pin);
    void stop();
    void run(int strength);
};

//---Class for P controller conveniently---
class P_Controller {
  private:
    float P;
    float lowerbound_saturation;
    float upperbound_saturation;

  public:
    P_Controller(float P, float lowerbound_saturation, float upperbound_saturation);
    float accuation(float error);
};