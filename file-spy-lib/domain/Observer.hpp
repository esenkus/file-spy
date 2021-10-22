#pragma once

#include <string>
#include <unordered_map>
#include <chrono>
#include "FileUtil.hpp"

class Observer {
public:
    explicit Observer(std::string path, void (* callback)(StatusMap&));

    void start(bool use_async_reader);

    void stop();

private:
    void run(bool use_async_reader);

private:
    std::string path;

    void (* callback)(StatusMap&);

    bool is_running;
};
