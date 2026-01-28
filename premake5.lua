workspace "BHive"
   architecture "x86_64"
   startproject "BHive-Runtime"
   configurations { "Debug", "RelWithDebInfo", "Release" }
   platforms { "x64" }
   flags { "multiprocessorcompile" }

outputdir =  "%{wks.location}/bin/%{cfg.system}/%{cfg.buildcfg}"
vendordir = "%{wks.location}/vendor"

-- Include per-subproject premake files (these files are created below)
group "Dependencies"
    include "vendor/premake"
    group "Dependencies/modules"
        include "build_premake/modules"
    group ""
group ""

group "BHive"
    include "BHive-Engine"
    include "BHive-Editor"
    include "BHive-Runtime"
    include "Shaders"
group ""