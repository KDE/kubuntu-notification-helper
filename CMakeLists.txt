project(notificationhelper)

# Find the required Libaries
find_package(KDE4 REQUIRED)
include(KDE4Defaults)

add_definitions (${QT_DEFINITIONS} ${KDE4_DEFINITIONS})
include_directories( ${KDE4_INCLUDES} ${QT_INCLUDES} )


########### next target ###############

set(notificationhelper_SRCS
    module.cpp
    notificationhelper.cpp
    event.cpp
    rebootevent.cpp
    apportevent.cpp
    hookevent.cpp
)

kde4_add_plugin(kded_notificationhelper ${notificationhelper_SRCS})

target_link_libraries(kded_notificationhelper ${KDE4_KIO_LIBS} )

install(TARGETS kded_notificationhelper  DESTINATION ${PLUGIN_INSTALL_DIR} )


########### install files ###############

install( FILES notificationhelper.notifyrc DESTINATION ${DATA_INSTALL_DIR}/notificationhelper )
install( FILES notificationhelper.desktop  DESTINATION  ${SERVICES_INSTALL_DIR}/kded )
