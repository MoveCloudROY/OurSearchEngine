#pragma once

#include <iostream>
#include <filesystem>
#include <map>
#include <string>
#include <unordered_set>
#include <vector>

namespace SG {
namespace fs       = std::filesystem;
using InvIndexList = std::map<std::string, std::map<int, double>>;
class IndexBuilder {
private:
    const char* const DICT_PATH      = "../assets/seperate_models/jieba.dict.utf8";
    const char* const HMM_PATH       = "../assets/seperate_models/hmm_model.utf8";
    const char* const USER_DICT_PATH = "../assets/seperate_models/user.dict.utf8";
    const char* const IDF_PATH       = "../assets/seperate_models/idf.utf8";
    const char* const STOP_WORD_PATH = "../assets/seperate_models/stop_words.utf8";

    size_t                             totalDocs = 0;
    std::map<int, std::pair<int, int>> offsets; //偏移量
    std::map<std::string, double>      idfs;    //idf

    std::map<std::string, double> getTF(std::vector<std::string>& words); //计算单词的词频tf
    void
         UpdateInvertedIndex(InvIndexList& InvertedIndex, std::map<std::string, double>& TF,
                             int docID);                                                             //更新倒排索引
    void LoadStopWordDict(const std::string& filePath, std::unordered_set<std::string>& stopWords_); //加载停用词词典
    void load_offsets();                                                                             //读取偏移量
    void traverse_und_divide();                                                                      //遍历网页库并分词
    void delete_stopwords();                                                                         //从倒排索引中删去停用词
    void write_idf();                                                                                //计算idf并写入文件中

public:
    InvIndexList InvertedIndex; //倒排索引,<词语，<文档id，词频tf>>

    ~IndexBuilder();
    void outputIndex(); //将倒排索引输出到文件中进行查阅
    void build();       //构建倒排索引
};
} // namespace SG