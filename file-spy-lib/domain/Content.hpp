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

using StatusMap = std::unordered_map<std::string, Content::Status>;
using ChangeMap = std::unordered_map<std::string, long>;