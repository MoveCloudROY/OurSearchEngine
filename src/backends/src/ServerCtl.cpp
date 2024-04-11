#include "ServerCtl.h"

#include <spdlog/spdlog.h>


namespace SG {
namespace Server {

ServerCtl::ServerCtl()
    : queryer(Queryer::getInstance()) {
    spdlog::info("Create the ServerCtl Model");
}


void ServerCtl::search(const drogon::HttpRequestPtr &req, ResponseCallback &&callback, const std::string &content, const std::string &slice) const {
    spdlog::info("Get the Search Content {}", content);
    // spdlog::info("Get the Answer in rank: {}", slice);
    spdlog::info("Get the Answer in rank: {}: {}", Utils::toUInt64(slice.substr(0, slice.find(':'))), Utils::toUInt64(slice.substr(slice.find(':') + 1)));

    auto ret = queryer.get(content, Utils::toUInt64(slice.substr(0, slice.find(':'))), Utils::toUInt64(slice.substr(slice.find(':') + 1)));

    Json::StreamWriterBuilder builder;
    builder.settings_["emitUTF8"] = true;
    // spdlog::info("\n{}", Json::writeString(builder, ret));
    auto resp                     = drogon::HttpResponse::newHttpJsonResponse(ret);


    // std::ifstream           inputFile("../assets/test.json");
    // Json::CharReaderBuilder readerBuilder;
    // Json::Value             singleFile;
    // std::string             errs;
    // Json::parseFromStream(readerBuilder, inputFile, &singleFile, &errs);
    // spdlog::info("\n{}", Json::writeString(builder, singleFile));
    // auto resp = drogon::HttpResponse::newHttpJsonResponse(singleFile);

    resp->addHeader("Access-Control-Allow-Origin", "http://localhost:3333");
    resp->addHeader("Access-Control-Allow-Methods", "GET");
    resp->addHeader("Access-Control-Allow-Headers", "Content-Type");
    callback(resp);
}


} // namespace Server
} // namespace SG
