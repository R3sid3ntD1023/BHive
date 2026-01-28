project "BHive-Editor"
    kind "ConsoleApp" -- change to "WindowedApp" or other if appropriate
    language "C++"
    cppdialect "C++20"

    targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

    files { "src/**.h", "src/**.cpp" }

    includedirs {"src"}

    links{ "BHive-Engine"}

    filter "system:windows"
        defines { "_CRT_SECURE_NO_WARNINGS" }
    filter {}

    filter "configurations:Debug"
        defines { "BH_DEBUG" }
        symbols "On"
    filter "configurations:RelWithDebInfo"
        defines { "BH_RELEASE" }
        optimize "On"
        symbols "On"
    filter "configurations:Release"
        defines { "BH_DIST" }
        optimize "On"
        symbols "Off"
    filter {}
