#include <iostream>
#include <thread>
#include <chrono>
#include <memory>
#include <csignal>

#include "SolarPanel.h"
#include "LithiumBattery.h"
#include "Inverter.h"
#include "SharedData.h"
#include "Logger.h"

SharedData sharedData;
// Ngắt
void emergencyInterruptHandler(int signal)
{
    std::cout << "\n\n[NGAT PHAN CUNG] Kich hoat nut dung khan cap (Ma ngat: " << signal << ")!\n";
    std::cout << "Ghi nhan loi va thuc hien Shutdown an toan...\n";
    Logger::getInstance()->log("CANH BAO: Ngat khan cap (E-Stop) duoc kich hoat boi nguoi dung!");
    sharedData.isRunning = false;
}

// Hen gio
void watchdogTimerThread()
{
    int counter = 0;
    while (sharedData.isRunning)
    {
        std::this_thread::sleep_for(std::chrono::seconds(3));
        if (sharedData.isRunning)
        {
            counter++;
            std::cout << ">>> [WATCHDOG TIMER] Tick " << counter << ": He thong van dang phan hoi.\n";
            Logger::getInstance()->log("[Timer] Watchdog xac nhan he thong binh thuong.");
        }
    }
}

// Luồng 1: Đọc dữ liệu liên tục từ cảm biến
void producerThread(SolarPanel &panel, LithiumBattery &battery)
{
    while (sharedData.isRunning)
    {
        panel.updateTelemetry();
        battery.updateTelemetry();

        {
            std::lock_guard<std::mutex> lock(sharedData.mtx);
            sharedData.panelVoltage = panel.getVoltage();
            sharedData.batteryTemp = battery.getTemperature();
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}

// Luồng 2: Xử lý trung tâm và ghi log
void consumerThread(Inverter &inverter)
{
    Logger *logger = Logger::getInstance();
    while (sharedData.isRunning)
    {
        float currentPanelVoltage = 0.0f;
        float currentBatTemp = 0.0f;
        {
            std::lock_guard<std::mutex> Lock(sharedData.mtx);
            currentPanelVoltage = sharedData.panelVoltage;
            currentBatTemp = sharedData.batteryTemp;
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
    signal(SIGINT, emergencyInterruptHandler);
    Logger::getInstance()->log("BAT DAU KHOI DONG HE THONG");
    auto panel = std::make_unique<SolarPanel>("SP-01");
    auto battery = std::make_unique<LithiumBattery>("BMS 48V-150Ah");
    auto inverter = std::make_unique<Inverter>("LuxPower-6.5kW");

    std::cout << "=== HE THONG DANG CHAY===\n";
    std::thread t1(producerThread, std::ref(*panel), std::ref(*battery));
    std::thread t2(consumerThread, std::ref(*inverter));
    std::thread t_watchdog(watchdogTimerThread);

    std::this_thread::sleep_for(std::chrono::seconds(30));
    sharedData.isRunning = false;
    t1.join();
    t2.join();
    t_watchdog.join();
    Logger::getInstance()->log("TAT HE THONG");
    std::cout << "=== HE THONG DA DUOC TAT ===\n";
    std::cout << "Thong tin chi tiet da duoc luu vao file log: system_log.csv\n";
    return 0;
}