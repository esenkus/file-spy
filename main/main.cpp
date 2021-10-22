#include <thread>
#include "Observer.hpp"
#include "Logger.hpp"

int main() {
    LOG::logger.set_level(Logger::Level::VERBOSE);
    LOG::logger.println(Logger::Level::INFO, "Hello, World!");

//    Observer observer("/Users/eivydassenkus/Desktop/test");
    Observer observer("/Users/eivydassenkus/Documents/git/secure-dex");
    observer.start(true);
    std::this_thread::sleep_for(std::chrono::milliseconds(30 * 1000));
    observer.stop();
    return 0;
}