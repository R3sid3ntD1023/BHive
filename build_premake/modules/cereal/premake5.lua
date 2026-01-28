project "cereal"
	kind "None"
	language "C++"

	targetdir ("%{wks.location}/bin/" .. outputdir)
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		(vendordir .. "/cereal/include/**.hpp")
	}
	
	includedirs
	{
		(vendordir .. "/cereal/include")
	}

	
	