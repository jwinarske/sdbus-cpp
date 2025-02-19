# Building examples

add_executable(obj-manager-server org.freedesktop.DBus.ObjectManager/obj-manager-server.cpp)
target_link_libraries(obj-manager-server sdbus-c++)

add_executable(obj-manager-client org.freedesktop.DBus.ObjectManager/obj-manager-client.cpp)
target_link_libraries(obj-manager-client sdbus-c++)

add_executable(flatpak-client org.freedesktop.Flatpak/main.cpp org.freedesktop.Flatpak/flatpak-client.cpp)
target_link_libraries(flatpak-client sdbus-c++)

add_executable(upower-client org.freedesktop.UPower/main.cpp org.freedesktop.UPower/upower-client.cpp)
target_link_libraries(upower-client sdbus-c++)

add_executable(aptdaemon-client org.debian.apt/aptdaemon-client.cpp org.debian.apt/main.cpp)
target_link_libraries(aptdaemon-client sdbus-c++)

add_executable(network-client network/network-client.cpp network/main.cpp)
target_link_libraries(network-client sdbus-c++)

add_executable(bluez-client bluez/bluetooth-client.cpp bluez/main.cpp)
target_link_libraries(bluez-client sdbus-c++)

if (SDBUSCPP_INSTALL)
    install(TARGETS obj-manager-server DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT sdbus-c++-examples)
    install(TARGETS obj-manager-client DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT sdbus-c++-examples)

    install(TARGETS flatpak-client DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT sdbus-c++-examples)
    install(TARGETS upower-client DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT sdbus-c++-examples)
    install(TARGETS network-client DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT sdbus-c++-examples)
endif ()
