#include <Server.h>

int main() {
    SG::Server::Server app("../assets/config.json");
    app.run();
}