#pragma once

#include "core/CoreAPI.h"

namespace BHive
{
	struct BHIVE_API EngineConfig
	{
		// Window
		static inline std::string Title = "BHive";
		static inline uint32_t WindowWidth = 800;
		static inline uint32_t WindowHeight = 600;
		static inline bool Maximized = false;

		// Engine
		static inline bool EnableLogging = false;
		static inline double TargetFPS = 1000.0;

		// Features
		static inline bool EnablePhysics = 0u;
		static inline bool EnableAudio = 0u;

		// EditorCamera
		static inline float MoveSpeed = 1.0f;
		static inline float ZoomSpeed = 1.0f;
		static inline float PanSpeed = 1.0f;
		static inline float OrbitSpeed = 1.0f;

		static inline constexpr uint32_t GLOBAL_SET_INDEX = 0;
		static inline constexpr uint32_t MATERIAL_SET_INDEX = 1;
		static inline constexpr uint32_t OBJECT_SET_INDEX = 3;
		static inline constexpr uint32_t BATCH_SET_INDEX = 4;
		static inline constexpr uint32_t PASS_SET_INDEX = 5;

		static void Load(const std::filesystem::path &config);
	};

} // namespace BHive