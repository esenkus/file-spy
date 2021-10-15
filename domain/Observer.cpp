#include <iostream>
#include <utility>
#include "Observer.hpp"
#include "../util/Logger.hpp"
#include <dirent.h>
#include <vector>
#include <sys/stat.h>
#include <unordered_map>

Observer::Observer(std::string path) : path(std::move(path)) {
}

void Observer::start() {
    LOG::logger.println(Logger::Level::INFO, "starting observer in path " + path);
    std::unordered_map<std::string, long> result = read_dir_recursive(path);
    LOG::logger.println(Logger::Level::DEBUG, "result:");
    for (auto &item: result) {
        LOG::logger.println(Logger::Level::DEBUG, "  " + item.first + " - " + std::to_string(item.second));
    }
}

void Observer::stop() {
    LOG::logger.println(Logger::Level::INFO, "stopping observer of path " + path);
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
