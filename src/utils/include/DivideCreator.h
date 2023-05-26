#pragma once
#include <string>
#include <vector>
#include <unordered_set>
#include <cppjieba/Jieba.hpp>
#include "Datatype.h"

namespace SG {
class Divider {
private:
    static constexpr const char    *DICT_PATH      = "../assets/seperate_models/jieba.dict.utf8";
    static constexpr const char    *HMM_PATH       = "../assets/seperate_models/hmm_model.utf8";
    static constexpr const char    *USER_DICT_PATH = "../assets/seperate_models/user.dict.utf8";
    static constexpr const char    *IDF_PATH       = "../assets/seperate_models/idf.utf8";
    static constexpr const char    *STOP_WORD_PATH = "../assets/seperate_models/stop_words.utf8";
    void                            LoadStopWordDict(const std::string &filePath, std::unordered_set<std::string> &stopWords_); //加载停用词词典
    void                            delete_stopwords(std::vector<std::string> &raws);                                           //删去停用词
    std::unordered_set<std::string> stopWords_;
    cppjieba::Jieba                 jieba;

public:
    Divider();
    ~Divider();
    DivideResult divide(const std::string &sentence);
    std::vector<std::string> divide_raw(const std::string &sentence);
};
} // namespace SG