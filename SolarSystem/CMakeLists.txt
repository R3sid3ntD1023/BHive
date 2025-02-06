cmake_minimum_required(VERSION 3.8)

file(GLOB_RECURSE SRC CMAKE_CONFIGURE_DEPENDS src/**.cpp)

set(PROJECT_NAME SolarSystem)
add_executable(${PROJECT_NAME} ${SRC})
set_target_properties(${PROJECT_NAME} PROPERTIES
	CXX_STANDARD 20
	CXX_STANDARD_REQUIRED YES
)
target_compile_options(${PROJECT_NAME} PUBLIC /ZI /INCREMETAL /MP /bigobj /W0 )
target_compile_definitions(${PROJECT_NAME} PUBLIC _CRT_SECURE_NO_WARNINGS PUBLIC RESOURCE_PATH="${CMAKE_CURRENT_SOURCE_DIR}/Resources" )
target_link_libraries(${PROJECT_NAME} PUBLIC Engine)




 