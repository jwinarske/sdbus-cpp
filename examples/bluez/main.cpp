#include "bluetooth-client.h"
#include <iostream>
#include <chrono>
#include <thread>

int main() {
    const BluetoothClient client;

    std::cout << "Bluetooth Controllers:" << std::endl;
    auto controllers = client.listControllers();
    for (const auto &controller : controllers) {
        std::cout << "  " << controller << std::endl;
    }

    std::cout << "Available Services:" << std::endl;
    for (auto services = client.listServices(controllers[0]); const auto &service : services) {
        std::cout << "  " << BluetoothClient::getServiceName(service) << std::endl;
    }

    if (!controllers.empty()) {
        client.monitorProperties(controllers[0]);
        client.startDiscovery(controllers[0]);
        std::this_thread::sleep_for(std::chrono::seconds(20));
        client.stopDiscovery(controllers[0]);
    }

    return 0;
}