#pragma once

#include <memory>
#include <string>
#include <vector>
#include <json/json.h>
#include "Datatype.h"
#include "DivideCreator.h"
#include "Fst.h"

namespace SG {
using DocOffset = int;
using DocSize   = int;
class Queryer {
public:
    static Queryer &getInstance() {
        static Queryer instance;
        return instance;
    }
    Json::Value get(const std::string &content, uint64_t rkBegin, uint64_t rkEnd);

private:
    Divider                                           m_divider; // 分词器
    std::map<uint64_t, std::pair<DocOffset, DocSize>> m_offsets; // 偏移量
    fst::map<uint64_t>                               *m_matcher;

    Queryer();
    ~Queryer();
    Queryer(const Queryer &);
    Queryer &operator=(const Queryer &);

    void                                           load_offsets();                                                            //读取偏移量
    std::vector<std::string>                       predict_sentence(const std::string &content, const std::string &lastWord); //对当前搜索语句进行预测
    DivideResult                                   createPartsInfo(const std::string &content);
    std::vector<std::unique_ptr<SearchResultItem>> createResultList(const PartsInfo &partsInfo, uint64_t rkBegin, uint64_t rkEnd);
};

} // namespace SG
