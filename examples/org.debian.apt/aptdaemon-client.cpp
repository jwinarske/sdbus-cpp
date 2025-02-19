#include "aptdaemon-client.h"
#include <iostream>

AptDaemonClient::AptDaemonClient()
    : connection(sdbus::createSystemBusConnection()),
      proxy(createProxy(*sdbus::createSystemBusConnection(), sdbus::ServiceName("org.debian.apt"), sdbus::ObjectPath("/org/debian/apt"))) {
}

AptDaemonClient::~AptDaemonClient() {
    proxy->unregister();
}

std::vector<std::string> AptDaemonClient::getTrustedVendorKeys() const {
    std::vector<std::string> result;
    try {
        proxy->callMethod("GetTrustedVendorKeys").onInterface("org.debian.apt").storeResultsTo(result);
    } catch (const sdbus::Error &e) {
        std::cerr << "Error listing Trusted Vendor Keys: " << e.what() << std::endl;
    }
    return result;
}
