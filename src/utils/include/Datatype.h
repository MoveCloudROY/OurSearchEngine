#pragma once

#include <json/value.h>
#include <json/writer.h>
#include <sstream>
#include <string>
#include <vector>

namespace SG {

using SSIZE_T   = unsigned long long;
using PartsInfo = std::map<std::string, SSIZE_T>;

struct SearchReq {
    std::string content;
    SSIZE_T     begin;
    SSIZE_T     end;
};

struct ItemInfo {
    std::string title;
    std::string desc;
    std::string text;
};


struct SearchResultItem {
    double                        score;
    std::string                   url;
    ItemInfo                      info;
    std::map<std::string, double> correlation;
    SSIZE_T                       same_domain_numbers;
};

struct SearchResult {
    std::vector<std::string>       participles;
    std::vector<SSIZE_T>           numbers;
    std::vector<SearchResultItem*> results;
};

namespace Utils {
static inline auto toUInt64(const std::string& s) -> SSIZE_T {
    std::stringstream ss{s};
    SSIZE_T           ret;
    ss >> ret;
    return ret;
};

static inline std::string JsonToString(const Json::Value& root) {
    std::ostringstream        stream;
    Json::StreamWriterBuilder stream_builder;
    stream_builder.settings_["emitUTF8"] = true; // !
    std::unique_ptr<Json::StreamWriter> writer(stream_builder.newStreamWriter());
    writer->write(root, &stream);
    return stream.str();
}
} // namespace Utils


} // namespace SG


/*

export interface SearchResult {
  分数: any
  网址: string
  信息: {
    标题: string
    描述: string
    文本: string
  }
  原因: Record<string, number>
  相同域名个数: number
}

export interface SearchData {
  分词: string[]
  结果: SearchResult[]
  总数: number
}

*/