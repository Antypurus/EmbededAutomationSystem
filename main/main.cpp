#include "esp_wifi.h"
#include "nvs_flash.h"
#include <string>
#include <iostream>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"

#include "./Wi-Fi/WiFi_STA.h"
#include "./Wi-Fi/WiFi_AP.h"
#include "./HardwareController/HardwareController.h"
#include "./Socket/UDPSocket.h"

#include "./Virtual_Devices/io_event.h"
#include "./Virtual_Devices/LED.h"
#include "./Virtual_Devices/Button.h"

#include "./System/NetworkDispatcher.h"

#include "esp_timer.h"

void sendTask(void* par)
{
    UDPSocket socket("255.255.255.255",80);
    while(true)
    {
        auto ret = socket.receive();
        std::cout<<ret<<std::endl;
        vTaskDelay( 5 / portTICK_PERIOD_MS);
    }
}

static System* str = NULL;
static xQueueHandle gpio_evt_queue = NULL;

static void io_dispatcher(void* args)
{
    uint32_t io_num;
    for(;;) {
        if(xQueueReceive(gpio_evt_queue, &io_num, portMAX_DELAY)) {
            if(str!=NULL)
            {
                str->serve_io_event(static_cast<io_event>(io_num));
            }
        }
        vTaskDelay( 30 / portTICK_PERIOD_MS);
    }
}

static void IRAM_ATTR handler(void* arg)
{
    uint32_t gpio_num = (uint32_t) arg;
    xQueueSendFromISR(gpio_evt_queue, &gpio_num, NULL);
}

void wifi_init() {
    WiFi_STA("ZON-E930","790d0b981af2");

    while(!WiFi_STA::isSTAReady())
    {
        vTaskDelay( 15 / portTICK_PERIOD_MS);
    }

    UDPSocket socket("255.255.255.255",80);

    LED led(1234,33);
    Button button(12,34,&socket,handler);

    System system;
    system.register_virtual_device(&led);
    system.register_virtual_device(&button);

    str = &system;

    NetworkDispatcher dispatcher(&socket,&system);

    gpio_evt_queue = xQueueCreate(10, sizeof(uint32_t));

    xTaskCreate(&io_dispatcher, "io dispatcher", 2048, NULL, 2, NULL);

    while(true)
    {
        vTaskDelay( 15 / portTICK_PERIOD_MS);
    }
}


extern "C" void app_main() {

    nvs_flash_init();

    wifi_init();
}
