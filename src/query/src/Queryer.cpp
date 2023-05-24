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

PartsInfo Queryer::createPartsInfo(const std::string &content) {
}

std::vector<SearchResultItem *> Queryer::createResultList(const PartsInfo &partsInfo, uint64_t rkBegin, uint64_t ekEnd) {
}

} // namespace SG