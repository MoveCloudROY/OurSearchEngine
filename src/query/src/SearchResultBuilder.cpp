#include "SearchResultBuilder.h"
#include <cstddef>
#include <json/value.h>

#include <memory>

namespace SG {

SearchResultBuilder::SearchResultBuilder() {
}

void SearchResultBuilder::addPartsInfo(const std::map<std::string, uint64_t> &parts) {
    partsInfo = parts;
}

void SearchResultBuilder::addItem(SearchResultItem &&item) {
    results.push_back(&item);
}

Json::Value SearchResultBuilder::build() {
    Json::Value ret;
    ret["数量"] = Json::Value{};
    for (auto &&i : partsInfo) {
        ret["分词"].append(i.first);
        ret["数量"][i.first] = Json::Value(static_cast<unsigned int>(i.second));
    }
    for (auto &&i : results) {
        Json::Value item;
        Json::Value itemInfo;
        itemInfo["分数"]         = i->score;
        itemInfo["网址"]         = i->url;
        itemInfo["相同域名个数"] = 0;
        itemInfo["原因"]         = Json::objectValue;

        itemInfo["信息"]["标题"] = i->info.title;
        itemInfo["信息"]["描述"] = i->info.desc;
        itemInfo["信息"]["文本"] = i->info.text;
        for (auto &&j : i->correlation) {
            itemInfo["相关性"][j.first] = j.second;
        }
        ret["结果"].append(itemInfo);
    }
    return ret;
}


} // namespace SG

/*
{
  "分词": [
    "test"
  ],
  "数量": {
    "test": 30000
  },
  "结果": [
    {
      "分数": 0.5305737980762121,
      "原因": {
        "内容与关键词相关": 0.047607421875,
        "反向链接加成": 14.297143555969406,
        "域名的语种": 0.9743878739871981,
        "对域名的预调整": 0.8
      },
      "网址": "https://www.youtube.com/",
      "信息": {
        "标题": "YouTube",
        "描述": "YouTube 上盡享你喜愛的影片和音樂、上載原創內容，並與親友和世界各地的人分享。",
        "文本": ""
      },
      "相关性": {
        "test": 0.047607421875
      },
      "相同域名个数": 150
    },
*/