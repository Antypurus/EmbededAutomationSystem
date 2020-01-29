#include "NetworkDispatcher.h"

#include <iostream>
#include <vector>

NetworkDispatcher::NetworkDispatcher(UDPSocket* socket,System* system)
{
    this->socket = socket;
    this->system = system;
    this->start_dispatcher();
    this->start_parser();
};

void add_to_queue(std::queue<std::string>* q,std::string val)
{
    q->push(val);
}

void NetworkDispatcher::dispatcher(void* args)
{
    std::vector<void*>* arg_list = static_cast<std::vector<void*>*>(args);
    UDPSocket* socket = static_cast<UDPSocket*>((*arg_list)[0]);
    std::queue<std::string>* network_queue = static_cast<std::queue<std::string>*>((*arg_list)[1]);

    delete arg_list;

    std::string packet = "";
    while (true)
    {
        packet = socket->receive();
        if(packet!="")
        {
            add_to_queue(network_queue,packet);
            packet="";
        }

        vTaskDelay( 20 / portTICK_PERIOD_MS);
    }
}

void NetworkDispatcher::parser(void* args)
{
    std::vector<void*>* arg_list = static_cast<std::vector<void*>*>(args);
    System* system = static_cast<System*>((*arg_list)[0]);
    std::queue<std::string>* network_queue = static_cast<std::queue<std::string>*>((*arg_list)[1]);

    delete arg_list;

    while(true)
    {
        while(!network_queue->empty())
        {
            for(auto device:system->getVirtualDevices())
            {
                device->serve_network_event(network_queue->front());
            }
            network_queue->pop();
        }

        vTaskDelay( 40 / portTICK_PERIOD_MS);
    }
}

void NetworkDispatcher::start_dispatcher()
{
    std::vector<void*>* args = new std::vector<void*>{this->socket,&this->network_packet_queue};
    xTaskCreate(&NetworkDispatcher::dispatcher, "Network dispatcher", 2048, args, 3, NULL);
}

void NetworkDispatcher::start_parser()
{
    std::vector<void*>* args = new std::vector<void*>{this->system,&this->network_packet_queue};
    xTaskCreate(&NetworkDispatcher::parser, "Network Parser", 2048, args, 1, NULL);
}
