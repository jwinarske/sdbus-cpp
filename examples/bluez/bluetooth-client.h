#ifndef BLUETOOTH_CLIENT_H
#define BLUETOOTH_CLIENT_H

#include <sdbus-c++/sdbus-c++.h>
#include <string>
#include <vector>
#include <memory>

class BluetoothClient {
public:
    BluetoothClient();

    ~BluetoothClient();

    [[nodiscard]] std::vector<std::string> listControllers() const;

    void startDiscovery(const std::string &adapterPath) const;

    void stopDiscovery(const std::string &adapterPath) const;

    [[nodiscard]] std::vector<std::string> listServices(const std::string &devicePath) const;

    [[nodiscard]] static std::string getServiceName(const std::string &uuid) ;

    void monitorProperties(const std::string &devicePath) const;

private:
    void onInterfacesAdded(sdbus::Message &message) const;

    std::unique_ptr<sdbus::IConnection> connection;
    std::unique_ptr<sdbus::IProxy> proxy;
};

#endif // BLUETOOTH_CLIENT_H
