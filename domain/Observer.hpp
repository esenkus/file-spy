#pragma once

#include <string>
#include <unordered_map>

class Observer {
public:
    enum class Status {
        Created,
        Deleted,
        Modified
    };

    explicit Observer(std::string path);

    void start();

    void stop();

private:

    // TODO: make async
    std::unordered_map<std::string, long> read_dir_recursive(const std::string &directory);

private:
    std::string path;
};
