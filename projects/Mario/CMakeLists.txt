cmake_minimum_required(VERSION 3.8)

set(PROJECT_NAME Mario)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/${CMAKE_SYSTEM_NAME}/${CMAKE_BUILD_TYPE})

file(GLOB_RECURSE SRC CONFIGURE_DEPENDS src/**.cpp)
file(GLOB_RECURSE HEADER_FILES CONFIGURE_DEPENDS src/**.h)

# Ensure the generated directory exists
set(GENERATED_DIR ${CMAKE_BINARY_DIR}/generated)

file(MAKE_DIRECTORY ${GENERATED_DIR})

# Add pre-build command to generate .cpp files from .h files
add_custom_command(
    OUTPUT ${GENERATED_DIR}/generated_files.cpp
    COMMAND ${CMAKE_COMMAND} -E echo "Running pre-build command"
    COMMAND ${CMAKE_COMMAND} -E echo "Generating reflection data..."
    COMMAND ${CMAKE_COMMAND} -E env "PATH=$ENV{PATH}" ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/CPPReflector.exe --input "${CMAKE_SOURCE_DIR}/Engine/src" --output ${GENERATED_DIR} --include ${CMAKE_SOURCE_DIR}/projects/Mario/src
    DEPENDS ${HEADER_FILES}
    COMMENT "Generating .cpp files from .h files"
)

# Add a custom target to ensure the pre-build command runs
add_custom_target(GenerateFiles ALL DEPENDS ${GENERATED_DIR}/generated_files.cpp)

# Add the generated files to the source list
file(GLOB_RECURSE GENERATED_SRC CONFIGURE_DEPENDS ${GENERATED_DIR}/*.cpp)

find_package(EnTT CONFIG REQUIRED)

add_executable(${PROJECT_NAME} ${SRC} ${GENERATED_SRC})
set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED YES
)

target_compile_definitions(${PROJECT_NAME} PUBLIC RESOURCE_PATH="${CMAKE_CURRENT_SOURCE_DIR}/resources/" PUBLIC EDITOR_RESOURCE_PATH="${CMAKE_CURRENT_SOURCE_DIR}/editor_resources/")
target_link_libraries(${PROJECT_NAME} PUBLIC Engine PUBLIC Inspectors PUBLIC EnTT::EnTT)
target_include_directories(${PROJECT_NAME} PUBLIC src ${GENERATED_DIR})

add_dependencies(${PROJECT_NAME} CPPReflector)
add_dependencies(${PROJECT_NAME} GenerateFiles)
