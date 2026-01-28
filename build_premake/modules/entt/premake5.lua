project "entt"
	kind "None"
	language "C++"
	cppdialect "C++20"

	targetdir ("%{wks.location}/bin/" .. outputdir)
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		(vendordir .. "/entt/src/entt/**"),
	}
	
	includedirs
	{
		(vendordir .. "/entt/src")
	}