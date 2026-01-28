cmake_minimum_required(VERSION 3.10...3.24)

if(NOT DEFINED VULKAN_SDK_PATH)
	message(WARNING "VULKAN_SDK_PATH not defined before including spirv_cross_core.cmake")
    return()
endif()

set(_lib_dir "${VULKAN_SDK_PATH}/Lib")
set(TARGET_NAME spirv-cross-core)

# Detect spirv-cross core + glsl with per-config logic similar to shaderc above.
if(NOT TARGET  spirv-cross-core)
	file(GLOB _spirv_files "${_lib_dir}/*spirv-cross*.lib")

	if(_spirv_files)
		set(_spirv_core_debug "")
		set(_spirv_core_release "")

		foreach(_lib IN LISTS _spirv_files)
			get_filename_component(_lib_name ${_lib} NAME)
			string(TOLOWER "${_lib_name}" _lib_name_lc)

			# core / core-shared
			if(_lib_name_lc MATCHES ".*spirv-cross-core.*d\\.lib$")
				set(_spirv_core_debug ${_lib})
			elseif(_lib_name_lc MATCHES ".*spirv-cross-core.*\\.lib$")
				if(NOT _spirv_core_release)
					set(_spirv_core_release ${_lib})
				endif()
			elseif(_lib_name_lc MATCHES ".*spirv-cross-core-shared.*d\\.lib$")
				# prefer explicit shared debug if present
				if(NOT _spirv_core_debug)
					set(_spirv_core_debug ${_lib})
				endif()
			elseif(_lib_name_lc MATCHES ".*spirv-cross-core-shared.*\\.lib$")
				if(NOT _spirv_core_release)
					set(_spirv_core_release ${_lib})
				endif()

			endif()
		endforeach()

		add_library( ${TARGET_NAME} UNKNOWN IMPORTED)
		add_library( ${TARGET_NAME}::${TARGET_NAME} ALIAS  ${TARGET_NAME})

		set_target_properties( ${TARGET_NAME} PROPERTIES
				IMPORTED_LOCATION_DEBUG "${_spirv_core_debug}"
				IMPORTED_LOCATION_RELEASE "${_spirv_core_release}"
				IMPORTED_LOCATION_RELWITHDEBINFO "${_spirv_core_release}"
				IMPORTED_LOCATION_MINSIZEREL "${_spirv_core_release}"
				INTERFACE_LINK_LIBRARIES "${_spirv_release}"
			)
		message(STATUS "Found  ${TARGET_NAME} debug libs: ${_spirv_core_debug}")
		message(STATUS "Found  ${TARGET_NAME} release libs: ${_spirv_core_release}")
	endif()
endif()

if(TARGET ${TARGET_NAME} AND TARGET vulkan)
    target_link_libraries(vulkan INTERFACE  ${TARGET_NAME}::${TARGET_NAME})
    message(STATUS "Linked  ${TARGET_NAME} to vulkan target")
endif()