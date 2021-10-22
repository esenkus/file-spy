#pragma once

#include <string>
#include <unordered_map>
#include <chrono>
#include "Content.hpp"

class Observer {
public:
    enum class Status {
        CREATED = 0,
        DELETED = 1,
        MODIFIED = 2
    };

    explicit Observer(std::string path);

    void start(bool use_async_reader);

    void stop();

private:
    static Content read_dir_one_depth(const std::string &directory);

    static std::unordered_map<std::string, long> read_dir(const std::string &directory);

    static std::unordered_map<std::string, long> read_dir_threaded(const std::string &directory);

    static std::unordered_map<std::string, long> thread_job(const std::string &directory);

    void run(bool use_async_reader);

    std::unordered_map<std::string, Status>
    check_for_changes(std::unordered_map<std::string, long> &new_observed_files);

    static std::string get_string_from_enum(Status status);

private:
    std::string path;
    bool is_running;
    std::unordered_map<std::string, long> old_observed_files;
};
