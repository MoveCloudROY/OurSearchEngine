#include <iostream>
#include "DivideCreator.h"


namespace SG {
Divider::Divider()
    : jieba{DICT_PATH, HMM_PATH, USER_DICT_PATH, IDF_PATH, STOP_WORD_PATH} {

    LoadStopWordDict(STOP_WORD_PATH, stopWords_);
}
Divider::~Divider() {}

void Divider::LoadStopWordDict(const std::string &filePath, std::unordered_set<std::string> &stopWords_) {
    std::ifstream ifs(filePath.c_str());
    XCHECK(ifs.is_open()) << "open " << filePath << " failed";
    std::string line;
    while (getline(ifs, line)) {
        stopWords_.insert(line);
    }
    assert(stopWords_.size());
}

void Divider::delete_stopwords(std::vector<std::string> &raws) {
    //search all words
    raws.erase(std::remove_if(raws.begin(), raws.end(), [&](const std::string &word) {
                   return stopWords_.find(word) != stopWords_.end();
               }),
               raws.end());
}

DivideResult Divider::divide(const std::string &sentence) {
    std::vector<std::string> raws;
    DivideResult             result;
    jieba.CutForSearch(sentence, raws, true);
    delete_stopwords(raws);
    result.lastWord=raws.back();
    //duplicate removal und write to result
    for (const std::string &raw : raws) {
        if (result.words.find(raw) == result.words.end()) {
            result.words.insert({raw, 1});
        } else {
            result.words[raw] += 1;
        }
    }
    result.totalFreq = raws.size();
    return result;
}

} // namespace SG