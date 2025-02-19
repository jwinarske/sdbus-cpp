#ifndef NETWORK_CLIENT_H
#define NETWORK_CLIENT_H

#include <sdbus-c++/sdbus-c++.h>
#include <string>
#include <memory>
#include <iostream>

static constexpr char kNetworkManagerService[] = "org.freedesktop.NetworkManager";
static constexpr char kConnectionActive[] = "org.freedesktop.NetworkManager.Connection.Active";
static constexpr char kSettingsConnection[] = "org.freedesktop.NetworkManager.Settings.Connection";
static constexpr char kDhcp4Config[] = "org.freedesktop.NetworkManager.DHCP4Config";
static constexpr char kDhcp6Config[] = "org.freedesktop.NetworkManager.DHCP6Config";
static constexpr char kIp4Config[] = "org.freedesktop.NetworkManager.IP4Config";
static constexpr char kIp6Config[] = "org.freedesktop.NetworkManager.IP6Config";
static constexpr char kAccessPoint[] = "org.freedesktop.NetworkManager.AccessPoint";

class NetworkClient
{
public:
    NetworkClient();

    ~NetworkClient();

    [[nodiscard]] std::vector<sdbus::ObjectPath> getActiveConnections() const;

    [[nodiscard]] std::vector<sdbus::ObjectPath> getDevices(const sdbus::ObjectPath& activeConnectionPath) const;

    static std::string objectToInterfaceName(const std::string& input)
    {
        if (input == "/")
        {
            return "";
        }
        std::string result = input;
        // Remove the leading '/'
        result.erase(0, 1);
        // Remove the trailing part after the last '/'
        result = result.substr(0, result.find_last_of('/'));
        // Replace '/' with '.'
        std::ranges::replace(result, '/', '.');

        if (result == "org.freedesktop.NetworkManager.ActiveConnection")
        {
            return "org.freedesktop.NetworkManager.Connection.Active";
        }
        if (result == "org.freedesktop.NetworkManager.Settings")
        {
            return "org.freedesktop.NetworkManager.Settings.Connection";
        }
        if (result == "org.freedesktop.NetworkManager.Devices")
        {
            return "org.freedesktop.NetworkManager.Device";
        }
        return result;
    }

    void print_properties(const std::map<sdbus::MemberName, sdbus::Variant>& props);

    template <typename ValueType>
    ValueType get_property(const sdbus::ObjectPath& activeConnectionPath, const std::string& interface_name,
                           const std::string& property) const
    {
        const auto activeConnectionProxy = sdbus::createProxy(*connection,
                                                              sdbus::ServiceName("org.freedesktop.NetworkManager"),
                                                              activeConnectionPath);
        sdbus::Variant stateVariant;
        activeConnectionProxy->callMethod("Get").onInterface("org.freedesktop.DBus.Properties")
                             .withArguments(interface_name, property)
                             .storeResultsTo(stateVariant);
        const auto state = stateVariant.get<ValueType>();
        return state;
    }

    template <typename ValueType>
    [[nodiscard]] ValueType get_all_properties(const sdbus::ObjectPath& activeConnectionPath,
                                               const std::string& bus_name,
                                               const std::string& interface_name) const
    {
        const auto activeConnectionProxy = sdbus::createProxy(*connection,
                                                              sdbus::ServiceName(bus_name),
                                                              activeConnectionPath);
        ValueType state;
        activeConnectionProxy->callMethod("GetAll").onInterface("org.freedesktop.DBus.Properties")
                             .withArguments(interface_name)
                             .storeResultsTo(state);
        return state;
    }

private:
    std::unique_ptr<sdbus::IConnection> connection;
    std::unique_ptr<sdbus::IProxy> proxy;
};

#endif // NETWORK_CLIENT_H
