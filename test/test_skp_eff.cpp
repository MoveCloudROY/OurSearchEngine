#include "SkipList.h"
#include <chrono>
#include <fstream>
#include <iostream>
#include <vector>
#include <json/json.h>

struct Item {
    int id;
    Item() = default;
    Item(int id)
        : id{id} {}
    bool operator==(const Item &t) const {
        return id == t.id;
    }
    bool operator<(const Item &t) const & {
        return id < t.id;
    }
    bool operator<(const Item &t) const && {
        return id < t.id;
    }

    friend std::ostream &operator<<(std::ostream &os, const Item &t) {
        os << '(' << t.id << ')';
        return os;
    }

    static Json::Value toJson(const Item &t) {
        Json::Value ret;
        ret["id"] = t.id;
        return ret;
    }
    static Item fromJson(const Json::Value &src) {
        return {src["id"].asInt()};
    }
};
using SG::Core::SkipList;

void test(const int &size, std::ofstream &outputFile) {
    srand(time(nullptr));

    SkipList<Item> skipList;

    const int largeSize = size;

    // test insertion
    std::vector<int> insertData;
    srand(time(nullptr));
    for (int i = 0; i < largeSize; i++) {
        insertData.push_back(rand() % largeSize);
    }

    auto start = std::chrono::steady_clock::now();
    for (int i = 0; i < insertData.size(); i++) {
        skipList.insert(Item(insertData[i]));
    }
    auto end      = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    outputFile << size << "," << duration.count();

    // outputFile << "\n========\n"
    //            << skipList.dump().toStyledString() << "\n========\n";
    // std::cout << "=====\n";
    // test deletion
    // std::vector<int> removeData;
    // srand(time(nullptr));
    // for (int i = 0; i < largeSize; i++) {
    //     removeData.push_back(rand() % largeSize);
    // }

    // start = std::chrono::steady_clock::now();
    // for (int i = 0; i < removeData.size(); i++) {
    //     skipList.remove(removeData[i]);
    // }
    // end      = std::chrono::steady_clock::now();
    // duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    // outputFile << "," << duration.count();

    // test search
    std::vector<int> searchData;
    for (int i = 0; i < largeSize; i++) {
        searchData.push_back(rand() % largeSize);
    }

    start = std::chrono::steady_clock::now();
    for (int i = 0; i < searchData.size(); i++) {
        auto l = skipList.search(Item(searchData[i]));
    }
    end      = std::chrono::steady_clock::now();
    duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    outputFile << "," << duration.count() << std::endl;

    // std::cout << "=====\n";

    // skipList.print();
}

int main() {
    std::ofstream outputFile("result.csv");
    outputFile << "DataSize,InsertionTime,SearchTime" << std::endl;

    for (int i = 100; i <= 10000000; i *= 10) {
        test(i, outputFile);
    }

    outputFile.close();

    return 0;
}
