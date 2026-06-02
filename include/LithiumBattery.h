#ifndef LITHIUMBATTERY_H
#define LITHIUMBATTERY_H
#include "Device.h"
#include <iostream>

class LithiumBattery : public Device
{
private:
    float currentVoltage;
    float stateOfCharge;
    float temperature;

    const float MAX_VOLTAGE = 58.4f;

public:
    LithiumBattery(std::string n) : Device(n), currentVoltage(51.2f), stateOfCharge(80.0f), temperature(30.0f) {}

    void updateTelemetry() override
    {
        if (!isActive)
        {
            return;
        }
        temperature += 0.5f;
        currentVoltage += 0.1f;
        if (currentVoltage > MAX_VOLTAGE)
        {
            currentVoltage = MAX_VOLTAGE;
        }
    }
    void displayStatus() override
    {
        std::cout << "[Lithium Battery - " << name << "] Voltage: " << currentVoltage << "V, SoC: " << stateOfCharge << "%, Temp: " << temperature << "°C\n";
    }
    float getVoltage() const
    {
        return currentVoltage;
    }
    float getTemperature() const
    {
        return temperature;
    }
};

#endif