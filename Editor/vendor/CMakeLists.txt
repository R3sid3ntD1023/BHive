cmake_minimum_required(VERSION 3.8)

include(FetchContent)

set(FECTHCONTENT_BASE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
FetchContent_Declare(
    ImGuizmo
    GIT_REPOSITORY https://github.com/R3sid3ntD1023/ImGuizmo.git
    GIT_TAG master
)

FetchContent_MakeAvailable(ImGuizmo)