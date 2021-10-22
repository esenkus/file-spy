#pragma once

#include <string>

class Logger {
public:
    enum class Level {
        INFO = 1,
        VERBOSE = 2,
        DEBUG = 3,
        TRACE = 4
    };

    static Logger& get_instance();

    Logger(Logger const&) = delete;

    void operator=(Logger const&) = delete;

    void println(Level pLevel, const std::string& pMessage);

    void set_level(Level pLevel);

private:
    Logger() = default;

private:
    Level level;
};

namespace LOG {
    extern Logger& logger;
}