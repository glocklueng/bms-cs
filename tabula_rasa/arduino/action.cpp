#include "action.h"


/**
 Calling timer functions repeatedly is slow,
 so this global stores the current time, in milliseconds,
 the last time you called loop()
*/
long action_time_ms; 

/* A "meta device" that runs this action_list every N milliseconds.
   These are used to make the faster rungs call the slower rungs. */
template <long every_N_ms>
class timer_device : public action {
  long time_last_ms; // the last time we ran
  action_list &devices;
public:
  timer_device(action_list &devices_) :time_last_ms(-1),devices(devices_) {}
  void loop() {
    if (action_time_ms-time_last_ms>=every_N_ms) 
    {
       devices.loop();
       time_last_ms=action_time_ms;
    }
  }
};

/** 
  These are scheduling rungs, used to avoid inefficient duplicate polling.
  
  You add your action to the appropriate rung, like
      actions_10ms.add(new my_crazy_device(A3));
*/
action_list actions_always; // run every loop()
action_list actions_1ms; // run at most every 1 ms
action_list actions_10ms; // every 10ms
action_list actions_100ms; // every 100ms
action_list actions_1s; // every 1 second
action_list actions_10s; // every 10 seconds
action_list actions_100s; // every 100 seconds
action_list actions_1000s; // every 1000 seconds


void action_setup() {
  // Link a timer into each faster list, to run the next-slower list
  actions_always.set(new timer_device<1>(actions_1ms));
  actions_1ms.set(new timer_device<10>(actions_10ms));
  actions_10ms.set(new timer_device<100>(actions_100ms));
  actions_100ms.set(new timer_device<1000>(actions_1s));
  actions_1s.set(new timer_device<10000>(actions_10s));
  actions_10s.set(new timer_device<100000>(actions_100s));
  actions_100s.set(new timer_device<1000000>(actions_1000s));
}

void action_loop() {
  // Update the timers and run the loop
  action_time_ms=micros()>>10;
  actions_always.loop();
}


#ifdef ACTION_TEST
/********* Example code, measure latency */
class serial_print : public action {
  const char *msg;
public:
  serial_print(const char *msg_) :msg(msg_) {}
  virtual void loop() { 
    Serial.println(msg); 
  }
};

// Print an int every call
class int_print : public serial_print {
  long *print_this;
public:
  int_print(long *print_this_,const char *msg_) 
     :serial_print(msg_), print_this(print_this_) {}
  
  virtual void loop() { 
    Serial.print(*print_this); 
    serial_print::loop(); 
  }
};

// Latency tester
long max_latency=1;
long last_time=0;
class latency_watcher : public action {
public:
  virtual void loop() {
    long micro_time=micros();
    long cur_latency=micro_time-last_time;
    last_time=micro_time;
    if (cur_latency>max_latency) max_latency=cur_latency;
  }
};

// Testing counter
long counter=1234;
class int_counter : public action {
public:
  virtual void loop() { 
    counter++;
  }
};
class int_counter_reset : public action {
public:
  virtual void loop() { 
    counter=0;
  }
};

template<int LATENCY>
class latency_spike : public action {
public:
  virtual void loop() {
    delay(LATENCY);
  }
};


void setup() {
  Serial.begin(57600);
  Serial.println("Action tester starting up!");
  
  action_setup();

  // Test with a serial print device
  actions_1s.add(new serial_print("Every 1 second"));
  actions_10s.add(new serial_print("Every 10 seconds"));
  actions_100s.add(new serial_print("Every 100 seconds"));
  
  actions_always.add(new int_counter);
  actions_1s.add(new int_counter_reset);
  actions_1s.add(new int_print(&counter," actions_always per second"));
  
  actions_always.add(new latency_watcher);
  actions_1s.add(new int_print(&max_latency," max latency"));
  // actions_10s.add(new latency_spike<20>);
}


void loop() {
  action_loop();
}

#endif
