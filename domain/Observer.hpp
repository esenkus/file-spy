#pragma once

#include <string>

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

    // TODO: make async, change return to map of filename and modification date
    std::vector<std::string> read_dir_recursive(const std::string &directory);

private:
    std::string path;
};
