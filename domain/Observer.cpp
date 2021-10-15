#include <utility>
#include "Observer.hpp"
#include "../util/Logger.hpp"
#include <dirent.h>
#include <sys/stat.h>
#include <unordered_map>
#include <thread>
#include <chrono>

static const std::chrono::milliseconds SLEEP_TIME = std::chrono::milliseconds(5 * 1000); // NOLINT(cert-err58-cpp)

Observer::Observer(std::string path) : path(std::move(path)) {
    is_running = false;
}

#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"

void Observer::start() {
    LOG::logger.println(Logger::Level::INFO, "starting observer in path " + path);
    is_running = true;
    bool first_run = true;
    while (is_running) {
        LOG::logger.println(Logger::Level::VERBOSE, "Looping...");
        std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
        std::unordered_map<std::string, long> changed_files = read_dir_recursive(path);
        if (first_run) {
            LOG::logger.println(Logger::Level::INFO, "Marking files as observed...");
            observed_files = changed_files;
            first_run = false;
            LOG::logger.println(Logger::Level::INFO, "Observing for file changes...");
            LOG::logger.println(Logger::Level::VERBOSE, "Files found: " + std::to_string(observed_files.size()));
            continue;
        }
        std::unordered_map<std::string, Observer::Status> changes = check_for_changes(changed_files);
        for (auto &item: changes) {
            LOG::logger.println(Logger::Level::INFO, get_string_from_enum(item.second) + " -> " + item.first);
        }
        observed_files = changed_files;
        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
        LOG::logger.println(Logger::Level::VERBOSE, "Loop finished in (ms): " + std::to_string(duration));
        std::this_thread::sleep_for(SLEEP_TIME);
    }
}

#pragma clang diagnostic pop

void Observer::stop() {
    LOG::logger.println(Logger::Level::INFO, "stopping observer of path " + path);
    is_running = false;
}

std::unordered_map<std::string, long> Observer::read_dir_recursive(const std::string &directory) {
    DIR *dir;
    struct dirent *ent;
    std::unordered_map<std::string, long> result;
    struct stat file_stats{};

    LOG::logger.println(Logger::Level::TRACE, "Reading directory: " + directory);
    if ((dir = opendir(directory.c_str())) != nullptr) {
        while ((ent = readdir(dir)) != nullptr) {
            std::string filename = ent->d_name;
            LOG::logger.println(Logger::Level::TRACE, "Reading file: " + filename);
            if (filename == "." || filename == "..") {
                LOG::logger.println(Logger::Level::TRACE, "Skipping: " + filename);
                continue;
            }
            //std::unordered_map
            std::string full_path = directory + "/" + filename; // NOLINT(performance-inefficient-string-concatenation)
            long &last_modification_time = file_stats.st_mtimespec.tv_nsec;
            if (stat(full_path.c_str(), &file_stats) == 0) {
                LOG::logger.println(Logger::Level::TRACE,
                                    "File modification date: " + std::to_string(last_modification_time));
            } else {
                LOG::logger.println(Logger::Level::INFO, "stat() failed for this file " + filename);
            }
            if (ent->d_type != DT_DIR) {
                result.insert({full_path, last_modification_time});
            } else {
                std::unordered_map<std::string, long> inner = read_dir_recursive(full_path);
                result.insert(inner.begin(), inner.end());
            }
        }
        closedir(dir);
    } else {
        /* could not open directory */
        perror("FAILED");
    }
    return result;
}

std::unordered_map<std::string, Observer::Status>
Observer::check_for_changes(std::unordered_map<std::string, long> &new_observed_files) {
    std::unordered_map<std::string, Status> result;
    for (const auto &observed_file: observed_files) {
        if (!new_observed_files.count(observed_file.first)) {
            result.insert({observed_file.first, Status::DELETED});
        } else {
            auto &modify_date = new_observed_files[observed_file.first];
            if (modify_date != observed_file.second) {
                result.insert({observed_file.first, Status::MODIFIED});
            }
        }
    }
    for (const auto &changed_file: new_observed_files) {
        if (!observed_files.count(changed_file.first)) {
            result.insert({changed_file.first, Status::CREATED});
        }
    }
    return result;
}

std::string Observer::get_string_from_enum(Status status) {
    switch (status) {
        case Status::CREATED:
            return "Created";
        case Status::DELETED:
            return "Deleted";
        case Status::MODIFIED:
            return "Modified";
        default:
            throw std::runtime_error("Status value is not implemented");
    }
}
