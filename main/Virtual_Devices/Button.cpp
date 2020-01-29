#include "Button.h"

Button::Button(uint32_t id, uint32_t pin, UDPSocket* socket, void isr(void*))
{
    this->m_virtual_device_id = id;
    this->socket = socket;

    this->button_controller = new HardwareController(pin,HardwareController::IO_MODE::INPUT);
    this->button_controller->hook_isr(isr);

    this->register_event_handler(static_cast<io_event>(pin),[this](io_event event, VirtualDevice* vd)
    {
        this->socket->send("LED");
    });

    this->register_hardware_controller(*this->button_controller);

    this->start_vd_io_dispatcher();
}