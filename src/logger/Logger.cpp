//Logger.cpp
#include "Logger.h"
#include <iostream>
#include <ctime>

void Logger::log(LogLevel level, const std::string& message) {
    const char* levelStr = "";
    switch (level) {
        case LogLevel::INFO: levelStr = "INFO"; break;
        case LogLevel::WARNING: levelStr = "WARNING"; break;
        case LogLevel::LOG_ERROR: levelStr = "ERROR"; break;
    }

    std::time_t now = std::time(nullptr);
    char buf[20];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", std::localtime(&now));

    std::cout << "[" << buf << "] [" << levelStr << "] " << message << std::endl;
}
