#include "components/Components.h"
#include "GameObject.h"
#include "gfx/RenderCommand.h"
#include "renderers/Renderer.h"
#include "World.h"

namespace BHive
{
	World::World(const std::string &name)
		: mName(name)
	{
	}

	void World::Update(float dt)
	{
		RenderCommand::Clear();

		Camera *camera = nullptr;
		FTransform *transform = nullptr;

		{
			auto view = mRegistry.view<CameraComponent, TransformComponent>();
			view.each(
				[&](CameraComponent &c, TransformComponent &t)
				{
					if (c.Primary)
					{
						camera = &c.Camera;
						transform = &t.Transform;
					}
				});
		}

		if (camera && transform)
		{
			Renderer::SubmitCamera(camera->GetProjection(), transform->inverse());

			Renderer::Begin();

			{
				auto view = mRegistry.view<SpriteComponent, TransformComponent>();
				view.each([](SpriteComponent &s, TransformComponent &t)
						  { QuadRenderer::DrawSprite(s.SpriteSize, s.SpriteColor, t.Transform, s.Sprite); });
			}

			Renderer::End();
		}
	}

	void World::Resize(uint32_t w, uint32_t h)
	{
		auto view = mRegistry.view<CameraComponent>();
		view.each([w, h](CameraComponent &component) { component.Camera.Resize(w, h); });

		RenderCommand::SetViewport(0, 0, w, h);
	}

	void World::AddGameObject(const Ref<GameObject> &object)
	{
		mObjects.emplace(object->GetID(), object);
	}

	Ref<GameObject> World::GetGameObject(const UUID &id) const
	{
		return mObjects.at(id);
	}
} // namespace BHive