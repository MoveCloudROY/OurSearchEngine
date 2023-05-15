#include <fstream>
#include "IndexBuilder.h"
#include "limonp/Logging.hpp"
#include "cppjieba/Jieba.hpp"
#include "XMLParser.hpp"

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

std::map<std::string, double> IndexBuilder::getTF(std::vector<std::string>& words) {
    size_t                        totalWords = words.size();
    std::map<std::string, double> TF;       //<词语，词频tf>
    for (const std::string& word : words) { //获得<词语，次数>
        if (TF.find(word) == TF.end()) {
            TF.insert({word, 1.0});
        } else {
            TF[word] += 1.0;
        }
    }
    // 计算频率：每个词语的频率等于该词语在文本中出现的次数除以文本中的总词数
    for (auto& pair : TF) {
        pair.second /= totalWords;
    }
    return TF;
}

void IndexBuilder::UpdateInvertedIndex(InvIndexList& InvertedIndex, std::map<std::string, double>& TF, int docID) {
    for (const auto& pair : TF) {
        if (InvertedIndex.find(pair.first) == InvertedIndex.end()) {
            InvertedIndex.insert({pair.first, std::map<int, double>{{docID, pair.second}}});
        } else {
            InvertedIndex[pair.first].insert(std::make_pair(docID, pair.second));
        }
    }
}

void IndexBuilder::LoadStopWordDict(const std::string& filePath, std::unordered_set<std::string>& stopWords_) {
    std::ifstream ifs(filePath.c_str());
    XCHECK(ifs.is_open()) << "open " << filePath << " failed";
    std::string line;
    while (getline(ifs, line)) {
        stopWords_.insert(line);
    }
    assert(stopWords_.size());
}

void IndexBuilder::traverse_und_divide() {
    cppjieba::Jieba jieba(DICT_PATH, HMM_PATH, USER_DICT_PATH, IDF_PATH, STOP_WORD_PATH);
    std::ifstream   lib_file("../assets/library/docLibrary.lib");
    for (size_t index = 1; index <= offsets.size(); ++index) {
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
        std::vector<std::string> words;
        jieba.CutForSearch(content, words, true);
        std::map<std::string, double> TF = getTF(words);
        //更新倒排索引
        UpdateInvertedIndex(InvertedIndex, TF, index);
    }
    lib_file.close();
}

void IndexBuilder::delete_stopwords() {
    //蛮力，查停用表中的词有没有在倒排索引中
    std::unordered_set<string> stopWords_;
    LoadStopWordDict(STOP_WORD_PATH, stopWords_);
    for (auto&& it : stopWords_) {
        if (InvertedIndex.find(it) != InvertedIndex.end())
            InvertedIndex.erase(it);
    }
    if (InvertedIndex.find("\n") != InvertedIndex.end())
        InvertedIndex.erase("\n");
    if (InvertedIndex.find("\r") != InvertedIndex.end())
        InvertedIndex.erase("\r");
}

void IndexBuilder::outputIndex() {
    std::cout << "Begin Write InvertedIndex into output.txt" << std::endl;
    std::ofstream file("../assets/library/output.txt");
    if (file.is_open()) {
        // 遍历InvertedIndex并将数据写入文件
        for (const auto& outerPair : InvertedIndex) {
            // 写入外部map的键
            file << outerPair.first << ":";
            // 遍历内部map并将键值对写入文件
            for (const auto& innerPair : outerPair.second) {
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
    for (auto& entry : InvertedIndex) {
        idf = std::log(totalDocs / entry.second.size());
        file << entry.first + " " + to_string(idf) + "\n";
        file.flush();
        idfs.insert({entry.first, idf});
    }
    file.close();
}

void IndexBuilder::build() {
    load_offsets();
    traverse_und_divide();
    delete_stopwords();
    write_idf();
}

IndexBuilder::~IndexBuilder() {}
} // namespace SG