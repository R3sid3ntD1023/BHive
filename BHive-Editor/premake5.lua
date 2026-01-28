project "BHive-Editor"
   kind "ConsoleApp" -- change to "WindowedApp" or other if appropriate
   language "C++"
   cppdialect "C++20"

    set_output_dirs()

    files { "src/**.h", "src/**.cpp" }

   links { "BHive-Engine" } -- assumes engine is a static lib in the same workspace

   filter "system:windows"
      defines { "_CRT_SECURE_NO_WARNINGS" }
   filter {}

   apply_config_filters()
