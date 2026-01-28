cmake_minimum_required(VERSION 3.8...3.10)

FETCHCONTENT_DECLARE(
	imgui 
	GIT_REPOSITORY https://github.com/ocornut/imgui.git
	GIT_TAG docking
    OVERRIDE_FIND_PACKAGE
)

FETCHCONTENT_MAKEAVAILABLE(imgui)

set(IMGUI_SOURCES
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/misc/cpp/imgui_stdlib.cpp
    ${imgui_SOURCE_DIR}/imgui_demo.cpp
)

add_library(imgui STATIC ${IMGUI_SOURCES}) 
add_library(imgui::imgui ALIAS imgui)
target_include_directories(imgui PUBLIC "${imgui_SOURCE_DIR}" PUBLIC "${imgui_SOURCE_DIR}/misc/cpp")