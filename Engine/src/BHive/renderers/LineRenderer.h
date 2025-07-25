#pragma once

#include "gfx/Color.h"
#include "core/math/boundingbox/AABB.h"
#include "core/math/Frustum.h"
#include "core/math/Transform.h"

namespace BHive
{
	struct BHIVE_API Line
	{
		glm::vec3 p0{};
		glm::vec3 p1{};
		FColor color{};
	};

	struct BHIVE_API FGrid
	{
		float size = 10.0f;
		uint32_t divisions = 10;
		FColor color{.5f};
		FColor stepcolor{};
	};

	struct BHIVE_API LineRenderer
	{
		static void Init();
		static void Shutdown();

		static void Begin();
		static void End();

		static void
		DrawLine(const glm::vec3 &p0, const glm::vec3 &p1, const FColor &color, const FTransform &transform = {});
		static void DrawLine(const Line &line, const FTransform &transform = {});
		static void DrawTriangle(
			const glm::vec3 &p0, const glm::vec3 &p1, const glm::vec3 &p2, const FColor &color,
			const FTransform &transform = {});
		static void DrawTriangle(const Line &l0, const Line &l1, const Line &l2, const FTransform &transform = {});
		static void DrawRect(const glm::vec2 &size, const FColor &color, const FTransform &transform = {});
		static void DrawRect(
			const glm::vec3 &p0, const glm::vec3 &p1, const glm::vec3 &p2, const glm::vec3 &p3, const FColor &color,
			const FTransform &transform = {});
		static void
		DrawBox(const glm::vec3 &extents, const glm::vec3 &offset, const FColor &color, const FTransform &transform = {});
		static void DrawArc(
			float radius, uint32_t sides, float start, float end, const glm::vec3 &offset, const FColor &color,
			const FTransform &transform = {});
		static void DrawCircle(
			float radius, uint32_t sides, const glm::vec3 &offset, const FColor &color, const FTransform &transform = {});
		static void DrawSphere(
			float radius, uint32_t sides, const glm::vec3 &offset, const FColor &color, const FTransform &transform = {});
		static void DrawGrid(const FGrid &grid, const FTransform &transform = {});
		static void DrawCylinder(
			float radius, float height, uint32_t sides, const glm::vec3 &offset, const FColor &color,
			const FTransform &transform = {});
		static void DrawAABB(const AABB &aabb, const FColor &color, const FTransform &transform = {});
		static void
		DrawCone(float height, float radius, uint32_t sides, const FColor &color, const FTransform &transform = {});
		static void DrawCapsule(
			float radius, float height, uint32_t sides, const glm::vec3 &offset, const FColor &color,
			const FTransform &transform = {});

		static void DrawArrow(float size, const FColor &color, const FTransform &transform = {});

		static void DrawFrustum(const FrustumViewer &frustum, const FColor &color);

		static void DrawJoint(const glm::mat4 &joint, float size = 1.f, const FColor &color = Colors::Cyan);

	private:
		static void StartBatch();
		static void NextBatch();
		static void Flush();

		struct RenderData;
		static RenderData *sData;
	};
} // namespace BHive