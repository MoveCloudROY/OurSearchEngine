#pragma once

#include <iostream>
#include <filesystem>
#include <vector>

namespace SG {
namespace fs = std::filesystem;

class LibraryBuilder {
private:
    std::vector<std::string> file_path_list;
    fs::path                 folder_path;

    void        scan_directory();                           //扫描目录
    void        traverse_directory();                       //遍历目录
    std::string read_content(const std::string &file_path); //读取文本内容
    std::string getTitle(const std::string &file_path);     //读取文本标题

public:
    LibraryBuilder(const char *folder_path);

    ~LibraryBuilder();

    void build(); //构建网页库
};
} // namespace SG