#include "upower-client.h"

UPowerClient::UPowerClient()
    : connection(sdbus::createSystemBusConnection())
      , proxy(sdbus::createProxy(*connection, sdbus::ServiceName(kUPowerService), sdbus::ObjectPath("/org/freedesktop/UPower")))
{
}

UPowerClient::~UPowerClient()
{
    proxy->unregister();
}

std::string UPowerClient::getDaemonVersion() const
{
    return proxy->getProperty("DaemonVersion").onInterface(kUPowerService).get<std::string>();
}

bool UPowerClient::isOnBattery() const
{
    return proxy->getProperty("OnBattery").onInterface(kUPowerService).get<bool>();
}

bool UPowerClient::isLidClosed() const
{
    return proxy->getProperty("LidIsClosed").onInterface(kUPowerService).get<bool>();
}

bool UPowerClient::isLidPresent() const
{
    return proxy->getProperty("LidIsPresent").onInterface(kUPowerService).get<bool>();
}

std::string UPowerClient::getCriticalAction() const
{
    std::string result;
    proxy->callMethod("GetCriticalAction").onInterface(kUPowerService).storeResultsTo(result);
    return result;
}

std::vector<sdbus::ObjectPath> UPowerClient::enumerateDevices() const
{
    std::vector<sdbus::ObjectPath> result;
    proxy->callMethod("EnumerateDevices").onInterface(kUPowerService).storeResultsTo(result);
    return result;
}

sdbus::ObjectPath UPowerClient::getDisplayDevice() const
{
    sdbus::ObjectPath displayDevice;
    proxy->callMethod("GetDisplayDevice").onInterface(kUPowerService).storeResultsTo(displayDevice);
    return displayDevice;
}

