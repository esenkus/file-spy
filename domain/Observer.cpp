#include <iostream>
#include <utility>
#include "Observer.hpp"
#include <dirent.h>
#include <vector>

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

    // TODO: could use some logger
//    std::cout << "READING directory: " << directory << std::endl;
    if ((dir = opendir(directory.c_str())) != nullptr) {
        while ((ent = readdir(dir)) != nullptr) {
            // TODO: is this copy? I want reference
            std::string filename = ent->d_name;
//            std::cout << "filename: " << filename << std::endl;
            if (filename.compare(filename.length() - 1, 1, ".") == 0) {
//                std::cout << "SKIPPING " << filename << std::endl;
                continue;
            }
            std::string full_path = directory + "/" + filename; // NOLINT(performance-inefficient-string-concatenation)
            if (ent->d_type != DT_DIR) {
//                std::cout << "FILE: " << full_path << std::endl;
                result.push_back(full_path);
            } else {
//                std::cout << "DIR: " << full_path << std::endl;
                std::vector<std::string> inner = read_dir_recursive(full_path);;
                result.insert(std::end(result), std::begin(inner), std::end(inner));
            }
        }
        closedir(dir);
    } else {
        /* could not open directory */
        perror("FAILED");
    }
    return result;
}
