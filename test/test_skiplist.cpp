#include <cstddef>
#include <iostream>
#include "spdlog/spdlog.h"
#include <SkipList.h>
#include <json/value.h>
#include <string>
#include <random>
#include <vector>

using SG::Core::SkipList;

std::random_device               rd;
std::mt19937                     gen(rd());
std::uniform_real_distribution<> distr;


struct Doc {
    std::size_t docId;
    double      tf;

    bool operator==(const Doc &t) const {
        return docId == t.docId;
    }
    bool operator<(const Doc &t) const {
        return docId < t.docId;
    }

    friend std::ostream &operator<<(std::ostream &os, const Doc &t) {
        os << '(' << t.docId << ' ' << t.tf << ')';
        return os;
    }

    static Json::Value toJson(const Doc &t) {
        Json::Value ret;
        ret["docId"] = t.docId;
        ret["tf"]    = t.tf;
        return ret;
    }
    static Doc fromJson(const Json::Value &src) {
        return {src["docId"].asUInt64(), src["tf"].asDouble()};
    }
};


int main() {
    // 0 2 4 6 8 10 12 14
    SkipList<Doc> l1;
    for (size_t i = 0; i <= 15; i = i + 2) {
        l1.insert({i, distr(gen)});
    }
    l1.print();
    // 1 4 7 10 13 16 19
    SkipList<Doc> l2;
    for (size_t i = 1; i <= 20; i = i + 3) {
        l2.emplace_insert(i, distr(gen));
    }
    l2.print();
    // 4 10 16 22 28
    SkipList<Doc> l3;
    for (size_t i = 4; i <= 30; i = i + 6) {
        l3.emplace_insert(i, distr(gen));
    }
    l3.print();

    std::vector<SkipList<Doc> *> p;
    p.push_back(&l1);
    p.push_back(&l2);
    p.push_back(&l3);

    auto ans = SkipList<Doc>::combine(p);

    for (auto &&i : ans) {
        std::cout << "(docid, tf) = " << i << '\n';
    }

    spdlog::info("\n{}", l1.dump().toStyledString());

    auto ls_json = l1.dump();
    auto ls_copy = SkipList<Doc>(ls_json);
    spdlog::info("\n{}", ls_copy.dump().toStyledString());
}