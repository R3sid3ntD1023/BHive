#include "EngineConfig.h"
#include <mini/ini.h>

namespace BHive
{
	namespace ini::utils
	{
		template <typename T>
		T GetReturnValue(const std::string &v);

		template <typename T>
		T Get(const mINI::INIMap<std::string> &map, const std::string &key, T defaultValue)
		{
			if (!map.has(key))
				return defaultValue;

			auto v = map.get(key);
			return GetReturnValue<T>(v);
		}

		template <typename T>
		T GetReturnValue(const std::string &v)
		{
			if constexpr (std::is_same_v<T, double>)
				return stod(v);

			else if constexpr (std::is_same_v<T, float>)
				return stof(v);

			else if constexpr (std::is_same_v<T, int>)
				return stoi(v);

			else if constexpr (std::is_same_v<T, unsigned>)
				return stoul(v);

			else if constexpr (std::is_same_v<T, long>)
				return stol(v);

			else if constexpr (std::is_same_v<T, long long>)
				return stoll(v);

			else if constexpr (std::is_same_v<T, unsigned long>)
				return stoul(v);

			else if constexpr (std::is_same_v<T, unsigned long long>)
				return stoull(v);

			else if constexpr (std::is_same_v<T, bool>)
				return v == "true" || v == "1";

			else if constexpr (std::is_same_v<T, std::string>)
				return v;
		}

	} // namespace ini::utils

	void EngineConfig::Load(const std::filesystem::path &config)
	{
		mINI::INIFile file(config);

		mINI::INIStructure ini;

		auto success = file.read(ini);

		if (!success)
			return;

		if (ini.has("Window"))
		{
			Title = ini::utils::Get(ini["Window"], "Title", Title);

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
			WindowWidth = ini::utils::Get(ini["Window"], "Width", WindowWidth);
			WindowHeight = ini::utils::Get(ini["Window"], "Height", WindowHeight);
			Maximized = ini::utils::Get(ini["Window"], "Maximized", Maximized);
		}

		if (ini.has("Engine"))
		{
			EnableLogging = ini::utils::Get(ini["Engine"], "EnableLogging", EnableLogging);
			TargetFPS = ini::utils::Get(ini["Engine"], "TargetFPS", TargetFPS);
		}

		if (ini.has("Physics"))
			EnablePhysics = ini::utils::Get(ini["Physics"], "Enable", EnablePhysics);

		if (ini.has("Audio"))
			EnableAudio = ini::utils::Get(ini["Audio"], "Enable", EnableAudio);

		if (ini.has("EditorCamera"))
		{
			MoveSpeed = ini::utils::Get(ini["EditorCamera"], "MoveSpeed", MoveSpeed);
			ZoomSpeed = ini::utils::Get(ini["EditorCamera"], "ZoomSpeed", ZoomSpeed);
			PanSpeed = ini::utils::Get(ini["EditorCamera"], "PanSpeed", PanSpeed);
			OrbitSpeed = ini::utils::Get(ini["EditorCamera"], "OrbitSpeed", OrbitSpeed);
		}
	}
} // namespace BHive