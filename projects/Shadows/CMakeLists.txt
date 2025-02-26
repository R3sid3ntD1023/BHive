cmake_minimum_required(VERSION 3.8)

file(GLOB_RECURSE SRC CMAKE_CONFIGURE_DEPENDS src/**)
add_executable(Shadows ${SRC})
set_target_properties(Shadows PROPERTIES
	CXX_STANDARD 20
	CXX_STANDARD_REQUIRED YES
)

target_compile_definitions(Shadows PUBLIC RESOURCE_PATH="${CMAKE_CURRENT_SOURCE_DIR}/resources/")
target_link_libraries(Shadows PUBLIC Engine)
target_include_directories(Shadows PUBLIC src)