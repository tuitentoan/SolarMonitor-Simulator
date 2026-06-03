#ifndef INVERTER_H
#define INVERTER_H
#include "Device.h"
#include <cstdint>
class Inverter : public Device
{
private:
    float inputVoltage;
    float outputPower;
    float internalTemp;

    uint8_t status_register;

    static const uint8_t FAULT_NONE = 0;
    static const uint8_t FAULT_OVER_VOLTAGE = 1 << 0;
    static const uint8_t FAULT_OVER_TEMP = 1 << 1;
    static const uint8_t FAULT_OVER_LOAD = 1 << 2;

public:
    Inverter(std::string n);

    void updateTelemetry() override;
    void displayStatus() const override;
    void setFault(uint8_t fault_mask);
    void clearFault(uint8_t fault_mask);
    bool hasFault(uint8_t fault_mask) const;

    void processEnergyInput(float panelPower, float batteryTemp);
};

#endif
