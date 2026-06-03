#ifndef SHAREDDATA_H
#define SHAREDDATA_H

#include <mutex>

struct SharedData
{
    float panelVoltage = 0.0f;
    float batteryTemp = 0.0f;
    bool isRunning = true;
    std::mutex mtx;
};

#endif