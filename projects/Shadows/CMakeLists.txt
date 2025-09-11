cmake_minimum_required(VERSION 3.8...3.10)

file(GLOB_RECURSE HEADER_FILES CMAKE_CONFIGURE_DEPENDS src/**.h)
file(GLOB_RECURSE SOURCE_FILES CMAKE_CONFIGURE_DEPENDS src/**.cpp)

add_executable(Shadows ${HEADER_FILES} ${SOURCE_FILES})
set_target_properties(Shadows PROPERTIES
	CXX_STANDARD 20
	CXX_STANDARD_REQUIRED YES
)

target_compile_definitions(Shadows PUBLIC RESOURCE_PATH="${CMAKE_CURRENT_SOURCE_DIR}/resources/")
target_link_libraries(Shadows PUBLIC BHive-Engine)
target_include_directories(Shadows PUBLIC src)