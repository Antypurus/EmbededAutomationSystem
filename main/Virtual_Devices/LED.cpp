#include "LED.h"

#include <iostream>

LED::LED(const uint32_t id, const uint32_t pin)
{
    this->m_virtual_device_id = id;

    this->led_controller = new HardwareController(pin,HardwareController::IO_MODE::OUTPUT);

    this->register_hardware_controller(*this->led_controller);

    this->register_network_event_handler([](std::string event,VirtualDevice* vd)
        {
            if(event=="LED") {
                LED *led = dynamic_cast<LED *>(vd);
                led->toggle();
            }
        });

    this->start_vd_io_dispatcher();
    this->start_vd_network_dispatcher();
}

bool LED::toggle()
{
    this->status = !this->status;
    if(this->status)
    {
        this->led_controller->set_pin_level(1);
    } else {
        this->led_controller->set_pin_level(0);
    }
    return this->status;
}

void LED::turn_on()
{
    this->status = true;
    this->led_controller->set_pin_level(1);
}

void LED::turn_off()
{
    this->status = false;
    this->led_controller->set_pin_level(0);
}
