project(updatehelpernotifier)

# Find the required Libaries
find_package(KDE4 REQUIRED)
include(KDE4Defaults)

add_definitions (${QT_DEFINITIONS} ${KDE4_DEFINITIONS})
include_directories( ${KDE4_INCLUDES} ${QT_INCLUDES} )


########### next target ###############

set(updatehelpernotifier_SRCS
    module.cpp
    updatehelpernotifier.cpp)

# kde4_add_executable(updatehelpernotifier ${updatehelpernotifier_SRCS})
kde4_add_plugin(kded_updatehelpernotifier ${updatehelpernotifier_SRCS})

target_link_libraries(kded_updatehelpernotifier ${KDE4_KIO_LIBS} )

# install(TARGETS updatehelpernotifier  DESTINATION ${INSTALL_TARGETS_DEFAULT_ARGS} )
install(TARGETS kded_updatehelpernotifier  DESTINATION ${PLUGIN_INSTALL_DIR} )


########### install files ###############

install( FILES updatehelpernotifier.notifyrc DESTINATION ${DATA_INSTALL_DIR}/updatehelpernotifier )
# install( FILES updatehelpernotifier.desktop  DESTINATION ${XDG_APPS_INSTALL_DIR} )
install( FILES updatehelpernotifier.desktop  DESTINATION  ${SERVICES_INSTALL_DIR}/kded )
