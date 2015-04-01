project(kubuntu-notification-helper)

cmake_minimum_required(VERSION 2.8.12)

find_package(ECM 0.0.14 REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR})

include(FeatureSummary)
include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings)

find_package(Qt5 5.0.0 CONFIG REQUIRED DBus)

find_package(KF5 5.0.0 REQUIRED COMPONENTS
    Config
    ConfigWidgets
    CoreAddons
    DBusAddons
    KIO
    I18n
    Notifications
    Service
    WindowSystem
    XmlGui)

find_package(QApt 3.0.0 REQUIRED)

find_package(Kubuntu 1 REQUIRED QUIET CONFIG)
set_package_properties(Kubuntu
    PROPERTIES DESCRIPTION "Kubuntu Platform Integration"
#    URL ""
    TYPE REQUIRED
    PURPOSE "Kubuntu specific l10n bits."
)

add_definitions(-DVERSION_STRING=\"${VERSION_STRING}\")

if(START_TIMEOUT)
    message(STATUS "timeout ${START_TIMEOUT}")
    add_definitions(-DSTART_TIMEOUT=${START_TIMEOUT})
endif()

add_subdirectory(autotests)
add_subdirectory(data)
add_subdirectory(src)

# Add translations
# ecm_optional_add_subdirectory(po)

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
