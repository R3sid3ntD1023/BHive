cmake_minimum_required(VERSION 3.8)

file(GLOB_RECURSE SRC CMAKE_CONFIGURE_DEPENDS **.cpp)

set(PROJECT_NAME WindowTest)
add_executable(${PROJECT_NAME} ${SRC})
set_target_properties(${PROJECT_NAME} PROPERTIES
	CXX_STANDARD 20
	CXX_STANDARD_REQUIRED YES
)

if(MSVC)
	target_compile_options(${PROJECT_NAME} PUBLIC /ZI /INCREMETAL /MP /bigobj /W0 )
	target_compile_definitions(${PROJECT_NAME} PUBLIC _CRT_SECURE_NO_WARNINGS PUBLIC GLFW_INCLUDE_NONE AL_LIBTYPE_STATIC
		PUBLIC IMGUI_DEFINE_MATH_OPERATORS )
	set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDebug$<$<CONFIG:Debug>:Debug>")
	set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Release>:Release>")
	
endif()


target_link_libraries(${PROJECT_NAME} PUBLIC Engine)




 