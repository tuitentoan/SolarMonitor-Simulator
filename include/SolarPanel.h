#ifndef SOLARPANEL_H
#define SOLARPANEL_H
#include "Device.h"
#include <iostream>

class SolarPanel : public Device
{

private:
    float voltage;
    float current;

public:
    SolarPanel(std::string n) : Device(n), voltage(0.0f), current(0.0f) {}

    void updateTelemetry() override
    {
        if (!isActive)
        {
            return;
        }
        voltage = 45.5f;
        current = 12.5f;
    }
    void displayStatus() const override
    {
        std::cout << "[Solar Panel - " << name << "] V: " << voltage << "V, I: " << current << "A, P: " << getPower() << "W\n";
    }
    float getPower() const
    {
        return voltage * current;
    }
    float getVoltage() const
    {
        return voltage;
    }
};

#endif