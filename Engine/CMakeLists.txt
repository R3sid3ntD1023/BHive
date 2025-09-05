cmake_minimum_required( VERSION 3.10 )

option(BUILD_SHARED_LIB "Build Engine dll" ON)

include(cmake/vulkan.cmake)
include(FindVulkan)
include(GNUInstallDirs)

set(VENDOR_DIR ${CMAKE_CURRENT_SOURCE_DIR}/vendor)
set(RENDERDOC_Dir "C:/Program Files/RenderDoc")



add_subdirectory(vendor)

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
find_package(mINI CONFIG REQUIRED)

message(STATUS "Vulkan VERSION: ${Vulkan_VERSION}")
message(STATUS "Vulkan Includes: ${Vulkan_INCLUDE_DIR} - Libs ${Vulkan_LIBRARIES}")
set(Vulkan_LIB_DIR $ENV{VULKAN_SDK}/Lib)


file(GLOB_RECURSE HEADER_FILES CMAKE_CONFIGURE_DEPENDS src/**.h src/**.inl)
file(GLOB_RECURSE SOURCE_FILES CMAKE_CONFIGURE_DEPENDS src/**.cpp src/**.hpp)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${HEADER_FILES} ${SOURCE_FILES} )

set(TARGET_NAME BHive-Engine)

if(BUILD_SHARED_LIB)
	add_library(${TARGET_NAME} SHARED ${HEADER_FILES} ${SOURCE_FILES})
	target_compile_definitions(${TARGET_NAME} 	PUBLIC BUILD_SHARED PRIVATE BUILD_DLL)
else()
	add_library(${TARGET_NAME} STATIC ${HEADER_FILES} ${SOURCE_FILES})
endif()

set_target_properties(${TARGET_NAME} PROPERTIES CXX_STANDARD 20 CXX_STANDARD_REQUIRED ON)

target_compile_options(${TARGET_NAME} PUBLIC /ZI /INCREMETAL /MP /bigobj /W0 )
target_compile_definitions(${TARGET_NAME}
	PUBLIC _CRT_SECURE_NO_WARNINGS 
	PUBLIC GLFW_INCLUDE_NONE 
	PUBLIC AL_LIBTYPE_STATIC 
	PUBLIC IMGUI_DEFINE_MATH_OPERATORS
	PUBLIC SPDLOG_COMPILED_LIB
	PUBLIC ENGINE_PATH="${CMAKE_CURRENT_SOURCE_DIR}"
	PUBLIC ENGINE_SHADER_PATH="${CMAKE_CURRENT_SOURCE_DIR}/../shaders"

)

target_link_directories(${TARGET_NAME} PUBLIC ${Vulkan_LIB_DIR})
target_link_libraries(${TARGET_NAME} PUBLIC glfw)
target_link_libraries(${TARGET_NAME} PUBLIC glad::glad)
target_link_libraries(${TARGET_NAME} PUBLIC opengl32 )
target_link_libraries(${TARGET_NAME} PUBLIC OpenAL::OpenAL)
target_link_libraries(${TARGET_NAME} PUBLIC assimp::assimp )
target_link_libraries(${TARGET_NAME} PUBLIC imgui::imgui)
target_link_libraries(${TARGET_NAME} PUBLIC ImGuizmo)
target_link_libraries(${TARGET_NAME} PUBLIC ImNodeFlow)
target_link_libraries(${TARGET_NAME} PUBLIC waveparser)
target_link_libraries(${TARGET_NAME} PUBLIC RTTR::Core)
target_link_libraries(${TARGET_NAME} PUBLIC spdlog::spdlog)
target_link_libraries(${TARGET_NAME} PUBLIC glm::glm-header-only)
target_link_libraries(${TARGET_NAME} PUBLIC cereal::cereal)
target_link_libraries(${TARGET_NAME} PUBLIC implot::implot)
target_link_libraries(${TARGET_NAME} PUBLIC msdf-atlas-gen::msdf-atlas-gen)
target_link_libraries(${TARGET_NAME} PUBLIC EnTT::EnTT)
target_link_libraries(${TARGET_NAME} PUBLIC mINI)
target_link_libraries(${TARGET_NAME} PUBLIC unofficial::omniverse-physx-sdk::sdk)
target_link_libraries(${TARGET_NAME} PUBLIC ${Vulkan_LIBRARIES})
target_link_libraries(${TARGET_NAME} PUBLIC shaderc_combined$<$<CONFIG:DEBUG>:d>)
target_link_libraries(${TARGET_NAME} PUBLIC spirv-cross-core$<$<CONFIG:DEBUG>:d>)
target_link_libraries(${TARGET_NAME} PUBLIC spirv-cross-glsl$<$<CONFIG:DEBUG>:d>)


target_include_directories(${TARGET_NAME} PUBLIC 
	${CMAKE_CURRENT_SOURCE_DIR}/src 
	${CMAKE_CURRENT_SOURCE_DIR}/src/BHive 
	${VENDOR_DIR}
	${Vulkan_INCLUDE_DIRS}
	${Stb_INCLUDE_DIR}
	${RENDERDOC_Dir}
)

target_precompile_headers(${TARGET_NAME} PUBLIC src/BHivePCH.h)

set(INSTALL_BIN "bin/${CMAKE_SYSTEM_NAME}/${CMAKE_BUILD_TYPE}")

install(TARGETS ${TARGET_NAME} DESTINATION ${INSTALL_BIN})
 