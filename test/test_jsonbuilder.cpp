
#include "Datatype.h"
#include "spdlog/spdlog.h"
#include <SearchResultBuilder.h>
#include "json/value.h"

#include <iostream>

int main() {
    SG::SearchResultBuilder obj;

    SG::PartsInfo parts{
        {"a", 0.5},
        {"b", 0.9}

    };
    obj.addPartsInfo(parts);

    SG::SearchResultItem item{
        1.0,
        "/path/to/url",
        {
            "Title",
            "desc",
            "text",
        },
        {{"a", 10}, {"b", 11}},
        0

    };
    obj.addItem(std::move(item));

    auto ret = obj.build();
    std::cout << SG::Utils::JsonToString(ret);
}