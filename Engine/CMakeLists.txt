cmake_minimum_required( VERSION 3.8 )

add_subdirectory(vendor)
set(VENDOR_DIR ${CMAKE_CURRENT_SOURCE_DIR}/vendor)

include(FindVulkan)

find_package(glfw3 CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(Stb REQUIRED)
find_package(cereal CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(rttr CONFIG REQUIRED)
find_package(implot CONFIG REQUIRED)
find_package(OpenAL CONFIG REQUIRED)
find_package(EnTT CONFIG REQUIRED)
find_package(assimp CONFIG REQUIRED)
find_package(unofficial-omniverse-physx-sdk CONFIG REQUIRED)
find_package(Vulkan REQUIRED COMPONENTS shaderc_combined)

message(STATUS "Vulkan VERSION: ${Vulkan_VERSION}")
message(STATUS "Vulkan Includes: ${Vulkan_INCLUDE_DIR} - Libs ${Vulkan_LIBRARIES}")
set(Vulkan_LIB_DIR $ENV{VULKAN_SDK}/Lib)


file(GLOB_RECURSE ENGINE_SRC CMAKE_CONFIGURE_DEPENDS src/** data/** PARENT_SCOPE)



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

target_link_directories(Engine PUBLIC ${Vulkan_LIB_DIR})
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
	PUBLIC unofficial::omniverse-physx-sdk::sdk
	PUBLIC ${Vulkan_LIBRARIES}
	PUBLIC shaderc_combined$<$<CONFIG:DEBUG>:d>
	PUBLIC spirv-cross-core$<$<CONFIG:DEBUG>:d>
	PUBLIC spirv-cross-glsl$<$<CONFIG:DEBUG>:d>
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





 