remove_definitions(-DQT_NO_CAST_FROM_ASCII)

find_package(Qt5Test 5.0.0 REQUIRED NO_MODULE)
set_package_properties(Qt5Test PROPERTIES PURPOSE "Required for tests")

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR})

include(ECMAddTests)

add_definitions( -DTEST_DATA="${CMAKE_CURRENT_SOURCE_DIR}/data" )

ecm_add_test(TEST_NAME hooktest
    hooktest.cpp
    ../src/daemon/hookevent/hook.cpp
    ../src/daemon/hookevent/locale.cpp
    LINK_LIBRARIES
        Qt5::Core
        Qt5::Test
        KF5::CoreAddons
        KF5::Service
)

ecm_add_test(TEST_NAME localetest
    localetest.cpp
    ../src/daemon/hookevent/locale.cpp
    LINK_LIBRARIES
        Qt5::Core
        Qt5::Test
        KF5::CoreAddons
        KF5::Service
)

