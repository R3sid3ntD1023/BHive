cmake_minimum_required(VERSION 3.8)

#entt
FetchContent_Declare(
	entt
	GIT_REPOSITORY https://github.com/skypjack/entt.git
	GIT_TAG v3.14.0
)
list(APPEND components entt)

FetchContent_Declare(
	ImPlot
	GIT_REPOSITORY https://github.com/R3sid3ntD1023/implot.git
	GIT_TAG master
)
list(APPEND components ImPlot)

if(components)
FETCHCONTENT_MAKEAVAILABLE(${components})
endif()

file(GLOB_RECURSE SRC CMAKE_CONFIGURE_DEPENDS src/**.cpp)

set(PROJECT_NAME SolarSystem)
add_executable(${PROJECT_NAME} ${SRC})
set_target_properties(${PROJECT_NAME} PROPERTIES
	CXX_STANDARD 20
	CXX_STANDARD_REQUIRED YES
)
target_compile_options(${PROJECT_NAME} PUBLIC /ZI /INCREMETAL /MP /bigobj /W0 )
target_compile_definitions(${PROJECT_NAME} PUBLIC _CRT_SECURE_NO_WARNINGS DEFINE_IMGUI_MATH_OPERATORS PUBLIC RESOURCE_PATH="${CMAKE_CURRENT_SOURCE_DIR}/Resources/" )
target_link_libraries(${PROJECT_NAME} PUBLIC Engine PUBLIC EnTT::EnTT PUBLIC ImPlot)
target_include_directories(${PROJECT_NAME} PUBLIC src)




 