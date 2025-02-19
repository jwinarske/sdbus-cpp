#include "network-client.h"
#include <iostream>

std::vector<std::string> dbusSystemListNames()
{
    const auto connection = sdbus::createSystemBusConnection();

    const auto proxy = sdbus::createProxy(*connection, sdbus::ServiceName("org.freedesktop.DBus"),
                                          sdbus::ObjectPath("/org/freedesktop/DBus"));

    std::vector<std::string> dbus_interfaces;
    proxy->callMethod("ListNames").onInterface("org.freedesktop.DBus").storeResultsTo(dbus_interfaces);

    return std::move(dbus_interfaces);
}

bool isServicePresent(const std::vector<std::string>& dbus_interfaces, const std::string_view& service)
{
    return std::ranges::find(dbus_interfaces, service) != dbus_interfaces.end();
}

int main()
{
    if (auto interface_names = dbusSystemListNames(); !isServicePresent(interface_names, kNetworkManagerService))
    {
        std::cout << kNetworkManagerService << " is not available on the bus." << std::endl;
        return 0;
    }

    using PropertyName = sdbus::MemberName;

    NetworkClient client;

    auto connections = client.getActiveConnections();
    std::cout << "Active Connections: " << connections.size() << std::endl;
    for (const auto& connection : connections)
    {
        std::cout << "Connection: " << connection << std::endl;;
        auto settings = client.get_all_properties<std::map<sdbus::MemberName, sdbus::Variant>>(
            connection, kNetworkManagerService,
            "org.freedesktop.NetworkManager.Connection.Active");
        client.print_properties(settings);

        std::cout << "\tDefault6: " << (client.get_property<bool>(connection, kConnectionActive, "Default6")
                                            ? "True"
                                            : "False") <<
            std::endl;
        std::cout << "\tDefault: " << (client.get_property<bool>(connection, kConnectionActive, "Default")
                                           ? "True"
                                           : "False") <<
            std::endl;
        std::cout << "\tVpn: " << (client.get_property<bool>(connection, kConnectionActive, "Vpn")
                                       ? "True"
                                       : "False")
            <<
            std::endl;
        auto active_connection_path = client.get_property<sdbus::ObjectPath>(
            connection, kConnectionActive, "Connection");
        std::cout << "\tConnection: " << active_connection_path <<
            std::endl;

        auto connection_settings = client.get_all_properties<std::map<sdbus::MemberName, sdbus::Variant>>(
            active_connection_path, kNetworkManagerService,
            NetworkClient::objectToInterfaceName(active_connection_path));
        client.print_properties(connection_settings);

        auto dhcp4_config = client.get_property<sdbus::ObjectPath>(connection, kConnectionActive, "Dhcp4Config");
        std::cout << "\tDhcp4Config: " << dhcp4_config << std::endl;
        if (dhcp4_config != "/")
        {
            auto config_props = client.get_all_properties<std::map<PropertyName, sdbus::Variant>>(
                dhcp4_config, kNetworkManagerService, kDhcp4Config);
            if (config_props.contains(PropertyName("Options")))
            {
                const auto props = config_props[sdbus::MemberName("Options")].get<std::map<sdbus::MemberName,
                    sdbus::Variant>>();
                client.print_properties(props);
            }
        }

        auto dhcp6_config = client.get_property<sdbus::ObjectPath>(connection, kConnectionActive, "Dhcp6Config");
        std::cout << "\tDhcp6Config: " << dhcp6_config << std::endl;
        if (dhcp6_config != "/")
        {
            auto config_props = client.get_all_properties<std::map<PropertyName, sdbus::Variant>>(
                dhcp6_config, kNetworkManagerService, kDhcp6Config);
            if (config_props.contains(PropertyName("Options")))
            {
                const auto props = config_props[sdbus::MemberName("Options")].get<std::map<sdbus::MemberName,
                    sdbus::Variant>>();
                client.print_properties(props);
            }
        }

        auto ip4_config = client.get_property<sdbus::ObjectPath>(connection, kConnectionActive, "Ip4Config");
        std::cout << "\tIp4Config: " << ip4_config << std::endl;
        if (ip4_config != "/")
        {
            auto config_props = client.get_all_properties<std::map<PropertyName, sdbus::Variant>>(
                ip4_config, kNetworkManagerService, kIp4Config);
            client.print_properties(config_props);
        }

        auto ip6_config = client.get_property<sdbus::ObjectPath>(connection, kConnectionActive, "Ip6Config");
        std::cout << "\tIp6Config: " << ip6_config << std::endl;
        if (ip6_config != "/")
        {
            auto config_props = client.get_all_properties<std::map<PropertyName, sdbus::Variant>>(
                ip6_config, kNetworkManagerService, kIp6Config);
            client.print_properties(config_props);
        }

        std::cout << "\tMaster: " << client.get_property<sdbus::ObjectPath>(
                connection, kConnectionActive, "Master") <<
            std::endl;

        auto specific_object = client.get_property<sdbus::ObjectPath>(
            connection, kConnectionActive, "SpecificObject");
        std::cout << "\tSpecificObject: " << specific_object <<
            std::endl;
        if (specific_object != "/")
        {
            auto config_props = client.get_all_properties<std::map<PropertyName, sdbus::Variant>>(
                specific_object, kNetworkManagerService, kAccessPoint);
            client.print_properties(config_props);
        }

        std::cout << "\tId: " << client.get_property<std::string>(connection, kConnectionActive, "Id") <<
            std::endl;
        std::cout << "\tType: " << client.get_property<std::string>(connection, kConnectionActive, "Type") <<
            std::endl;
        std::cout << "\tUUID: " << client.get_property<std::string>(connection, kConnectionActive, "Uuid") <<
            std::endl;
        std::cout << "\tState: " << client.get_property<std::uint32_t>(connection, kConnectionActive, "State")
            <<
            std::endl;
        std::cout << "\tStateFlags: " << client.get_property<std::uint32_t>(
                connection, kConnectionActive, "StateFlags")
            <<
            std::endl;
    }

    return 0;
}
