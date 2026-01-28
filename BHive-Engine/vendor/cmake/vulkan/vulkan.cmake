cmake_minimum_required(VERSION 3.8...3.24)

execute_process(COMMAND ${CMAKE_COMMAND} -E echo "Configuring Vulkan SDK")

if(DEFINED ENV{VULKAN_SDK})
    message(STATUS "Using existing VULKAN_SDK at $ENV{VULKAN_SDK}")
else()
    set(VULKAN_VERSION "1.4.335.0")

    message(STATUS "VULKAN_SDK not defined, will attempt to download and install Vulkan SDK version ${VULKAN_VERSION}")

    set(ENV{VULKAN_SDK} "C:/VulkanSDK/${VULKAN_VERSION}")
    set(VULKAN_SDK_DOWNLOAD_DIR ${CMAKE_SOURCE_DIR}/downloads)
    set(VULKAN_SDK_NAME vulkansdk-windows-X64-${VULKAN_VERSION}.exe)
    set(VULKAN_RT_NAME VulkanRT-X64-${VULKAN_VERSION}-Installer.exe)
    set(VULKAN_SDK_DOWNLOAD ${VULKAN_SDK_DOWNLOAD_DIR}/${VULKAN_SDK_NAME})
    set(VULKAN_RT_DOWNLOAD ${VULKAN_SDK_DOWNLOAD_DIR}/${VULKAN_RT_NAME})

    #download vulkan sdk if not present
    if(NOT EXISTS ${VULKAN_SDK_DOWNLOAD})
        file(DOWNLOAD https://sdk.lunarg.com/sdk/download/${VULKAN_VERSION}/windows/${VULKAN_SDK_NAME} 
            ${VULKAN_SDK_DOWNLOAD} STATUS status LOG log SHOW_PROGRESS)
    endif()

    #download vulkan runtime if not present
    if(NOT EXISTS ${VULKAN_RT_DOWNLOAD})
        file(DOWNLOAD https://sdk.lunarg.com/sdk/download/${VULKAN_VERSION}/windows/${VULKAN_RT_NAME} 
            ${VULKAN_RT_DOWNLOAD}  STATUS status LOG log SHOW_PROGRESS)
    endif()

    #install vulkan runtime and sdk
    execute_process(
        COMMAND ${CMAKE_COMMAND} -E echo "Running vulkan runtime"
        COMMAND powershell -NoProfile -ExecutionPolicy Bypass -Command "Start-Process -FilePath '${VULKAN_RT_DOWNLOAD}' -Verb RunAs -Wait" 
        RESULT_VARIABLE VULKAN_RT_EXIT_CODE
    )

    execute_process(
        COMMAND ${CMAKE_COMMAND} -E echo "Running vulkan sdk"
        COMMAND powershell -NoProfile -ExecutionPolicy Bypass -Command "Start-Process -FilePath '${VULKAN_SDK_DOWNLOAD}' -Verb RunAs -Wait"
        RESULT_VARIABLE VULKAN_SDK_EXIT_CODE
    )

    if(${VULKAN_SDK_EXIT_CODE} EQUAL 0 AND ${VULKAN_RT_EXIT_CODE} EQUAL 0 )
        message(STATUS "Installed Vulkan sdk and Vulkan runtime")
    else()
        message(FATAL_ERROR "Failed to install Vulkan sdk and Vulkan runtime")
    endif()
endif()

set(VULKAN_SDK_PATH $ENV{VULKAN_SDK})

if(VULKAN_SDK_PATH)
    file(TO_CMAKE_PATH "${VULKAN_SDK_PATH}" VULKAN_SDK_PATH)
endif()

if(VULKAN_SDK_PATH)
    if(EXISTS "${VULKAN_SDK_PATH}")
        list(APPEND CMAKE_PREFIX_PATH "${VULKAN_SDK_PATH}")
        list(APPEND CMAKE_INCLUDE_PATH "${VULKAN_SDK_PATH}/Include")
        list(APPEND CMAKE_LIBRARY_PATH "${VULKAN_SDK_PATH}/Lib")
        message(STATUS "Appended Vulkan SDK paths from VULKAN_SDK: ${VULKAN_SDK_PATH}")
    else()
        message(WARNING "VULKAN_SDK is set to '${VULKAN_SDK_PATH}' but that path does not exist on disk.")
    endif()
else()
    message(WARNING "VULKAN_SDK environment variable is not set. find_package may find a different system Vulkan.")
endif()

set(Vulkan_FOUND FALSE)
if(VULKAN_SDK_PATH AND EXISTS "${VULKAN_SDK_PATH}")
    set(Vulkan_INCLUDE_DIRS "${VULKAN_SDK_PATH}/Include")

    find_library(
        _VULKAN_LIB_NAMED
        NAMES vulkan-1 vulkan
        PATHS
            "${VULKAN_SDK_PATH}/Lib"
            "${VULKAN_SDK_PATH}/lib"
        NO_DEFAULT_PATH
    )

    if(_VULKAN_LIB_NAMED)
        set(Vulkan_LIBRARIES "${_VULKAN_LIB_NAMED}")
        set(Vulkan_FOUND TRUE)
    endif()
endif()

if(NOT Vulkan_FOUND)
    include(FindVulkan)
endif()

message(STATUS "Vulkan INCLUDE_DIRS: ${Vulkan_INCLUDE_DIRS}")
message(STATUS "Vulkan LIBRARIES: ${Vulkan_LIBRARIES}")

if(Vulkan_FOUND)
    if(NOT Vulkan_INCLUDE_DIRS)
        set(Vulkan_INCLUDE_DIRS "")
    endif()
    if(NOT Vulkan_LIBRARIES)
        set(Vulkan_LIBRARIES "")
    endif()

    set(TARGET_NAME vulkan)
    if(NOT TARGET ${TARGET_NAME})
        file(GLOB_RECURSE HEADER_FILES CMAKE_CONFIGURE_DEPENDS ${Vulkan_INCLUDE_DIRS}/**.h)

        add_library(${TARGET_NAME} INTERFACE ${HEADER_FILES})
        add_library(${TARGET_NAME}::${TARGET_NAME} ALIAS ${TARGET_NAME})
        
         # Provide include dirs to consumers
        if(Vulkan_INCLUDE_DIRS)
            target_include_directories(${TARGET_NAME} INTERFACE ${Vulkan_INCLUDE_DIRS})
        endif()

        # Provide link library (loader) to consumers
        if(Vulkan_LIBRARIES)
            # If Vulkan_LIBRARIES is a full path, set IMPORTED_LOCATION so linking works.
            # Otherwise set INTERFACE_LINK_LIBRARIES to the library name.
            get_filename_component(_maybe_lib_dir "${Vulkan_LIBRARIES}" DIRECTORY)
            if(_maybe_lib_dir)
                set_target_properties(${TARGET_NAME} PROPERTIES
                    IMPORTED_LOCATION "${Vulkan_LIBRARIES}"
                    INTERFACE_LINK_LIBRARIES "${Vulkan_LIBRARIES}"
                )
            else()
                set_target_properties(${TARGET_NAME} PROPERTIES
                    INTERFACE_LINK_LIBRARIES "${Vulkan_LIBRARIES}"
                )
            endif()
        endif()
    endif()
endif()

include(cmake/vulkan/shaderc.cmake)
include(cmake/vulkan/spirv_cross_core.cmake)
include(cmake/vulkan/spirv_cross_glsl.cmake)


