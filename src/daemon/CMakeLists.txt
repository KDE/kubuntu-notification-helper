set(notificationhelper_SRCS
    notificationhelpermodule.cpp
    event.cpp
    apportevent/apportevent.cpp
    hookevent/hookevent.cpp
    hookevent/hookgui.cpp
    hookevent/hookparser.cpp
#     installevent/installevent.cpp
    rebootevent/rebootevent.cpp
)

kde4_add_plugin(kded_notificationhelper ${notificationhelper_SRCS})

target_link_libraries(kded_notificationhelper ${KDE4_KIO_LIBS})

install(TARGETS kded_notificationhelper DESTINATION ${PLUGIN_INSTALL_DIR})

install(FILES notificationhelper.notifyrc DESTINATION ${DATA_INSTALL_DIR}/notificationhelper)
install(FILES notificationhelper.desktop  DESTINATION  ${SERVICES_INSTALL_DIR}/kded)
