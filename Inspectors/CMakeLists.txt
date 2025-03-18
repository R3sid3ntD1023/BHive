cmake_minimum_required(VERSION 3.8)

file(GLOB_RECURSE SRC CMAKE_CONFIGURE_DEPENDS src/**)

set(PROJECT_NAME  Inspectors)
add_library(${PROJECT_NAME} SHARED ${SRC})
set_target_properties(${PROJECT_NAME} PROPERTIES
	CXX_STANDARD 20
	CXX_STANDARD_REQUIRED YES
)
target_link_libraries(${PROJECT_NAME} PUBLIC Engine )
target_compile_definitions(${PROJECT_NAME} PUBLIC BUILD_INSPECTOR)
target_include_directories(${PROJECT_NAME} PUBLIC include)