#include "System.h"

void System::register_virtual_device(VirtualDevice* vd)
{
    this->virtual_devices.push_back(vd);
}

std::vector<VirtualDevice*>& System::getVirtualDevices()
{
    return this->virtual_devices;
}

void System::serve_io_event(io_event event)
{
    for(auto vd:this->virtual_devices)
    {
        vd->serve_io_event(event);
    }
}