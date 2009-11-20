set(notificationhelper_SRCS
    daemon/notificationhelpermodule.cpp
    daemon/event.cpp
    daemon/apportevent/apportevent.cpp
    daemon/hookevent/hookevent.cpp
    daemon/hookevent/hookgui.cpp
    daemon/hookevent/hookparser.cpp
#     daemon/installevent/installevent.cpp
    daemon/rebootevent/rebootevent.cpp
)

kde4_add_plugin(kded_notificationhelper ${notificationhelper_SRCS})

target_link_libraries(kded_notificationhelper ${KDE4_KIO_LIBS})

install(TARGETS kded_notificationhelper DESTINATION ${PLUGIN_INSTALL_DIR})

########### install files ###############

install( FILES notificationhelper.notifyrc DESTINATION ${DATA_INSTALL_DIR}/notificationhelper )
install( FILES notificationhelper.desktop  DESTINATION  ${SERVICES_INSTALL_DIR}/kded )
