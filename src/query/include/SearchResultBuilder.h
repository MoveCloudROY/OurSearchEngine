#pragma once

#include "Datatype.h"
#include <memory>

namespace SG {

class SearchResultBuilder {
public:
    SearchResultBuilder();
    void        addItem(SearchResultItem &&item);
    void        addPartsInfo(const std::map<std::string, uint64_t> &parts);
    Json::Value build();

private:
    std::map<std::string, uint64_t> partsInfo;
    std::vector<SearchResultItem *> results;
};

} // namespace SG