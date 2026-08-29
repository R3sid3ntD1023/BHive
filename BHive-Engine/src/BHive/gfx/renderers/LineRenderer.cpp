#include "batches/LineRenderBatch.h"
#include "core/profiler/CPUGPUProfiler.h"
#include "LineRenderer.h"
#include "Renderer.h"

namespace BHive
{

	void LineRenderer::Initialize()
	{
		LineBatch.Initialize();
	}

	void LineRenderer::BeginRecording()
	{
		LineBatch.StartBatch();
	}

	void LineRenderer::Flush(Renderer &renderer)
	{
		GPU_PROFILER_FUNCTION();

		LineBatch.Flush(renderer);
	}

	void LineRenderer::DrawLine(const glm::vec3 &p0, const glm::vec3 &p1, const FColor &color, const FTransform &transform, int32_t entityID)
	{
		if (!LineBatch.IsActive())
			LineBatch.StartBatch();

		if (LineBatch.NeedsFlush(2, 0))
		{
			LineBatch.NextBatch(Renderer::Get());
		}

		auto v0 = LineBatch.GetBuffer().PushVertex();
		auto v1 = LineBatch.GetBuffer().PushVertex();

		v0->Position = transform.ToMat4() * glm::vec4(p0, 1.0f);
		v0->Color = color;
		v0->EntityID = entityID;

		v1->Position = transform.ToMat4() * glm::vec4(p1, 1.0f);
		v1->Color = color;
		v1->EntityID = entityID;
	}

	void LineRenderer::DrawLine(const Line &line, const FTransform &transform, int32_t entityID)
	{
		DrawLine(line.p0, line.p1, line.color, transform, entityID);
	}

	void LineRenderer::DrawTriangle(const glm::vec3 &p0, const glm::vec3 &p1, const glm::vec3 &p2, const FColor &color, const FTransform &transform, int32_t entityID)
	{
		DrawLine(p0, p1, color, transform, entityID);
		DrawLine(p1, p2, color, transform, entityID);
		DrawLine(p2, p0, color, transform, entityID);
	}

	void LineRenderer::DrawTriangle(const Line &l0, const Line &l1, const Line &l2, const FTransform &transform, int32_t entityID)
	{
		DrawLine(l0, transform, entityID);
		DrawLine(l1, transform, entityID);
		DrawLine(l2, transform, entityID);
	}

	void LineRenderer::DrawRect(const glm::vec2 &size, const FColor &color, const FTransform &transform, int32_t entityID)
	{
		float w = size.x * .5f;
		float h = size.y * .5f;
		DrawRect({-w, -h, 0}, {w, -h, 0}, {w, h, 0}, {-w, h, 0}, color, transform, entityID);
	}

	void LineRenderer::DrawRect(const glm::vec3 &p0, const glm::vec3 &p1, const glm::vec3 &p2, const glm::vec3 &p3, const FColor &color, const FTransform &transform, int32_t entityID)
	{
		DrawLine(p0, p1, color, transform, entityID);
		DrawLine(p1, p2, color, transform, entityID);
		DrawLine(p2, p3, color, transform, entityID);
		DrawLine(p3, p0, color, transform, entityID);
	}

	void LineRenderer::DrawBox(const glm::vec3 &halfExtents, const glm::vec3 &offset, const FColor &color, const FTransform &transform, int32_t entityID)
	{
		float x = halfExtents.x;
		float y = halfExtents.y;
		float z = halfExtents.z;

		glm::vec3 top[4] = {{x, y, z}, {x, y, -z}, {-x, y, -z}, {-x, y, z}};
		glm::vec3 bottom[4] = {{x, -y, z}, {x, -y, -z}, {-x, -y, -z}, {-x, -y, z}};

		// top
		DrawLine(top[0] + offset, top[1] + offset, color, transform, entityID);
		DrawLine(top[1] + offset, top[2] + offset, color, transform, entityID);
		DrawLine(top[2] + offset, top[3] + offset, color, transform, entityID);
		DrawLine(top[3] + offset, top[0] + offset, color, transform, entityID);

		// sides
		DrawLine(bottom[0] + offset, top[0] + offset, color, transform, entityID);
		DrawLine(bottom[1] + offset, top[1] + offset, color, transform, entityID);
		DrawLine(bottom[2] + offset, top[2] + offset, color, transform, entityID);
		DrawLine(bottom[3] + offset, top[3] + offset, color, transform, entityID);

		// bottom
		DrawLine(bottom[0] + offset, bottom[1] + offset, color, transform, entityID);
		DrawLine(bottom[1] + offset, bottom[2] + offset, color, transform, entityID);
		DrawLine(bottom[2] + offset, bottom[3] + offset, color, transform, entityID);
		DrawLine(bottom[3] + offset, bottom[0] + offset, color, transform, entityID);
	}

