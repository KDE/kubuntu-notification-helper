set(notificationhelper_SRCS
    notificationhelpermodule.cpp
    events/event.cpp
    events/apportevent.cpp
    events/hookevent.cpp
    events/hookgui.cpp
#     events/installevent.cpp
    events/rebootevent.cpp
)

kde4_add_plugin(kded_notificationhelper ${notificationhelper_SRCS})

target_link_libraries(kded_notificationhelper ${KDE4_KIO_LIBS})

install(TARGETS kded_notificationhelper DESTINATION ${PLUGIN_INSTALL_DIR})
