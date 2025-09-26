cmake_minimum_required(VERSION 3.8...3.10)

file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS *.cpp)
file(GLOB_RECURSE HEADER_FILES CONFIGURE_DEPENDS *.h)

add_executable(BHive-Runtime ${SOURCE_FILES} ${HEADER_FILES})
set_target_properties(BHive-Runtime PROPERTIES
	CXX_STANDARD 20
	CXX_STANDARD_REQUIRED YES
)
target_link_libraries(BHive-Runtime PUBLIC BHive-Engine)