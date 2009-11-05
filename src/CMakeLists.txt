set(notificationhelper_SRCS
    notificationhelpermodule.cpp
    events/event.cpp
    events/apport/apportevent.cpp
    events/hook/hookevent.cpp
    events/hook/hookgui.cpp
    events/hook/hookparser.cpp
#     events/install/installevent.cpp
    events/reboot/rebootevent.cpp
)

kde4_add_plugin(kded_notificationhelper ${notificationhelper_SRCS})

target_link_libraries(kded_notificationhelper ${KDE4_KIO_LIBS})

install(TARGETS kded_notificationhelper DESTINATION ${PLUGIN_INSTALL_DIR})
