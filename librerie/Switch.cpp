#include "Switch.h"
  
Switch::Switch(int pin) {
    this->pin = pin;
    lastReading = LOW;
    init();
}

void Switch::init() {
    pinMode(pin, INPUT);
}

int Switch::onChange(Callback fn) {

  int flag = LOW;
  
  int reading = digitalRead(pin);
  if (reading != lastReading) {
    lastDebounceTime = millis();
  }

  if (millis() - lastDebounceTime > debounceDelay) {
    if (reading != state) {
      state = reading;
      if (state == HIGH) {
        flag = HIGH;
        fn();
      }
    }
    
  }

  lastReading = reading;
  return flag;
  
}

int Switch::getState() {
    return state;
}
