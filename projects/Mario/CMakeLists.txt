cmake_minimum_required(VERSION 3.5...4.0)



file(GLOB_RECURSE SRC CMAKE_CONFIGURE_DEPENDS src/**)

set(PROJECT_NAME Mario)


add_library(${PROJECT_NAME} SHARED ${SRC})
set_target_properties(${PROJECT_NAME} PROPERTIES
	CXX_STANDARD 20
	CXX_STANDARD_REQUIRED YES
)

target_compile_definitions(${PROJECT_NAME} PUBLIC RESOURCE_PATH="${CMAKE_CURRENT_SOURCE_DIR}/resources/" PRIVATE BUILD_DLL )
target_link_libraries(${PROJECT_NAME} PUBLIC BHive-Engine)
target_include_directories(${PROJECT_NAME} PUBLIC src)