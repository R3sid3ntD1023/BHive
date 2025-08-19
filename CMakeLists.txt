cmake_minimum_required(VERSION 3.8)

project(BHive VERSION 1.0 LANGUAGES CXX C)

set(CXX_STANDARD 20)
set(CXX_STANDARD_REQUIRED ON)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "CMake")

set(BHIVE_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(CMAKE_INSTALL_PREFIX "${BHIVE_ROOT_DIR}/install")

#enable_testing()


add_subdirectory(Engine)
add_subdirectory(BHive-Editor)



