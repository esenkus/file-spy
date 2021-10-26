#include <thread>
#include "boost/filesystem.hpp"
#include "gtest/gtest.h"
#include "Observer.hpp"

void create_and_write_file(const std::string& path, const std::string& content) {
    std::ofstream outfile(path);
    if (!content.empty()) {
        outfile << content << std::endl;
    }
    outfile.close();
}

void create_and_write_file(const std::string& path) {
    create_and_write_file(path, "");
}

// le hackerman
StatusMap result;

TEST(ObserverIntegrationTest, test_observer_working) {
    auto temporary_dir = boost::filesystem::temp_directory_path().append("test");
    boost::filesystem::remove_all(temporary_dir);
    boost::filesystem::create_directory(temporary_dir);

    auto temporary_file1 = temporary_dir / boost::filesystem::unique_path();
    auto temporary_file2 = temporary_dir / boost::filesystem::unique_path();
    auto temporary_file3 = temporary_dir / boost::filesystem::unique_path();

    create_and_write_file(temporary_file1.string());
    create_and_write_file(temporary_file2.string());

    void (* changes)(StatusMap&) = [](StatusMap& statusMap) {
        result.insert(statusMap.begin(), statusMap.end());
    };
    Observer observer(temporary_dir.string(), changes);
    observer.start(false);
    std::this_thread::sleep_for(std::chrono::milliseconds(2 * 1000));

    // could also check file changes here after sleep, but there's really no difference?
    create_and_write_file(temporary_file3.string());
    boost::filesystem::remove(temporary_file1);
    // sleep for 6s, because observer checks for changes every 5s
    std::this_thread::sleep_for(std::chrono::milliseconds(6 * 1000));

    create_and_write_file(temporary_file2.string(), "Extra content");
    std::this_thread::sleep_for(std::chrono::milliseconds(6 * 1000));
    observer.stop();

    ASSERT_EQ(3, result.size());
    ASSERT_TRUE(result.count(temporary_file1.string()));
    ASSERT_TRUE(result.count(temporary_file2.string()));
    ASSERT_TRUE(result.count(temporary_file3.string()));

    auto file1 = result.find(temporary_file1.string());
    auto file2 = result.find(temporary_file2.string());
    auto file3 = result.find(temporary_file3.string());
    ASSERT_EQ(Content::Status::DELETED, file1->second);
    ASSERT_EQ(Content::Status::MODIFIED, file2->second);
    ASSERT_EQ(Content::Status::CREATED, file3->second);
}