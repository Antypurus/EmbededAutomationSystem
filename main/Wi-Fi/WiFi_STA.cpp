#include "WiFi_STA.h"

#include "esp_log.h"
#include "esp_system.h"
#include "esp_event_loop.h"

#include <lwip/err.h>
#include <lwip/sockets.h>
#include <lwip/sys.h>
#include <lwip/netdb.h>
#include <lwip/dns.h>

#include <iostream>
#include <string.h>

#if CONFIG_WIFI_ALL_CHANNEL_SCAN
#define DEFAULT_SCAN_METHOD WIFI_ALL_CHANNEL_SCAN
#elif CONFIG_WIFI_FAST_SCAN
#define DEFAULT_SCAN_METHOD WIFI_FAST_SCAN
#else
#define DEFAULT_SCAN_METHOD WIFI_FAST_SCAN
#endif /*CONFIG_SCAN_METHOD*/

#if CONFIG_WIFI_CONNECT_AP_BY_SIGNAL
#define DEFAULT_SORT_METHOD WIFI_CONNECT_AP_BY_SIGNAL
#elif CONFIG_WIFI_CONNECT_AP_BY_SECURITY
#define DEFAULT_SORT_METHOD WIFI_CONNECT_AP_BY_SECURITY
#else
#define DEFAULT_SORT_METHOD WIFI_CONNECT_AP_BY_SIGNAL
#endif /*CONFIG_SORT_METHOD*/

#if CONFIG_FAST_SCAN_THRESHOLD
#define DEFAULT_RSSI CONFIG_FAST_SCAN_MINIMUM_SIGNAL
#if CONFIG_EXAMPLE_OPEN
#define DEFAULT_AUTHMODE WIFI_AUTH_OPEN
#elif CONFIG_EXAMPLE_WEP
#define DEFAULT_AUTHMODE WIFI_AUTH_WEP
#elif CONFIG_EXAMPLE_WPA
#define DEFAULT_AUTHMODE WIFI_AUTH_WPA_PSK
#elif CONFIG_EXAMPLE_WPA2
#define DEFAULT_AUTHMODE WIFI_AUTH_WPA2_PSK
#else
#define DEFAULT_AUTHMODE WIFI_AUTH_OPEN
#endif
#else
#define DEFAULT_RSSI -127
#define DEFAULT_AUTHMODE WIFI_AUTH_OPEN
#endif /*CONFIG_FAST_SCAN_THRESHOLD*/

bool WiFi_STA::isReady = false;
std::string WiFi_STA::my_ip = "";

esp_err_t WiFi_STA::network_event_handler(void *ctx, system_event_t *event)
{
    esp_err_t err = ESP_OK;
    switch (event->event_id)
    {
    case SYSTEM_EVENT_STA_START:
    {
        std::cout << "station started" << std::endl;
        ESP_ERROR_CHECK(esp_wifi_connect());
        break;
    }
    case SYSTEM_EVENT_STA_STOP:
        std::cout << "station stopped" << std::endl;
        break;
    case SYSTEM_EVENT_STA_CONNECTED:
        std::cout << "station connected" << std::endl;
        break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
    {
        std::cout << "station disconnected" << std::endl;
        ESP_ERROR_CHECK(esp_wifi_connect());
        break;
    }
    case SYSTEM_EVENT_STA_AUTHMODE_CHANGE:
        std::cout << "ap authmode changed" << std::endl;
        break;
    case SYSTEM_EVENT_STA_GOT_IP:
        char ip[32];
        std::cout << "station got ip" << std::endl;
        std::cout << ip4addr_ntoa(&event->event_info.got_ip.ip_info.ip) << std::endl;
        sprintf(ip, IPSTR, IP2STR(&event->event_info.got_ip.ip_info.ip));
        WiFi_STA::my_ip = ip;
        WiFi_STA::isReady = true;
        break;
    case SYSTEM_EVENT_STA_LOST_IP:
        std::cout << "station lost ip" << std::endl;
        break;
    case SYSTEM_EVENT_STA_WPS_ER_SUCCESS:
        std::cout << "station wps success" << std::endl;
        break;
    case SYSTEM_EVENT_STA_WPS_ER_FAILED:
        std::cout << "station wps failed" << std::endl;
        break;
    case SYSTEM_EVENT_STA_WPS_ER_TIMEOUT:
        std::cout << "station wps timed out" << std::endl;
        break;
    case SYSTEM_EVENT_STA_WPS_ER_PIN:
        std::cout << "station wps pin code" << std::endl;
        break;
    default:
        break;
    }
    return err;
}

WiFi_STA::WiFi_STA(const std::string &ssid, const std::string &password)
{
    this->configure_sta(ssid, password);
    this->set_event_handler(WiFi_STA::network_event_handler);
    this->init_wifi_station();
}

void WiFi_STA::set_event_handler(esp_err_t (*event_handler)(void *, system_event_t *))
{
    this->event_handler = event_handler;
}

void WiFi_STA::init_wifi_station()
{
    tcpip_adapter_init();
    ESP_ERROR_CHECK(esp_event_loop_init(this->event_handler, NULL));
    wifi_init_config_t wifiInitializationConfig = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&wifiInitializationConfig));

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &config));
    ESP_ERROR_CHECK(esp_wifi_start());
}

void WiFi_STA::configure_sta(const std::string &ssid, const std::string &password)
{
    memcpy((char *)config.sta.ssid, ssid.c_str(), ssid.size() + 1);
    memcpy((char *)config.sta.password, password.c_str(), password.size() + 1);

    config.sta.scan_method = DEFAULT_SCAN_METHOD;
    config.sta.sort_method = DEFAULT_SORT_METHOD;
    config.sta.threshold.rssi = DEFAULT_RSSI;
    config.sta.threshold.authmode = DEFAULT_AUTHMODE;
}
