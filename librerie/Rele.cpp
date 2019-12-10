#include "Rele.h"

/*
 * @todo
 * Default constructor function parameters does't works and neither polymorphism also
 * You need to pass -1(int) in order to bypass lack of led
 *
 */


Rele::Rele(int pin, String name, String alias, int step, int pinLed = -1) {
  this->pin = pin;
  this->name = name;
  this->alias = alias;
  this->step = step;
  this->pinLed = pinLed;
  if (pinLed != -1) {
    Led led1;
    led1.init(pinLed);
    this->led = led1;
  }
  init();
}

void Rele::init() {
  pinMode(pin, OUTPUT);
  off();
}
int Rele::getState() {
    return !digitalRead(pin);
}
void Rele::off() {
  digitalWrite(pin, HIGH);
  if (pinLed != -1) {
    led.off();
  }
}
void Rele::on() {
  digitalWrite(pin, LOW);
  if (pinLed != -1) {
    led.on();
  }
}

void Rele::toggle() {
  if (getState() == HIGH) {
      off();
    }else{
      on();
    }
}
