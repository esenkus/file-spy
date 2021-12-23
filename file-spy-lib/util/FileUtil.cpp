#include "FileUtil.hpp"

#include "Logger.hpp"

#include <dirent.h>
#include <future>
#include <sys/stat.h>

StatusMap file_util::check_for_changes(ChangeMap& old_observed_files, ChangeMap& new_observed_files) {
    StatusMap result;
    for (const auto& new_observed_file : new_observed_files) {
        if (!old_observed_files.count(new_observed_file.first)) {
            result.insert({new_observed_file.first, Content::Status::CREATED});
        } else {
            auto item = old_observed_files.find(new_observed_file.first);
            if (item->second != new_observed_file.second) {
                result.insert({item->first, Content::Status::MODIFIED});
            }
            old_observed_files.erase(item);
        }
    }
    for (const auto& observed_file : old_observed_files) {
        if (!new_observed_files.count(observed_file.first)) {
            result.insert({observed_file.first, Content::Status::DELETED});
        }
    }
    return result;
}

Content file_util::read_dir_one_depth(const std::string& directory) {
    DIR* dir;
    struct dirent* ent;
    ChangeMap files;
    std::vector<std::string> directories;
    struct stat file_stats {};

    LOG::logger.println(Logger::Level::TRACE, "Reading directory: " + directory);
    if ((dir = opendir(directory.c_str())) != nullptr) {
        while ((ent = readdir(dir)) != nullptr) {
            std::string filename = ent->d_name;
            LOG::logger.println(Logger::Level::TRACE, "Reading file: " + filename);
            if (filename == "." || filename == "..") {
                LOG::logger.println(Logger::Level::TRACE, "Skipping: " + filename);
                continue;
            }
            std::string full_path = directory + "/" + filename; // NOLINT(performance-inefficient-string-concatenation)
            long& last_modification_time = file_stats.st_mtimespec.tv_nsec;
            if (stat(full_path.c_str(), &file_stats) == 0) {
                LOG::logger.println(
                    Logger::Level::TRACE,
                    "File modification date: " + std::to_string(last_modification_time));
            } else {
                LOG::logger.println(Logger::Level::INFO, "stat() failed for this file " + filename);
            }
            if (ent->d_type != DT_DIR) {
                files.insert({full_path, last_modification_time});
            } else {
                directories.push_back(full_path);
            }
        }
        closedir(dir);
    } else {
        /* could not open directory */
        perror("FAILED");
    }
    struct Content content;
    content.directories = directories;
    content.files = files;
    return content;
}


ChangeMap file_util::read_dir(const std::string& directory) {
    ChangeMap result;

    struct Content content = file_util::read_dir_one_depth(directory);
    result.insert(content.files.begin(), content.files.end());
    auto& dirs = content.directories;
    while (!dirs.empty()) {
        struct Content inner_content = file_util::read_dir_one_depth(dirs.back());
        dirs.pop_back();
        dirs.insert(dirs.end(), inner_content.directories.begin(), inner_content.directories.end());
        result.insert(inner_content.files.begin(), inner_content.files.end());
    }
    return result;
}

ChangeMap file_util::read_dir_async(const std::string& directory) {
    ChangeMap result;
    std::vector<std::future<ChangeMap>> workers;

    struct Content content = file_util::read_dir_one_depth(directory);
    result.insert(content.files.begin(), content.files.end());
    auto& dirs = content.directories;
    while (!dirs.empty()) {
        workers.push_back(std::async(read_dir, dirs.back()));
        dirs.pop_back();
    }
    LOG::logger.println(Logger::Level::DEBUG, "Created workers: " + std::to_string(workers.size()));
    for (auto& worker : workers) {
        auto thread_result = worker.get();
        result.insert(thread_result.begin(), thread_result.end());
    }
    return result;
}