#include "RenderSystem.h"
#include "renderers/SceneRenderer.h"
#include "World.h"

#include "Components.h"
#include "mesh/SkeletalMesh.h"
#include "renderers/LineRenderer.h"
#include "renderers/QuadRenderer.h"

namespace BHive
{
	void RenderSystem::OnUpdate(SceneRenderer *renderer, const World *world)
	{
		auto &registry = world->GetRegistry();

		{
			auto view = registry.view<CameraComponent>();
			for (const auto &e : view)
			{
				auto &c = view.get<CameraComponent>(e);
				if (c.IsPrimary)
				{
					auto world_transform = c.GetWorldTransform();
					Renderer::SubmitCamera(c.Camera.GetProjection(), world_transform.Inverse());
				}
			}
		}

		{
			auto view = registry.view<DirectionalLightComponent>();
			for (const auto &e : view)
			{
				auto &c = view.get<DirectionalLightComponent>(e);
				renderer->SubmitLight(c.Light, c.GetWorldTransform().GetForward());
			}
		}

		{
			auto view = registry.view<PointLightComponent>();
			for (const auto &e : view)
			{
				auto &c = view.get<PointLightComponent>(e);
				const auto world_transform = c.GetWorldTransform();
				renderer->SubmitLight(c.Light, world_transform.GetTranslation());
				LineRenderer::DrawSphere(c.Light.Radius, 16, {}, c.Light.Color, world_transform);
			}
		}

		{
			auto view = registry.view<SpotLightComponent>();
			for (const auto &e : view)
			{
				auto &c = view.get<SpotLightComponent>(e);
				const auto world_transform = c.GetWorldTransform();
				renderer->SubmitLight(c.Light, world_transform.GetForward(), world_transform.GetTranslation());
				LineRenderer::DrawCone(c.Light.Radius, c.Light.Radius, 16, c.Light.Color, world_transform);
			}
		}

		{
			auto view = registry.view<StaticMeshComponent>();
			for (const auto &e : view)
			{
				auto &sc = view.get<StaticMeshComponent>(e);
				auto mesh = sc.GetStaticMesh();
				if (!mesh)
					continue;

				renderer->SubmitMesh(mesh, sc.GetMaterials(), sc.GetWorldTransform());
			}
		}

		{
			auto view = registry.view<SkeletalMeshComponent>();
			for (const auto &e : view)
			{
				auto &sc = view.get<SkeletalMeshComponent>(e);

				if (!sc.SkeletalMeshAsset)
					continue;

				auto t = sc.GetWorldTransform();
				auto pose = sc.SkeletalMeshAsset->GetDefaultPose();
				renderer->SubmitMesh(sc.SkeletalMeshAsset, *pose, t);
				LineRenderer::DrawAABB(sc.SkeletalMeshAsset->GetBoundingBox(), FColor::Red, t);
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
				QuadRenderer::DrawSprite(params, sprite, c.GetWorldTransform());
			}
		}

		{
			auto view = registry.view<SpriteComponent>();
			for (const auto &e : view)
			{
				auto &c = view.get<SpriteComponent>(e);
				if (!c.SpriteAsset)
					continue;

				FQuadParams params{};
				params.Color = c.Color;
				params.Size = c.Size;
				params.Tiling = c.Tiling;

				QuadRenderer::DrawSprite(params, c.SpriteAsset, c.GetWorldTransform());
			}
		}

		{
			auto view = registry.view<TextComponent>();
			for (const auto &e : view)
			{
				auto &c = view.get<TextComponent>(e);

				QuadRenderer::DrawText(c.Size, c.Text, c.Params, c.GetWorldTransform());
			}
		}

		{
			auto view = registry.view<BoxColliderComponent>();
			for (const auto &e : view)
			{
				auto &c = view.get<BoxColliderComponent>(e);

				LineRenderer::DrawBox(c.Extents, c.Offset, c.Color, c.GetWorldTransform());
			}
		}

		{
			auto view = registry.view<SphereColliderComponent>();
			for (const auto &e : view)
			{
				auto &c = view.get<SphereColliderComponent>(e);

				LineRenderer::DrawSphere(c.Radius, 32, c.Offset, c.Color, c.GetWorldTransform());
			}
		}

		{
			auto view = registry.view<CapsuleColliderComponent>();
			for (const auto &e : view)
			{
				auto &c = view.get<CapsuleColliderComponent>(e);

				LineRenderer::DrawCapsule(c.Radius, c.HalfHeight, 16, c.Offset, c.Color, c.GetWorldTransform());
			}
		}
	}

} // namespace BHive