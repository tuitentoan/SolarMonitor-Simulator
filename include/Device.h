#ifndef DEVICE_H
#define DEVICE_H
#include <string>

class Device
{
protected:
    std::string name;
    bool isActive;

public:
    Device(std::string n) : name(n), isActive(true) {}

    virtual ~Device() = default;

    virtual void updateTelemetry() = 0;
    virtual void displayStatus() = 0;
};

#endif