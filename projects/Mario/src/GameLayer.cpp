#include "GameLayer.h"
#include "gfx/RenderCommand.h"

#include "importers/TextureImporter.h"
#include "objects/Player.h"
#include "objects/World.h"

namespace BHive
{
	void GameLayer::OnAttach()
	{
		RenderCommand::ClearColor(.1f, .1f, .1f);

		mCurrentWorld = CreateRef<World>("World 1-1");
		mCurrentWorld->CreateGameObject<Player>("Player");

		mTexture = Cast<Texture2D>(TextureImporter::Import(RESOURCE_PATH "sprites0.jpg"));
		auto sprite = CreateRef<Sprite>(mTexture, glm::vec2{0, 1}, glm::vec2{480, 480}, glm::vec2{1, 1});

		auto tile = mCurrentWorld->CreateGameObject<GameObject>("Tile");
		tile->AddComponent<SpriteComponent>().Sprite = sprite;
	}

	void GameLayer::OnUpdate(float dt)
	{
		mCurrentWorld->Update(dt);
	}

	void GameLayer::OnEvent(Event &event)
	{
		EventDispatcher dispatcher(event);
		dispatcher.Dispatch(this, &GameLayer::OnKeyEvent);
		dispatcher.Dispatch(this, &GameLayer::OnResizeEvent);
	}

	bool GameLayer::OnKeyEvent(KeyEvent &e)
	{
		return false;
	}

	bool GameLayer::OnResizeEvent(WindowResizeEvent &e)
	{

		mCurrentWorld->Resize(e.x, e.y);

		return false;
	}
} // namespace BHive