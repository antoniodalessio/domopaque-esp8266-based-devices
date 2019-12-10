#include "Led.h"


void Led::init(int pin) {
  this->pin = pin;
  pinMode(pin, OUTPUT);
  off();
}
int Led::getState() {
    return digitalRead(pin);
}
void Led::off() {
  digitalWrite(pin, LOW);
}
void Led::on() {
  digitalWrite(pin, HIGH);
}

void Led::toggle() {
  if (getState() == HIGH) {
      off();
    }else{
      on();
    }
}
