#pragma once

#include <string>
#include <unordered_map>
#include "Content.hpp"

using StatusMap = std::unordered_map<std::string, Content::Status>;
using ChangeMap = std::unordered_map<std::string, long>;

namespace file_util {
    Content read_dir_one_depth(const std::string& directory);

    StatusMap check_for_changes(ChangeMap& old_observed_files, ChangeMap& new_observed_files);

    ChangeMap read_dir(const std::string& directory);

    ChangeMap read_dir_async(const std::string& directory);
}
