#include "ServerCtl.h"

#include <spdlog/spdlog.h>

namespace SG {
namespace Server {

ServerCtl::ServerCtl() {
    spdlog::info("Create the ServerCtl Model");
}



void ServerCtl::search(const drogon::HttpRequestPtr& req, ResponseCallback&& callback, const std::string& content, const std::string& slice) const {
    spdlog::info("Get the Search Content {}", content);
    spdlog::info("Get the Answer in rank: {}", slice);

    // SearchReq searchReq{
    //     content,
    //     toUInt64(slice.substr(0, slice.find(':'))),
    //     toUInt64(slice.substr(slice.find(':') + 1))
    // };

    Json::Value ret;

    auto resp = drogon::HttpResponse::newHttpJsonResponse(ret);
    callback(resp);
}



} // namespace Server
} // namespace SG
