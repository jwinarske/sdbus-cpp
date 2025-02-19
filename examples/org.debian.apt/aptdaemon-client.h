#ifndef APTDAEMON_CLIENT_H
#define APTDAEMON_CLIENT_H

#include <sdbus-c++/sdbus-c++.h>
#include <string>
#include <vector>
#include <memory>

class AptDaemonClient {
public:
    AptDaemonClient();

    ~AptDaemonClient();

    [[nodiscard]] std::vector<std::string> getTrustedVendorKeys() const;

private:
    std::unique_ptr<sdbus::IConnection> connection;
    std::unique_ptr<sdbus::IProxy> proxy;
};

#endif // APTDAEMON_CLIENT_H
