#pragma once

#include "DivideCreator.h"
#include <cstdint>
#include <filesystem>
#include <map>
#include <string>
#include <unordered_set>
#include <vector>

#include <tbb/tbb.h>
#include <oneapi/tbb/queuing_mutex.h>

namespace SG {

using InvIndexList = std::map<std::string, std::map<uint64_t, double>>;
using DocOffset    = int;
using DocSize      = int;
class IndexBuilder {
private:
    uint64_t                                          docCnt = 0;    // 文件总数
    std::map<uint64_t, std::pair<DocOffset, DocSize>> offsets;       // 偏移量
    Divider                                           divi;          // 分词器
    tbb::queuing_mutex                                m_mutex;       // map 锁
    InvIndexList                                      InvertedIndex; // 倒排索引,<词语，<文档id，词频tf>>

    void load_offsets();                                             //读取偏移量
    void traverse_und_divide();                                      //遍历网页库并分词

public:
    IndexBuilder() = default;
    ~IndexBuilder();
    void dumpFst(const std::string &path);
    void dumpSkipList(const std::filesystem::path path); //将倒排索引输出到文件中进行查阅
    void build();                                        //构建倒排索引
};
} // namespace SG