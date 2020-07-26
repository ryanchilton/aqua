#ifndef MOTOR_TIMERS_HPP
#define MOTOR_TIMERS_HPP

class Timer
{
public:
  Timer(unsigned long timer_duration_ms) : duration_ms(timer_duration_ms),
                                           end_time_ms(0)
  {
    
  }

  void start()
  {
    end_time_ms = millis() + duration_ms;
  }

  bool isActive()
  {
    return millis() < end_time_ms;
  }
private:
  unsigned long duration_ms;
  unsigned long end_time_ms;
};

class TimeTrigger
{
public:

  TimeTrigger(unsigned long timer_period_ms, unsigned long timer_run_duration_ms) : period_ms(timer_period_ms), 
                                                                                    run_duration_ms(timer_run_duration_ms),
                                                                                    active(false)
  {
    unsigned long now = millis();
    next_start_time_ms = now + timer_period_ms;
    next_stop_time_ms  = now + timer_period_ms + run_duration_ms;
    
  }
  
  bool isActive()
  {
    updateState();
    return active;
  }

  unsigned long getCountdownTime_ms()
  {
    return next_start_time_ms - millis();
  }

private:
  void updateState()
  {
    unsigned long now = millis();
    if (active)
    {
      // see if run duration is complete
      if (now > next_stop_time_ms)
      {
        active = false;
      }
    }
    else // not active
    {
      // see if it's time to activate
      if (now > next_start_time_ms)
      {
        active = true;
        next_start_time_ms = now + period_ms;
        next_stop_time_ms  = now + run_duration_ms;
      }
    }
  }

  bool          active;               // indicates if trigger is active/on
  unsigned long period_ms;            // how often is this triggered?
  unsigned long run_duration_ms;      // how long should the motor run?'
  unsigned long next_start_time_ms;   // when should we go active again?
  unsigned long next_stop_time_ms;    // when should we go inactive again?
};

#endif
