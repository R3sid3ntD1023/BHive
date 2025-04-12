cmake_minimum_required(VERSION 3.8)

file(GLOB_RECURSE SRC CMAKE_CONFIG_DEPENDS src/**)
add_executable(VulkanTest ${SRC})
set_target_properties(VulkanTest PROPERTIES
	CXX_STANDARD 20
	CXX_STANDARD_REQUIRED YES
)
target_link_libraries(VulkanTest PUBLIC Engine)