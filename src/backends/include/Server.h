#pragma once

#include <string>
#include "ServerCtl.h"
#include <drogon/drogon.h>

namespace SG {
namespace Server {

class Server {
public:
    Server(const std::string& configFile);
    void run();
};

} // namespace Server
} // namespace SG
