#include <Queryer.h>
#include <memory>
#include <spdlog/spdlog.h>
#include <json/value.h>
#include <fstream>
#include <string>

#include "XMLParser.hpp"
#include "Datatype.h"
#include "SearchResultBuilder.h"
#include "SkipList.h"

namespace SG {
using SG::Core::SkipList;

Json::Value Queryer::get(const std::string &content, uint64_t rkBegin, uint64_t ekEnd) {
    SG::SearchResultBuilder ret;

    SG::PartsInfo partsInfo = createPartsInfo(content);
    ret.addPartsInfo(partsInfo);
    auto resultList = createResultList(partsInfo, rkBegin, ekEnd);

    return ret.build();
}

PartsInfo Queryer::createPartsInfo(const std::string &content) {
    DivideResult result = divi.divide(content);
    return result.words;
}

Queryer::Queryer() {
    load_offsets();
    std::ifstream     is("../assets/library/fst.lib");
    std::stringstream ss;
    ss << is.rdbuf();
    const auto &byte_code = ss.str();
    is.close();

    matcher = new fst::map<uint64_t>{byte_code.data(), byte_code.size()};
    if (!matcher) {
        spdlog::error("[Queryer::creatResultList] Failed to create matcher");
    }
}
Queryer::~Queryer() {
    delete matcher;
}

std::vector<SearchResultItem *> Queryer::createResultList(const PartsInfo &partsInfo, uint64_t begin, uint64_t end) {
    std::vector<SearchResultItem *> ret;

    //calculate score of each key word
    std::map<int, double> scores;                                   //<docID,scores>
    for (const auto &entry : partsInfo) {
        auto predictives = matcher->predictive_search(entry.first); //预测模式
        for (auto &&i : predictives) {
            uint64_t      outputID = i.second / 400;
            std::ifstream inputFile("../assets/library/output/" + std::to_string(outputID) + ".lib");
            if (!inputFile.is_open()) {
                spdlog::error("[Queryer::creatResultList] Failed to open json file");
            }
            Json::CharReaderBuilder readerBuilder;
            Json::Value             singleFile;
            std::string             errs;
            Json::parseFromStream(readerBuilder, inputFile, &singleFile, &errs);
            for (const auto &term : singleFile) { // 数组类型，遍历每个元素
                if (term.isMember("id")) {        // 检查是否有 "id" 字段
                    int id = term["id"].asInt();
                    if (id == i.second) {         // 找到匹配的 id 值，返回对应的 "skl"
                        if (term.isMember("skl")) {
                            SkipList<Doc> sl(term["skl"]);
                            sl.print();
                        } else { // 没有 "skl" 字段
                            break;
                        }
                    }
                }
            }
        }
    }

    //sort docs by scores
    std::vector<std::pair<int, double>> pairs;
    for (const auto &pair : scores) {
        pairs.push_back(pair);
    }
    std::sort(pairs.begin(), pairs.end(), [](const auto &a, const auto &b) {
        return a.second < b.second;
    });

    //find doc by offsets
    std::ifstream lib_file("../assets/library/docLibrary.lib");
    for (auto &&pair : scores) {
        int beg  = offsets[pair.first].first;
        int size = offsets[pair.first].second;
        lib_file.seekg(beg);
        std::string str(size, ' ');
        lib_file.read(&str[0], size);
        XMLParser xmlParser(str);

        ItemInfo info;
        info.title = xmlParser.parser("Title");
        info.text  = xmlParser.parser("Content");
        info.desc  = xmlParser.parser("Title"); //描述
        std::map<std::string, double>     correlation;
        std::unique_ptr<SearchResultItem> sr = std::make_unique<SearchResultItem>();
        // sr->score = pair.second;
        // sr->url = xmlParser.parser("Url");
        // sr->info = std::move(info);
        // sr->correlation = std::move(correlation);
        // sr->same_domain_numbers = 0;
        // ret.emplace_back(sr);
    }

    std::vector<SearchResultItem *> ret_output(ret.begin() + begin, ret.begin() + end);
    return ret_output;
}

void Queryer::load_offsets() {
    std::ifstream offset_file("../assets/library/docOffsets.lib");
    std::string   line;
    while (std::getline(offset_file, line)) {
        int                key, value1, value2;
        std::istringstream iss(line);
        while (iss >> key >> value1 >> value2)
            offsets.insert({key, std::make_pair(value1, value2)});
    }
    offset_file.close();
}

} // namespace SG