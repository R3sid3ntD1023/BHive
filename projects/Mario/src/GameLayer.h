#pragma once
#include "asset/AssetHandle.h"
#include "core/events/ApplicationEvents.h"
#include "core/events/KeyEvents.h"
#include "core/Layer.h"
#include "gfx/textures/Texture2D.h"

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

	private:
		bool OnKeyEvent(KeyEvent &e);
		bool OnResizeEvent(WindowResizeEvent &e);

	private:
		Ref<World> mCurrentWorld;
		Ref<Texture2D> mTexture;
		Ref<Texture2D> mMario;
		Ref<Texture2D> mSubTexture;
		Ref<Texture2D> mItems;

		UUID mPlayerID = UUID::Null;
	};
} // namespace BHive