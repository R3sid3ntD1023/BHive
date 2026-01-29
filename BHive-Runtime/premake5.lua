project "BHive-Runtime"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++20"
    staticruntime "off"

    targetdir ("%{wks.location}/bin/" .. outputdir)
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
