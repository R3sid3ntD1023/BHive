#pragma once

#include "math/AABB.hpp"
#include "gfx/Color.h"
#include "math/Frustum.h"

namespace BHive
{
	struct Line
	{
		glm::vec3 p0{};
		glm::vec3 p1{};
		Color color{};
	};

	struct FGrid
	{
		float size = 10.0f;
		uint32_t divisions = 10;
		Color color{.5f};
		Color stepcolor{1.0f};
	};

	struct LineRenderer
	{
		static void Init();
		static void Shutdown();

		static void Begin();
		static void End();

		static void DrawLine(const glm::vec3 &p0, const glm::vec3 &p1, const Color &color, const glm::mat4 &transform = {1.0f});
		static void DrawLine(const Line &line, const glm::mat4 &transform = {1.0f});
		static void
		DrawTriangle(const glm::vec3 &p0, const glm::vec3 &p1, const glm::vec3 &p2, const Color &color, const glm::mat4 &transform = {1.0f});
		static void DrawTriangle(const Line &l0, const Line &l1, const Line &l2, const glm::mat4 &transform = {1.0f});
		static void DrawRect(const float &size, const Color &color, const glm::mat4 &transform = {1.0f});
		static void DrawRect(
			const glm::vec3 &p0, const glm::vec3 &p1, const glm::vec3 &p2, const glm::vec3 &p3, const Color &color,
			const glm::mat4 &transform = {1.0f});
		static void DrawBox(const glm::vec3 &extents, const glm::vec3 &offset, const Color &color, const glm::mat4 &transform = {1.0f});
		static void DrawArc(
			float radius, uint32_t sides, float start, float end, const glm::vec3 &offset, const Color &color, const glm::mat4 &transform = {1.0f});
		static void DrawCircle(float radius, uint32_t sides, const glm::vec3 &offset, const Color &color, const glm::mat4 &transform = {1.0f});
		static void DrawSphere(float radius, uint32_t sides, const glm::vec3 &offset, const Color &color, const glm::mat4 &transform = {1.0f});
		static void DrawGrid(const FGrid &grid, const glm::mat4 &transform = {1.0f});
		static void
		DrawCylinder(float radius, float height, uint32_t sides, const glm::vec3 &offset, const Color &color, const glm::mat4 &transform = {1.0f});
		static void DrawAABB(const AABB &aabb, const Color &color, const glm::mat4 &transform = {1.0f});
		static void DrawCone(float height, float radius, uint32_t sides, const Color &color, const glm::mat4 &transform = {1.0f});
		static void
		DrawCapsule(float radius, float height, uint32_t sides, const glm::vec3 &offset, const Color &color, const glm::mat4 &transform = 1.0f);

		static void DrawArrow(float size, const Color &color, const glm::mat4 &transform = 1.f);

		static void DrawFrustum(const Frustum &frustum, const Color &color);

		static glm::vec3 perpendicular(const glm::vec3 &v);

	private:
		static void StartBatch();
		static void NextBatch();
		static void Flush();

		struct RenderData;
		static RenderData *sData;
	};
} // namespace BHive