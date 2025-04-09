cmake_minimum_required (VERSION 3.8)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)

set(ASSIMP_TREAT_ALL_WARNINGS_AS_ERRORS OFF CACHE BOOL "" FORCE)
set(ASSIMP_INJECT_DEBUG_POSTFIX OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ASSIMP_INSTALL OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ALL_EXPORTERS_BY_DEFAULT OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_GLTF_IMPORTER ON CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_FBX_IMPORTER ON CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_OBJ_IMPORTER ON CACHE BOOL "" FORCE)

#openal
set(ALSOFT_EXAMPLES OFF CACHE BOOL "" FORCE)
set(ALSOFT_TESTS OFF CACHE BOOL "" FORCE)
#set(LIBTYPE "STATIC")
set(ALSOFT_INSTALL OFF CACHE BOOL "" FORCE)
set(ALSOFT_INSTALL_CONFIG OFF CACHE BOOL "" FORCE)
set(ALSOFT_INSTALL_HRTF_DATA OFF CACHE BOOL "" FORCE)
set(ALSOFT_INSTALL_AMBDEC_PRESETS OFF CACHE BOOL "" FORCE)
set(ALSOFT_INSTALL_EXAMPLES OFF CACHE BOOL "" FORCE)
set(ALSOFT_INSTALL_UTILS OFF CACHE BOOL "" FORCE)
set(ALSOFT_UPDATE_BUILD_VERSION ON CACHE BOOL "" FORCE)

#imnodes
set(IMNODES_IMGUI_TARGET_NAME imgui)

#rttr
set(BUILD_UNIT_TESTS OFF CACHE BOOL "" FORCE)
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_DOCUMENTATION OFF CACHE BOOL "" FORCE)
set(BUILD_INSTALLER OFF CACHE BOOL "" FORCE)
set(BUILD_PACKAGE OFF CACHE BOOL "" FORCE)

set(SPDLOG_BUILD_SHARED ON CACHE BOOL "" FORCE)
#set(SPDLOG_ENABLE_PCH ON CACHE BOOL "" FORCE)
#set(SPDLOG_USE_STD_FORMAT ON CACHE BOOL "" FORCE)

#cereal
set(BUILD_SANDBOX OFF CACHE BOOL "" FORCE)
set(BUILD_DOC OFF CACHE BOOL "" FORCE)
set(SKIP_PERFORMANCE_COMPARISON ON CACHE BOOL "" FORCE)



#glfw
FetchContent_Declare(
	glfw
	GIT_REPOSITORY https://github.com/glfw/glfw.git
	GIT_TAG master
)
list( APPEND components glfw)

FetchContent_Declare(
	glad
	GIT_REPOSITORY https://github.com/R3sid3ntD1023/glad.git
	GIT_TAG main
)
list(APPEND components glad)

#glm
FetchContent_Declare(
	glm
	GIT_REPOSITORY https://github.com/g-truc/glm.git
	GIT_TAG master
)
list( APPEND components glm)

#spdlog
FetchContent_Declare(
	spdlog
	GIT_REPOSITORY https://github.com/gabime/spdlog.git
	GIT_TAG v1.15.0
)
list( APPEND components spdlog)
	
#imgui
FetchContent_Declare(
	imgui
	GIT_REPOSITORY https://github.com/R3sid3ntD1023/imgui.git
	GIT_TAG docking
	OVERRIDE_FIND_PACKAGE
)
list( APPEND components imgui)

FetchContent_Declare(
    ImGuizmo
    GIT_REPOSITORY https://github.com/R3sid3ntD1023/ImGuizmo.git
    GIT_TAG master
)

list(APPEND components ImGuizmo)

#stb
FetchContent_Declare(
	stb
	GIT_REPOSITORY https://github.com/R3sid3ntD1023/stb.git
	GIT_TAG master
)
list(APPEND components stb)

#wave
FetchContent_Declare(
	waveparser
	GIT_REPOSITORY https://github.com/R3sid3ntD1023/WaveParser.git
	GIT_TAG master
)
list(APPEND components waveparser)


#rttr
FetchContent_Declare(
	rttr
	GIT_REPOSITORY https://github.com/R3sid3ntD1023/rttr.git
	GIT_TAG master
)
list(APPEND components rttr)

FetchContent_Declare(
	cereal
	GIT_REPOSITORY https://github.com/USCiLab/cereal.git
	GIT_TAG v1.3.2
)
list(APPEND components cereal)


FETCHCONTENT_DECLARE(
	msdf-atlas-gen
	GIT_REPOSITORY https://github.com/Chlumsky/msdf-atlas-gen.git
	GIT_TAG master
)
list(APPEND components msdf-atlas-gen)

FetchContent_Declare(
	ImPlot
	GIT_REPOSITORY https://github.com/R3sid3ntD1023/implot.git
	GIT_TAG master
)
list(APPEND components ImPlot)

FetchContent_Declare(
	OpenAL
	GIT_REPOSITORY https://github.com/kcat/openal-soft.git
	GIT_TAG master
)
list(APPEND components OpenAL)

if(components)
	FETCHCONTENT_MAKEAVAILABLE(${components})
endif()

