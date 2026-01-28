project "Shaders"
	kind "None"

	files
	{
		"%{wks.location}/shaders/**.glsl",
		"%{wks.location}/shaders/**.vert",
		"%{wks.location}/shaders/**.frag",
		"%{wks.location}/shaders/**.comp",
		"%{wks.location}/shaders/**.geom"
	}

	postbuildmessage("Collecting shaders")