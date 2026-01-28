project "glm"
	kind "None"
	language "C++"
	cppdialect "C++20"

	targetdir ("%{wks.location}/bin/" .. outputdir)
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		(vendordir .. "/glm/glm/**.inl"),
		(vendordir .. "/glm/glm/**.h"),
	}
	
	includedirs
	{
		(vendordir .. "/glm")
	}