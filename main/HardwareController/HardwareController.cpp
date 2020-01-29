#include "HardwareController.h"
#include <iostream>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

HardwareController::HardwareController(const unsigned int t_pin,  const IO_MODE t_io_mode)
{
    m_pin = static_cast<gpio_num_t>(t_pin);
    m_mode = t_io_mode;
    // TODO: need ot validate selected pin
    if(this->m_mode == IO_MODE::INPUT)
    {
        this->m_raise = EDGE_RAISE_MODE::EDGE_ALL;
    }
    this->create_gpio_config();
}

HardwareController::HardwareController(const unsigned int t_pin,  const EDGE_RAISE_MODE t_raise_mode)
{
    m_pin = static_cast<gpio_num_t>(t_pin);
    m_mode = IO_MODE::INPUT;
    m_raise = t_raise_mode;
    // TODO: Need to validated selected pin number
    this->create_gpio_config();
}

void HardwareController::create_gpio_config()
{
    switch(this->m_mode)
    {
        case (IO_MODE::INPUT):
        {
            this->create_input_gpio_config();
            break;
        }
        case (IO_MODE::OUTPUT):
        {
            this->create_output_gpio_config();
            break;
        }
        default:break;
    }
}

void HardwareController::create_input_gpio_config()
{
    // create bitmask for the selected pin
    auto pin_selection = 1ULL << this->m_pin;

    // disable interrupts
    this->m_config.intr_type = GPIO_INTR_POSEDGE;
    // set pin to output mode
    this->m_config.mode = GPIO_MODE_INPUT;
    // enable the pull up and pull down mode
    this->m_config.pull_up_en = GPIO_PULLUP_DISABLE;
    //this->m_config.pull_down_en = GPIO_PULLDOWN_DISABLE;
    // set bitmask to select pin
    this->m_config.pin_bit_mask = pin_selection;
    // configure interrupt mode for the edge raise mode
    switch(this->m_raise)
    {
        case (EDGE_RAISE_MODE::RISING_EDGE):
        {
            gpio_set_intr_type(this->m_pin, GPIO_INTR_POSEDGE);
            break;
        }
        case (EDGE_RAISE_MODE::EDGE_DOWN):
        {
            gpio_set_intr_type(this->m_pin, GPIO_INTR_NEGEDGE);
            break;
        }
        case (EDGE_RAISE_MODE::EDGE_ALL):
        {
            gpio_set_intr_type(this->m_pin, GPIO_INTR_ANYEDGE);
            break;
        }
        default:break;
    }
}

void HardwareController::create_output_gpio_config()
{
    // create bitmask for the selected pin
    auto pin_selection = 1ULL << this->m_pin;

    // disable interrupts
    this->m_config.intr_type = GPIO_INTR_DISABLE;
    // set pin to output mode
    this->m_config.mode = GPIO_MODE_OUTPUT;
    // enable the pull up and pull down mode
    this->m_config.pull_up_en = GPIO_PULLUP_ENABLE;
    this->m_config.pull_down_en = GPIO_PULLDOWN_ENABLE;
    // set bitmask to select pin
    this->m_config.pin_bit_mask = pin_selection;
}

HardwareController::PIN_LEVEL HardwareController::get_pin_level() const
{
    return gpio_get_level(this->m_pin);
}

void HardwareController::set_pin_level(const PIN_LEVEL t_level)
{
    gpio_set_level(this->m_pin,t_level);
}

HardwareController::GPIO_CONFIG HardwareController::get_gpio_config() const
{
    return this->m_config;
}

HardwareController::IO_MODE HardwareController::get_gpio_mode() const
{
    return this->m_mode;
}

HardwareController::EDGE_RAISE_MODE HardwareController::get_raise_mode() const
{
    return this->m_raise;
}

void HardwareController::register_pin() const
{
    gpio_config(&this->m_config);
}

void HardwareController::hook_isr(void handler(void*))
{
    gpio_install_isr_service(0);
    gpio_isr_handler_add(this->m_pin, handler, (void*) this->m_pin);
}

uint32_t HardwareController::get_pin() const
{
    return static_cast<uint32_t>(this->m_pin);
}