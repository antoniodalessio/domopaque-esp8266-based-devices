#include <Arduino.h>

typedef std::function<void(void)> Callback;

class Button {
  private:
    int pin;
    int state;
    unsigned long lastDebounceTime = 0;
    unsigned long debounceDelay = 50;
  
  public:
    Button(int pin);
    int lastReading;
    void init();
    int getState();
    void onChange(Callback fn);
};
