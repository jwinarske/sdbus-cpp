#ifndef UPOWER_CLIENT_H
#define UPOWER_CLIENT_H

#include <sdbus-c++/sdbus-c++.h>
#include <string>
#include <vector>
#include <memory>

#include <iostream>

static constexpr char kUPowerService[] = "org.freedesktop.UPower";
static constexpr char kUPowerDevice[] = "org.freedesktop.UPower.Device";


class UPowerClient
{
public:
    UPowerClient();

    ~UPowerClient();

    [[nodiscard]] std::string getDaemonVersion() const;

    [[nodiscard]] bool isOnBattery() const;

    [[nodiscard]] bool isLidClosed() const;

    [[nodiscard]] bool isLidPresent() const;

    [[nodiscard]] std::string getCriticalAction() const;

    [[nodiscard]] std::vector<sdbus::ObjectPath> enumerateDevices() const;

    [[nodiscard]] sdbus::ObjectPath getDisplayDevice() const;

    [[nodiscard]] std::vector<std::string, sdbus::Variant> getDisplayDeviceProperties(
        const sdbus::ObjectPath& displayDevice) const;

    template <typename ValueType>
    ValueType get_all_properties(const sdbus::ObjectPath& devicePath,
                                 const std::string& bus_name,
                                 const std::string& interface_name) const
    {
        const auto activeConnectionProxy = sdbus::createProxy(*connection,
                                                              sdbus::ServiceName(bus_name),
                                                              devicePath);
        ValueType state;
        activeConnectionProxy->callMethod("GetAll").onInterface("org.freedesktop.DBus.Properties")
                             .withArguments(interface_name)
                             .storeResultsTo(state);
        return state;
    }

    static void print_properties(const std::map<sdbus::MemberName, sdbus::Variant>& props);

private:
    std::unique_ptr<sdbus::IConnection> connection;
    std::unique_ptr<sdbus::IProxy> proxy;
};

#endif // UPOWER_CLIENT_H
