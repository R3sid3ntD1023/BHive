cmake_minimum_required(VERSION 3.8)

include(FETCHCONTENT)

project(BHive VERSION 1.0 LANGUAGES CXX C)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
#enable_testing()

set(Vulkan_SDK_PATH "C:/VulkanSDK/1.4.309.0")
set(ENV{VULKAN_SDK} ${Vulkan_SDK_PATH})
set(VULKAN_DOWNLOAD_DIR ${CMAKE_CURRENT_SOURCE_DIR}/downloads/VulkanSDK-1.4.309.0-Installer.exe)

if(NOT EXISTS ${VULKAN_DOWNLOAD_DIR} AND NOT EXISTS ${Vulkan_SDK_PATH})
    file(DOWNLOAD https://sdk.lunarg.com/sdk/download/1.4.309.0/windows/VulkanSDK-1.4.309.0-Installer.exe 
        ${VULKAN_DOWNLOAD_DIR} STATUS status LOG log SHOW_PROGRESS)
endif()

if(NOT EXISTS ${Vulkan_SDK_PATH})
    execute_process(
    COMMAND ${CMAKE_COMMAND} -E echo "Running vulkan sdk"
    COMMAND ${CMAKE_COMMAND} -E env "PATH=$ENV{PATH}" ${VULKAN_DOWNLOAD_DIR} 
    DEPENDS ${VULKAN_DOWNLOAD_DIR}
    RESULT_VARIABLE COMPLETED)

    if(EXISTS ${Vulkan_SDK_PATH})
        message(STATUS "Installed Vulkan SDK")
    else()
        message(FATAL_ERROR "Failed to install Vulkan SDK")
    endif()
endif()



foreach( OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES} )
    string( TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG )
    set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_SOURCE_DIR}/bin/lib/${CMAKE_SYSTEM_NAME}/${CMAKE_BUILD_TYPE} )
    set( CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_SOURCE_DIR}/bin/${CMAKE_SYSTEM_NAME}/${CMAKE_BUILD_TYPE} )
    set( CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_SOURCE_DIR}/bin/${CMAKE_SYSTEM_NAME}/${CMAKE_BUILD_TYPE} )
endforeach( OUTPUTCONFIG CMAKE_CONFIGURATION_TYPES )


add_subdirectory(Engine)
add_subdirectory(Inspectors)
add_subdirectory(projects)
add_subdirectory(BHive-Editor)
#add_subdirectory(ReflectionBuilder)



