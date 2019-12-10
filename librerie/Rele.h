#include <Arduino.h>
#include "Led.h"

//typedef Led LED;

class Rele {
  
  private:
    int pin;
    
  public:
    Rele(int pin, String name, String alias, int step, int pinLed);
    String name;
    String alias;
    int step;
    int pinLed;
    Led led;
    void init();
    int getState();
    void on();
    void off();
    void toggle();
};
