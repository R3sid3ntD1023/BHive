
set(VULKAN_VERSION "1.4.309.0")
set(ENV{VULKAN_SDK} "C:/VulkanSDK/${VULKAN_VERSION}")
set(VULKAN_DOWNLOAD_DIR ${CMAKE_CURRENT_SOURCE_DIR}/downloads)
set(VULKAN_SDK_DOWNLOAD_DIR ${VULKAN_DOWNLOAD_DIR}/VulkanSDK-${VULKAN_VERSION}-Installer.exe)
set(VULKAN_RUNTIME_DOWNLOAD_DIR ${VULKAN_DOWNLOAD_DIR}/VulkanRT-${VULKAN_VERSION}-Installer.exe)

if(NOT EXISTS ${VULKAN_SDK_DOWNLOAD_DIR} AND NOT EXISTS $ENV{VULKAN_SDK})
    file(DOWNLOAD https://sdk.lunarg.com/sdk/download/${VULKAN_VERSION}/windows/VulkanSDK-${VULKAN_VERSION}-Installer.exe 
        ${VULKAN_SDK_DOWNLOAD_DIR} STATUS status LOG log SHOW_PROGRESS)

    #download runtime files
    file(DOWNLOAD https://sdk.lunarg.com/sdk/download/${VULKAN_VERSION}/windows/VulkanRT-${VULKAN_VERSION}-Installer.exe
        ${VULKAN_RUNTIME_DOWNLOAD_DIR} STATUS status LOG log SHOW_PROGRESS)
endif()

if(NOT EXISTS $ENV{VULKAN_SDK})
    execute_process(
        COMMAND ${CMAKE_COMMAND} -E echo "Running vulkan sdk"
        COMMAND ${CMAKE_COMMAND} -E env "PATH=$ENV{PATH}" ${VULKAN_SDK_DOWNLOAD_DIR} 
        DEPENDS ${VULKAN_SDK_DOWNLOAD_DIR}
    RESULT_VARIABLE SDK_COMPLETED)

    if(SDK_COMPLETED)
        message(STATUS "Installed Vulkan SDK")
    else()
        message(FATAL_ERROR "Failed to install Vulkan SDK")
    endif()
else()
    message(STATUS "Using Vulkan SDK Path - $ENV{VULKAN_SDK}")
endif()

