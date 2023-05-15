#include <fstream>
#include <stack>
#include "LibraryBuilder.h"

namespace SG {

    LibraryBuilder::LibraryBuilder(const char *folder_path) {
        this->folder_path = folder_path;
    }

    LibraryBuilder::~LibraryBuilder() {}

    void LibraryBuilder::scan_directory() {
        //非递归遍历(使用堆栈模拟)，扫描并储存目录中所有文件的路径
        std::stack<fs::path> path_stack;
        path_stack.push(folder_path);
        while (!path_stack.empty()) {
            fs::path current_path = path_stack.top();
            path_stack.pop();
            for (const auto &entry: fs::directory_iterator(current_path)) {
                if (fs::is_regular_file(entry) && entry.path().extension() == ".txt") {
                    file_path_list.push_back(entry.path().string());
                } else if (fs::is_directory(entry)) {
                    path_stack.push(entry.path());
                }
            }
        }
    }

    void LibraryBuilder::traverse_directory() {
        std::ofstream lib_file("../assets/library/docLibrary.lib");
        std::ofstream offset_file("../assets/library/docOffsets.lib");
        long long offset = 0; //记录写入位置的偏移量，用于遍历库文件
        int docID = 1;
        for (int index = 0; index < file_path_list.size(); index++) {
            //循环字符缓冲直接写入会导致文件读写丢失大量数据，可能是刷新不及时，需要手动flush
            //需要排除内容为空的文件
            std::string content = read_content(file_path_list[index]);
            if (content.empty())
                continue;
            std::string doc_str = "<doc>\n"
                                  "<ID>" + std::to_string(docID) + "</ID>\n"
                                                                   "<Url>" +
                                  file_path_list[index] + "</Url>\n"
                                                          "<Title>" +
                                  getTitle(file_path_list[index]) + "</Title>\n"
                                                                    "<Content>\n" +
                                  content + "</Content>\n" + "</doc>\n";
            lib_file.seekp(offset); // 定位到当前偏移量的位置
            lib_file << doc_str;
            lib_file.flush(); //必须立即刷新
            offset_file << docID << " " << offset << " " << doc_str.size() << "\n";
            offset_file.flush();
            offset = lib_file.tellp(); // 更新偏移量为当前位置
            docID++;
        }
        lib_file.close();
        offset_file.close();
    }

    void LibraryBuilder::build() {
        scan_directory();
        traverse_directory();
        std::cout << "Build Library Complete!" << std::endl;
    }

    std::string LibraryBuilder::read_content(std::string &file_path) {
        std::ifstream file(file_path);
        if (!file.is_open()) {
            //文件路径含中文编码可能会导致打开出错
            std::cerr << "Failed to open file: " << file_path << std::endl;
            return "";
        }
        std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        return content;
    }

    std::string LibraryBuilder::getTitle(std::string &file_path) {
        std::ifstream file(file_path);
        //获得文本标题
        std::string line;
        std::string line_0;
        std::string keyword = "【 标  题 】";
        int max_lines_to_check = 20;
        int current_line = 0;
        bool find_title = false;
        std::string title;
        while (std::getline(file, line) && current_line < max_lines_to_check) {
            if (current_line == 0)
                line_0 = line;
            if (line.find(keyword) != std::string::npos) {
                //包含关键字，获取标题
                title = line.substr(line.find(keyword) + keyword.length());
                find_title = true;
                break;
            }
            current_line++;
        }
        if (!find_title) {
            //没找到标题，默认第一行为标题
            title = line_0;
        }
        return title;
    }
}



