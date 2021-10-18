#include <iostream>
#include <thread>
#include "domain/Observer.hpp"
#include "util/Logger.hpp"

int main() {
    LOG::logger.set_level(Logger::Level::VERBOSE);
    LOG::logger.println(Logger::Level::INFO, "Hello, World!");

//    Observer observer("/Users/eivydassenkus/Desktop/test");
    Observer observer("/Users/eivydassenkus/Documents/git/secure-dex");
    observer.start();
    std::this_thread::sleep_for(std::chrono::milliseconds(30 * 1000));
    observer.stop();
    return 0;
}