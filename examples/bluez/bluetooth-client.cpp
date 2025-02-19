#include "bluetooth-client.h"
#include <iostream>

BluetoothClient::BluetoothClient()
    : connection(sdbus::createSystemBusConnection())
      , proxy(sdbus::createProxy(*connection, sdbus::ServiceName("org.bluez"), sdbus::ObjectPath("/"))) {
    connection->enterEventLoopAsync();
}

BluetoothClient::~BluetoothClient() {
    proxy->unregister();
}

std::vector<std::string> BluetoothClient::listControllers() const {
    std::vector<std::string> controllers;
    try {
        std::map<sdbus::ObjectPath, std::map<std::string, std::map<std::string, sdbus::Variant> > > managedObjects;
        proxy->callMethod("GetManagedObjects").onInterface("org.freedesktop.DBus.ObjectManager")
                .storeResultsTo(managedObjects);

        for (const auto &[path, interfaces]: managedObjects) {
            if (interfaces.contains("org.bluez.Adapter1")) {
                controllers.push_back(path);
            }
        }
    } catch (const sdbus::Error &e) {
        std::cerr << "Error listing controllers: " << e.what() << std::endl;
    }
    return controllers;
}

void BluetoothClient::startDiscovery(const std::string &adapterPath) const {
    try {
        const auto adapterProxy = sdbus::createProxy(*connection, sdbus::ServiceName("org.bluez"),
                                                     sdbus::ObjectPath(adapterPath));
        adapterProxy->callMethod("StartDiscovery").onInterface("org.bluez.Adapter1");

        connection->addMatch("type='signal',interface='org.freedesktop.DBus.ObjectManager',member='InterfacesAdded'",
                             [this](sdbus::Message message) {
                                 this->onInterfacesAdded(message);
                             }
        );
    } catch (const sdbus::Error &e) {
        std::cerr << "Error starting discovery on adapter " << adapterPath << ": " << e.what() << std::endl;
    }
}

void BluetoothClient::stopDiscovery(const std::string &adapterPath) const {
    try {
        const auto adapterProxy = sdbus::createProxy(*connection, sdbus::ServiceName("org.bluez"),
                                                     sdbus::ObjectPath(adapterPath));
        adapterProxy->callMethod("StopDiscovery").onInterface("org.bluez.Adapter1");
    } catch (const sdbus::Error &e) {
        std::cerr << "Error stopping discovery on adapter " << adapterPath << ": " << e.what() << std::endl;
    }
}

