cmake_minimum_required(VERSION 3.8)

include(FetchContent)

FetchContent_Declare(
    ImGuizmo
    GIT_REPOSITORY https://github.com/R3sid3ntD1023/ImGuizmo.git
    GIT_TAG master
)

FetchContent_MakeAvailable(ImGuizmo)

#mINI
FetchContent_Declare(
	mINI
	GIT_REPOSITORY https://github.com/metayeti/mINI.git
	GIT_TAG 0.9.17
)
FetchContent_MakeAvailable(mINI)

add_subdirectory(ImNodeFlow)
