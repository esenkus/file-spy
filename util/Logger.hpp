#pragma once

#include <string>

class Logger {
public:
    enum class Level {
        INFO = 1,
        DEBUG = 2,
        TRACE = 3
    };

    static Logger &get_instance();

    Logger(Logger const &) = delete;

    void operator=(Logger const &) = delete;

    void println(Level pLevel, const std::string &pMessage);

    void set_level(Level pLevel);

private:
    Logger() = default;

private:
    Level level;
};

namespace LOG {
    extern Logger &logger;
}