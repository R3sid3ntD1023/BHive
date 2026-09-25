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

		// Renderer
		static inline bool DebugEnabled = false;
		static inline bool DebugLabels = false;
		static inline bool DebugPhaseLabels = false;
		static inline bool DebugAssertErrors = false;

		// Features
		static inline bool EnablePhysics = 0u;
		static inline bool EnableAudio = 0u;

		// EditorCamera
		static inline float MoveSpeed = 1.0f;
		static inline float ZoomSpeed = 1.0f;
		static inline float PanSpeed = 1.0f;
		static inline float OrbitSpeed = 1.0f;

		static void Load(const std::filesystem::path &config);
	};

} // namespace BHive