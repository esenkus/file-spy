#ifndef FILE_SPY_OBSERVER_HPP
#define FILE_SPY_OBSERVER_HPP


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
    std::string path;

    // TODO: make async, change return to structure that has file path and fileSize?
    std::vector<std::string> read_dir_recursive(const std::string &directory);
};


#endif //FILE_SPY_OBSERVER_HPP
