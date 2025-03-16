#include "core/Application.h"
#include "core/Window.h"
#include "GameLayer.h"
#include "gfx/RenderCommand.h"

#include "importers/TextureImporter.h"
#include "objects/Block.h"
#include "objects/Player.h"
#include "objects/World.h"

#include "components/BoxComponent.h"
#include "components/PhysicsComponent.h"
#include "components/SpriteComponent.h"
#include "components/TagComponent.h"

namespace BHive
{
	void GameLayer::OnAttach()
	{
		PhysicsContext::Init();

		auto &window = Application::Get().GetWindow();
		auto &size = window.GetSize();

		RenderCommand::ClearColor(.1f, .1f, .1f);

		mCurrentWorld = CreateRef<World>("World 1-1");

		mItems = TextureLoader::Import(RESOURCE_PATH "textures/items.png");
		mTexture = TextureLoader::Import(RESOURCE_PATH "sprites0.jpg");
		mMario = TextureLoader::Import(RESOURCE_PATH "textures/character_and_enemies_32.png");

		mSubTexture = mTexture->CreateSubTexture(0, 480, 480, 480);
		auto sprite = CreateRef<Sprite>(mSubTexture, glm::vec2{0, 0}, glm::vec2{480, 480}, glm::vec2{1, 1});
		auto mario_sprite = CreateRef<Sprite>(mMario, glm::vec2{0, 1}, glm::vec2{16, 16}, glm::vec2{1, 1});

		{
			/*auto camera = mCurrentWorld->CreateGameObject("Camera");
			auto &c = camera->AddComponent<CameraComponent>();
			c.Camera.SetPerspective(75.f, size.x / (float)size.y, 0.01f, 100.f);
			camera->GetComponent<TransformComponent>().Transform.set_translation(0, 1, 10);*/

			auto player = mCurrentWorld->CreateGameObject<Player>("Player");
			player->GetComponent<SpriteComponent>().Sprite = mario_sprite;
			player->GetLocalTransform().set_translation(0, 1, 0);
			mPlayerID = player->GetID();
		}

		{
			auto tile = mCurrentWorld->CreateGameObject<Block>("Block-T0");
			tile->SetSize({20, 1});
			tile->SetSprite(sprite);
			tile->SetTiling({20, 1});

			auto item = Ref<Sprite>(new Sprite(mItems, {0, 4}, {16, 16}, {1, 1}));
			auto stile = mCurrentWorld->CreateGameObject<Block>("Block-T1");
			stile->SetSprite(item);
			stile->GetLocalTransform().set_translation(5, 5, 0);
			stile->GetComponent<TagComponent>().Groups |= BREAKABLE_BLOCKS;

			auto t1 = mCurrentWorld->CreateGameObject<Block>("Block-T2");
			t1->SetSprite(sprite);
			t1->GetLocalTransform().set_translation(4, 5, 0);
			t1->GetComponent<TagComponent>().Groups |= BREAKABLE_BLOCKS;

			auto t2 = mCurrentWorld->CreateGameObject<Block>("Block-T2");
			t2->SetSprite(sprite);
			t2->GetLocalTransform().set_translation(6, 5, 0);
			t2->GetComponent<TagComponent>().Groups |= BREAKABLE_BLOCKS;
		}

		{
			auto texture =
				TextureLoader::Import(RESOURCE_PATH "textures/NES - Super Mario Bros - Background 1 Mountains.png");

			auto mountains = texture->CreateSubTexture(0, 1440, 768, 211);
			auto sprite = CreateRef<Sprite>(mountains, glm::vec2(0, 0), glm::vec2(768, 211), glm::vec2(1, 1));
			auto background = mCurrentWorld->CreateGameObject("Background-mountains");
			background->AddComponent<SpriteComponent>();

			auto sp = background->GetComponent<SpriteComponent>();
			sp.Sprite = sprite;
			sp.SpriteSize = {20, 20};

			background->GetLocalTransform().set_translation(0, 10, -1);
		}

		mCurrentWorld->Begin();
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

	void GameLayer::OnDetach()
	{
		mCurrentWorld->End();

		PhysicsContext::Shutdown();
	}

	bool GameLayer::OnKeyEvent(KeyEvent &e)
	{
		switch (e.Key)
		{
		case Key::R:
		{
			auto player = mCurrentWorld->GetGameObject(mPlayerID);
			player->GetComponent<PhysicsComponent>().SetBodyType(EBodyType::Kinematic);
			player->GetLocalTransform().set_translation(0, 1, 0);
			return true;
		}
		default:
			break;
		}
		return false;
	}

	bool GameLayer::OnResizeEvent(WindowResizeEvent &e)
	{

		mCurrentWorld->Resize(e.x, e.y);

		return false;
	}
} // namespace BHive