std::string BluetoothClient::getServiceName(const std::string &uuid) {
    static const std::map<std::string, std::string> uuidToServiceName = {
        {"00001800-0000-1000-8000-00805f9b34fb", "Generic Access"},
        {"00001801-0000-1000-8000-00805f9b34fb", "Generic Attribute"},
        {"0000180a-0000-1000-8000-00805f9b34fb", "Device Information"},
        {"0000110e-0000-1000-8000-00805f9b34fb", "A/V Remote Control"},
        {"0000110c-0000-1000-8000-00805f9b34fb", "A/V Remote Control Target"},
        {"0000110b-0000-1000-8000-00805f9b34fb", "Advanced Audio Distribution"},
        {"0000110d-0000-1000-8000-00805f9b34fb", "Audio/Video Control Transport Protocol"},
        {"0000110a-0000-1000-8000-00805f9b34fb", "Audio/Video Distribution Transport Protocol"},
        {"0000110f-0000-1000-8000-00805f9b34fb", "Basic Imaging Profile"},
        {"00001112-0000-1000-8000-00805f9b34fb", "Basic Printing Profile"},
        {"00001133-0000-1000-8000-00805f9b34fb", "Basic Telephony Profile"},
        {"00001117-0000-1000-8000-00805f9b34fb", "Browsing Profile"},
        {"00001105-0000-1000-8000-00805f9b34fb", "Cordless Telephony Profile"},
        {"00001132-0000-1000-8000-00805f9b34fb", "Dial-up Networking Profile"},
        {"00001116-0000-1000-8000-00805f9b34fb", "Fax Profile"},
        {"0000112f-0000-1000-8000-00805f9b34fb", "Hands-Free Profile"},
        {"00001108-0000-1000-8000-00805f9b34fb", "Headset Profile"},
        {"0000112d-0000-1000-8000-00805f9b34fb", "Human Interface Device Service"},
        {"0000112c-0000-1000-8000-00805f9b34fb", "Message Access Profile"},
        {"00001115-0000-1000-8000-00805f9b34fb", "Object Push Profile"},
        {"0000112e-0000-1000-8000-00805f9b34fb", "Personal Area Networking User"},
        {"0000112b-0000-1000-8000-00805f9b34fb", "Phonebook Access Profile"},
        {"0000112a-0000-1000-8000-00805f9b34fb", "SIM Access Profile"},
        {"0000111f-0000-1000-8000-00805f9b34fb", "Synchronization Profile"},
        {"00001120-0000-1000-8000-00805f9b34fb", "Synchronization Profile"},
        {"00001121-0000-1000-8000-00805f9b34fb", "Synchronization Profile"},
        {"00001122-0000-1000-8000-00805f9b34fb", "Synchronization Profile"},
        {"00001123-0000-1000-8000-00805f9b34fb", "Synchronization Profile"},
        {"00001124-0000-1000-8000-00805f9b34fb", "Synchronization Profile"},
        {"00001125-0000-1000-8000-00805f9b34fb", "Synchronization Profile"},
        {"00001126-0000-1000-8000-00805f9b34fb", "Synchronization Profile"},
        {"00001127-0000-1000-8000-00805f9b34fb", "Synchronization Profile"},
        {"00001128-0000-1000-8000-00805f9b34fb", "Synchronization Profile"},
        {"00001129-0000-1000-8000-00805f9b34fb", "Synchronization Profile"},
        {"0000112f-0000-1000-8000-00805f9b34fb", "Synchronization Profile"},
        {"00001130-0000-1000-8000-00805f9b34fb", "Synchronization Profile"},
        {"00001131-0000-1000-8000-00805f9b34fb", "Synchronization Profile"},
        {"00001134-0000-1000-8000-00805f9b34fb", "Synchronization Profile"},
        {"00001133-0000-1000-8000-00805f9b34fb", "SAP (SIM Access Profile)"},
        {"00005005-0000-1000-8000-0002ee000001", "Wi-Fi Direct"},
        {"00001200-0000-1000-8000-00805f9b34fb", "PnP Information"},
        {"00001104-0000-1000-8000-00805f9b34fb", "OBEX Object Push Profile"},
        {"00001106-0000-1000-8000-00805f9b34fb", "OBEX File Transfer Profile"},
        {"0000110a-0000-1000-8000-00805f9b34fb", "A2DP Source"},
        {"0000110b-0000-1000-8000-00805f9b34fb", "A2DP Sink"},
        {"0000110c-0000-1000-8000-00805f9b34fb", "AVRCP Target"},
        {"0000110e-0000-1000-8000-00805f9b34fb", "AVRCP Controller"},
        {"0000110f-0000-1000-8000-00805f9b34fb", "HSP AG"},
        {"00001112-0000-1000-8000-00805f9b34fb", "HCR Printer"},
        {"0000111e-0000-1000-8000-00805f9b34fb", "HCR Scan"},
        {"0000111f-0000-1000-8000-00805f9b34fb", "HCR Print"},
        {"00001132-0000-1000-8000-00805f9b34fb", "DUN-GW"},
        {"00001133-0000-1000-8000-00805f9b34fb", "DUN-DCE"},
        {"00001134-0000-1000-8000-00805f9b34fb", "Handsfree"},
        {"00001135-0000-1000-8000-00805f9b34fb", "Handsfree AG"},
        {"00001136-0000-1000-8000-00805f9b34fb", "HFP AG"},
        {"00001137-0000-1000-8000-00805f9b34fb", "HFP HF"},
        {"00001138-0000-1000-8000-00805f9b34fb", "HFP"},
        {"00001139-0000-1000-8000-00805f9b34fb", "Phonebook Access"},
        {"0000113a-0000-1000-8000-00805f9b34fb", "Message Access"},
        // Add more UUID to service name mappings as needed
    };

    if (const auto it = uuidToServiceName.find(uuid); it != uuidToServiceName.end()) {
        return it->second;
    }
    return uuid;
}

