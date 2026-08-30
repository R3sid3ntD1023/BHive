#include "RenderSystem.h"
#include "gfx/renderers/SceneRenderer.h"
#include "runtime/GameObject.h"
#include "runtime/World.h"
#include "runtime/Components.h"
#include <physx/PxPhysicsAPI.h>
#include "gfx/renderers/Renderer.h"

namespace BHive
{
	void RenderSystem::OnUpdate(SceneRenderer *renderer, const World *world)
	{
		auto &render_settings = renderer->GetRenderSettings();
		auto &registry = world->GetRegistry();

		{

			auto camera_components = registry.view<TransformComponent, CameraComponent>();
			for (const auto &[e, transform, component] : camera_components.each())
			{
				if (component.IsPrimary)
				{
					const auto &proj = component.Camera.GetProjection();
					const auto view = transform.GetWorldTransform().Inverse();

					// if (world->IsRunning())
					// 	Renderer::Get().SubmitCamera(proj, view);

					if (render_settings.DrawColliders)
					{
						// const auto &mainView = Renderer::Get().GetViewSystem().GetMainView();
						// FrustumViewer viewer(mainView.Projection, mainView.View);

						// LineRenderer::DrawFrustum(viewer, FColor::Green, (int32_t)e);
					}

					break;
				}
			}
		}

		{
			auto view = registry.view<DirectionalLightComponent>();
			for (const auto &e : view)
			{
				auto &c = view.get<DirectionalLightComponent>(e);

				DirectionalLight light{};
				light.SetColor(c.Color).SetDirection(c.GetWorldTransform().GetForward()).SetIntensity(c.Color.a);

				renderer->Submit(light);
			}
		}

		{
			auto view = registry.view<PointLightComponent>();
			for (const auto &e : view)
			{
				auto &c = view.get<PointLightComponent>(e);
				const auto world_transform = c.GetWorldTransform();
				const auto max_scale = glm::compMax(world_transform.GetScale());

				PointLight light{};
				light.SetColor(c.Color).SetPosition(world_transform.GetTranslation()).SetRadius(c.Radius * max_scale).SetIntensity(c.Color.a);

				renderer->Submit(light);

				// renderer->SubmitCommand([=]() { LineRenderer::DrawSphere(c.Radius, 16, {}, 0xFFFFFFFF, world_transform); });
			}
		}

		{
			auto view = registry.view<SpotLightComponent>();
			for (const auto &e : view)
			{
				auto &c = view.get<SpotLightComponent>(e);
				const auto world_transform = c.GetWorldTransform();
				const auto max_scale = glm::compMax(world_transform.GetScale());

				SpotLight light{};
				light.SetColor(c.Color)
					.SetDirection(world_transform.GetForward())
					.SetPosition(world_transform.GetTranslation())
					.SetRadius(c.Radius * max_scale)
					.SetIntensity(c.Color.a)
					.SetInnerAngleDegrees(c.InnerCutoff)
					.SetOuterAngleDegrees(c.OuterCutoff);

				renderer->Submit(light);

				// LineRenderer::DrawCone(c.Radius, c.Radius, 16, 0xFFFFFFFF, world_transform, (int32_t)e);
			}
		}

		{
			auto view = registry.view<StaticMeshComponent>();
			for (const auto &[e, component] : view.each())
			{
				auto mesh = component.GetStaticMesh();
				if (!mesh)
					continue;

				FMeshSubmissionRequest info{};
				info.Mesh = mesh;
				info.Materials = component.GetMaterials();
				info.Transform = component.GetWorldTransform();
				info.EntityID = (int32_t)e;

				renderer->SubmitMesh(info);
			}
		}

		{
			auto view = registry.view<InstancedStaticMeshComponent>();
			for (const auto &e : view)
			{
				auto &component = view.get<InstancedStaticMeshComponent>(e);
				auto mesh = component.GetStaticMesh();
				if (!mesh)
					continue;

				const auto &instances = component.GetInstances();

				FMeshSubmissionRequest info{};
				info.Mesh = mesh;
				info.Materials = component.GetMaterials();
				info.Transform = component.GetWorldTransform();
				info.EntityID = (int32_t)e;
				info.InstanceTransforms = {instances};

				renderer->SubmitMesh(info);
			}
		}

		{
			auto view = registry.view<SkeletalMeshComponent>();
			for (const auto &e : view)
			{
				auto &sc = view.get<SkeletalMeshComponent>(e);

				if (!sc.GetSkeletalMesh())
					continue;

				auto t = sc.GetWorldTransform();
				auto pose = sc.GetSkeletalMesh().As<SkeletalMesh>()->GetDefaultPose();

				FMeshSubmissionRequest info{};
				info.Mesh = sc.GetSkeletalMesh();
				info.Materials = sc.GetMaterials();
				info.Transform = t;
				info.EntityID = (int32_t)e;
				info.BoneTransforms = pose->GetTransformsJointSpace();

				renderer->SubmitMesh(info);
				// LineRenderer::DrawAABB(sc.GetSkeletalMesh()->GetBoundingBox(), FColor::Red, t, (int32_t)e);
			}
		}

		{
			auto view = registry.view<FlipBookComponent>();
			for (const auto &e : view)
			{
				auto &c = view.get<FlipBookComponent>(e);
				if (!c.FlipBookAsset)
					continue;

				FQuadParams params{};
				params.Color = c.Color;
				params.Size = c.Size;
				params.Tiling = c.Tiling;

				auto sprite = c.Instance() ? c.Instance()->GetCurrentSprite() : c.FlipBookAsset->GetCurrentSprite();
				// QuadRenderer::DrawSprite(params, sprite, c.GetWorldTransform(), (int32_t)e);
			}
		}

		{
			auto view = registry.view<SpriteComponent>();
			for (const auto &e : view)
			{
				auto &c = view.get<SpriteComponent>(e);
				if (!c.SpriteAsset)
					continue;

				/*FQuadParams params{};
				params.Color = c.Color;
				params.Size = c.Size;
				params.Tiling = c.Tiling;*/

				// QuadRenderer::DrawSprite(params, c.SpriteAsset, c.GetWorldTransform(), (int32_t)e);
			}
		}

		{
			auto view = registry.view<TextComponent>();
			// for (const auto &e : view)
			//{
			//	//auto &c = view.get<TextComponent>(e);

			//	//QuadRenderer::DrawText(c.Size, c.Text, c.Params, c.GetWorldTransform(), (int32_t)e);
			//}
		}

		if (render_settings.DrawColliders)
		{
			auto box_colliders = registry.view<BoxColliderComponent>();
			auto sphere_colliders = registry.view<SphereColliderComponent>();
			auto capsule_colliders = registry.view<CapsuleColliderComponent>();

			for (const auto &[e, collider] : box_colliders.each())
			{
				// LineRenderer::DrawBox(collider.Extents, collider.Offset, collider.Color, collider.GetWorldTransform(), (int32_t)e);
			}

			for (const auto &[e, collider] : sphere_colliders.each())
			{
				// LineRenderer::DrawSphere(collider.Radius, 32, collider.Offset, collider.Color, collider.GetWorldTransform(), (int32_t)e);
			}

			for (const auto &[e, collider] : capsule_colliders.each())
			{
				// LineRenderer::DrawCapsule(collider.Radius, collider.HalfHeight, 16, collider.Offset, collider.Color, collider.GetWorldTransform(), (int32_t)e);
			}
		}
	}

	void RenderSystem::OnResize(const glm::uvec2 &size, World *world)
	{
		if (!world)
			return;

		auto &registry = world->GetRegistry();
		auto camera_components = registry.view<CameraComponent>();
		for (const auto &[e, component] : camera_components.each())
		{
			component.Camera.Resize(size.x, size.y);
		}
	}

} // namespace BHive