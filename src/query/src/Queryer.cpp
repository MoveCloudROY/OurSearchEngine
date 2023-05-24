#include <Queryer.h>
#include <memory>

#include "Datatype.h"
#include "SearchResultBuilder.h"

namespace SG {

Json::Value Queryer::get(const std::string &content, uint64_t rkBegin, uint64_t ekEnd) {
    SG::SearchResultBuilder ret;

    SG::PartsInfo partsInfo = createPartsInfo(content);
    ret.addPartsInfo(partsInfo);
    auto resultList = createResultList(partsInfo, rkBegin, ekEnd);

    return ret.build();
}

PartsInfo Queryer::createPartsInfo(const std::string& content) {
    DivideResult result=divi.divide(content);
    return result.words;
}

std::vector<SearchResultItem*> Queryer::createResultList(const PartsInfo& partsInfo, uint64_t begin, uint64_t end) {
    for(const auto &entry : partsInfo)
    {
        //对每个搜索关键词，计算分数
        
    }
}

} // namespace SG