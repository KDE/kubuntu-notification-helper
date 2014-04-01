project(kubuntu-notification-helper)

# Find the required Libaries
find_package(KDE4 REQUIRED)
include(KDE4Defaults)

find_package(QApt)

find_package(Kubuntu QUIET CONFIG)
set_package_properties(Kubuntu
    PROPERTIES DESCRIPTION "Kubuntu Platform Integration"
#    URL ""
    TYPE REQUIRED
    PURPOSE "Kubuntu specific l10n bits."
)

include_directories(${QT_INCLUDES} ${KDE4_INCLUDES} ${KUBUNTU_INCLUDE_DIR})
add_definitions(${QT_DEFINITIONS} ${KDE4_DEFINITIONS} -DVERSION_STRING=\\"${VERSION_STRING}\\")
add_definitions(-std=c++11)

if(START_TIMEOUT)
    message(STATUS "timeout ${START_TIMEOUT}")
    add_definitions(-DSTART_TIMEOUT=${START_TIMEOUT})
endif()

add_subdirectory(data)
add_subdirectory(src)

# Add translations
include(MacroOptionalAddSubdirectory)
macro_optional_add_subdirectory(po)
