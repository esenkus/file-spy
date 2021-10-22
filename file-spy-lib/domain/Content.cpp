#include <unordered_map>
#include <vector>

struct Content {
    std::vector<std::string> directories;
    std::unordered_map<std::string, long> files;
};
