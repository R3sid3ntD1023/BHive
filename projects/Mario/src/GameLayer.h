#pragma once
#include "core/UUID.h"
#include "core/events/ApplicationEvents.h"
#include "core/events/KeyEvents.h"
#include "core/Layer.h"
#include "gfx/textures/Texture2D.h"
#include "ResourceManager.h"

namespace BHive
{
	class World;

	class GameLayer : public Layer
	{
	public:
		void OnAttach() override;
		void OnUpdate(float dt) override;
		void OnEvent(Event &event) override;
		void OnDetach() override;
		void OnGuiRender() override;

	private:
		bool OnKeyEvent(KeyEvent &e);
		bool OnResizeEvent(WindowResizeEvent &e);

	private:
		Ref<World> mCurrentWorld;

		UUID mPlayerID = UUID::Null;
		ResourceManager mResourceManager{RESOURCE_PATH, "AssetRegistry.json"};
	};
} // namespace BHive