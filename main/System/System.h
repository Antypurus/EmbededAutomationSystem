#ifndef SETR_SYSTEM_H
#define SETR_SYSTEM_H

#include <vector>
#include "../Virtual_Devices/Virtual_Device.h"
#include "../Virtual_Devices/io_event.h"

class System {
private:
    std::vector<VirtualDevice*> virtual_devices;
public:
    System() = default;
    void register_virtual_device(VirtualDevice* vd);
    std::vector<VirtualDevice*>& getVirtualDevices();
    void serve_io_event(io_event event);
};


#endif //SETR_SYSTEM_H
