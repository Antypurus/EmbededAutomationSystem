#ifndef SETR_LED_H
#define SETR_LED_H

#include "./Virtual_Device.h"

class LED: public VirtualDevice {
private:
    bool status = false;
    HardwareController* led_controller;
public:
    LED(const uint32_t id, const uint32_t pin);
    bool toggle();
    void turn_on();
    void turn_off();
    void default_action() override {this->toggle();};
};


#endif //SETR_LED_H
