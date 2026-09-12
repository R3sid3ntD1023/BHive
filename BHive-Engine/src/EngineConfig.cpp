#include "EngineConfig.h"
#include <mini/ini.h>

namespace BHive
{
	void EngineConfig::Load(const std::filesystem::path &config)
	{
		mINI::INIFile file(config);

		mINI::INIStructure ini;

		auto success = file.read(ini);

		if (!success)
			return;

		auto &enableLogging = ini["Engine"]["EnableLogging"];
		auto &width = ini["Window"]["Width"];
		auto &height = ini["Window"]["Height"];
		auto maximized = ini["Window"]["Maximized"];
		auto &title = ini["Window"]["Title"];
		auto &physics = ini["Physics"]["Enable"];
		auto &audio = ini["Audio"]["Enable"];
		auto &moveSpeed = ini["EditorCamera"]["MoveSpeed"];
		auto &zoomSpeed = ini["EditorCamera"]["ZoomSpeed"];
		auto &panSpeed = ini["EditorCamera"]["PanSpeed"];
		auto &orbitSpeed = ini["EditorCamera"]["OrbitSpeed"];

		Title = title;

#if defined(BHIVE_EDITOR)
		Title == " -Editor";
#endif

#if defined(BUILD_TYPE)
	#if BUILD_TYPE == 0
		Title += " Debug";
	#elif BUILD_TYPE == 1
		Title += " RelWithDebInfo";
	#else
		Title += " Release";
	#endif
#endif
		WindowWidth = stoi(width);
		WindowHeight = stoi(height);
		Maximized = maximized == "true" || maximized == "1";

		EnableLogging = enableLogging == "true" || enableLogging == "1";
		EnablePhysics = physics == "true" || physics == "1";
		EnableAudio = audio == "true" || audio == "1";

		MoveSpeed = stof(moveSpeed);
		ZoomSpeed = stof(zoomSpeed);
		PanSpeed = stof(panSpeed);
		OrbitSpeed = stof(orbitSpeed);
	}
} // namespace BHive