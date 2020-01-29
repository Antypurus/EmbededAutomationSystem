#ifndef SETR_NETWORKDISPATCHER_H
#define SETR_NETWORKDISPATCHER_H

#include "../Socket/UDPSocket.h"
#include <queue>
#include <string>
#include "./System.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

enum VD_MAP {
    LE,
};

class NetworkDispatcher {
private:
    UDPSocket* socket;
    std::queue<std::string> network_packet_queue;
    //xQueueHandle network_packet_queue = NULL;

    System* system;
public:
    NetworkDispatcher(UDPSocket* socket,System* system);
    static void dispatcher(void* args);
    static void parser(void* args);
private:
    void start_dispatcher();
    void start_parser();
};


#endif //SETR_NETWORKDISPATCHER_H
