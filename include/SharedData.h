#ifndef SHAREDDATA_H
#define SHAREDDATA_H

#include <mutex>
#include <atomic>

struct SharedData
{
    float panelVoltage = 0.0f;
    float batteryTemp = 0.0f;
    std::atomic<bool> isRunning{true};
    std::mutex mtx;
};

#endif