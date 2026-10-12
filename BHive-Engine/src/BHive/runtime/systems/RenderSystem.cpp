#include "RenderSystem.h"
#include "gfx/renderers/Renderer.h"
#include "gfx/renderers/SceneRenderer.h"
#include "runtime/Components.h"
#include "runtime/GameObject.h"
#include "runtime/World.h"

namespace BHive
{
	void RenderSystem::OnUpdate(SceneRenderer &sceneRenderer, const World *world)
	{
		auto &renderer = Renderer::Get();
		auto cameraView = world->GetRegistry().view<CameraComponent>();
		const CameraComponent *primaryCamera = nullptr;
		entt::entity primaryCameraEntity = entt::null;
		for (const auto entity : cameraView)
		{
			auto &camera = cameraView.get<CameraComponent>(entity);
			if (camera.IsPrimary && (primaryCameraEntity == entt::null || entity < primaryCameraEntity))
			{
				primaryCamera = &camera;
				primaryCameraEntity = entity;
			}
		}

		if (primaryCamera)
			primaryCamera->SubmitCamera(sceneRenderer);

		for (const auto &entry : world->GetGameObjects())
		{
			const auto &object = entry.second;
			if (!object || object->IsPendingDestroy())
				continue;

			const auto entityID = static_cast<int32_t>(static_cast<entt::entity>(*object));
			auto *transform = object->GetComponent<TransformComponent>();
			const bool transformDirty = transform && transform->IsDirty();

			for (auto *component : object->GetComponents())
			{
				if (!component || dynamic_cast<CameraComponent *>(component))
					continue;

				const bool isDirty = component->IsDirty() || transformDirty;
				component->ClearDirty();
				component->OnRender({sceneRenderer, renderer, entityID, isDirty});
			}

			if (transformDirty)
				transform->ClearDirty();
		}
	}

	void RenderSystem::OnResize(const glm::uvec2 &size, World *world)
	{
		if (!world)
			return;

		auto cameraComponents = world->GetRegistry().view<CameraComponent>();
		for (const auto &[entity, component] : cameraComponents.each())
			component.Camera.Resize(size.x, size.y);
	}
} // namespace BHive
