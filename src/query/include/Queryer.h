#pragma once

#include <memory>
#include <string>
#include <vector>
#include <json/json.h>
#include "Datatype.h"
#include "DivideCreator.h"

namespace SG {

class Queryer {
    static Queryer& getInstance() {
        static Queryer instance;
        return instance;
    }
    Json::Value get(const std::string& content, SSIZE_T begin, SSIZE_T end);


private:
    Queryer();
    ~Queryer();
    Queryer(const Queryer&);
    Queryer& operator=(const Queryer&);
    Divider divi;

    PartsInfo                      createPartsInfo(const std::string& content);
    std::vector<SearchResultItem*> createResultList(const PartsInfo& partsInfo, SSIZE_T begin, SSIZE_T end);
};

} // namespace SG
