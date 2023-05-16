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
    int    docId;
    double tf;

    bool operator==(const Doc& t) const {
        return docId == t.docId;
    }
    bool operator<(const Doc& t) const {
        return docId < t.docId;
    }

    friend std::ostream& operator<<(std::ostream& os, const Doc& t) {
        os << '(' << t.docId << ' ' << t.tf << ')';
        return os;
    }

    static Json::Value toJson(const Doc& t) {
        Json::Value rt;
        rt["docId"] = t.docId;
        rt["tf"]    = t.tf;
        return rt;
    }
};



int main() {
    // 0 2 4 6 8 10 12 14
    SkipList<std::string, Doc> l1;
    for (int i = 0; i <= 15; i = i + 2) {
        std::cout << "#" << std::endl;
        l1.emplace_insert(i, distr(gen));
    }
    l1.print();
    // auto ans1 = l1.dump(Doc::toJson).toStyledString();

    // spdlog::info("\n{}", ans1);

    // // 1 4 7 10 13 16 19
    // SkipList<std::string, Doc> l2;
    // for (int i = 1; i <= 20; i = i + 3) {
    //     l2.emplace_insert(i, distr(gen));
    // }
    // // l2.dump();

    // // 4 10 16 22 28
    // SkipList<std::string, Doc> l3;
    // for (int i = 4; i <= 30; i = i + 6) {
    //     l3.emplace_insert(i, distr(gen));
    // }
    // // l3.dump();
    // std::vector<SkipList<std::string, Doc>*> p;
    // p.push_back(&l1);
    // p.push_back(&l2);
    // p.push_back(&l3);

    // auto ans = SkipList<std::string, Doc>::combine(p);

    // for (auto&& i : ans) {
    //     spdlog::info("Node -- docid: {}, tf: {}", i.docId, i.tf);
    // }
}