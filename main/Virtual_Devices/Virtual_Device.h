#ifndef SETR_VIRTUAL_DEVICE_H
#define SETR_VIRTUAL_DEVICE_H

#include <vector>
#include <queue>
#include <string>
#include <set>
#include <unordered_map>
#include <functional>

#include <iostream>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "./io_event.h"
#include "../HardwareController/HardwareController.h"

#include "esp_timer.h"

class VirtualDevice
{
protected:
    std::set<io_event> m_subscribed_io_events;
    std::vector<HardwareController> m_controllers;
    uint32_t m_virtual_device_id;

    std::queue<io_event> m_io_event_queue;
    std::queue<std::string> network_event_queue;
    std::unordered_multimap<io_event,std::function<void(io_event,VirtualDevice*)>>
            m_io_event_handlers;

    std::function<void(std::string,VirtualDevice*)> network_event_handler;
public:
    const std::set<io_event>& get_subscribed_events() const
    {
        return this->m_subscribed_io_events;
    }

    uint32_t get_virtual_device_id() const
    {
        return this->m_virtual_device_id;
    }

    void serve_io_event(const io_event event)
    {
        this->m_io_event_queue.push(event);
    }

    void serve_network_event(const std::string& network_packet)
    {
        this->network_event_queue.push(network_packet);
    }

    void subscribe_io_event(const io_event event)
    {
        this->m_subscribed_io_events.insert(event);
    }

    void register_event_handler(const io_event event,std::function<void(io_event,VirtualDevice*)> handler)
    {
        this->m_io_event_handlers.emplace(event ,handler);
    }

    void register_network_event_handler(std::function<void(std::string,VirtualDevice*)> handler)
    {
        this->network_event_handler = handler;
    }

    static void vd_io_dispatcher(void* args)
    {
        auto arg_list = static_cast<std::vector<void*>*>(args);
        auto io_q = static_cast<std::queue<io_event>*>((*arg_list)[0]);
        auto io_event_handlers = static_cast<
                std::unordered_multimap<
                        io_event,std::function<void(io_event,VirtualDevice*)>>*>((*arg_list)[1]);
        auto virtual_device = static_cast<VirtualDevice*>((*arg_list)[2]);

        while(true)
        {
            while(!io_q->empty())
            {
                auto event = io_q->front();
                auto handlers = io_event_handlers->equal_range(event);
                for(auto it = handlers.first;it != handlers.second; ++it)
                {
                    it->second(event,virtual_device);
                }
                io_q->pop();
            }
            vTaskDelay( 50 / portTICK_PERIOD_MS);
        }
    }

    static void vd_network_dispatcher(void* args)
    {
        auto arg_list = static_cast<std::vector<void*>*>(args);
        auto network_q = static_cast<std::queue<std::string>*>((*arg_list)[0]);
        auto handler = static_cast<std::function<void(std::string,VirtualDevice*)>*>((*arg_list)[1]);
        auto virtual_device = static_cast<VirtualDevice*>((*arg_list)[2]);

        delete arg_list;

        while(true)
        {
            while(!network_q->empty())
            {
                (*handler)(network_q->front(),virtual_device);
                network_q->pop();
            }

            vTaskDelay( 60 / portTICK_PERIOD_MS);
        }
    }

    virtual void default_action() = 0;
protected:
    void register_hardware_controllers(const std::vector<HardwareController>& controllers)
    {
        for(auto& controller:controllers)
        {
            this->register_hardware_controller(controller);
        }
    }

    void register_hardware_controller(const HardwareController& controller)
    {
        controller.register_pin();
        this->m_controllers.push_back(controller);
    }

    void start_vd_io_dispatcher()
    {
        std::vector<void*>* args = new std::vector<void*>{&this->m_io_event_queue,&this->m_io_event_handlers,&*this};
        xTaskCreate(&VirtualDevice::vd_io_dispatcher, "virtual device io dispatcher", 2048, args, 4, NULL);
    }

    void start_vd_network_dispatcher()
    {
        std::vector<void*>* args = new std::vector<void*>{&this->network_event_queue,&this->network_event_handler,this};
        xTaskCreate(&VirtualDevice::vd_network_dispatcher, "virtual device network dispatcher", 2048, args, 5, NULL);
    }
};

#endif //SETR_VIRTUAL_DEVICE_H