std::vector<std::string> BluetoothClient::listServices(const std::string &devicePath) const {
    std::vector<std::string> services;
    try {
        const auto deviceProxy = sdbus::createProxy(*connection, sdbus::ServiceName("org.bluez"),
                                                    sdbus::ObjectPath(devicePath));
        std::map<std::string, sdbus::Variant> properties;
        deviceProxy->callMethod("GetAll").onInterface("org.freedesktop.DBus.Properties")
                .withArguments("org.bluez.Adapter1")
                .storeResultsTo(properties);

        if (properties.contains("UUIDs")) {
            auto uuids = properties["UUIDs"].get<std::vector<std::string> >();
            services.insert(services.end(), uuids.begin(), uuids.end());

            for (const auto &uuid: uuids) {
                if (properties.contains("Alias")) {
                }
                // Retrieve and print additional properties if needed
                // For example, you can retrieve the service name or other details
            }
        }
    } catch (const sdbus::Error &e) {
        std::cerr << "Error listing services: " << e.what() << std::endl;
    }
    return services;
}

void BluetoothClient::onInterfacesAdded(sdbus::Message &message) const {
    sdbus::ObjectPath objectPath;
    std::map<std::string, std::map<std::string, sdbus::Variant> > interfaces;
    message >> objectPath >> interfaces;

    if (interfaces.contains("org.bluez.Device1")) {
        const auto deviceProxy = sdbus::createProxy(*connection, sdbus::ServiceName("org.bluez"), objectPath);
        sdbus::Variant address;
        deviceProxy->callMethod("Get").onInterface("org.freedesktop.DBus.Properties")
                .withArguments("org.bluez.Device1", "Address")
                .storeResultsTo(address);
        std::cout << "Device found: " << address.get<std::string>() << std::endl;
    }
}

void BluetoothClient::monitorProperties(const std::string &devicePath) const {
    try {
        const auto deviceProxy = sdbus::createProxy(*connection, sdbus::ServiceName("org.bluez"),
                                                    sdbus::ObjectPath(devicePath));

        connection->addMatch(
            "type='signal',interface='org.freedesktop.DBus.Properties',member='PropertiesChanged',path='" + devicePath +
            "'",
            [devicePath](sdbus::Message message) {
                std::string interfaceName;
                std::map<std::string, sdbus::Variant> changedProperties;
                std::vector<std::string> invalidatedProperties;
                message >> interfaceName >> changedProperties >> invalidatedProperties;

                if (interfaceName == "org.bluez.Adapter1") {
                    for (const auto &[property, value]: changedProperties) {
                        if (property == "Connected" && value.get<bool>()) {
                            std::cout << "Device " << devicePath << " connected" << std::endl;
                        } else if (property == "Connected" && !value.get<bool>()) {
                            std::cout << "Device " << devicePath << " disconnected" << std::endl;
                        } else if (property == "Alias") {
                            std::cout << "Alias " << value.get<std::string>() << " set on device " << devicePath <<
                                    std::endl;
                        } else if (property == "RSSI") {
                            std::cout << "RSSI " << value.get<int16_t>() << " on device " << devicePath << std::endl;
                        } else if (property == "Discovering") {
                            std::cout << "Discovery " << (value.get<bool>() ? "started" : "stopped") << " on device "
                                    << devicePath <<
                                    std::endl;
                        }
                    }
                }
            });
    } catch (const sdbus::Error &e) {
        std::cerr << "Error monitoring properties on device " << devicePath << ": " << e.what() << std::endl;
    }
}
