#include "flatpak-client.h"

FlatpakClient::FlatpakClient()
    : connection(sdbus::createSystemBusConnection())
    , proxy(sdbus::createProxy(*connection, sdbus::ServiceName("org.freedesktop.Flatpak"), sdbus::ObjectPath("/org/freedesktop/Flatpak"))) {
}

FlatpakClient::~FlatpakClient() {
    proxy->unregister();
}

std::string FlatpakClient::getVersion() const {
    return proxy->getProperty("version").onInterface("org.freedesktop.Flatpak").get<std::string>();
}

std::vector<std::string> FlatpakClient::listInstalledApps() const {
    std::vector<std::string> result;
    proxy->callMethod("ListInstalledApps").onInterface("org.freedesktop.Flatpak").storeResultsTo(result);
    return result;
}