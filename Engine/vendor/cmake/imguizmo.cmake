FETCHCONTENT_DECLARE(
	ImGuizmo 
	GIT_REPOSITORY https://github.com/CedricGuillemet/ImGuizmo.git
	GIT_TAG master
    OVERRIDE_FIND_PACKAGE
)

FETCHCONTENT_MAKEAVAILABLE(ImGuizmo)

find_package(imgui CONFIG REQUIRED)

set(IMGUIZMO_SOURCES
   ${imguizmo_SOURCE_DIR}/src/ImGuizmo.cpp
   ${imguizmo_SOURCE_DIR}/src/ImSequencer.cpp
   ${imguizmo_SOURCE_DIR}/src/ImCurveEdit.cpp
   ${imguizmo_SOURCE_DIR}/src/GraphEditor.cpp
   ${imguizmo_SOURCE_DIR}/src/ImGradient.cpp
)

add_library(ImGuizmo STATIC ${IMGUIZMO_SOURCES})
add_library(ImGuizmo::ImGuizmo ALIAS ImGuizmo)

target_include_directories(ImGuizmo PUBLIC ${imguizmo_SOURCE_DIR}/src)
target_link_libraries(ImGuizmo PUBLIC imgui)