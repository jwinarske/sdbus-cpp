#ifndef FLATPAK_CLIENT_H
#define FLATPAK_CLIENT_H

#include <sdbus-c++/sdbus-c++.h>
#include <string>
#include <vector>
#include <memory>

class FlatpakClient {
public:
    FlatpakClient();
    ~FlatpakClient();

    [[nodiscard]] std::string getVersion() const;
    [[nodiscard]] std::vector<std::string> listInstalledApps() const;

private:
    std::unique_ptr<sdbus::IConnection> connection;
    std::unique_ptr<sdbus::IProxy> proxy;
};

#endif // FLATPAK_CLIENT_H