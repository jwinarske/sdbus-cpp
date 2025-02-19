#include "upower-client.h"
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
    if (const auto interface_names = dbusSystemListNames(); !isServicePresent(interface_names, kUPowerService))
    {
        std::cout << kUPowerService << " is not available on the bus." << std::endl;
        return 0;
    }

    const UPowerClient client;

    std::cout << "Daemon Version: " << client.getDaemonVersion() << std::endl;
    std::cout << "On Battery: " << (client.isOnBattery() ? "Yes" : "No") << std::endl;
    std::cout << "Lid Is Closed: " << (client.isLidClosed() ? "Yes" : "No") << std::endl;
    std::cout << "Lid Is Present: " << (client.isLidPresent() ? "Yes" : "No") << std::endl;
    std::cout << "Critical Action: " << client.getCriticalAction() << std::endl;

    std::cout << "Devices:" << std::endl;
    for (const auto& device : client.enumerateDevices())
    {
        std::cout << "  " << device << std::endl;
    }

    const auto& display_device = client.getDisplayDevice();
    std::cout << "Display Device: " << display_device << std::endl;

    const auto& display_properties = client.get_all_properties<std::map<sdbus::MemberName, sdbus::Variant>>(
        display_device, kUPowerService, kUPowerDevice);
    UPowerClient::print_properties(display_properties);

    return 0;
}
