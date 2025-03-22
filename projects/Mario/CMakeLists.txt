cmake_minimum_required(VERSION 3.8)

find_package(EnTT CONFIG REQUIRED)

file(GLOB_RECURSE SRC CMAKE_CONFIGURE_DEPENDS src/**)

set(PROJECT_NAME Mario)


add_executable(${PROJECT_NAME} ${SRC})
set_target_properties(${PROJECT_NAME} PROPERTIES
	CXX_STANDARD 20
	CXX_STANDARD_REQUIRED YES
)

target_compile_definitions(${PROJECT_NAME} PUBLIC RESOURCE_PATH="${CMAKE_CURRENT_SOURCE_DIR}/resources/" PUBLIC EDITOR_RESOURCE_PATH="${CMAKE_CURRENT_SOURCE_DIR}/editor_resources/")
target_link_libraries(${PROJECT_NAME} PUBLIC Engine PUBLIC Inspectors PUBLIC EnTT::EnTT)
target_include_directories(${PROJECT_NAME} PUBLIC src)