void UPowerClient::print_properties(const std::map<sdbus::MemberName, sdbus::Variant>& props)
{
    for (auto& [key, value] : props)
    {
        std::string type = value.peekValueType();
        std::cout << "  " << key << ": ";
        if (type == "u")
        {
            const auto v = value.get<std::uint32_t>();
            std::cout << "\t" << v << std::endl;
        }
        else if (type == "d")
        {
            const auto v = value.get<double>();
            std::cout << "\t" << v << std::endl;
        }
        else if (type == "o")
        {
            auto object_path = value.get<sdbus::ObjectPath>();
            std::cout << object_path << std::endl;
        }
        else if (type == "ao")
        {
            auto object_paths = value.get<std::vector<sdbus::ObjectPath>>();
            for (const auto& object_path : object_paths)
            {
                std::cout << object_path << std::endl;
            }
        }
        else if (type == "i")
        {
            const auto v = value.get<std::int32_t>();
            std::cout << "\t" << v << std::endl;
        }
        else if (type == "b")
        {
            const auto v = value.get<bool>();
            std::cout << (v ? "\tTrue" : "\tFalse") << std::endl;
        }
        else if (type == "s")
        {
            const auto v = value.get<std::string>();
            std::cout << (v.empty() ? "\t\"\"" : v) << std::endl;
        }
        else if (type == "x")
        {
            const auto v = value.get<std::int64_t>();
            std::cout << "\t" << v << std::endl;
        }
        else if (type == "t")
        {
            const auto v = value.get<std::uint64_t>();
            std::cout << "\t" << v << std::endl;
        }
        else if (type == "as")
        {
            std::cout << std::endl;
            const auto v = value.get<std::vector<std::string>>();
            for (const auto& s : v)
            {
                std::cout << "\t\t" << s << std::endl;
            }
            if (v.empty()) std::cout << "\t\t" << "\"\"" << std::endl;
        }
        else if (type == "au")
        {
            std::cout << std::endl;
            const auto v = value.get<std::vector<std::uint32_t>>();
            for (const auto& s : v)
            {
                std::cout << "\t\t" << s << std::endl;
            }
            if (v.empty()) std::cout << "\t\t" << "\"\"" << std::endl;
        }
        else if (type == "aau")
        {
            std::cout << std::endl;
            const auto v = value.get<std::vector<std::vector<std::uint32_t>>>();
            for (const auto& it : v)
            {
                for (const auto& address : it)
                {
                    std::cout << "\t\t" << address << std::endl;
                }
            }
            if (v.empty()) std::cout << "\t\t" << "\"\"" << std::endl;
        }
        else if (type == "aay")
        {
            std::cout << std::endl;
            const auto v = value.get<std::vector<std::vector<uint8_t>>>();
            for (const auto& it : v)
            {
                for (const auto& b : it)
                {
                    std::cout << "\t\t" << b << std::endl;
                }
            }
            if (v.empty()) std::cout << "\t\t" << "\"\"" << std::endl;
        }
        else if (type == "a{sv}")
        {
            std::cout << std::endl;
            for (auto arg_a_sv = value.get<std::vector<std::map<std::string, sdbus::Variant>>>(); const auto& it :
                 arg_a_sv)
            {
                for (const auto& [key, value] : it)
                {
                    std::cout << "\t\t" << key << ": ";
                    if (std::string t = value.peekValueType(); t == "s")
                    {
                        std::cout << value.get<std::string>() << std::endl;
                    }
                    else if (t == "u")
                    {
                        std::cout << value.get<std::uint32_t>() << std::endl;
                    }
                    else
                    {
                        std::cout << "Unknown type: " << t << std::endl;
                    }
                }
            }
        }
        else if (type == "a(ayuay)")
        {
            std::cout << std::endl;
            try
            {
                auto v = value.get<std::vector<sdbus::Struct<
                    std::vector<std::uint8_t>, std::uint32_t, std::vector<std::uint8_t>>>>();
                for (const auto& tuple : v)
                {
                    const auto& array1 = std::get<0>(tuple);
                    const auto& uint_value = std::get<1>(tuple);
                    const auto& array2 = std::get<2>(tuple);

                    std::cout << "\t\tArray1: ";
                    for (const auto& b : array1)
                    {
                        std::cout << static_cast<int>(b) << " ";
                    }
                    std::cout << std::endl;

                    std::cout << "\t\tUint32: " << uint_value << std::endl;

                    std::cout << "\t\tArray2: ";
                    for (const auto& b : array2)
                    {
                        std::cout << static_cast<int>(b) << " ";
                    }
                    std::cout << std::endl;
                }
            }
            catch (const sdbus::Error& e)
            {
                std::cerr << "Error: " << e.what() << std::endl;
                std::cerr << "Type of value: " << value.peekValueType() << std::endl;
            }
        }
        else if (type == "a(ayuayu)")
        {
            std::cout << std::endl;
            try
            {
                auto v = value.get<std::vector<sdbus::Struct<
                    std::vector<std::uint8_t>, std::uint32_t, std::vector<std::uint8_t>, std::uint32_t>>>();
                for (const auto& tuple : v)
                {
                    const auto& array1 = std::get<0>(tuple);
                    const auto& uint_value1 = std::get<1>(tuple);
                    const auto& array2 = std::get<2>(tuple);
                    const auto& uint_value2 = std::get<3>(tuple);

                    std::cout << "\t\tArray1: ";
                    for (const auto& b : array1)
                    {
                        std::cout << static_cast<int>(b) << " ";
                    }
                    std::cout << std::endl;

                    std::cout << "\t\tUint32_1: " << uint_value1 << std::endl;

                    std::cout << "\t\tArray2: ";
                    for (const auto& b : array2)
                    {
                        std::cout << static_cast<int>(b) << " ";
                    }
                    std::cout << std::endl;

                    std::cout << "\t\tUint32_2: " << uint_value2 << std::endl;
                }
            }
            catch (const sdbus::Error& e)
            {
                std::cerr << "Error: " << e.what() << std::endl;
                std::cerr << "Type of value: " << type << std::endl;
            }
        }
        else if (type == "aa{sv}")
        {
            std::cout << std::endl;
            for (auto arg_aa_sv = value.get<std::vector<std::map<std::string, sdbus::Variant>>>(); const auto&
                 arg_a_sv : arg_aa_sv)
            {
                for (const auto& [key, value] : arg_a_sv)
                {
                    std::cout << "\t\t" << key << ": ";
                    std::string t = value.peekValueType();
                    if (t == "s")
                    {
                        std::cout << value.get<std::string>() << std::endl;
                    }
                    else if (t == "u")
                    {
                        std::cout << value.get<std::uint32_t>() << std::endl;
                    }
                    else
                    {
                        std::cout << "Unknown type: " << t << std::endl;
                    }
                }
            }
        }
        else
        {
            std::cout << "Unknown type: " << type << std::endl;
        }
    }
}
