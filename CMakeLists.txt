cmake_minimum_required(VERSION 3.8)

#if(MSCV)
#execute_process(
#    COMMAND ping www.google.com -n 2 -w 1000
#    ERROR_QUIET
#    RESULT_VARIABLE NO_CONNECTION
#)
#endif()
#
#if(NOT NO_CONNECTION EQUAL 0)
#    set(FETCHCONTENT_FULLY_DISCONNECTED ON)
#    message(WARNING, "Fetch offline mode: requires already populated _deps")
#else()
#    set(FETCHCONTENT_FULLY_DISCONNECTED OFF)
#endif()


include(cmake/functions.cmake)
include(FETCHCONTENT)

project(BHive VERSION 1.0 LANGUAGES CXX C)

foreach( OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES} )
    string( TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG )
    set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_SOURCE_DIR}/bin/lib/${CMAKE_SYSTEM_NAME}/${CMAKE_BUILD_TYPE} )
    set( CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_SOURCE_DIR}/bin/${CMAKE_SYSTEM_NAME}/${CMAKE_BUILD_TYPE} )
    set( CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_SOURCE_DIR}/bin/${CMAKE_SYSTEM_NAME}/${CMAKE_BUILD_TYPE} )
endforeach( OUTPUTCONFIG CMAKE_CONFIGURATION_TYPES )

set_property(GLOBAL PROPERTY USE_FOLDERS ON)


add_subdirectory(Engine)
add_subdirectory(Editor)
add_subdirectory(Project)
add_subdirectory(ProjectLauncher)
add_subdirectory(SolarSystem)

set(VS_STARTUP_PROJECT, Editor)


