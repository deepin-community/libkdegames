include(ECMAddTests)

set( EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR} )

remove_definitions(
    -DQT_NO_CAST_FROM_ASCII
    -DQT_NO_CAST_TO_ASCII
    -DQT_NO_CAST_FROM_BYTEARRAY
)

configure_file(config-tests.h.in ${CMAKE_CURRENT_BINARY_DIR}/config-tests.h)

if (TARGET Qt6::Core5Compat)
    set(PRIVATE_LIBS Qt6::Core5Compat)
endif()

ecm_add_tests(
    kgamesvgdocumenttest.cpp
    kgamepropertytest.cpp
    NAME_PREFIX "libkdegamesprivate-"
    LINK_LIBRARIES Qt${QT_MAJOR_VERSION}::Test KF5::CoreAddons KF5KDEGamesPrivate ${PRIVATE_LIBS}
)
