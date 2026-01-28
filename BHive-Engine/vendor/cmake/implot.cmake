FETCHCONTENT_DECLARE(
	implot
	GIT_REPOSITORY https://github.com/epezent/implot.git
	GIT_TAG master
    OVERRIDE_FIND_PACKAGE
)

FETCHCONTENT_MAKEAVAILABLE(implot)

find_package(imgui CONFIG REQUIRED)

option(IMPLOT_DEMO "Build impot_demo.cpp" ON)

set(IMPLOT_SOURCES
    ${implot_SOURCE_DIR}/implot.cpp
    ${implot_SOURCE_DIR}/implot_items.cpp
)

if(IMPLOT_DEMO)
  list(APPEND IMPLOT_SOURCES ${implot_SOURCE_DIR}/implot_demo.cpp)
endif()

add_library(implot STATIC ${IMPLOT_SOURCES})
add_library(implot::implot ALIAS implot)
target_include_directories(implot PUBLIC ${implot_SOURCE_DIR})
target_link_libraries(implot PUBLIC imgui)