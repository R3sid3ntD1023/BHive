project "imgui"
	kind "StaticLib"
	language "C++"
	cppdialect "C++20"

	targetdir ("%{wks.location}/bin/" .. outputdir)
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	files { 
		(vendordir .. "/imgui/*.cpp"),
		(vendordir .. "/imgui/misc/cpp/*.cpp"),
	}

	includedirs 	{
		(vendordir .. "/imgui"),
	}

	filter "configurations:Debug"
        symbols "On"

    filter "configurations:RelWithDebInfo"
        optimize "On"
        symbols "On"

    filter "configurations:Release"
        optimize "On"
        symbols "Off"