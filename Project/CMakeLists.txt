cmake_minimum_required(VERSION 3.8)

file(GLOB_RECURSE SRC CMAKE_CONFIGURE_DEPENDS src/** resources/** PARENT_SCOPE)

set(VENDOR_DIR ${CMAKE_CURRENT_SOURCE_DIR}/vendor)

set(PROJECT_NAME Project)
add_library(${PROJECT_NAME} SHARED ${SRC})
set_target_properties(${PROJECT_NAME} PROPERTIES
	CXX_STANDARD 20
	CXX_STANDARD_REQUIRED YES
)

target_link_libraries(${PROJECT_NAME} PUBLIC Engine)




 