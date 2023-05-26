#pragma once

#include <json/value.h>
#include <json/writer.h>
#include <sstream>
#include <string>
#include <vector>

namespace SG {

using PartsInfo = std::map<std::string, uint64_t>;

struct DivideResult {
    PartsInfo words; //<word,frequency>
    uint64_t  totalFreq;
    std::string lastWord;
};

struct SearchReq {
    std::string content;
    uint64_t    begin;
    uint64_t    end;
};

struct ItemInfo {
    std::string title;
    std::string desc;
    std::string text;
};


struct SearchResultItem {
    double                        score;
    std::string                   url ;
    ItemInfo                      info;
    std::map<std::string, double> correlation;
    uint64_t                      same_domain_numbers;
    
    // SearchResultItem () = default;
    // SearchResultItem (SearchResultItem&& t)
    //     :score(t.score),
    //      url(std::move(t.url)),
    //      info(std::move(t.info)),
    //      correlation(std::move(t.correlation)),
    //      same_domain_numbers(t.same_domain_numbers) {
    // }
};

struct SearchResult {
    std::vector<std::string>        participles;
    std::vector<uint64_t>           numbers;
    std::vector<SearchResultItem *> results;
};


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


namespace Utils {
static inline auto toUInt64(const std::string &s) -> uint64_t {
    std::stringstream ss{s};
    uint64_t          ret;
    ss >> ret;
    return ret;
};

static inline std::string JsonToString(const Json::Value &root) {
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