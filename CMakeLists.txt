project(notificationhelper)

# Find the required Libaries
find_package(KDE4 REQUIRED)
include(KDE4Defaults)

add_definitions (${QT_DEFINITIONS} ${KDE4_DEFINITIONS})
include_directories( ${KDE4_INCLUDES} ${QT_INCLUDES} )

add_subdirectory( src )

# Add translations
include(MacroOptionalAddSubdirectory)
macro_optional_add_subdirectory( po )

########### install files ###############

install( FILES notificationhelper.notifyrc DESTINATION ${DATA_INSTALL_DIR}/notificationhelper )
install( FILES notificationhelper.desktop  DESTINATION  ${SERVICES_INSTALL_DIR}/kded )
