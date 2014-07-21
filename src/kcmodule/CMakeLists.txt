set(notificationhelperconfig_SRCS
    notificationhelperconfigmodule.cpp
)

add_library(kcm_notificationhelper MODULE ${notificationhelperconfig_SRCS})

target_link_libraries(kcm_notificationhelper
    Qt5::DBus
    KF5::ConfigWidgets
    KF5::I18n)

install(TARGETS kcm_notificationhelper DESTINATION ${PLUGIN_INSTALL_DIR})

install(FILES kcm_notificationhelper.desktop  DESTINATION  ${SERVICES_INSTALL_DIR})
