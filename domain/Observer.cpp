#include <iostream>
#include <utility>
#include "Observer.hpp"
#include <dirent.h>
#include <vector>
#include <sys/stat.h>
#include <unordered_map>

Observer::Observer(std::string path) : path(std::move(path)) {
}

void Observer::start() {
    std::cout << "starting observer in path " << path << std::endl;
    std::vector<std::string> result = read_dir_recursive(path);
    std::cout << "result: " << std::endl;
    for (const auto &item: result) {
        std::cout << "  " << item << std::endl;
    }
}

void Observer::stop() {
    std::cout << "stopping observer of path " << path << std::endl;
}

std::vector<std::string> Observer::read_dir_recursive(const std::string &directory) {
    DIR *dir;
    struct dirent *ent;
    std::vector<std::string> result;
    struct stat file_stats;

    // TODO: could use some logger
//    std::cout << "READING directory: " << directory << std::endl;
    if ((dir = opendir(directory.c_str())) != nullptr) {
        while ((ent = readdir(dir)) != nullptr) {
            std::string filename = ent->d_name;
//            std::cout << "filename: " << filename << std::endl;
            if (filename == "." || filename == "..") {
//                std::cout << "SKIPPING " << filename << std::endl;
                continue;
            }
            //std::unordered_map
            std::string full_path = directory + "/" + filename; // NOLINT(performance-inefficient-string-concatenation)
//            std::cout << "full_path: " << full_path << std::endl;
            if (stat(full_path.c_str(), &file_stats) == 0) {
                // TODO: add this to map
                std::cout << " is size " << file_stats.st_mtimespec.tv_nsec << std::endl;
            } else {
                std::cout << "stat() failed for this file" << std::endl;
            }
            if (ent->d_type != DT_DIR) {
//                std::cout << "FILE: " << full_path << std::endl;
                result.push_back(full_path);
            } else {
//                std::cout << "DIR: " << full_path << std::endl;
                std::vector<std::string> inner = read_dir_recursive(full_path);
                result.insert(result.end(), inner.begin(), inner.end());
            }
        }
        closedir(dir);
    } else {
        /* could not open directory */
        perror("FAILED");
    }
    return result;
}
