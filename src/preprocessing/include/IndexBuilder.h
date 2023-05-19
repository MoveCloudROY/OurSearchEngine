#pragma once

#include <iostream>
#include <filesystem>
#include <map>
#include <string>
#include <unordered_set>
#include <vector>
#include "DivideCreator.h"

namespace SG {
namespace fs       = std::filesystem;
using InvIndexList = std::map<std::string, std::map<int, double>>;
using DocOffset = int;
using DocSize = int;
class IndexBuilder {
private:
    SSIZE_T                            totalDocs;                                           //文件总数
    std::map<int, std::pair<DocOffset, DocSize>> offsets;                                             //偏移量
    std::map<std::string, double>      idfs;                                                //idf
    Divider                            divi;                                                //分词器

    void UpdateInvertedIndex(InvIndexList &InvertedIndex, DivideResult &result, int docID); //更新倒排索引
    void load_offsets();                                                                    //读取偏移量
    void traverse_und_divide();                                                             //遍历网页库并分词
    void write_idf();                                                                       //计算idf并写入文件中

public:
    InvIndexList InvertedIndex; //倒排索引,<词语，<文档id，词频tf>>
    IndexBuilder() = default;
    ~IndexBuilder();
    void outputIndex(); //将倒排索引输出到文件中进行查阅
    void build();       //构建倒排索引
};
} // namespace SG