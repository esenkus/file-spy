#include <unordered_map>
#include <vector>

struct Content {
    std::vector<std::string> directories;
    std::unordered_map<std::string, long> files;

    enum class Status {
        CREATED = 0,
        DELETED = 1,
        MODIFIED = 2
    };
};