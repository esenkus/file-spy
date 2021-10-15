#include <iostream>
#include "domain/Observer.hpp"
#include "util/Logger.hpp"

int main() {
    LOG::logger.set_level(Logger::Level::DEBUG);
    LOG::logger.println(Logger::Level::INFO, "Hello, World!");

    Observer observer("/Users/eivydassenkus/Desktop/test");
    observer.start();
    observer.stop();
    return 0;
}
