#include "Fst.h"

#include <spdlog/spdlog.h>

#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>

int main() {
    std::ifstream is("../assets/library/fst.lib");
    if (!is.is_open()) {
        return 1;
    }

    std::stringstream ss;
    ss << is.rdbuf();
    const auto &byte_code = ss.str();

    is.close();

    fst::map<uint64_t> matcher(byte_code.data(), byte_code.size());
    if (!matcher) {
        spdlog::error("[test_fst] Failed to create matcher");
        return 0;
    }

    //精确测试
    uint64_t output;
    matcher.exact_match_search("使用寿命", output);
    std::cout<<output<<std::endl;

    // 前缀测试
    std::cout << "[Prefix Test]" << std::endl;
    auto prefixes = matcher.common_prefix_search("使用寿命");
    for (auto &&i : prefixes) {
        std::cout << "(key: " << i.first << " output: " << i.second << ")" << std::endl;
    }

    // 预测测试
    std::cout << "[Predictive Test]" << std::endl;
    auto predictives = matcher.predictive_search("使用");
    for (auto &&i : predictives) {
        std::cout << "(key: " << i.first << " output: " << i.second << ")" << std::endl;
    }

    // 建议
    std::cout << "[Suggestions]" << std::endl;
    for (auto [r, k, o] : matcher.suggest("使用")) {
        if (r > 0.6)
            std::cout << "ratio: " << r << " key: " << k << " output: " << o
                      << std::endl;
    }
}