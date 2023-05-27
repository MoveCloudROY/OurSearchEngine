#pragma once

#include <functional>

#include <drogon/HttpController.h>
#include <drogon/utils/FunctionTraits.h>
#include <drogon/drogon.h>

#include <Queryer.h>

namespace SG {
namespace Server {
using ResponseCallback = std::function<void(const drogon::HttpResponsePtr &)>;

class ServerCtl : public drogon::HttpController<ServerCtl> {
public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(ServerCtl::search, "/search?q={1:content}&slice={2:slice}", drogon::Get);
    METHOD_LIST_END

    void search(const drogon::HttpRequestPtr &req, ResponseCallback &&callback, const std::string &content, const std::string &slice) const;

public:
    ServerCtl();

private:
    Queryer &queryer;
};


} // namespace Server
} // namespace SG
