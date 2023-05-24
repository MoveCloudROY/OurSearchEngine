#include "IndexBuilder.h"
#include "Datatype.h"
#include "XMLParser.hpp"
#include "Fst.h"
#include "SkipList.h"
#include <cstdint>
#include <json/value.h>
#include <ostream>


#include <cstddef>
#include <fstream>
#include <cmath>
#include <string>
#include <vector>

#include <oneapi/tbb/queuing_mutex.h>
#include <oneapi/tbb/task_arena.h>
#include <tbb/blocked_range.h>
#include <tbb/parallel_for.h>
#include <spdlog/spdlog.h>
#include <indicators/progress_bar.hpp>
#include <indicators/indeterminate_progress_bar.hpp>
#include <indicators/cursor_control.hpp>


namespace SG {
void IndexBuilder::load_offsets() {
    std::ifstream offset_file("../assets/library/docOffsets.lib");
    std::string   line;
    while (std::getline(offset_file, line)) {
        int                key, value1, value2;
        std::istringstream iss(line);
        while (iss >> key >> value1 >> value2)
            offsets.insert({key, std::make_pair(value1, value2)});
    }
    offset_file.close();
    docCnt = offsets.size();
}

void IndexBuilder::UpdateInvertedIndex(InvIndexList &InvertedIndex, DivideResult &result, int docID) {
    tbb::queuing_mutex::scoped_lock lock{m_mutex};
    for (const auto &pair : result.words) {
        if (InvertedIndex.find(pair.first) == InvertedIndex.end()) {
            InvertedIndex.insert({pair.first, {{docID, 1.0 * pair.second / result.totalFreq}}});
        } else {
            InvertedIndex[pair.first].insert(std::make_pair(docID, 1.0 * pair.second / result.totalFreq));
        }
    }
}

void IndexBuilder::traverse_und_divide() {
    spdlog::info("[IndexBuilder] Start to Divide Documents");

    using namespace indicators;
    IndeterminateProgressBar bar{
        indicators::option::BarWidth{50},
        indicators::option::Start{"["},
        indicators::option::Fill{"·"},
        indicators::option::Lead{"<==>"},
        indicators::option::End{"]"},
        indicators::option::PrefixText{"Dividing Documents"},
        indicators::option::ForegroundColor{indicators::Color::yellow},
        indicators::option::FontStyles{
            std::vector<indicators::FontStyle>{indicators::FontStyle::bold}}

    };
    indicators::show_console_cursor(false);
    constexpr size_t    MAX_THREADS = 16;
    tbb::global_control c{tbb::global_control::max_allowed_parallelism, MAX_THREADS};
    InvIndexList        tmp[MAX_THREADS];

    tbb::parallel_for(tbb::blocked_range<size_t>{1, offsets.size() + 1}, [&](tbb::blocked_range<size_t> r) {
        std::ifstream lib_file("../assets/library/docLibrary.lib");
        bar.tick();
        for (size_t index = r.begin(); index < r.end(); ++index) {
            int beg  = offsets[index].first;
            int size = offsets[index].second;

            lib_file.seekg(beg); //设置文件指针位置
            //读取指定大小的数据
            std::string str(size, ' ');   //创建大小为size的字符串并填充空格字符
            lib_file.read(&str[0], size); //将数据读取到字符串中
            //解析xml，读出content
            XMLParser   xmlParser(str);
            std::string content = xmlParser.parser("Content");
            //开始分词
            DivideResult result = divi.divide(content);

            //更新倒排索引
            for (const auto &item : result.words) {
                if (item.first.size() <= 1)
                    continue;
                if (tmp[tbb::this_task_arena::current_thread_index()].find(item.first) == tmp[tbb::this_task_arena::current_thread_index()].end()) {
                    tmp[tbb::this_task_arena::current_thread_index()].insert({item.first, {{index, 1.0 * item.second / result.totalFreq}}});
                } else {
                    tmp[tbb::this_task_arena::current_thread_index()][item.first].insert(std::make_pair(index, 1.0 * item.second / result.totalFreq));
                }
            }

            // UpdateInvertedIndex(InvertedIndex, result, index);
        }
        lib_file.close();
    });

    for (int i = 0; i < MAX_THREADS; ++i)
        InvertedIndex.insert(tmp[i].begin(), tmp[i].end());

    bar.mark_as_completed();
    indicators::show_console_cursor(true);
    spdlog::info("[IndexBuilder] Divide Documents Completely");
}

void IndexBuilder::dumpFst(const std::string &path) {
    // build FST
    spdlog::info("[IndexBuilder] Start to Output Fst.lib");
    std::vector<std::pair<std::string, uint64_t>> items;
    items.reserve(InvertedIndex.size());
    uint64_t cnt = 0;
    for (const auto &item : InvertedIndex) {
        items.emplace_back(item.first, cnt++);
    }

    std::stringstream os;
    auto [result, err] = fst::compile<uint64_t>(items, os, true);
    if (result == fst::Result::Success) {

        std::ofstream file(path);
        file << os.str();
        file.flush();
        file.close();

        spdlog::info("[IndexBuilder] Successfully Compile Fst");
    } else {
        spdlog::error("[IndexBuilder] Failed to Compile Fst, result={}, error_code={}", (int)result, err);
    }
    spdlog::info("[IndexBuilder] Successfully Output Fst.lib");
}


void IndexBuilder::dumpSkipList(const std::filesystem::path path) {

    spdlog::info("[IndexBuilder] Start to Write InvertedIndex into output.txt");
    uint64_t    cnt = 0; // 记录插入个数
    Json::Value singleFile;
    // 遍历InvertedIndex并将数据写入文件
    for (const auto &item : InvertedIndex) {
        SG::Core::SkipList<SG::Doc> skipList;
        Json::Value                 term;
        // 计算 IDF
        double idf = log10(docCnt / item.second.size());
        // 构建 SkipList
        for (const auto &DocInfo : item.second) {
            skipList.emplace_insert(DocInfo.first, DocInfo.second);
        }
        term["key"] = item.first;
        term["id"]  = cnt++;
        term["idf"] = idf;
        term["skl"] = skipList.dump();
        singleFile.append(term);
        if (cnt % 400 == 0) {
            auto dumpPath = path / (std::to_string(cnt / 400) + ".lib");
            spdlog::info("[IndexBuilder] Dump block {}-{} to {}", cnt - 400, cnt - 1, dumpPath.u8string());
            std::ofstream file(dumpPath);
            if (!file.is_open()) {
                spdlog::error("[IndexBuilder] Failed to dump block {}-{}", cnt - 400, cnt - 1);
                file.close();
                return;
            }
            file << singleFile.toStyledString();
            file.close();
            term = Json::Value{};
        }
    }
    if (cnt % 400 != 0) {
        auto dumpPath = path / (std::to_string(cnt / 400 + 1) + ".lib");
        spdlog::info("[IndexBuilder] Dump block {}-{} to {}", cnt / 400 * 400, cnt - 1, dumpPath.u8string());
        std::ofstream file(dumpPath);
        if (!file.is_open()) {
            spdlog::error("[IndexBuilder] Failed to dump block {}-{}", cnt / 400 * 400, cnt - 1);
            file.close();
            return;
        }
        file << singleFile.toStyledString();
        file.close();
    }

    spdlog::info("[IndexBuilder] Successfully Write InvertedIndex.");
}

void IndexBuilder::write_idf() {
    double        idf;
    std::ofstream file("../assets/library/IDF.lib");
    for (auto &item : InvertedIndex) {
        idf = log10(docCnt / item.second.size());
        file << item.first + " " + to_string(idf) + "\n";
        file.flush();
        idfs.insert({item.first, idf});
    }
    file.close();
}

void IndexBuilder::build() {
    load_offsets();
    traverse_und_divide();
    write_idf();
}

IndexBuilder::~IndexBuilder() {}
} // namespace SG