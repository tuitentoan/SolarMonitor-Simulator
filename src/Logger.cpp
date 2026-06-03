#include "Logger.h"
#include <iostream>
#include <chrono>
#include <ctime>

Logger *Logger::instance = nullptr;
std::mutex Logger::mutex_;

Logger::Logger()
{
    logFile.open("system_log.csv", std::ios::app);
    if (!logFile.is_open())
    {
        std::cerr << "LOI I/O: Khong the mo file log!" << std::endl;
    }
    else
    {
        logFile << "--- HE THONG KHOI DONG ---\n";
        logFile << "Thoi gian, Su kien\n";
    }
}
Logger::~Logger()
{
    if (logFile.is_open())
    {
        logFile << "--- HE THONG TAT ---\n";
        logFile.close();
    }
}
Logger *Logger::getInstance()
{
    std::lock_guard<std::mutex> Lock(mutex_);
    if (instance == nullptr)
    {
        instance = new Logger();
    }
    return instance;
}

void Logger::log(const std::string &message)
{
    std::lock_guard<std::mutex> lock(mutex_);

    if (logFile.is_open())
    {
        auto now = std::chrono::system_clock::now();
        std::time_t now_time = std::chrono::system_clock::to_time_t(now);
        std::string time_str = std::ctime(&now_time);
        time_str.pop_back();

        logFile << time_str << ", " << message << "\n";
        logFile.flush();
    }
}