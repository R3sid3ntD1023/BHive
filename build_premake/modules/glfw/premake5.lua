project "glfw"
	kind "StaticLib"
	language "C"

	targetdir ("%{wks.location}/bin/" .. outputdir)
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	
	files
	{
		(vendordir .. "/glfw/include/GLFW/glfw3.h"),
		(vendordir .. "/glfw/include/GLFW/glfw3native.h"),
		(vendordir .. "/glfw/src/glfw_config.h"),
		(vendordir .. "/glfw/src/context.c"),
		(vendordir .. "/glfw/src/init.c"),
		(vendordir .. "/glfw/src/input.c"),
		(vendordir .. "/glfw/src/monitor.c"),
		(vendordir .. "/glfw/src/vulkan.c"),
		(vendordir .. "/glfw/src/window.c"),
		(vendordir .. "/glfw/src/platform.c")
	}
	filter "system:windows"
		systemversion "latest"
		files
		{
			(vendordir .. "/glfw/src/win32_init.c"),
			(vendordir .. "/glfw/src/win32_joystick.c"),
			(vendordir .. "/glfw/src/win32_monitor.c"),
			(vendordir .. "/glfw/src/win32_time.c"),
			(vendordir .. "/glfw/src/win32_thread.c"),
			(vendordir .. "/glfw/src/win32_window.c"),
			(vendordir .. "/glfw/src/wgl_context.c"),
			(vendordir .. "/glfw/src/egl_context.c"),
			(vendordir .. "/glfw/src/osmesa_context.c")
		}
		defines
		{
			"_GLFW_WIN32",
			"_CRT_SECURE_NO_WARNINGS"
		}
	
	filter "configurations:Debug"
        symbols "On"

    filter "configurations:RelWithDebInfo"
        optimize "On"
        symbols "On"

    filter "configurations:Release"
        optimize "On"
        symbols "Off"