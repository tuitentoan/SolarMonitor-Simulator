#ifndef LOGGGER_H
#define LOGGGER_H

#include <string>
#include <fstream>
#include <mutex>

class Logger
{
private:
    static Logger *instance;
    static std::mutex mutex_;
    std::ofstream logFile;
    Logger();
    ~Logger();

public:
    Logger(Logger const &) = delete;
    void operator=(Logger const &) = delete;
    static Logger *getInstance();
    void log(const std::string &message);
};

#endif