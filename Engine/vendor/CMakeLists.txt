cmake_minimum_required (VERSION 3.8)

# Disable unnecessary build options for dependencies
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

# OpenAL
set(ALSOFT_EXAMPLES OFF CACHE BOOL "" FORCE)
set(ALSOFT_TESTS OFF CACHE BOOL "" FORCE)
set(ALSOFT_INSTALL OFF CACHE BOOL "" FORCE)
set(ALSOFT_INSTALL_CONFIG OFF CACHE BOOL "" FORCE)
set(ALSOFT_INSTALL_HRTF_DATA OFF CACHE BOOL "" FORCE)
set(ALSOFT_INSTALL_AMBDEC_PRESETS OFF CACHE BOOL "" FORCE)
set(ALSOFT_INSTALL_EXAMPLES OFF CACHE BOOL "" FORCE)
set(ALSOFT_INSTALL_UTILS OFF CACHE BOOL "" FORCE)
set(ALSOFT_UPDATE_BUILD_VERSION ON CACHE BOOL "" FORCE)

# ReactPhysics3D
set(RP3D_COMPILE_TESTBED OFF CACHE BOOL "" FORCE)
set(RP3D_PROFILING_ENABLED OFF CACHE BOOL "" FORCE)
set(RP3D_DOUBLE_PRECISION_ENABLED OFF CACHE BOOL "" FORCE)
set(RP3D_GENERATE_DOCUMENTATION OFF CACHE BOOL "" FORCE)

# ImNodes
set(IMNODES_IMGUI_TARGET_NAME imgui)

# RTTR
set(BUILD_UNIT_TESTS OFF CACHE BOOL "" FORCE)
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_DOCUMENTATION OFF CACHE BOOL "" FORCE)
set(BUILD_INSTALLER OFF CACHE BOOL "" FORCE)
set(BUILD_PACKAGE OFF CACHE BOOL "" FORCE)

# Spdlog
set(SPDLOG_BUILD_SHARED ON CACHE BOOL "" FORCE)

# Cereal
set(BUILD_SANDBOX OFF CACHE BOOL "" FORCE)
set(BUILD_DOC OFF CACHE BOOL "" FORCE)
set(SKIP_PERFORMANCE_COMPARISON ON CACHE BOOL "" FORCE)

include(FetchContent)

# Define common repository URLs
set(REPO_URL_GLFW https://github.com/glfw/glfw.git)
set(REPO_URL_GLAD https://github.com/R3sid3ntD1023/glad.git)
set(REPO_URL_GLM https://github.com/g-truc/glm.git)
set(REPO_URL_SPDLOG https://github.com/gabime/spdlog.git)
set(REPO_URL_IMGUI https://github.com/R3sid3ntD1023/imgui.git)
set(REPO_URL_IMGUIZMO https://github.com/R3sid3ntD1023/ImGuizmo.git)
set(REPO_URL_STB https://github.com/R3sid3ntD1023/stb.git)
set(REPO_URL_REACTPHYSICS3D https://github.com/DanielChappuis/reactphysics3d.git)
set(REPO_URL_WAVEPARSER https://github.com/R3sid3ntD1023/WaveParser.git)
set(REPO_URL_RTTR https://github.com/R3sid3ntD1023/rttr.git)
set(REPO_URL_CEREAL https://github.com/USCiLab/cereal.git)
set(REPO_URL_MSDF_ATLAS_GEN https://github.com/Chlumsky/msdf-atlas-gen.git)
set(REPO_URL_IMPLOT https://github.com/R3sid3ntD1023/implot.git)
set(REPO_URL_OPENAL https://github.com/kcat/openal-soft.git)

# Declare dependencies
FetchContent_Declare(
    glfw
    GIT_REPOSITORY ${REPO_URL_GLFW}
    GIT_TAG master
)
list(APPEND components glfw)

FetchContent_Declare(
    glad
    GIT_REPOSITORY ${REPO_URL_GLAD}
    GIT_TAG main
)
list(APPEND components glad)

FetchContent_Declare(
    glm
    GIT_REPOSITORY ${REPO_URL_GLM}
    GIT_TAG master
)
list(APPEND components glm)

FetchContent_Declare(
    spdlog
    GIT_REPOSITORY ${REPO_URL_SPDLOG}
    GIT_TAG v1.15.0
)
list(APPEND components spdlog)

FetchContent_Declare(
    imgui
    GIT_REPOSITORY ${REPO_URL_IMGUI}
    GIT_TAG docking
    OVERRIDE_FIND_PACKAGE
)
list(APPEND components imgui)

FetchContent_Declare(
    ImGuizmo
    GIT_REPOSITORY ${REPO_URL_IMGUIZMO}
    GIT_TAG master
)
list(APPEND components ImGuizmo)

FetchContent_Declare(
    stb
    GIT_REPOSITORY ${REPO_URL_STB}
    GIT_TAG master
)
list(APPEND components stb)

FetchContent_Declare(
    reactphysics3d
    GIT_REPOSITORY ${REPO_URL_REACTPHYSICS3D}
    GIT_TAG master
)
list(APPEND components reactphysics3d)

FetchContent_Declare(
    waveparser
    GIT_REPOSITORY ${REPO_URL_WAVEPARSER}
    GIT_TAG master
)
list(APPEND components waveparser)

FetchContent_Declare(
    rttr
    GIT_REPOSITORY ${REPO_URL_RTTR}
    GIT_TAG master
)
list(APPEND components rttr)

FetchContent_Declare(
    cereal
    GIT_REPOSITORY ${REPO_URL_CEREAL}
    GIT_TAG v1.3.2
)
list(APPEND components cereal)

FetchContent_Declare(
    msdf-atlas-gen
    GIT_REPOSITORY ${REPO_URL_MSDF_ATLAS_GEN}
    GIT_TAG master
)
list(APPEND components msdf-atlas-gen)

FetchContent_Declare(
    ImPlot
    GIT_REPOSITORY ${REPO_URL_IMPLOT}
    GIT_TAG master
)
list(APPEND components ImPlot)

FetchContent_Declare(
    OpenAL
    GIT_REPOSITORY ${REPO_URL_OPENAL}
    GIT_TAG master
)
list(APPEND components OpenAL)

# Make all declared content available
if(components)
    FetchContent_MakeAvailable(${components})
endif()

if(MSVC)
	target_compile_options(reactphysics3d PUBLIC /W0 )
endif()
