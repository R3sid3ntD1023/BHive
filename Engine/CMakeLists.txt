cmake_minimum_required( VERSION 3.8 )

add_subdirectory(vendor)
find_package(OpenAL CONFIG REQUIRED)
find_package(assimp CONFIG REQUIRED)

file(GLOB_RECURSE ENGINE_SRC CMAKE_CONFIGURE_DEPENDS src/** data/** PARENT_SCOPE)

set(VENDOR_DIR ${CMAKE_CURRENT_SOURCE_DIR}/vendor)

add_library(Engine STATIC ${ENGINE_SRC})
set_target_properties(Engine PROPERTIES 
	CXX_STANDARD 20
	CXX_STANDARD_REQUIRED YES
)
target_compile_options(Engine PUBLIC /ZI /INCREMETAL /MP /bigobj /W0 )
target_compile_definitions(Engine 
PUBLIC _CRT_SECURE_NO_WARNINGS 
PUBLIC GLFW_INCLUDE_NONE 
PUBLIC AL_LIBTYPE_STATIC 
PUBLIC IMGUI_DEFINE_MATH_OPERATORS
PUBLIC SPDLOG_COMPILED_LIB)

target_link_libraries(Engine
	PRIVATE glfw 
	PUBLIC glad 
	opengl32 
	PRIVATE OpenAL::OpenAL
	PRIVATE assimp::assimp 
	PUBLIC imgui 
	PRIVATE stb
	PUBLIC waveparser
	PRIVATE ReactPhysics3D::reactphysics3d 
	PUBLIC rttr_core 
	PUBLIC spdlog::spdlog
	PUBLIC glm::glm-header-only
	PUBLIC cereal::cereal
	PUBLIC ImPlot
	PUBLIC mINI
	PRIVATE msdf-atlas-gen::msdf-atlas-gen
)

target_include_directories(Engine PUBLIC 
	${CMAKE_CURRENT_SOURCE_DIR}/src 
	${CMAKE_CURRENT_SOURCE_DIR}/src/BHive 
	${CMAKE_CURRENT_SOURCE_DIR}/src/platform 
	${VENDOR_DIR}
)

target_compile_definitions(Engine PUBLIC ENGINE_PATH="${CMAKE_CURRENT_SOURCE_DIR}")





 