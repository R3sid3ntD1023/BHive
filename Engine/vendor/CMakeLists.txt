cmake_minimum_required(VERSION 3.8)

include(FETCHCONTENT)

set(MSDF_ATLAS_BUILD_STANDALONE OFF CACHE BOOL "")

FETCHCONTENT_DECLARE(
	mINI
	GIT_REPOSITORY https://github.com/metayeti/mINI.git
	GIT_TAG 0.9.18
	OVERRIDE_FIND_PACKAGE
)

list(APPEND components mINI)

FetchContent_Declare(
	waveparser
	GIT_REPOSITORY https://github.com/R3sid3ntD1023/WaveParser.git
	GIT_TAG master
)
list(APPEND components waveparser)

FETCHCONTENT_DECLARE(
	msdf-atlas-gen
	GIT_REPOSITORY https://github.com/Chlumsky/msdf-atlas-gen.git
	GIT_TAG master
)
list(APPEND components msdf-atlas-gen)

FETCHCONTENT_DECLARE(
	imguizmo
	GIT_REPOSITORY https://github.com/R3sid3ntD1023/ImGuizmo.git
	GIT_TAG master
)
list(APPEND components imguizmo)

#imnodeflow
FetchContent_Declare(
    ImNodeFlow
    GIT_REPOSITORY https://github.com/R3sid3ntD1023/ImNodeFlow.git
    GIT_TAG master
)

list(APPEND components ImNodeFlow)

FetchContent_Declare(
	imgui
	GIT_REPOSITORY https://github.com/R3sid3ntD1023/imgui.git
	GIT_TAG docking
	OVERRIDE_FIND_PACKAGE
)
list(APPEND components imgui)

FetchContent_Declare(
	implot
	GIT_REPOSITORY https://github.com/R3sid3ntD1023/implot.git
	GIT_TAG master
	OVERRIDE_FIND_PACKAGE
)
list(APPEND components implot)

if(components)
	FETCHCONTENT_MAKEAVAILABLE(${components})
endif()

