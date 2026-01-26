cmake_minimum_required(VERSION 3.10...3.24)

if(NOT DEFINED VULKAN_SDK_PATH)
	message(WARNING "VULKAN_SDK_PATH not defined before including shaderc.cmake")
    return()
endif()

set(_lib_dir "${VULKAN_SDK_PATH}/Lib")

if(NOT TARGET shaderc)
	file(GLOB _shaderc_files "${_lib_dir}/*shaderc*.lib")

	if(_shaderc_files)
		set(_shaderc_debug "")
		set(_shaderc_release "")

		foreach(_lib IN LISTS _shaderc_files)
			get_filename_component(_lib_name ${_lib} NAME)
			string(TOLOWER "${_lib_name}" _lib_name_lc)

			# match common naming: combined + debug suffix (e.g. shaderc_combinedd.lib or shaderc_combined-d.lib)
			if(_lib_name_lc MATCHES ".*combined.*d\\.lib$")
				set(_shaderc_debug ${_lib})
			# match release combined
			elseif(_lib_name_lc MATCHES ".*combined.*\\.lib$")
				# prefer non-debug combined for release
				if(NOT _shaderc_release)
					set(_shaderc_release ${_lib})
				endif()
			# fallback generic shaderc debug
			elseif(_lib_name_lc MATCHES ".*shaderc.*d\\.lib$")
				if(NOT _shaderc_debug)
					set(_shaderc_debug ${_lib})
				endif()
			# fallback generic shaderc release
			elseif(_lib_name_lc MATCHES ".*shaderc.*\\.lib$")
				if(NOT _shaderc_release)
					set(_shaderc_release ${_lib})
				endif()
			endif()
		endforeach()

		add_library(shaderc UNKNOWN IMPORTED)
		add_library(shaderc::shaderc ALIAS shaderc)

		if(_shaderc_debug AND _shaderc_release)
			set_target_properties(shaderc PROPERTIES
				IMPORTED_LOCATION_DEBUG "${_shaderc_debug}"
				IMPORTED_LOCATION_RELEASE "${_shaderc_release}"
				IMPORTED_LOCATION_RELWITHDEBINFO "${_shaderc_release}"
				IMPORTED_LOCATION_MINSIZEREL "${_shaderc_release}"
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

if(TARGET shaderc AND TARGET vulkan)
    target_link_libraries(vulkan INTERFACE shaderc::shaderc)
    message(STATUS "Linked shaderc to vulkan target")
endif()

