//Logger.h
#ifndef LOGGER_H
#define LOGGER_H

#include <string>

enum class LogLevel {
    INFO,
    WARNING,
    LOG_ERROR  
};

class Logger {
public:
    static void log(LogLevel level, const std::string& message);
};

#endif // LOGGER_H