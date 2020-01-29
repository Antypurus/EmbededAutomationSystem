#ifndef SETR_UDPSOCKET_H
#define SETR_UDPSOCKET_H

#include <string>

#include "freertos/FreeRTOS.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include <lwip/err.h>
#include <lwip/sockets.h>
#include <lwip/sys.h>
#include <lwip/netdb.h>
#include <lwip/dns.h>
#include "esp_log.h"
#include "esp_system.h"
#include "esp_event_loop.h"
#include <iostream>
#include <string.h>

#define SOCKET_TIMEOUT_MS 10000

class UDPSocket {
public:
    typedef int SOCKET_FD;
    typedef struct sockaddr_in SOCKET_ADDRESS;
    typedef uint32_t SOCKET_PORT;
private:
    struct timeval timeoutTimeInMiliSeconds = {0};
    SOCKET_FD m_socket_fd;
    std::string m_str_my_address;
    std::string m_str_out_address;
    SOCKET_ADDRESS m_my_address;
    SOCKET_ADDRESS m_out_address;
    SOCKET_PORT m_port;
public:
    UDPSocket() = default;
    UDPSocket(const std::string& t_address, const SOCKET_PORT t_port);
    UDPSocket(const SOCKET_PORT t_port);
    void listen(const SOCKET_PORT t_port);
    void close_socket();
    int send(const std::string& t_data);
    int send(const std::string& t_out_address,const std::string& t_data);
    void set_out_address(const std::string& t_out_address);
    std::string receive() const;
    void join_multicast_group();
private:
    void configure_my_socket_address();
    void configure_out_socket_address();
    void create_socket();
    void bind_socket();
    void set_multicast_ttl();
    void disable_multicast_loop();
};


#endif //SETR_UDPSOCKET_H
