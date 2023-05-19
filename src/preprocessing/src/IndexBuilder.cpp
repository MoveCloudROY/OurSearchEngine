#include <fstream>
#include <cmath>
#include "IndexBuilder.h"
#include "XMLParser.hpp"
#include <spdlog/spdlog.h>

namespace SG {
void IndexBuilder::load_offsets() {
    std::ifstream offset_file("../assets/library/docOffsets.lib");
    std::string   line;
    while (std::getline(offset_file, line)) {
        int                key, value1, value2;
        std::istringstream iss(line);
        while (iss >> key >> value1 >> value2)
            offsets.insert({key, std::make_pair(value1, value2)});
    }
    offset_file.close();
    totalDocs = offsets.size();
}

void IndexBuilder::UpdateInvertedIndex(InvIndexList &InvertedIndex, DivideResult &result, int docID) {
    for (const auto &pair : result.words) {
        if (InvertedIndex.find(pair.first) == InvertedIndex.end()) {
            InvertedIndex.insert({pair.first, std::map<int, double>{{docID, 1.0 * pair.second / result.totalFreq}}});
        } else {
            InvertedIndex[pair.first].insert(std::make_pair(docID, 1.0 * pair.second / result.totalFreq));
        }
    }
}

void IndexBuilder::traverse_und_divide() {
    std::ifstream lib_file("../assets/library/docLibrary.lib");
    for (size_t index = 1; index <= offsets.size(); ++index) {
        spdlog::info("Divide Doc {}", index);
        int beg  = offsets[index].first;
        int size = offsets[index].second;

        
        lib_file.seekg(beg); //设置文件指针位置
        //读取指定大小的数据
        std::string str(size, ' ');   //创建大小为size的字符串并填充空格字符
        lib_file.read(&str[0], size); //将数据读取到字符串中
        //解析xml，读出content
        XMLParser   xmlParser(str);
        std::string content = xmlParser.parser("Content");
        //开始分词
        DivideResult result = divi.divide(content);


        //更新倒排索引
        UpdateInvertedIndex(InvertedIndex, result, index);
    }
    lib_file.close();
}

void IndexBuilder::outputIndex() {
    std::cout << "Begin Write InvertedIndex into output.txt" << std::endl;
    std::ofstream file("../assets/library/output.txt");
    if (file.is_open()) {
        // 遍历InvertedIndex并将数据写入文件
        for (const auto &outerPair : InvertedIndex) {
            // 写入外部map的键
            file << outerPair.first << ":";
            // 遍历内部map并将键值对写入文件
            for (const auto &innerPair : outerPair.second) {
                file << " (" << innerPair.first << "," << innerPair.second << ")";
            }
            // 写入换行符
            file << "\n";
        }
        // 关闭文件流
        file.close();
        std::cout << "Data written to file successfully." << std::endl;
    } else {
        std::cout << "Failed to open the file." << std::endl;
    }
}

void IndexBuilder::write_idf() {
    double        idf;
    std::ofstream file("../assets/library/IDF.lib");
    for (auto &entry : InvertedIndex) {
        idf = log10(totalDocs / entry.second.size());
        file << entry.first + " " + to_string(idf) + "\n";
        file.flush();
        idfs.insert({entry.first, idf});
    }
    file.close();
}

void IndexBuilder::build() {
    load_offsets();
    traverse_und_divide();
    write_idf();
}

IndexBuilder::~IndexBuilder() {}
} // namespace SG