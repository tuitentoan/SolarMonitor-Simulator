#include "Inverter.h"
#include <iostream>
#include <stdexcept>

Inverter::Inverter(std::string n) : Device(n), inputVoltage(0.0f), outputPower(0.0f), internalTemp(25.0f), status_register(FAULT_NONE)
{
}

void Inverter::setFault(uint8_t fault_mask)
{
    status_register |= fault_mask;
}

void Inverter::clearFault(uint8_t fault_mask)
{
    status_register &= ~fault_mask;
}

bool Inverter::hasFault(uint8_t fault_mask) const
{
    return (status_register & fault_mask) != 0;
}

void Inverter::processEnergyInput(float panelVoltage, float batteryTemp)
{
    try
    {
        if (panelVoltage > 60.0f)
        {
            setFault(FAULT_OVER_VOLTAGE);
            throw std::runtime_error("CANH BAO NGUY HIEM: Dien ap tam pin qua cao (>60V)!");
        }
        else
        {
            clearFault(FAULT_OVER_VOLTAGE);
        }
        if (batteryTemp > 55.0f)
        {
            setFault(FAULT_OVER_TEMP);
            throw std::runtime_error("CANH BAO NGUY HIEM: Nhiet do pin qua cao (>55C)!");
        }
        else
        {
            clearFault(FAULT_OVER_TEMP);
        }
    }
    catch (const std::runtime_error &e)
    {
        std::cerr << "Error processing energy input: " << e.what() << std::endl;
    }
}

void Inverter::updateTelemetry()
{
    if (!isActive)
    {
        return;
    }
    inputVoltage = 48.0f;
    outputPower = 3000.0f;
    internalTemp = 40.0f;
}
void Inverter::displayStatus() const
{
    std::cout << "[Inverter - " << name << "] Input V: " << inputVoltage
              << "V, Load P: " << outputPower << "W, Inverter Temp: " << internalTemp << "C\n";

    // Hiển thị trạng thái mã nhị phân của thanh ghi lỗi để kiểm tra (Debug Technique)
    std::cout << "-> Thanh ghi loi (Status Register Binary): ";
    for (int i = 7; i >= 0; --i)
    {
        std::cout << ((status_register >> i) & 1);
    }
    std::cout << "\n";
}