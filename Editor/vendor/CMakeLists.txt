cmake_minimum_required(VERSION 3.8)

include(FetchContent)

set(FECTHCONTENT_BASE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
FetchContent_Declare(
    ImGuizmo
    GIT_REPOSITORY https://github.com/R3sid3ntD1023/ImGuizmo.git
    GIT_TAG master
    SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/ImGuizmo"
)

FetchContent_MakeAvailable(ImGuizmo)

include(FetchContent)
FetchContent_Declare(ImNodeFlow
     GIT_REPOSITORY https://github.com/Fattorino/ImNodeFlow.git
     GIT_TAG master
     SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/ImNodeFlow"
)
FetchContent_MakeAvailable(ImNodeFlow)

#mINI
FetchContent_Declare(
	mINI
	GIT_REPOSITORY https://github.com/metayeti/mINI.git
	GIT_TAG 0.9.17
    SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/mINI"
)
FetchContent_MakeAvailable(mINI)
