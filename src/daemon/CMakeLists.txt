set(notificationhelper_SRCS
    notificationhelpermodule.cpp
    event.cpp
    configwatcher.cpp
    apportevent/apportevent.cpp
    hookevent/hookevent.cpp
    hookevent/hookgui.cpp
    hookevent/hook.cpp
    hookevent/locale.cpp
    installevent/installdbuswatcher.cpp
    installevent/installevent.cpp
    installevent/installgui.cpp
    l10nevent/l10nevent.cpp
    rebootevent/rebootevent.cpp
    driverevent/Device.cpp
    driverevent/driverevent.cpp
)
message(WARNING "hookevent is a pile of madness including locale....")

QT5_ADD_DBUS_ADAPTOR(notificationhelper_SRCS org.kubuntu.NotificationHelper.xml
                     configwatcher.h ConfigWatcher)
QT5_ADD_DBUS_ADAPTOR(notificationhelper_SRCS installevent/org.kubuntu.restrictedInstall.xml
                     installevent/installdbuswatcher.h InstallDBusWatcher)
QT5_ADD_DBUS_ADAPTOR(notificationhelper_SRCS org.kubuntu.NotificationHelper.xml
                     configwatcher.h ConfigWatcher)

set(drivermanagerobject_xml driverevent/org.kubuntu.DriverManager.xml)
set_source_files_properties(${drivermanagerobject_xml}
    PROPERTIES INCLUDE driverevent/drivermanagerdbustypes.h)
QT5_ADD_DBUS_INTERFACE(notificationhelper_SRCS ${drivermanagerobject_xml}
                       drivermanager_interface)

# KI18N Translation Domain for this library
add_definitions(-DTRANSLATION_DOMAIN=\"notificationhelper\")

add_library(kded_notificationhelper MODULE ${notificationhelper_SRCS})

target_link_libraries(kded_notificationhelper
    KF5::ConfigCore
    KF5::CoreAddons
    KF5::DBusAddons
    KF5::I18n
    KF5::Notifications
    KF5::Service
    KF5::WindowSystem
    KF5::XmlGui
    Kubuntu::Main
    QApt::Main)

install(TARGETS kded_notificationhelper DESTINATION ${PLUGIN_INSTALL_DIR})

install(FILES notificationhelper.notifyrc DESTINATION ${DATA_INSTALL_DIR}/notificationhelper)
install(FILES notificationhelper.desktop  DESTINATION  ${SERVICES_INSTALL_DIR}/kded)

