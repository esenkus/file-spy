#include <iostream>
#include <utility>
#include "Observer.hpp"
#include <dirent.h>

Observer::Observer(std::string path) : path(std::move(path)) {
}

void Observer::start() {
    std::cout << "starting observer in path " << path << std::endl;
    read_dir_recursive(path);
}

void Observer::stop() {
    std::cout << "stopping observer of path " << path << std::endl;
}

void Observer::read_dir_recursive(const std::string &directory) {
    DIR *dir;
    struct dirent *ent;

    std::cout << "READING directory: " << directory << std::endl;
    std::cout << "--------------------" << std::endl;
    if ((dir = opendir(directory.c_str())) != nullptr) {
        while ((ent = readdir(dir)) != nullptr) {
            std::string filename = ent->d_name;
            if (filename.compare(filename.length() - 1, 1, ".") == 0) {
                std::cout << "SKIPPING " << filename << std::endl;
                continue;
            }
            std::string full_path = directory + "/" + filename; // NOLINT(performance-inefficient-string-concatenation)
            if (ent->d_type != DT_DIR) {
                std::cout << "FILE: " << full_path << std::endl;
            } else {
                // TODO: resolve path
                std::cout << "DIR: " << full_path << std::endl;

                read_dir_recursive(full_path);
            }
        }
        closedir(dir);
    } else {
        /* could not open directory */
        perror("FAILED");
    }
}
