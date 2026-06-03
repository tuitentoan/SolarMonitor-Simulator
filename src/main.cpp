#include <iostream>
#include <thread>
#include <chrono>
#include <memory>

#include "SolarPanel.h"
#include "LithiumBattery.h"
#include "Inverter.h"
#include "SharedData.h"
#include "Logger.h"

// Luồng 1: Đọc dữ liệu liên tục từ cảm biến
void producerThread(SharedData &data, SolarPanel &panel, LithiumBattery &battery)
{
    while (data.isRunning)
    {
        panel.updateTelemetry();
        battery.updateTelemetry();

        {
            std::lock_guard<std::mutex> lock(data.mtx);
            data.panelVoltage = panel.getVoltage();
            data.batteryTemp = battery.getTemperature();
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}

// Luồng 2: Xử lý trung tâm và ghi log
void consumerThread(SharedData &data, Inverter &inverter)
{
    Logger *logger = Logger::getInstance();
    while (data.isRunning)
    {
        float currentPanelVoltage = 0.0f;
        float currentBatTemp = 0.0f;
        {
            std::lock_guard<std::mutex> Lock(data.mtx);
            currentPanelVoltage = data.panelVoltage;
            currentBatTemp = data.batteryTemp;
        }
        std::cout << "-------------------------------------\n";
        inverter.processEnergyInput(currentPanelVoltage, currentBatTemp);
        inverter.displayStatus();
        if (inverter.hasFault(1 << 1))
        {
            logger->log("CANH BAO NGUY HIEM: Nhiet do pin qua cao (>55C)!");
        }
        else
        {
            logger->log("He thong hoat dong on dinh");
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
}
// Ham Main: Thiet lap va quan li he thong
int main()
{
    Logger::getInstance()->log("BAT DAU KHOI DONG HE THONG");
    auto panel = std::make_unique<SolarPanel>("SP-01");
    auto battery = std::make_unique<LithiumBattery>("BMS 48V-150Ah");
    auto inverter = std::make_unique<Inverter>("LuxPower-6.5kW");
    SharedData sharedData;
    std::cout << "=== HE THONG DANG CHAY===\n";
    std::thread t1(producerThread, std::ref(sharedData), std::ref(*panel), std::ref(*battery));
    std::thread t2(consumerThread, std::ref(sharedData), std::ref(*inverter));

    std::this_thread::sleep_for(std::chrono::seconds(10));
    sharedData.isRunning = false;
    t1.join();
    t2.join();
    Logger::getInstance()->log("TAT HE THONG");
    std::cout << "=== HE THONG DA DUOC TAT ===\n";
    std::cout << "Thong tin chi tiet da duoc luu vao file log: system_log.csv\n";
    return 0;
}