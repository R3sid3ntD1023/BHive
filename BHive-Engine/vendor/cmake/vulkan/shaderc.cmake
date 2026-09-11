cmake_minimum_required(VERSION 3.10...3.24)

if(NOT DEFINED VULKAN_SDK_PATH)
	message(WARNING "VULKAN_SDK_PATH not defined before including shaderc.cmake")
    return()
endif()

set(_lib_dir "${VULKAN_SDK_PATH}/Lib")
set(TARGET_NAME shaderc)

if(NOT TARGET shaderc)
	file(GLOB _shaderc_files "${_lib_dir}/*shaderc*.lib")

	if(_shaderc_files)
		set(_shaderc_debug "")
		set(_shaderc_release "")
		set(_shaderc_debug_fallback "")
		set(_shaderc_release_fallback "")

		foreach(_lib IN LISTS _shaderc_files)
			get_filename_component(_lib_name "${_lib}" NAME)
			string(TOLOWER "${_lib_name}" _lib_name_lc)

			# match common naming: combined + debug suffix (e.g. shaderc_combinedd.lib or shaderc_combined-d.lib)
			if(_lib_name_lc MATCHES ".*combined.*d\\.lib$")
				set(_shaderc_debug "${_lib}")
			# match release combined
			elseif(_lib_name_lc MATCHES ".*combined.*\\.lib$")
				# prefer non-debug combined for release
				set(_shaderc_release "${_lib}")
			# fallback generic shaderc debug
			elseif(_lib_name_lc MATCHES ".*shaderc.*d\\.lib$")
					set(_shaderc_debug_fallback "${_lib}")
			# fallback generic shaderc release
			elseif(_lib_name_lc MATCHES ".*shaderc.*\\.lib$")
				set(_shaderc_release_fallback "${_lib}")
			endif()
		endforeach()

		if(NOT _shaderc_release)
			set(_shaderc_release ${_shaderc_release_fallback})
		endif()

		if(NOT _shaderc_debug)
			set(_shaderc_debug ${_shaderc_debug_fallback})
		endif()

		add_library(${TARGET_NAME} UNKNOWN IMPORTED)
		add_library(${TARGET_NAME}::${TARGET_NAME} ALIAS ${TARGET_NAME})

		if(_shaderc_debug AND _shaderc_release)
			set_target_properties(shaderc PROPERTIES
				IMPORTED_LOCATION_DEBUG "${_shaderc_debug}"
				IMPORTED_LOCATION_RELEASE "${_shaderc_release}"
				IMPORTED_LOCATION_RELWITHDEBINFO "${_shaderc_release}"
				IMPORTED_LOCATION_MINSIZEREL "${_shaderc_release}"
				INTERFACE_LINK_LIBRARIES "${_shaderc_release}"
			)
			message(STATUS "Found shaderc debug: ${_shaderc_debug}")
			message(STATUS "Found shaderc release: ${_shaderc_release}")
		elseif(_shaderc_release)
			set_target_properties(shaderc PROPERTIES IMPORTED_LOCATION "${_shaderc_release}")
			message(STATUS "Found shaderc library: ${_shaderc_release} (used for all configs)")
		elseif(_shaderc_debug)
			set_target_properties(shaderc PROPERTIES IMPORTED_LOCATION "${_shaderc_debug}")
			message(STATUS "Found shaderc debug library: ${_shaderc_debug} (used for all configs)")
		else()
			message(WARNING "shaderc libraries not found in ${_lib_dir}")
		endif()
	endif()
endif()

if(TARGET ${TARGET_NAME} AND TARGET vulkan)
    target_link_libraries(vulkan INTERFACE ${TARGET_NAME}::${TARGET_NAME})
    message(STATUS "Linked shaderc to vulkan target")
endif()

