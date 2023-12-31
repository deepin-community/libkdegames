set( EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR} )

remove_definitions(
    -DQT_NO_CAST_FROM_ASCII
    -DQT_NO_CAST_TO_ASCII
    -DQT_NO_CAST_FROM_BYTEARRAY
)

MACRO(LIBKDEGAMES_EXECUTABLE_TESTS)
       FOREACH(_testname ${ARGN})
               set(${_testname}_SRCS ${_testname}.cpp)
               #if test has an ui file, use it
               IF(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${_testname}.ui)
                  ki18n_wrap_ui(${_testname}_SRCS ${_testname}.ui)
               ENDIF(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${_testname}.ui)

               add_executable(${_testname} TEST ${${_testname}_SRCS})
               target_link_libraries(${_testname} Qt${QT_MAJOR_VERSION}::Test KF5KDEGames)
       ENDFOREACH(_testname)
ENDMACRO(LIBKDEGAMES_EXECUTABLE_TESTS)

if (Q_WS_X11)
LIBKDEGAMES_EXECUTABLE_TESTS(
#    kxerrorhandlertest # File missing from svn
#     kgamepopupitemtest
)
endif (Q_WS_X11)