	void LineRenderer::DrawArc(float radius, uint32_t sides, float start, float end, const glm::vec3 &offset, const FColor &color, const FTransform &transform, int32_t entityID)
	{
		float step = glm::radians(360.0f / (float)sides);
		for (float theta = start; theta < end; theta += step)
		{
			float x0 = cos(theta);
			float y0 = 0.0f;
			float z0 = sin(theta);

			float x1 = cos(theta + step);
			float y1 = 0.0f;
			float z1 = sin(theta + step);

			DrawLine(glm::vec3{x0, y0, z0} * radius + offset, glm::vec3{x1, y1, z1} * radius + offset, color, transform, entityID);
		}
	}

	void LineRenderer::DrawCircle(float radius, uint32_t sides, const glm::vec3 &offset, const FColor &color, const FTransform &transform, int32_t entityID)
	{
		DrawArc(radius, sides, 0, PI * 2, offset, color, transform, entityID);
	}

	void LineRenderer::DrawSphere(float radius, uint32_t sides, const glm::vec3 &offset, const FColor &color, const FTransform &transform, int32_t entityID)
	{

		auto rotationZ = glm::toMat4(glm::quat({0, 0, PI / 2}));
		auto rotationX = glm::toMat4(glm::quat({PI / 2, 0, 0}));

		DrawCircle(radius, sides, offset, color, transform, entityID);
		DrawCircle(radius, sides, offset, color, transform * rotationZ, entityID);
		DrawCircle(radius, sides, offset, color, transform * rotationX, entityID);
	}

	void LineRenderer::DrawGrid(const FGrid &grid, const FTransform &transform, int32_t entityID)
	{
		float stepsize = grid.size / grid.divisions;
		float size = grid.size * .5f;
		int divx = 0, divz = 0;

		for (float x = -size; x <= size; x += stepsize, divx++)
		{
			auto color = (divx % 10) ? grid.stepcolor : grid.color;
			DrawLine(Line{.p0 = {x, 0, -size}, .p1 = {x, 0, size}, .color = color}, transform, entityID);
		}

		for (float z = -size; z <= size; z += stepsize, divz++)
		{
			auto color = (divz % 10) ? grid.stepcolor : grid.color;
			DrawLine(Line{.p0 = {-size, 0, z}, .p1 = {size, 0, z}, .color = color}, transform, entityID);
		}
	}

	void LineRenderer::DrawAABB(const AABB &aabb, const FColor &color, const FTransform &transform, int32_t entityID)
	{
		auto size = aabb.GetExtent();

		float x = size.x;
		float y = size.y;
		float z = size.z;

		glm::vec3 top[4] = {{x, y, z}, {x, y, -z}, {-x, y, -z}, {-x, y, z}};
		glm::vec3 bottom[4] = {{x, -y, z}, {x, -y, -z}, {-x, -y, -z}, {-x, -y, z}};

		// top
		DrawLine(top[0], top[1], color, transform, entityID);
		DrawLine(top[1], top[2], color, transform, entityID);
		DrawLine(top[2], top[3], color, transform, entityID);
		DrawLine(top[3], top[0], color, transform, entityID);

		// sides
		DrawLine(bottom[0], top[0], color, transform, entityID);
		DrawLine(bottom[1], top[1], color, transform, entityID);
		DrawLine(bottom[2], top[2], color, transform, entityID);
		DrawLine(bottom[3], top[3], color, transform, entityID);

		// bottom
		DrawLine(bottom[0], bottom[1], color, transform, entityID);
		DrawLine(bottom[1], bottom[2], color, transform, entityID);
		DrawLine(bottom[2], bottom[3], color, transform, entityID);
		DrawLine(bottom[3], bottom[0], color, transform, entityID);
	}

	void LineRenderer::DrawCone(float height, float radius, uint32_t sides, const FColor &color, const FTransform &transform, int32_t entityID)
	{

		auto theta = glm::radians(360.0f / sides);

		// top
		for (uint32_t i = 0; i < sides; i++)
		{
			glm::vec3 pos = {glm::cos(theta * i), glm::sin(theta * i), height};

			DrawLine({}, pos * radius, color, transform, entityID);
		}

		// bottom
		for (uint32_t i = 0; i < sides; i++)
		{
			glm::vec3 start = {cos(theta * (i)), glm::sin(theta * (i)), height};
			glm::vec3 next = {cos(theta * (i + 1)), glm::sin(theta * (i + 1)), height};

			DrawLine(start * radius, next * radius, color, transform, entityID);
		}
	}

