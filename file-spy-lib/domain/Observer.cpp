#include <utility>
#include "Observer.hpp"
#include "../util/Logger.hpp"
#include <dirent.h>
#include <sys/stat.h>
#include <thread>
#include <chrono>
#include <future>

static const std::chrono::milliseconds SLEEP_TIME = std::chrono::milliseconds(5 * 1000); // NOLINT(cert-err58-cpp)

Observer::Observer(std::string path) : path(std::move(path)) {
    is_running = false;
}

void Observer::stop() {
    LOG::logger.println(Logger::Level::INFO, "stopping observer of path " + path);
    is_running = false;
}

std::unordered_map<std::string, Observer::Status>
Observer::check_for_changes(std::unordered_map<std::string, long> &new_observed_files) {
    std::unordered_map<std::string, Status> result;
    for (const auto &new_observed_file: new_observed_files) {
        if (!old_observed_files.count(new_observed_file.first)) {
            result.insert({new_observed_file.first, Status::CREATED});
        } else {
            auto item = old_observed_files.find(new_observed_file.first);
            if (item->second != new_observed_file.second) {
                result.insert({item->first, Status::MODIFIED});
            }
            old_observed_files.erase(item);
        }
    }
    for (const auto &observed_file: old_observed_files) {
        if (!new_observed_files.count(observed_file.first)) {
            result.insert({observed_file.first, Status::DELETED});
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

#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"
#pragma ide diagnostic ignored "ConstantConditionsOC"

Content Observer::read_dir_one_depth(const std::string &directory) {
    DIR *dir;
    struct dirent *ent;
    std::unordered_map<std::string, long> files;
    std::vector<std::string> directories;
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
            std::string full_path = directory + "/" + filename; // NOLINT(performance-inefficient-string-concatenation)
            long &last_modification_time = file_stats.st_mtimespec.tv_nsec;
            if (stat(full_path.c_str(), &file_stats) == 0) {
                LOG::logger.println(Logger::Level::TRACE,
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

std::unordered_map<std::string, long> Observer::read_dir(const std::string &directory) {
    std::unordered_map<std::string, long> result;

    struct Content content = read_dir_one_depth(directory);
    result.insert(content.files.begin(), content.files.end());
    auto &dirs = content.directories;
    while (!dirs.empty()) {
        struct Content inner_content = read_dir_one_depth(dirs.back());
        dirs.pop_back();
        dirs.insert(dirs.end(), inner_content.directories.begin(), inner_content.directories.end());
        result.insert(inner_content.files.begin(), inner_content.files.end());
    }
    return result;
}

std::unordered_map<std::string, long> Observer::thread_job(const std::string &directory) {
    std::unordered_map<std::string, long> result;
    struct Content content = read_dir_one_depth(directory);
    result.insert(content.files.begin(), content.files.end());
    auto &dirs = content.directories;
    while (!dirs.empty()) {
        struct Content inner_content = read_dir_one_depth(dirs.back());
        dirs.pop_back();
        dirs.insert(dirs.end(), inner_content.directories.begin(), inner_content.directories.end());
        result.insert(inner_content.files.begin(), inner_content.files.end());
    }
    return result;
}

std::unordered_map<std::string, long> Observer::read_dir_threaded(const std::string &directory) {
    std::unordered_map<std::string, long> result;
    std::vector<std::future<std::unordered_map<std::string, long>>> workers;

    struct Content content = read_dir_one_depth(directory);
    result.insert(content.files.begin(), content.files.end());
    auto &first_dirs = content.directories;

    while (!first_dirs.empty()) {
        workers.push_back(std::async(thread_job, first_dirs.back()));
        first_dirs.pop_back();
    }
    LOG::logger.println(Logger::Level::DEBUG, "Created workers: " + std::to_string(workers.size()));
    for (auto &worker: workers) {
        auto thread_result = worker.get();
        result.insert(thread_result.begin(), thread_result.end());
    }
    return result;
}

void Observer::run(bool use_async_reader) {
    is_running = true;
    bool first_run = true;
    if (use_async_reader) {
        LOG::logger.println(Logger::Level::VERBOSE, "Using async reader...");
    } else {
        LOG::logger.println(Logger::Level::VERBOSE, "Using sync reader...");
    }
    while (is_running) {
        LOG::logger.println(Logger::Level::VERBOSE, "Looping...");
        std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
        std::unordered_map<std::string, long> changed_files =
                use_async_reader ? read_dir_threaded(path) : read_dir(path);
        if (first_run) {
            LOG::logger.println(Logger::Level::INFO, "Marking files as observed...");
            old_observed_files = changed_files;
            first_run = false;
            LOG::logger.println(Logger::Level::INFO, "Observing for file changes...");
            LOG::logger.println(Logger::Level::VERBOSE, "Files found: " + std::to_string(old_observed_files.size()));
            continue;
        }
        std::unordered_map<std::string, Observer::Status> changes = check_for_changes(changed_files);
        for (auto &item: changes) {
            LOG::logger.println(Logger::Level::INFO, get_string_from_enum(item.second) + " -> " + item.first);
        }
        old_observed_files = changed_files;
        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
        LOG::logger.println(Logger::Level::VERBOSE, "Loop finished in (ms): " + std::to_string(duration));
        std::this_thread::sleep_for(SLEEP_TIME);
    }
}

#pragma clang diagnostic pop

void Observer::start(bool use_async_reader) {
    LOG::logger.println(Logger::Level::INFO, "starting observer in path " + path);
    std::thread t1(&Observer::run, this, use_async_reader);
    t1.detach();
}