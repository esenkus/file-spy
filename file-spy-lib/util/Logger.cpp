#include "Logger.hpp"

#include <iostream>

Logger& Logger::get_instance() {
    static Logger instance;
    return instance;
}

void Logger::set_level(Logger::Level pLevel) {
    level = pLevel;
}

void Logger::println(Logger::Level pLevel, const std::string& pMessage) {
    if (level >= pLevel) {
        std::cout << pMessage << std::endl;
    }
}

namespace LOG {
    Logger& logger = Logger::get_instance(); // NOLINT(cert-err58-cpp)
}
