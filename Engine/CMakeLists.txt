cmake_minimum_required( VERSION 3.8 )


find_package(glfw3 CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(Stb REQUIRED)
find_package(cereal CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(rttr CONFIG REQUIRED)
find_package(implot CONFIG REQUIRED)
#find_package(imguizmo CONFIG REQUIRED)
find_package(OpenAL CONFIG REQUIRED)
find_package(EnTT CONFIG REQUIRED)
find_package(assimp CONFIG REQUIRED)
find_package(unofficial-omniverse-physx-sdk CONFIG REQUIRED)

add_subdirectory(vendor)


file(GLOB_RECURSE ENGINE_SRC CMAKE_CONFIGURE_DEPENDS src/** data/** PARENT_SCOPE)

set(VENDOR_DIR ${CMAKE_CURRENT_SOURCE_DIR}/vendor)

include(FindVulkan)
if(DEFINED Vulkan_SDK_PATH)
    set(Vulkan_INCLUDE_DIRS "${Vulkan_SDK_PATH}/Include")
    set(Vulkan_LIBRARIES "${Vulkan_SDK_PATH}/Lib")
	set(Vulkan_FOUND true)
	message(STATUS "Using Vulkan at: ${Vulkan_SDK_PATH}")
else()
    find_package(Vulkan REQUIRED COMPONENTS shaderc_shared spirv_cross_core spirv_cross_glsl)
endif()

if(DEFINED Vulkan_FOUND)
message(STATUS "Vulkan VERSION: ${Vulkan_VERSION}")
endif()

add_library(Engine STATIC ${ENGINE_SRC} "src/BHive/world/components/SphereComponent.cpp")
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
	PUBLIC glad::glad
	PUBLIC opengl32 
	PUBLIC OpenAL::OpenAL
	PUBLIC assimp::assimp 
	PUBLIC imgui::imgui
	PUBLIC ImGuizmo
	PUBLIC waveparser
	PUBLIC RTTR::Core
	PUBLIC spdlog::spdlog
	PUBLIC glm::glm-header-only
	PUBLIC cereal::cereal
	PUBLIC implot::implot
	PUBLIC msdf-atlas-gen::msdf-atlas-gen
	PUBLIC EnTT::EnTT
	PUBLIC vulkan-1 
	PUBLIC shaderc_shared$<$<CONFIG:Debug>:d> 
	PUBLIC spirv-cross-core$<$<CONFIG:Debug>:d> 
	PUBLIC spirv-cross-glsl$<$<CONFIG:Debug>:d> 
	PUBLIC unofficial::omniverse-physx-sdk::sdk
)

target_include_directories(Engine PUBLIC 
	${CMAKE_CURRENT_SOURCE_DIR}/src 
	${CMAKE_CURRENT_SOURCE_DIR}/src/BHive 
	${CMAKE_CURRENT_SOURCE_DIR}/src/BHive/world
	${VENDOR_DIR}
	${Vulkan_INCLUDE_DIRS}
	${Stb_INCLUDE_DIR}
	
)

target_compile_definitions(Engine PUBLIC ENGINE_PATH="${CMAKE_CURRENT_SOURCE_DIR}")
target_precompile_headers(Engine PUBLIC src/BHivePCH.h)





 