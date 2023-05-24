#pragma once

#include <memory>
#include <string>
#include <vector>
#include <json/json.h>
#include "Datatype.h"

namespace SG {

class Queryer {
    static Queryer &getInstance() {
        static Queryer instance;
        return instance;
    }
    Json::Value get(const std::string &content, uint64_t rkBegin, uint64_t rkEnd);


private:
    Queryer();
    ~Queryer();
    Queryer(const Queryer &);
    Queryer &operator=(const Queryer &);

    PartsInfo                       createPartsInfo(const std::string &content);
    std::vector<SearchResultItem *> createResultList(const PartsInfo &partsInfo, uint64_t rkBegin, uint64_t rkEnd);
};

} // namespace SG
