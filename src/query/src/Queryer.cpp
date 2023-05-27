#include <Queryer.h>
#include <cstdint>
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

Json::Value Queryer::get(const std::string &content, uint64_t rkBegin, uint64_t rkEnd) {
    SG::SearchResultBuilder ret;

    SG::PartsInfo partsInfo = createPartsInfo(content);
    ret.addPartsInfo(partsInfo);
    auto [resultList, totalCnt] = createResultList(partsInfo, rkBegin, rkEnd);
    ret.addItems(std::move(resultList));
    ret.addItemTotalNumber(totalCnt);
    return ret.build();
}

PartsInfo Queryer::createPartsInfo(const std::string &content) {
    return m_divider.divide(content).words;
}

Queryer::Queryer() {
    load_offsets();
    std::ifstream     is("../assets/library/fst.lib");
    std::stringstream ss;
    ss << is.rdbuf();
    const auto &byte_code = ss.str();
    is.close();

    m_matcher = new fst::map<uint64_t>{byte_code.data(), byte_code.size()};
    if (!m_matcher) {
        spdlog::error("[Queryer::creatResultList] Failed to create m_matcher");
    }
}
Queryer::~Queryer() {
    delete m_matcher;
}

std::pair<std::vector<std::unique_ptr<SearchResultItem>>, uint64_t> Queryer::createResultList(const PartsInfo &partsInfo, uint64_t begin, uint64_t end) {
    std::vector<std::unique_ptr<SearchResultItem>> ret;
    std::vector<std::unique_ptr<SkipList<Doc>>>    sls;
    std::vector<double>                            idfs;
    std::vector<std::pair<std::size_t, double>>    scores;
    //calculate score of each key word
    for (const auto &entry : partsInfo) {
        uint64_t output;
        m_matcher->exact_match_search(entry.first, output);
        uint64_t      outputID = output / 400;
        std::ifstream inputFile("../assets/library/skl/" + std::to_string(outputID) + ".lib");
        if (!inputFile.is_open()) {
            spdlog::error("[Queryer::creatResultList] Failed to open json file");
            inputFile.close();
            break;
        }
        Json::CharReaderBuilder readerBuilder;
        Json::Value             singleFile;
        std::string             errs;
        Json::parseFromStream(readerBuilder, inputFile, &singleFile, &errs);
        inputFile.close();
        for (auto &&term : singleFile) {        // 数组类型，遍历每个元素
            if (term["id"].asInt() == output) { // 找到匹配的id值，返回对应的"skl"
                sls.push_back(std::make_unique<SkipList<Doc>>(term["skl"]));
                idfs.push_back(term["idf"].asDouble());
                break;
            }
        }
    }

    std::vector<std::vector<Doc>> combineResult = SkipList<Doc>::combine(sls);

    std::cout << combineResult.size() << std::endl;
    for (auto &i : combineResult) {
        std::cout << i.size() << std::endl;
    }
    std::cout << idfs.size() << std::endl;

    for (int i = 0; i < combineResult.size(); ++i) {
        double score = 0;
        for (int j = 0; j < idfs.size(); ++j) {
            score += (idfs[j] * combineResult[i][j].tf);
        }
        scores.push_back(std::make_pair(combineResult[i][0].docId, score));
    }
    std::sort(scores.begin(), scores.end(), [](const auto &a, const auto &b) {
        return a.second > b.second;
    });

    //find doc by m_offsets
    std::ifstream lib_file("../assets/library/docLibrary.lib");
    for (auto i = begin; i < scores.size() && i < end; ++i) {
        int beg  = m_offsets[scores[i].first].first;
        int size = m_offsets[scores[i].first].second;
        lib_file.seekg(beg);
        std::string str(size, ' ');
        lib_file.read(&str[0], size);
        XMLParser xmlParser(str);

        ItemInfo info;
        info.title = xmlParser.parser("Title");
        info.text  = xmlParser.parser("Content");
        info.desc  = info.text.substr(0, std::min(static_cast<uint64_t>(info.text.size()), 256ul)); //描述暂时用标题替代
        std::map<std::string, double>     correlation;                                              //相关性暂时设为空
        std::unique_ptr<SearchResultItem> item = std::make_unique<SearchResultItem>(scores[i].second, "file://" + xmlParser.parser("Url"), info, correlation, 0);

        ret.push_back(std::move(item));
    }
    lib_file.close();
    return {std::move(ret), scores.size()};
}

void Queryer::load_offsets() {
    std::ifstream offset_file("../assets/library/docOffsets.lib");
    std::string   line;
    while (std::getline(offset_file, line)) {
        int                key, value1, value2;
        std::istringstream iss(line);
        while (iss >> key >> value1 >> value2)
            m_offsets.insert({key, std::make_pair(value1, value2)});
    }
    offset_file.close();
}

} // namespace SG