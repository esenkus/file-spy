#include "FileUtil.hpp"

#include "gtest/gtest.h"

TEST(FileUtilTest, check_for_changes_finds_delete) {
    ChangeMap original_files = {
            {"com/test/File.txt",  1},
            {"com/test/File2.txt", 1},
            {"com/test/File3.txt", 1},
            {"com/File.txt",       1},
    };
    ChangeMap changed_files = {
            {"com/test/File2.txt", 1},
            {"com/test/File3.txt", 1},
            {"com/File.txt",       1},
    };
    StatusMap statuses = file_util::check_for_changes(original_files, changed_files);
    EXPECT_EQ(statuses.size(), 1);
    EXPECT_TRUE(statuses.count("com/test/File.txt"));
    EXPECT_EQ(Content::Status::DELETED, statuses.find("com/test/File.txt")->second);
}

TEST(FileUtilTest, check_for_changes_finds_modified) {
    ChangeMap original_files = {
            {"com/test/File.txt",  1},
            {"com/test/File2.txt", 1},
            {"com/test/File3.txt", 1},
            {"com/File.txt",       1},
    };
    ChangeMap changed_files = {
            {"com/test/File.txt",  1},
            {"com/test/File2.txt", 1},
            {"com/test/File3.txt", 99},
            {"com/File.txt",       1},
    };
    StatusMap statuses = file_util::check_for_changes(original_files, changed_files);
    EXPECT_EQ(statuses.size(), 1);
    EXPECT_TRUE(statuses.count("com/test/File3.txt"));
    EXPECT_EQ(Content::Status::MODIFIED, statuses.find("com/test/File3.txt")->second);
}

TEST(FileUtilTest, check_for_changes_finds_created) {
    ChangeMap original_files = {
            {"com/test/File.txt",  1},
            {"com/test/File2.txt", 1},
            {"com/test/File3.txt", 1},
            {"com/File.txt",       1},
    };
    ChangeMap changed_files = {
            {"com/test/File.txt",   1},
            {"com/test/File2.txt",  1},
            {"com/test/File3.txt",  1},
            {"com/File.txt",        1},
            {"com/test/File99.txt", 1},
    };
    StatusMap statuses = file_util::check_for_changes(original_files, changed_files);
    EXPECT_EQ(statuses.size(), 1);
    EXPECT_TRUE(statuses.count("com/test/File99.txt"));
    EXPECT_EQ(Content::Status::CREATED, statuses.find("com/test/File99.txt")->second);
}