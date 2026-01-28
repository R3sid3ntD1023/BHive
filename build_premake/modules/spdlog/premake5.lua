project "spdlog"
	kind "SharedLib"
	language "C++"

	targetdir ("%{wks.location}/bin/" .. outputdir)
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		(vendordir .. "/spdlog/include/**"),
		(vendordir .. "/spdlog/src/**"),
	}
	
	includedirs
	{
		(vendordir .. "/spdlog/include")
	}

	defines
	{
		"SPDLOG_COMPILED_LIB"
	}

	buildoptions
	{
		"/utf-8"
	}

	filter "configurations:Debug"
        symbols "On"

    filter "configurations:RelWithDebInfo"
        optimize "On"
        symbols "On"

    filter "configurations:Release"
        optimize "On"
        symbols "Off"