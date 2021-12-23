#include "Observer.hpp"

#include "Logger.hpp"

#include <chrono>
#include <thread>
#include <utility>

static const std::chrono::milliseconds SLEEP_TIME = std::chrono::milliseconds(5 * 1000); // NOLINT(cert-err58-cpp)

// TODO: I would love to have this inside `Content.hpp` but it fails with `duplicate symbol` error :(
std::string get_string_from_enum(Content::Status status) {
    switch (status) {
    case Content::Status::CREATED:
        return "Created";
    case Content::Status::DELETED:
        return "Deleted";
    case Content::Status::MODIFIED:
        return "Modified";
    default:
        throw std::runtime_error("Status value is not implemented");
    }
}

Observer::Observer(std::string path, void (*callback)(StatusMap&)): path(std::move(path)), callback(callback) {
    is_running = false;
}

void Observer::stop() {
    LOG::logger.println(Logger::Level::INFO, "stopping observer of path " + path);
    is_running = false;
}

#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"
#pragma ide diagnostic ignored "ConstantConditionsOC"

void Observer::run(bool use_async_reader) {
    is_running = true;
    bool first_run = true;
    std::unordered_map<std::string, long> old_observed_files;
    if (use_async_reader) {
        LOG::logger.println(Logger::Level::VERBOSE, "Using async reader...");
    } else {
        LOG::logger.println(Logger::Level::VERBOSE, "Using sync reader...");
    }
    while (is_running) {
        LOG::logger.println(Logger::Level::VERBOSE, "Looping...");
        std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
        std::unordered_map<std::string, long> changed_files =
            use_async_reader ? file_util::read_dir_async(path) : file_util::read_dir(path);
        if (first_run) {
            LOG::logger.println(Logger::Level::INFO, "Marking files as observed...");
            old_observed_files = changed_files;
            first_run = false;
            LOG::logger.println(Logger::Level::INFO, "Observing for file changes...");
            LOG::logger.println(Logger::Level::VERBOSE, "Files found: " + std::to_string(old_observed_files.size()));
            continue;
        }
        StatusMap changes = file_util::check_for_changes(old_observed_files, changed_files);
        if (!changes.empty()) {
            callback(changes);
        }
        for (auto& item : changes) {
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