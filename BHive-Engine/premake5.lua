project "BHive-Engine"
    kind "SharedLib"
    language "C++"
    cppdialect "C++20"

    targetdir ("%{wks.location}/bin/" .. outputdir)
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

    files { "src/**.h", "src/**.cpp","src/**.inl" }

    includedirs {
        "src",
        "%{IncludeDir.vendor}",
        "%{IncludeDir.glfw}",
        "%{IncludeDir.vulkan}",
        "%{IncludeDir.cereal}",
        "%{IncludeDir.glm}",
        "%{IncludeDir.spdlog}",
        "%{IncludeDir.entt}",
        "%{IncludeDir.imgui}"
    }

    links { 
        "glfw", 
        "spdlog", 
        "imgui",
        "%{Library.vulkan}"
    }

    defines {
        "GLFW_INCLUDE_NONE",
        "BUILD_SHARED",
        "BUILD_DLL",
        "AL_LIBTYPE_STATIC",
	    "NOMINMAX",
	    "IMGUI_DEFINE_MATH_OPERATORS",
	    "SPDLOG_COMPILED_LIB",
	    "ENGINE_PATH='%{prj.location}'",
	    "ENGINE_SHADER_PATH='%{prj.location}/../shaders'"
    }

    filter "system:windows"
        defines { "_CRT_SECURE_NO_WARNINGS" }

    filter "configurations:Debug"
        defines { "BH_DEBUG" }
        symbols "On"

        links { 
            "%{Library.shaderc_debug}",
            "%{Library.spirv_cross_debug}",
            "%{Library.spirv_cross_glsl_debug}"
        }

    filter "configurations:RelWithDebInfo"
        defines { "BH_RELEASE" }
        optimize "On"
        symbols "On"

        links { 
            "%{Library.shaderc}",
            "%{Library.spirv_cross}",
            "%{Library.spirv_cross_glsl}"
        }

    filter "configurations:Release"
        defines { "BH_DIST" }
        optimize "On"
        symbols "Off"

         links { 
            "%{Library.shaderc}",
            "%{Library.spirv_cross}",
            "%{Library.spirv_cross_glsl}"
        }
