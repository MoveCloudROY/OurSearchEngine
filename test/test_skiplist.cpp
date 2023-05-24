#include "Datatype.h"
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


int main() {
    // 0 2 4 6 8 10 12 14
    SkipList<SG::Doc> l1;
    for (size_t i = 0; i <= 15; i = i + 2) {
        l1.insert({i, distr(gen)});
    }
    l1.print();
    // 1 4 7 10 13 16 19
    SkipList<SG::Doc> l2;
    for (size_t i = 1; i <= 20; i = i + 3) {
        l2.emplace_insert(i, distr(gen));
    }
    l2.print();
    // 4 10 16 22 28
    SkipList<SG::Doc> l3;
    for (size_t i = 4; i <= 30; i = i + 6) {
        l3.emplace_insert(i, distr(gen));
    }
    l3.print();

    std::vector<SkipList<SG::Doc> *> p;
    p.push_back(&l1);
    p.push_back(&l2);
    p.push_back(&l3);

    auto ans = SkipList<SG::Doc>::combine(p);

    for (auto &&i : ans) {
        for (auto &&j : i) {
            spdlog::info("(docid, tf) = ({}, {})", j.docId, j.tf);
        }
        spdlog::info("====================");
    }

    // spdlog::info("\n{}", l1.dump().toStyledString());

    // auto ls_json = l1.dump();
    // auto ls_copy = SkipList<SG::Doc>(ls_json);
    // spdlog::info("\n{}", ls_copy.dump().toStyledString());
}