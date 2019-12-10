#include <Arduino.h>

typedef std::function<void(void)> Callback;

class Switch {
  private:
    int pin;
    int state;
    unsigned long lastDebounceTime = 0;
    unsigned long debounceDelay = 50;
  
  public:
    Switch(int pin);
    int lastReading;
    void init();
    int getState();
    int onChange(Callback fn);
};
