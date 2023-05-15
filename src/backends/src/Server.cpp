#include "Server.h"
#include <spdlog/spdlog.h>


namespace SG {
namespace Server {

Server::Server(const std::string& configFile) {
    spdlog::set_level(spdlog::level::trace);
    spdlog::info("Create the Server from the config path '{}' ", configFile);
    drogon::app().loadConfigFile(configFile);
}

void Server::run() {
    spdlog::info("Start the Server");
    drogon::app().run();
}


} // namespace Server


} // namespace SG