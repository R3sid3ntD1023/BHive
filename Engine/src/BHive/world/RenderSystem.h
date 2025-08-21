#pragma once

namespace BHive
{
	class SceneRenderer;
	class World;

	struct RenderSystem
	{
		void OnUpdate(SceneRenderer *renderer, const World *world);
	};
} // namespace BHive