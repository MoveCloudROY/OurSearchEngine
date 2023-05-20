#pragma once
#include <string>
#include <unordered_set>
#include <cppjieba/Jieba.hpp>
#include "Datatype.h"

namespace SG {
class Divider {
private:
    static constexpr char *DICT_PATH      = "../assets/seperate_models/jieba.dict.utf8";
    static constexpr char *HMM_PATH       = "../assets/seperate_models/hmm_model.utf8";
    static constexpr char *USER_DICT_PATH = "../assets/seperate_models/user.dict.utf8";
    static constexpr char *IDF_PATH       = "../assets/seperate_models/idf.utf8";
    static constexpr char *STOP_WORD_PATH = "../assets/seperate_models/stop_words.utf8";
    void                   LoadStopWordDict(const std::string &filePath, std::unordered_set<std::string> &stopWords_); //加载停用词词典
    void                   delete_stopwords(std::vector<std::string> &raws);                                           //删去停用词
    // void                            calculateTF(DivideResult &result);                                                          //计算TF值
    std::unordered_set<std::string> stopWords_;
    cppjieba::Jieba                 jieba;

public:
    Divider();
    ~Divider();
    DivideResult divide(const std::string &sentence);
};
} // namespace SG