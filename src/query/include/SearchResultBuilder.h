#pragma once

#include "Datatype.h"
#include <memory>

namespace SG {

class SearchResultBuilder {
public:
    SearchResultBuilder();
    void        addItem(SearchResultItem&& item);
    void        addPartsInfo(const std::map<std::string, SSIZE_T>& parts);
    Json::Value build();

private:
    std::map<std::string, SSIZE_T> partsInfo;
    std::vector<SearchResultItem*> results;
};

} // namespace SG