	void LineRenderer::DrawFrustum(const Frustum &frustum, const FColor &color, int32_t entityID)
	{
		auto points = frustum.GetPoints();
		LineRenderer::DrawRect(points[0], points[1], points[2], points[3], color, {}, entityID);
		LineRenderer::DrawRect(points[4], points[5], points[6], points[7], color, {}, entityID);

		LineRenderer::DrawLine(points[0], points[4], color, {}, entityID);
		LineRenderer::DrawLine(points[1], points[5], color, {}, entityID);
		LineRenderer::DrawLine(points[2], points[6], color, {}, entityID);
		LineRenderer::DrawLine(points[3], points[7], color, {}, entityID);
	}

	void LineRenderer::DrawCylinder(float radius, float half_height, uint32_t sides, const glm::vec3 &offset, const FColor &color, const FTransform &transform, int32_t entityID)
	{
		float h = half_height * 2;
		float step = (PI * 2) / sides;

		for (float theta = 0.f; theta <= (PI * 2); theta += step)
		{
			float x0 = cos(theta);
			float y0 = h;
			float z0 = sin(theta);

			float x1 = cos(theta + step);
			float y1 = h;
			float z1 = sin(theta + step);

			float x2 = cos(theta);
			float y2 = -h;
			float z2 = sin(theta);

			float x3 = cos(theta + step);
			float y3 = -h;
			float z3 = sin(theta + step);

			DrawLine(glm::vec3{x0, y0, z0} * radius + offset, glm::vec3{x1, y1, z1} * radius + offset, color, transform, entityID);
			DrawLine(glm::vec3{x2, y2, z2} * radius + offset, glm::vec3{x3, y3, z3} * radius + offset, color, transform, entityID);
			DrawLine(glm::vec3{x0, y0, z0} * radius + offset, glm::vec3{x2, y2, z2} * radius + offset, color, transform, entityID);
			DrawLine(glm::vec3{x1, y1, z1} * radius + offset, glm::vec3{x3, y3, z3} * radius + offset, color, transform, entityID);
		}
	}

	void LineRenderer::DrawCapsule(float radius, float half_height, uint32_t sides, const glm::vec3 &offset, const FColor &color, const FTransform &transform, int32_t entityID)
	{
		auto rotationY = glm::toMat4(glm::quat({0, PI / 2, 0}));
		auto rotationX = glm::toMat4(glm::quat({PI / 2, 0, 0}));
		glm::vec3 h = {0, 0, half_height};

		DrawArc(radius, sides, 0.f, PI, offset + h, color, transform * rotationX, entityID);
		DrawArc(radius, sides, 0.f, PI, offset + h, color, transform * rotationY * rotationX, entityID);

		DrawCylinder(radius, half_height, sides, offset, color, transform, entityID);

		DrawArc(radius, sides, PI, PI * 2.f, offset - h, color, transform * rotationY * rotationX, entityID);
		DrawArc(radius, sides, PI, PI * 2, offset - h, color, transform * rotationX, entityID);
	}

	void LineRenderer::DrawSpotlightCone(const glm::vec3 &pos, const glm::vec3 &dir, float radius, float outerCutOff, uint32_t sides, const FColor &color, int32_t entityID)
	{
		float angle = glm::radians(outerCutOff);
		float height = radius;
		float baseRadius = glm::tan(angle) * height;

		glm::vec3 forward = glm::normalize(dir);

		// build orthonormal basis
		glm::vec3 up = glm::abs(forward.y) > 0.99f ? glm::vec3(1, 0, 0) : glm::vec3(0, 1, 0);
		glm::vec3 right = glm::normalize(glm::cross(forward, up));
		up = glm::cross(right, forward);

		float step = glm::two_pi<float>() / sides;

		for (uint32_t i = 0; i < sides; i++)
		{
			float t0 = i * step;
			float t1 = (i + 1) * step;

			glm::vec3 p0 = pos + forward * height + right * glm::cos(t0) * baseRadius + up * glm::sin(t0) * baseRadius;
			glm::vec3 p1 = pos + forward * height + right * glm::cos(t1) * baseRadius + up * glm::sin(t1) * baseRadius;

			// apex -> circle
			DrawLine(pos, p0, color, {}, entityID);

			// circle edge
			DrawLine(p0, p1, color, {}, entityID);
		}
	}

	void LineRenderer::DrawArrow(float size, const FColor &color, const FTransform &transform, int32_t entityID)
	{
		auto forward = glm::vec3{1, 0, 0};

		DrawLine({}, forward * size, color, transform, entityID);
		DrawLine(forward * size, (glm::vec3{.75f, 0, .25f}) * size, color, transform, entityID);
		DrawLine(forward * size, (glm::vec3{.75f, 0, -.25f}) * size, color, transform, entityID);
	}

	void LineRenderer::DrawJoint(const glm::mat4 &joint, float size, const FColor &color, int32_t entityID)
	{
		DrawSphere(0.05f, 16, {}, color, joint, entityID);
	}

	void LineRenderer::SetLineWidth(float width)
	{
		LineBatch.SetLineWidth(width);
	}

} // namespace BHive