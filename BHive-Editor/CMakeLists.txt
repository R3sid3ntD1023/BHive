cmake_minimum_required(VERSION 3.8)

file(GLOB_RECURSE  EDITOR_SRC CMAKE_CONFIGURE_DEPENDS src/** PARENT_SCOPE)

add_executable(BHive-Editor ${EDITOR_SRC})
set_target_properties(BHive-Editor PROPERTIES
	CXX_STANDARD 20
	CXX_STANDARD_REQUIRED YES
)
target_compile_definitions(BHive-Editor PUBLIC EDITOR_RESOURCE_PATH="${CMAKE_CURRENT_SOURCE_DIR}/resources/")
target_link_libraries(BHive-Editor PUBLIC Engine )
target_include_directories(BHive-Editor PRIVATE src)