project "BHive-Runtime"
   kind "ConsoleApp"
   language "C++"
   cppdialect "C++20"

   set_output_dirs()

   files { "src/**.h", "src/**.cpp" }

   links { "BHive-Engine" }

   filter "system:windows"
      defines { "_CRT_SECURE_NO_WARNINGS" }
   filter {}

   apply_config_filters()
