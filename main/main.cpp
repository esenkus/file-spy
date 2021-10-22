#include <thread>
#include <iostream>
#include "Observer.hpp"
#include "Logger.hpp"


// TODO: this sucks
std::string get_string_from_enum1(Content::Status status) {
    switch (status) {
        case Content::Status::CREATED:
            return "Created";
        case Content::Status::DELETED:
            return "Deleted";
        case Content::Status::MODIFIED:
            return "Modified";
        default:
            throw std::runtime_error("Status value is not implemented");
    }
}

void on_callback(StatusMap& statusMap) {
    std::cout << "Changes received: " << std::endl;
    for (auto& status: statusMap) {
        std::cout << get_string_from_enum1(status.second) << ": " << status.first << std::endl;
    }
}

int main() {
    LOG::logger.set_level(Logger::Level::VERBOSE);
    LOG::logger.println(Logger::Level::INFO, "Hello, World!");

//    Observer observer("/Users/eivydassenkus/Desktop/test", on_callback);
    Observer observer("/Users/eivydassenkus/Documents/git/secure-dex", on_callback);
    observer.start(true);
    std::this_thread::sleep_for(std::chrono::milliseconds(30 * 1000));
//    observer.stop();
    return 0;
}