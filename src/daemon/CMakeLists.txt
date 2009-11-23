set(notificationhelper_SRCS
    notificationhelpermodule.cpp
    event.cpp
    configwatcher.cpp
    apportevent/apportevent.cpp
    hookevent/hookevent.cpp
    hookevent/hookgui.cpp
    hookevent/hookparser.cpp
    installevent/installdbuswatcher.cpp
    installevent/installevent.cpp
    installevent/installgui.cpp
    rebootevent/rebootevent.cpp
)

QT4_ADD_DBUS_ADAPTOR(notificationhelper_SRCS org.kubuntu.NotificationHelper.xml
                     configwatcher.h ConfigWatcher)
QT4_ADD_DBUS_ADAPTOR(notificationhelper_SRCS installevent/org.kubuntu.restrictedInstall.xml
                     installevent/installdbuswatcher.h InstallDBusWatcher)

kde4_add_plugin(kded_notificationhelper ${notificationhelper_SRCS})

target_link_libraries(kded_notificationhelper ${KDE4_KIO_LIBS})

install(TARGETS kded_notificationhelper DESTINATION ${PLUGIN_INSTALL_DIR})

install(FILES notificationhelper.notifyrc DESTINATION ${DATA_INSTALL_DIR}/notificationhelper)
install(FILES notificationhelper.desktop  DESTINATION  ${SERVICES_INSTALL_DIR}/kded)
