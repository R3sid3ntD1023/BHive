#pragma once

#include "core/Core.h"

namespace BHive
{
	class SceneRenderer;
	class World;

	struct RenderSystem
	{
		void OnUpdate(SceneRenderer *renderer, const World *world);

		void OnResize(const glm::uvec2 &size, World *world);
	};
} // namespace BHive