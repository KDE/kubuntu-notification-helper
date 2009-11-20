project(notificationhelper)

# Find the required Libaries
find_package(KDE4 REQUIRED)
include(KDE4Defaults)

add_definitions (${QT_DEFINITIONS} ${KDE4_DEFINITIONS})
include_directories(${KDE4_INCLUDES} ${QT_INCLUDES})

add_subdirectory(src)

# Add translations
include(MacroOptionalAddSubdirectory)
macro_optional_add_subdirectory(po)
