cmake_minimum_required(VERSION 3.8)

add_subdirectory(vendor)

set(SRC_DIR  ${CMAKE_CURRENT_SOURCE_DIR}/src)

file(GLOB_RECURSE EDITOR_SRC CMAKE_CONFIGURE_DEPENDS ${SRC_DIR}/**  PARENT_SCOPE)

set(PROJECT_NAME Editor)


add_executable(${PROJECT_NAME} ${EDITOR_SRC})
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)
target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src" )
target_link_libraries(${PROJECT_NAME} PUBLIC Engine PUBLIC ImNodeFlow PUBLIC ImGuizmo PUBLIC mINI )

target_compile_definitions(${PROJECT_NAME} PUBLIC RESOURCES_PATH="${CMAKE_CURRENT_SOURCE_DIR}/resources" PUBLIC EDITOR_DATA="${CMAKE_CURRENT_SOURCE_DIR}/data") #asset paths
target_compile_options(${PROJECT_NAME} PRIVATE /ZI PRIVATE /INCREMENTAL PRIVATE /MP)
target_compile_definitions(${PROJECT_NAME} PUBLIC _CRT_SECURE_NO_WARNINGS PUBLIC IMGUI_DEFINE_MATH_OPERATORS)

