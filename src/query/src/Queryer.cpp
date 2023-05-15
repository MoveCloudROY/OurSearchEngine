#include <Queryer.h>
#include <memory>

#include "Datatype.h"
#include "SearchResultBuilder.h"

namespace SG {

Json::Value Queryer::get(const std::string& content, SSIZE_T begin, SSIZE_T end) {
    SG::SearchResultBuilder ret;

    SG::PartsInfo partsInfo = createPartsInfo(content);
    ret.addPartsInfo(partsInfo);
    auto resultList = createResultList(partsInfo, begin, end);

    return ret.build();
}

PartsInfo Queryer::createPartsInfo(const std::string& content) {
}

std::vector<SearchResultItem*> Queryer::createResultList(const PartsInfo& partsInfo, SSIZE_T begin, SSIZE_T end) {
}

} // namespace SG