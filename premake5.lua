workspace "BHive"
   architecture "x86_64"
   startproject "BHive-Runtime"
   configurations { "Debug", "RelWithDebInfo", "Release" }
   platforms { "x64" }
   flags { "multiprocessorcompile" }

-- Common language / project defaults
function set_output_dirs()
   -- put outputs under the workspace root to mirror CMake layout
   targetdir ("%{wks.location}/bin/%{cfg.system}/%{cfg.buildcfg}")
   objdir    ("%{wks.location}/bin-int/%{cfg.system}/%{cfg.buildcfg}")
end

function apply_config_filters()
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
end

-- vcpkg helpers --------------------------------------------------------------

local function read_file(path)
   local f = io.open(path, "r")
   if not f then return nil end
   local content = f:read("*a")
   f:close()
   return content
end

-- Very small, robust extraction of "name" fields from vcpkg.json manifest.
-- This is intentionally permissive (not full JSON parser) and will pick up
-- "name": "pkg" entries under dependencies.
local function parse_vcpkg_manifest(manifest_path)
   local txt = read_file(manifest_path)
   if not txt then return {} end
   local deps = {}
   for name in txt:gmatch('"name"%s*:%s*%"([%w%-%_%.]+)%"') do
      table.insert(deps, name)
   end
   return deps
end

-- Find installed triplet folder and return root (installed/<triplet>) or nil
local function vcpkg_installed_root()
   local vcpkg_root = os.getenv("VCPKG_ROOT")
   if not vcpkg_root or vcpkg_root == "" then return nil end
   local triplet = os.getenv("VCPKG_DEFAULT_TRIPLET") or "x64-windows"
   local installed = path.join(vcpkg_root, "installed", triplet)
   if os.isdir(installed) then
      return installed
   end
   return nil
end

-- Called from a project to wire vcpkg into that project.
-- Adds includedirs/libdirs, links heuristically discovered libs, and creates
-- postbuild copy commands for any found DLLs from vcpkg's bin directory.
function vcpkg_setup_project(manifest_path)
   local installed = vcpkg_installed_root()
   if not installed then
      -- nothing to do
      return
   end

   -- include & lib dirs
   includedirs { path.join(installed, "include") }
   libdirs { path.join(installed, "lib"), path.join(installed, "lib64") }

   -- parse manifest (defaults to workspace root vcpkg.json)
   manifest_path = manifest_path or path.join(wks.location, "vcpkg.json")
   local pkgs = parse_vcpkg_manifest(manifest_path)
   if #pkgs == 0 then
      return
   end

   -- heuristically discover libs/dlls to link & copy
   local libs_to_link = {}
   local dlls_to_copy = {}

   local function add_lib_if_matches(basepath, file)
      local base = path.getbasename(file):lower()
      for _, pkg in ipairs(pkgs) do
         local pkgn = pkg:lower()
         if base:find(pkgn, 1, true) or pkgn:find(base, 1, true) then
            table.insert(libs_to_link, path.getbasename(file))
            return
         end
      end
   end

   -- scan lib dirs for .lib and .a
   local lib_patterns = {
      path.join(installed, "lib", "*.lib"),
      path.join(installed, "lib", "*.a"),
      path.join(installed, "lib64", "*.lib"),
      path.join(installed, "lib64", "*.a"),
      path.join(installed, "debug", "lib", "*.lib"),
      path.join(installed, "debug", "lib", "*.a"),
   }
   for _, patt in ipairs(lib_patterns) do
      for _, f in ipairs(os.matchfiles(patt)) do
         add_lib_if_matches(f, f)
      end
   end

   -- scan bin for dlls that match packages (to copy beside exe)
   local bin_patterns = {
      path.join(installed, "bin", "*.dll"),
      path.join(installed, "debug", "bin", "*.dll"),
   }
   for _, patt in ipairs(bin_patterns) do
      for _, f in ipairs(os.matchfiles(patt)) do
         local base = path.getbasename(f):lower()
         for _, pkg in ipairs(pkgs) do
            local pkgn = pkg:lower()
            if base:find(pkgn, 1, true) or pkgn:find(base, 1, true) then
               table.insert(dlls_to_copy, f)
               break
            end
         end
      end
   end

   -- apply collected links (must be called inside project)
   if #libs_to_link > 0 then
      links(libs_to_link)
   end

   -- create postbuildcommands to copy dlls to the project's targetdir
   if #dlls_to_copy > 0 then
      local copycmds = {}
      for _, dll in ipairs(dlls_to_copy) do
         table.insert(copycmds, "{COPY} \"" .. dll .. "\" \"%{cfg.targetdir}\"")
      end
      postbuildcommands(copycmds)
      postbuildmessage("Copying vcpkg runtime DLLs to target directory")
   end
end



-- Include per-subproject premake files (these files are created below)
group "Dependencies"
    include "vendor/premake"
group ""

group "BHive"
    include "BHive-Engine"
    include "BHive-Editor"
    include "BHive-Runtime"
group ""