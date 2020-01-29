#ifndef WIFI_STA
#define WIFI_STA

#include "esp_wifi.h"
#include "esp_event.h"
#include <string>

class WiFi_STA {
private:
    static bool isReady;
    wifi_config_t config = {};
    esp_err_t (*event_handler)(void*,system_event_t*);
    static std::string my_ip;
public:
    WiFi_STA(const std::string& ssid, const std::string& password);
    void set_event_handler(esp_err_t (*event_handler)(void *, system_event_t *));
    static bool isSTAReady() {return WiFi_STA::isReady;}
    static std::string get_ip(){return WiFi_STA::my_ip;}
private:
    void configure_sta(const std::string& ssid, const std::string& password);
    void init_wifi_station();
    static esp_err_t network_event_handler(void* ctx, system_event_t* event);
};


#endif //SETR_WIFI_STA_H
