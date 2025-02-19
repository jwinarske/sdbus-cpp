#include "aptdaemon-client.h"
#include <iostream>

int main() {
    const AptDaemonClient client;

    std::cout << "Trusted Vendor Keys:" << std::endl;
    for (const auto &pkg : client.getTrustedVendorKeys()) {
        std::cout << "  " << pkg << std::endl;
    }

    return 0;
}