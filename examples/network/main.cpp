#include "network-client.h"
#include <iostream>

std::vector<std::string> dbusSystemListNames(const std::unique_ptr<sdbus::IConnection>& connection)
{
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


class ObjectManager : public sdbus::ProxyInterfaces<sdbus::ObjectManager_proxy>
{
public:
    ObjectManager(sdbus::IConnection& connection, const sdbus::ServiceName& destination, sdbus::ObjectPath path)
        : ProxyInterfaces(connection, destination, std::move(path))
    {
        registerProxy();
    }

    ~ObjectManager()
    {
        unregisterProxy();
    }

    void handleExistingObjects()
    {
        for (const auto& [object, interfacesAndProperties] : GetManagedObjects())
        {
            onInterfacesAdded(object, interfacesAndProperties);
        }
    }

private:
    void onInterfacesAdded(const sdbus::ObjectPath& objectPath
                           , const std::map<sdbus::InterfaceName, std::map<sdbus::PropertyName, sdbus::Variant>>&
                           interfacesAndProperties) override
    {
        for (const auto& [interface, properties] : interfacesAndProperties)
        {
            std::cout << "added: " << interface << std::endl;
            NetworkClient::print_properties(properties);
        }
    }

    void onInterfacesRemoved(const sdbus::ObjectPath& objectPath
                             , const std::vector<sdbus::InterfaceName>& interfaces) override
    {
        for (const auto& interface : interfaces)
        {
            std::cout << "removed: " << interface << std::endl;
        }
    }
};

int main()
{
    const auto connection = sdbus::createSystemBusConnection();

    // check if service is available on the bus
    if (auto interface_names = dbusSystemListNames(connection); !isServicePresent(
        interface_names, kNetworkManagerService))
    {
        std::cout << kNetworkManagerService << " is not available on the bus." << std::endl;
        return 0;
    }

    // get existing objects
    const auto objManager = std::make_unique<ObjectManager>(
        *connection, sdbus::ServiceName("org.freedesktop.NetworkManager"), sdbus::ObjectPath("/org/freedesktop"));
    try
    {
        objManager->handleExistingObjects();
    }
    catch (const sdbus::Error& e)
    {
        if (e.getName() == "org.freedesktop.DBus.Error.ServiceUnknown")
        {
            std::cout << "Waiting for server to start ..." << std::endl;
        }
    }

    NetworkClient client(*connection);

    auto connections = client.getActiveConnections();
    std::cout << "Active Connections: " << connections.size() << std::endl;
    for (const auto& objectPath : connections)
    {
        std::cout << "Connection: " << objectPath << std::endl;;
        auto settings = client.get_all_properties<std::map<sdbus::MemberName, sdbus::Variant>>(
            objectPath, kNetworkManagerService,
            "org.freedesktop.NetworkManager.Connection.Active");
        NetworkClient::print_properties(settings);

        std::cout << "\tDefault6: " << (client.get_property<bool>(objectPath, kConnectionActive, "Default6")
                                            ? "True"
                                            : "False") <<
            std::endl;
        std::cout << "\tDefault: " << (client.get_property<bool>(objectPath, kConnectionActive, "Default")
                                           ? "True"
                                           : "False") <<
            std::endl;
        std::cout << "\tVpn: " << (client.get_property<bool>(objectPath, kConnectionActive, "Vpn")
                                       ? "True"
                                       : "False")
            <<
            std::endl;
        auto active_connection_path = client.get_property<sdbus::ObjectPath>(
            objectPath, kConnectionActive, "Connection");
        std::cout << "\tConnection: " << active_connection_path <<
            std::endl;

        auto connection_settings = client.get_all_properties<std::map<sdbus::MemberName, sdbus::Variant>>(
            active_connection_path, kNetworkManagerService,
            NetworkClient::objectToInterfaceName(active_connection_path));
        NetworkClient::print_properties(connection_settings);

        auto dhcp4_config = client.get_property<sdbus::ObjectPath>(objectPath, kConnectionActive, "Dhcp4Config");
        std::cout << "\tDhcp4Config: " << dhcp4_config << std::endl;
        if (dhcp4_config != "/")
        {
            auto config_props = client.get_all_properties<std::map<sdbus::MemberName, sdbus::Variant>>(
                dhcp4_config, kNetworkManagerService, kDhcp4Config);
            if (config_props.contains(sdbus::MemberName("Options")))
            {
                const auto props = config_props[sdbus::MemberName("Options")].get<std::map<sdbus::MemberName,
                    sdbus::Variant>>();
                NetworkClient::print_properties(props);
            }
        }

        auto dhcp6_config = client.get_property<sdbus::ObjectPath>(objectPath, kConnectionActive, "Dhcp6Config");
        std::cout << "\tDhcp6Config: " << dhcp6_config << std::endl;
        if (dhcp6_config != "/")
        {
            auto config_props = client.get_all_properties<std::map<sdbus::MemberName, sdbus::Variant>>(
                dhcp6_config, kNetworkManagerService, kDhcp6Config);
            if (config_props.contains(sdbus::MemberName("Options")))
            {
                const auto props = config_props[sdbus::MemberName("Options")].get<std::map<sdbus::MemberName,
                    sdbus::Variant>>();
                NetworkClient::print_properties(props);
            }
        }

        auto ip4_config = client.get_property<sdbus::ObjectPath>(objectPath, kConnectionActive, "Ip4Config");
        std::cout << "\tIp4Config: " << ip4_config << std::endl;
        if (ip4_config != "/")
        {
            auto config_props = client.get_all_properties<std::map<sdbus::MemberName, sdbus::Variant>>(
                ip4_config, kNetworkManagerService, kIp4Config);
            NetworkClient::print_properties(config_props);
        }

        auto ip6_config = client.get_property<sdbus::ObjectPath>(objectPath, kConnectionActive, "Ip6Config");
        std::cout << "\tIp6Config: " << ip6_config << std::endl;
        if (ip6_config != "/")
        {
            auto config_props = client.get_all_properties<std::map<sdbus::MemberName, sdbus::Variant>>(
                ip6_config, kNetworkManagerService, kIp6Config);
            NetworkClient::print_properties(config_props);
        }

        std::cout << "\tMaster: " << client.get_property<sdbus::ObjectPath>(
                objectPath, kConnectionActive, "Master") <<
            std::endl;

        auto specific_object = client.get_property<sdbus::ObjectPath>(
            objectPath, kConnectionActive, "SpecificObject");
        std::cout << "\tSpecificObject: " << specific_object <<
            std::endl;
        if (specific_object != "/")
        {
            auto config_props = client.get_all_properties<std::map<sdbus::MemberName, sdbus::Variant>>(
                specific_object, kNetworkManagerService, kAccessPoint);
            NetworkClient::print_properties(config_props);
        }

        std::cout << "\tId: " << client.get_property<std::string>(objectPath, kConnectionActive, "Id") <<
            std::endl;
        std::cout << "\tType: " << client.get_property<std::string>(objectPath, kConnectionActive, "Type") <<
            std::endl;
        std::cout << "\tUUID: " << client.get_property<std::string>(objectPath, kConnectionActive, "Uuid") <<
            std::endl;
        std::cout << "\tState: " << client.get_property<std::uint32_t>(objectPath, kConnectionActive, "State")
            <<
            std::endl;
        std::cout << "\tStateFlags: " << client.get_property<std::uint32_t>(
                objectPath, kConnectionActive, "StateFlags")
            <<
            std::endl;
    }

    return 0;
}
