#pragma once

#include "Datatype.h"
#include <cstdint>
#include <memory>

namespace SG {

class SearchResultBuilder {
public:
    SearchResultBuilder();
    void        addItem(SearchResultItem &&item);
    void        addItems(std::vector<std::unique_ptr<SG::SearchResultItem>> &&items);
    void        addPartsInfo(const std::map<std::string, uint64_t> &parts);
    void        addPartsInfo(std::map<std::string, uint64_t> &&parts);
    void        addItemTotalNumber(uint64_t tot);
    Json::Value build();

private:
    std::map<std::string, uint64_t>                partsInfo;
    std::vector<std::unique_ptr<SearchResultItem>> results;
    uint64_t                                       tot;
};

} // namespace SG