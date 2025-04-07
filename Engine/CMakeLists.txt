cmake_minimum_required( VERSION 3.8 )

add_subdirectory(vendor)

find_package(assimp CONFIG REQUIRED)

file(GLOB_RECURSE ENGINE_SRC CMAKE_CONFIGURE_DEPENDS src/** data/** PARENT_SCOPE)

set(VENDOR_DIR ${CMAKE_CURRENT_SOURCE_DIR}/vendor)

include(FindVulkan)
if(DEFINED Vulkan_SDK_PATH)
    set(Vulkan_INCLUDE_DIRS "${Vulkan_SDK_PATH}/Include")
    set(Vulkan_LIBRARIES "${Vulkan_SDK_PATH}/Lib")
	message(STATUS "Vulkan SDK found at ${Vulkan_SDK_PATH}")
else()
    find_package(Vulkan REQUIRED COMPONENTS shaderc_shared spirv_cross_core spirv_cross_glsl)
endif()

message(STATUS "Vulkan VERSION: ${Vulkan_VERSION}")

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

target_link_directories(Engine PUBLIC ${Vulkan_LIBRARIES})
target_link_libraries(Engine
	PUBLIC glfw 
	PUBLIC glad 
	opengl32 
	PUBLIC OpenAL::OpenAL
	PUBLIC assimp::assimp 
	PUBLIC imgui 
	PUBLIC ImGuizmo
	PUBLIC stb
	PUBLIC waveparser
	PUBLIC ReactPhysics3D::reactphysics3d 
	PUBLIC rttr_core 
	PUBLIC spdlog::spdlog
	PUBLIC glm::glm-header-only
	PUBLIC cereal::cereal
	PUBLIC ImPlot
	PUBLIC msdf-atlas-gen::msdf-atlas-gen
	vulkan-1 
	shaderc_shared$<$<CONFIG:Debug>:d> 
	spirv-cross-core$<$<CONFIG:Debug>:d> 
	spirv-cross-glsl$<$<CONFIG:Debug>:d> 
)

target_include_directories(Engine PUBLIC 
	${CMAKE_CURRENT_SOURCE_DIR}/src 
	${CMAKE_CURRENT_SOURCE_DIR}/src/BHive 
	${CMAKE_CURRENT_SOURCE_DIR}/src/BHive/world
	${VENDOR_DIR}
	${Vulkan_INCLUDE_DIRS}
	
)

target_compile_definitions(Engine PUBLIC ENGINE_PATH="${CMAKE_CURRENT_SOURCE_DIR}")
target_precompile_headers(Engine PUBLIC src/BHivePCH.h)





 