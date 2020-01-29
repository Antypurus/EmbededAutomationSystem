#include "WiFi_AP.h"
#include "esp_event_loop.h"
#include <iostream>

esp_err_t WiFi_AP::network_event_handler(void* ctx, system_event_t* event)
{
    esp_err_t err=ESP_OK;
    std::cout<<event->event_id<<std::endl;
    switch(event->event_id) {
        case SYSTEM_EVENT_AP_START:
            std::cout << "softap started" << std::endl;
            break;
        case SYSTEM_EVENT_AP_STOP:
            std::cout << "softAP stopped" << std::endl;
            break;
        case SYSTEM_EVENT_AP_STACONNECTED:
            std::cout << "station connected" << std::endl;
            break;
        case SYSTEM_EVENT_AP_STADISCONNECTED:
            std::cout << "station disconnected" << std::endl;
            break;
        case SYSTEM_EVENT_AP_STAIPASSIGNED:
            std::cout << "assigned ip to sta" << std::endl;
            break;
        case SYSTEM_EVENT_AP_PROBEREQRECVED:
            std::cout << "received probe request" << std::endl;
            break;
        default: 
            break;
    }
    return err;
}

WiFi_AP::WiFi_AP(const std::string& ssid,const std::string& password):m_ssid(ssid),m_password(password)
{
    this->configure_ap();
    this->s_wifi_event_group = xEventGroupCreate();
    this->set_event_handler(WiFi_AP::network_event_handler);
    this->init_wifi_access_point();
}

void WiFi_AP::configure_ap()
{
    this->m_ssid.copy((char*)config.ap.ssid,this->m_ssid.size(),0);
    this->m_password.copy((char*)config.ap.password,this->m_password.size(),0);

    config.ap.channel = AP_CHANNEL;
    config.ap.ssid_len = this->m_ssid.size()+1;
    config.ap.max_connection = AP_MAX_CONNECTION;
    if(this->m_password.length() != 0)
    {
        config.ap.authmode = WIFI_AUTH_WPA_WPA2_PSK;
    } else {
        config.ap.authmode = WIFI_AUTH_OPEN;
    }
    config.ap.ssid_hidden = 0;
    config.ap.beacon_interval = AP_BEACON_INTERVAL;
}

void WiFi_AP::set_event_handler(esp_err_t (*event_handler)(void *, system_event_t *)) {
    this->event_handler = event_handler;
}

void WiFi_AP::init_wifi_access_point() {
    
    tcpip_adapter_init();
    ESP_ERROR_CHECK(esp_event_loop_init(this->event_handler, NULL));
    wifi_init_config_t wifiInitializationConfig = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&wifiInitializationConfig));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_AP, &config));
    ESP_ERROR_CHECK(esp_wifi_start());
}
