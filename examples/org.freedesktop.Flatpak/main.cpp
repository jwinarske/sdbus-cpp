#include "flatpak-client.h"
#include <iostream>

int main() {
    FlatpakClient client;

    std::cout << "Flatpak Version: " << client.getVersion() << std::endl;

    std::cout << "Installed Apps:" << std::endl;
    for (const auto &app : client.listInstalledApps()) {
        std::cout << "  " << app << std::endl;
    }

    return 0;
}