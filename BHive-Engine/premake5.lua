project "BHive-Engine"
   kind "StaticLib"
   language "C++"
   cppdialect "C++20"

    set_output_dirs()

    files { "src/**.h", "src/**.cpp" }

    vcpkg_setup_project()

   filter "system:windows"
      defines { "_CRT_SECURE_NO_WARNINGS" }
   filter {}

   -- Add project-specific include dirs or links here if needed
   -- e.g. includedirs { "include" }
   apply_config_filters()
