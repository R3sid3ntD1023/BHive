cmake_minimum_required(VERSION 3.8)

message(STATUS "CMake version: ${CMAKE_VERSION}")

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    message(STATUS "This is the top-level CMakeLists.txt")
    project(CPPParser LANGUAGES CSharp)
else()
    message(STATUS "This is a subdirectory CMakeLists.txt")
    project(CPPParser LANGUAGES CSharp)
endif()

include(CSharpUtilities)

message(STATUS "Configuring REFLECTION ${REFLECTION_VERSION}")

file(GLOB_RECURSE SRC CMAKE_CONFIGURE_DEPENDS src/**)

set(PROJECT_NAME CPPReflector)
set(DOTNET_TARGET_FRAMEWORK "v4.7.2")
set(CMAKE_CSharp_FLAGS "/langversion:latest")

add_executable(${PROJECT_NAME} ${SRC})

set_property(TARGET ${PROJECT_NAME} PROPERTY VS_DOTNET_TARGET_FRAMEWORK_VERSION ${DOTNET_TARGET_FRAMEWORK})
set_property(TARGET ${PROJECT_NAME} PROPERTY VS_DOTNET_REFERENCES
    "System"
    "System.IO"
    "System.Text.RegularExpressions"
)
set_property(TARGET ${PROJECT_NAME} PROPERTY VS_DOTNET_REFERENCES_PATH
    "${CMAKE_CURRENT_SOURCE_DIR}/lib"
)

set_property(TARGET ${PROJECT_NAME} PROPERTY CMAKE_WIN32_EXECUTABLE TRUE)
set_target_properties(${PROJECT_NAME} PROPERTIES
    OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
    LINKER_LANGUAGE CSharp
)