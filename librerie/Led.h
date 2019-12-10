#include <Arduino.h>

class Led {
  
  private:
    int pin;
    
  public:
    void init(int pin);
    int getState();
    void on();
    void off();
    void toggle();
};
