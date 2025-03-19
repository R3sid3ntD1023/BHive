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
#include "asset/AssetFactory.h"
#include "core/FileDialog.h"
#include "gfx/textures/Texture2D.h"
#include "sprite/Sprite.h"

namespace BHive
{
	void GameLayer::OnAttach()
	{
		AssetManager::SetAssetManager(&mResourceManager);

		PhysicsContext::Init();

		auto &window = Application::Get().GetWindow();
		auto &size = window.GetSize();

		RenderCommand::ClearColor(.1f, .1f, .1f);

		mCurrentWorld = CreateRef<World>("World 1-1");

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

	void GameLayer::OnGuiRender()
	{
		auto items = AssetManager::GetAsset<Texture2D>("0f40f57f-e246-44b5-9f34-e675263440e3");
		auto characters = AssetManager::GetAsset<Texture2D>("68101e6e-91a1-446b-ab3a-c4703b598f87");
		auto brick = AssetManager::GetAsset<Texture2D>("dcb136ac-1165-4134-9f85-17cd180a0d2f");

		ImGui::Begin("Textures");
		ImGui::Image((ImTextureID)(uint64_t)*items, {200, 200}, {0, 1}, {1, 0});
		ImGui::Image((ImTextureID)(uint64_t)*characters, {200, 200}, {0, 1}, {1, 0});
		ImGui::Image((ImTextureID)(uint64_t)*brick, {200, 200}, {0, 1}, {1, 0});
		ImGui::End();

		ImGui::Begin("Importer");

		if (ImGui::Button("Import"))
		{
			auto path = FileDialogs::OpenFile(AssetFactory::GetFileFilters());
			if (!path.empty())
			{
				mResourceManager.Import(path);
			}
		}
		ImGui::End();
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