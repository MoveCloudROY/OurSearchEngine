#include "Queryer.h"
#include <spdlog/spdlog.h>
using namespace SG;
int main() {
    Queryer &queryer = Queryer::getInstance();
    auto     ans     = queryer.get("使用寿命", 0, 10);
    auto     ans2    = queryer.get("使用寿命", 0, 10);

    Json::StreamWriterBuilder builder;
    builder.settings_["emitUTF8"] = true;
    spdlog::info("\n{}", Json::writeString(builder, ans));
    spdlog::info("\n{}", Json::writeString(builder, ans2));
}