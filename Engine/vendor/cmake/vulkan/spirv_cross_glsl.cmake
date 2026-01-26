cmake_minimum_required(VERSION 3.10...3.24)

if(NOT DEFINED VULKAN_SDK_PATH)
	message(WARNING "VULKAN_SDK_PATH not defined before including spirv-cross_glsl.cmake")
    return()
endif()

set(_lib_dir "${VULKAN_SDK_PATH}/Lib")
set(TARGET_NAME spirv-cross-glsl)

# Detect spirv-cross core + glsl with per-config logic similar to shaderc above.
if(NOT TARGET ${TARGET_NAME})
	file(GLOB _spirv_files "${_lib_dir}/*spirv-cross*.lib")

	if(_spirv_files)
		set(_spirv_glsl_debug "")
		set(_spirv_glsl_release "")

		foreach(_lib IN LISTS _spirv_files)
			get_filename_component(_lib_name ${_lib} NAME)
			string(TOLOWER "${_lib_name}" _lib_name_lc)

			# glsl / glsl-shared
			if(_lib_name_lc MATCHES ".*spirv-cross-glsl.*d\\.lib$")
				set(_spirv_glsl_debug ${_lib})
			elseif(_lib_name_lc MATCHES ".*spirv-cross-glsl.*\\.lib$")
				if(NOT _spirv_glsl_release)
					set(_spirv_glsl_release ${_lib})
				endif()
			elseif(_lib_name_lc MATCHES ".*spirv-cross-glsl-shared.*d\\.lib$")
				if(NOT _spirv_glsl_debug)
					set(_spirv_glsl_debug ${_lib})
				endif()
			elseif(_lib_name_lc MATCHES ".*spirv-cross-glsl-shared.*\\.lib$")
				if(NOT _spirv_glsl_release)
					set(_spirv_glsl_release ${_lib})
				endif()
			endif()
		endforeach()

		
		add_library(${TARGET_NAME} UNKNOWN IMPORTED)
		add_library(${TARGET_NAME}::${TARGET_NAME} ALIAS ${TARGET_NAME})

		set_target_properties(${TARGET_NAME} PROPERTIES
				IMPORTED_LOCATION_DEBUG "${_spirv_glsl_debug}"
				IMPORTED_LOCATION_RELEASE "${_spirv_glsl_release}"
				IMPORTED_LOCATION_RELWITHDEBINFO "${_spirv_glsl_release}"
				IMPORTED_LOCATION_MINSIZEREL "${_spirv_glsl_release}"
				INTERFACE_LINK_LIBRARIES "${_spirv_glsl_release}"
			)
		message(STATUS "Found ${TARGET_NAME} debug libs: ${_spirv_glsl_debug}")
		message(STATUS "Found ${TARGET_NAME} release libs: ${_spirv_glsl_release}")
	endif()
endif()

if(TARGET ${TARGET_NAME} AND TARGET vulkan)
    target_link_libraries(vulkan INTERFACE ${TARGET_NAME}::${TARGET_NAME})
    message(STATUS "Linked ${TARGET_NAME} to vulkan target")
endif()