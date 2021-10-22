#pragma once

#include <string>
#include <unordered_map>
#include <chrono>

class Observer {
public:
    explicit Observer(std::string path);

    void start(bool use_async_reader);

    void stop();

private:
    void run(bool use_async_reader);

private:
    std::string path;
    bool is_running;
};
