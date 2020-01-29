#ifndef SETR_HARDWARECONTROLLER_H
#define SETR_HARDWARECONTROLLER_H

#include "driver/gpio.h"
#include <cstdint>

class HardwareController {
public:
    enum IO_MODE {
        INPUT,
        OUTPUT
    };
    enum EDGE_RAISE_MODE {
        RISING_EDGE,
        EDGE_DOWN,
        EDGE_ALL
    };
    typedef gpio_num_t GPIO_PIN;
    typedef uint8_t PIN_LEVEL;
    typedef gpio_config_t GPIO_CONFIG;
private:
    IO_MODE m_mode;
    GPIO_PIN m_pin;
    GPIO_CONFIG m_config;
    EDGE_RAISE_MODE m_raise;
public:
    HardwareController() = default;
    HardwareController(const unsigned int t_pin,  const IO_MODE t_io_mode);
    HardwareController(const unsigned int t_pin,  const EDGE_RAISE_MODE t_raise_mode);
    PIN_LEVEL get_pin_level() const;
    void set_pin_level(const PIN_LEVEL t_level);
    GPIO_CONFIG get_gpio_config() const;
    IO_MODE get_gpio_mode() const;
    EDGE_RAISE_MODE get_raise_mode() const;
    uint32_t get_pin() const;
    void register_pin() const;
    void hook_isr(void handler(void*));
private:
    void create_gpio_config();
    void create_input_gpio_config();
    void create_output_gpio_config();
};

#endif //SETR_HARDWARECONTROLLER_H
