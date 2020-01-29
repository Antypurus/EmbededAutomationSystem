#ifndef SETR_BUTTON_H
#define SETR_BUTTON_H

#include "./Virtual_Device.h"
#include "../Socket/UDPSocket.h"

class Button: public VirtualDevice {
private:
    HardwareController* button_controller;
    UDPSocket* socket;
public:
    Button(uint32_t id, uint32_t pin, UDPSocket* socket, void isr(void*));
    void default_action() override {};
};

#endif //SETR_BUTTON_H
