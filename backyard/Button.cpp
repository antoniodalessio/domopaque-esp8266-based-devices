#include "Button.h"
  
Button::Button(int pin) {
    this->pin = pin;
    lastReading = LOW;
    init();
}

void Button::init() {
    pinMode(pin, INPUT_PULLUP);
}

void Button::onChange(Callback fn) {
  int reading = digitalRead(pin);

   Serial.print(String(reading));

  if (lastReading != reading) {
    fn();
  }

  lastReading = reading;
 
}

int Button::getState() {
    return state;
}
