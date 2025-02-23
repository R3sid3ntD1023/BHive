#include "core/profiler/CPUGPUProfiler.h"
#include "gfx/RenderCommand.h"
#include "gfx/Shader.h"
#include "gfx/VertexArray.h"
#include "LineRenderer.h"
#include "Renderer.h"
#include "shaders/LineShader.h"

namespace BHive
{
	struct LineRenderer::RenderData
	{
		const static uint32_t sMaxVertexCount = 20'000;

		struct FLineVertex
		{
			glm::vec3 position;
			glm::vec4 color;
		};

		Ref<Shader> mLineShader;
		Ref<VertexBuffer> mVertexBuffer;
		Ref<VertexArray> mVertexArray;

		FLineVertex *mVertexDataBuffer = nullptr;
		FLineVertex *mVertexDataPtr = nullptr;
		uint32_t mVertexCount = 0;

		RenderData()
		{
			mVertexDataBuffer = new FLineVertex[sMaxVertexCount];

			mVertexBuffer = VertexBuffer::Create(RenderData::sMaxVertexCount * sizeof(FLineVertex));
			mVertexBuffer->SetLayout({{EShaderDataType::Float3}, {EShaderDataType::Float4}});

			mVertexArray = VertexArray::Create();
			mVertexArray->AddVertexBuffer(mVertexBuffer);

			mLineShader = ShaderLibrary::Load("Line", line_vert, line_frag);
		}

		~RenderData()
		{
			mVertexDataPtr = nullptr;
			delete[] mVertexDataBuffer;
		}
	};

	void LineRenderer::Init()
	{
		sData = new RenderData();
	}

	void LineRenderer::Shutdown()
	{
		delete sData;
	}

	void LineRenderer::Begin()
	{
		StartBatch();
	}

	void LineRenderer::End()
	{
		Flush();
	}

	void LineRenderer::DrawLine(const glm::vec3 &p0, const glm::vec3 &p1, const Color &color, const FTransform &transform)
	{
		NextBatch();

		sData->mVertexDataPtr->position = transform.to_mat4() * glm::vec4(p0, 1.0f);
		sData->mVertexDataPtr->color = color;
		sData->mVertexDataPtr++;

		sData->mVertexDataPtr->position = transform.to_mat4() * glm::vec4(p1, 1.0f);
		sData->mVertexDataPtr->color = color;
		sData->mVertexDataPtr++;

		sData->mVertexCount += 2;
	}

	void LineRenderer::DrawLine(const Line &line, const FTransform &transform)
	{
		DrawLine(line.p0, line.p1, line.color, transform);
	}

	void LineRenderer::DrawTriangle(
		const glm::vec3 &p0, const glm::vec3 &p1, const glm::vec3 &p2, const Color &color, const FTransform &transform)
	{
		DrawLine(p0, p1, color, transform);
		DrawLine(p1, p2, color, transform);
		DrawLine(p2, p0, color, transform);
	}

	void LineRenderer::DrawTriangle(const Line &l0, const Line &l1, const Line &l2, const FTransform &transform)
	{
		DrawLine(l0, transform);
		DrawLine(l1, transform);
		DrawLine(l2, transform);
	}

	void LineRenderer::DrawRect(const glm::vec2 &size, const Color &color, const FTransform &transform)
	{
		float w = size.x * .5f;
		float h = size.y * .5f;
		DrawRect({-w, -h, 0}, {w, -h, 0}, {w, h, 0}, {-w, h, 0}, color, transform);
	}

	void LineRenderer::DrawRect(
		const glm::vec3 &p0, const glm::vec3 &p1, const glm::vec3 &p2, const glm::vec3 &p3, const Color &color,
		const FTransform &transform)
	{
		DrawLine(p0, p1, color, transform);
		DrawLine(p1, p2, color, transform);
		DrawLine(p2, p3, color, transform);
		DrawLine(p3, p0, color, transform);
	}

	void
	LineRenderer::DrawBox(const glm::vec3 &extents, const glm::vec3 &offset, const Color &color, const FTransform &transform)
	{
		float x = extents.x;
		float y = extents.y;
		float z = extents.z;

		glm::vec3 top[4] = {{x, y, z}, {x, y, -z}, {-x, y, -z}, {-x, y, z}};
		glm::vec3 bottom[4] = {{x, -y, z}, {x, -y, -z}, {-x, -y, -z}, {-x, -y, z}};

		// top
		DrawLine(top[0] + offset, top[1] + offset, color, transform);
		DrawLine(top[1] + offset, top[2] + offset, color, transform);
		DrawLine(top[2] + offset, top[3] + offset, color, transform);
		DrawLine(top[3] + offset, top[0] + offset, color, transform);

		// sides
		DrawLine(bottom[0] + offset, top[0] + offset, color, transform);
		DrawLine(bottom[1] + offset, top[1] + offset, color, transform);
		DrawLine(bottom[2] + offset, top[2] + offset, color, transform);
		DrawLine(bottom[3] + offset, top[3] + offset, color, transform);

		// bottom
		DrawLine(bottom[0] + offset, bottom[1] + offset, color, transform);
		DrawLine(bottom[1] + offset, bottom[2] + offset, color, transform);
		DrawLine(bottom[2] + offset, bottom[3] + offset, color, transform);
		DrawLine(bottom[3] + offset, bottom[0] + offset, color, transform);
	}

	void LineRenderer::DrawArc(
		float radius, uint32_t sides, float start, float end, const glm::vec3 &offset, const Color &color,
		const FTransform &transform)
	{
		float step = glm::radians(360.0f / (float)sides);
		for (float theta = start; theta < end - step; theta += step)
		{
			float x0 = cos(theta);
			float y0 = 0.0f;
			float z0 = sin(theta);

			float x1 = cos(theta + step);
			float y1 = 0.0f;
			float z1 = sin(theta + step);

			DrawLine(glm::vec3{x0, y0, z0} * radius + offset, glm::vec3{x1, y1, z1} * radius + offset, color, transform);
		}
	}

	void LineRenderer::DrawCircle(
		float radius, uint32_t sides, const glm::vec3 &offset, const Color &color, const FTransform &transform)
	{
		DrawArc(radius, sides, 0, PI * 2, offset, color, transform);
	}

	void LineRenderer::DrawSphere(
		float radius, uint32_t sides, const glm::vec3 &offset, const Color &color, const FTransform &transform)
	{

		auto rotationZ = glm::toMat4(glm::quat({0, 0, PI / 2}));
		auto rotationX = glm::toMat4(glm::quat({PI / 2, 0, 0}));

		DrawCircle(radius, sides, offset, color, transform);
		DrawCircle(radius, sides, offset, color, transform * rotationZ);
		DrawCircle(radius, sides, offset, color, transform * rotationX);
	}

	void LineRenderer::DrawGrid(const FGrid &grid, const FTransform &transform)
	{
		float stepsize = grid.size / grid.divisions;
		float size = grid.size * .5f;
		int divx = 0, divz = 0;

		for (float x = -size; x <= size; x += stepsize, divx++)
		{
			auto color = (divx % 10) ? grid.color : grid.stepcolor;
			DrawLine(Line{.p0 = {x, 0, -size}, .p1 = {x, 0, size}, .color = color}, transform);
		}

		for (float z = -size; z <= size; z += stepsize, divz++)
		{
			auto color = (divz % 10) ? grid.color : grid.stepcolor;
			DrawLine(Line{.p0 = {-size, 0, z}, .p1 = {size, 0, z}, .color = color}, transform);
		}
	}

	void LineRenderer::DrawAABB(const AABB &aabb, const Color &color, const FTransform &transform)
	{
		auto size = aabb.get_extent();

		float x = size.x;
		float y = size.y;
		float z = size.z;

		glm::vec3 top[4] = {{x, y, z}, {x, y, -z}, {-x, y, -z}, {-x, y, z}};
		glm::vec3 bottom[4] = {{x, -y, z}, {x, -y, -z}, {-x, -y, -z}, {-x, -y, z}};

		// top
		DrawLine(top[0], top[1], color, transform);
		DrawLine(top[1], top[2], color, transform);
		DrawLine(top[2], top[3], color, transform);
		DrawLine(top[3], top[0], color, transform);

		// sides
		DrawLine(bottom[0], top[0], color, transform);
		DrawLine(bottom[1], top[1], color, transform);
		DrawLine(bottom[2], top[2], color, transform);
		DrawLine(bottom[3], top[3], color, transform);

		// bottom
		DrawLine(bottom[0], bottom[1], color, transform);
		DrawLine(bottom[1], bottom[2], color, transform);
		DrawLine(bottom[2], bottom[3], color, transform);
		DrawLine(bottom[3], bottom[0], color, transform);
	}

	void LineRenderer::DrawCone(float height, float radius, uint32_t sides, const Color &color, const FTransform &transform)
	{

		auto theta = glm::radians(360.0f / sides);
		for (uint32_t i = 0; i < sides; i++)
		{
			glm::vec3 pos = {glm::cos(theta * i), glm::sin(theta * i), height};

			DrawLine({}, pos * radius, color, transform);
		}

		for (uint32_t i = 0; i < sides; i++)
		{
			glm::vec3 start = {cos(theta * (i)), glm::sin(theta * (i)), height};
			glm::vec3 next = {cos(theta * (i + 1)), glm::sin(theta * (i + 1)), height};

			DrawLine(start * radius, next * radius, color, transform);
		}
	}

	void LineRenderer::DrawFrustum(const Frustum &frustum, const Color &color)
	{
		auto &points = frustum.get_points();
		LineRenderer::DrawRect(points[0], points[1], points[2], points[3], color);
		LineRenderer::DrawRect(points[4], points[5], points[6], points[7], color);

		LineRenderer::DrawLine(points[0], points[4], color);
		LineRenderer::DrawLine(points[1], points[5], color);
		LineRenderer::DrawLine(points[2], points[6], color);
		LineRenderer::DrawLine(points[3], points[7], color);

		auto &planes = frustum._get_planes();
		for (size_t i = 0; i < 6; i++)
		{
			LineRenderer::DrawLine(planes[i].Origin, planes[i].Normal, color);
		}
	}

	void LineRenderer::DrawCylinder(
		float radius, float height, uint32_t sides, const glm::vec3 &offset, const Color &color, const FTransform &transform)
	{
		float hh = height * .5f;
		float step = (PI * 2) / sides;

		for (float theta = 0.f; theta <= (PI * 2); theta += step)
		{
			float x0 = cos(theta);
			float y0 = hh;
			float z0 = sin(theta);

			float x1 = cos(theta + step);
			float y1 = hh;
			float z1 = sin(theta + step);

			float x2 = cos(theta);
			float y2 = -hh;
			float z2 = sin(theta);

			float x3 = cos(theta + step);
			float y3 = -hh;
			float z3 = sin(theta + step);

			DrawLine(glm::vec3{x0, y0, z0} * radius + offset, glm::vec3{x1, y1, z1} * radius + offset, color, transform);
			DrawLine(glm::vec3{x2, y2, z2} * radius + offset, glm::vec3{x3, y3, z3} * radius + offset, color, transform);
			DrawLine(glm::vec3{x0, y0, z0} * radius + offset, glm::vec3{x2, y2, z2} * radius + offset, color, transform);
			DrawLine(glm::vec3{x1, y1, z1} * radius + offset, glm::vec3{x3, y3, z3} * radius + offset, color, transform);
		}
	}

	void LineRenderer::DrawCapsule(
		float radius, float height, uint32_t sides, const glm::vec3 &offset, const Color &color, const FTransform &transform)
	{
		auto rotationY = glm::toMat4(glm::quat({0, PI / 2, 0}));
		auto rotationX = glm::toMat4(glm::quat({PI / 2, 0, 0}));
		glm::vec3 h = {0, 0, height * .5f * radius};

		DrawArc(radius, sides, 0.f, PI, offset + h, color, transform * rotationX);
		DrawArc(radius, sides, 0.f, PI, offset + h, color, transform * rotationY * rotationX);

		DrawCylinder(radius, height, sides, offset, color, transform);

		DrawArc(radius, sides, PI, PI * 2.f, offset - h, color, transform * rotationY * rotationX);
		DrawArc(radius, sides, PI, PI * 2, offset - h, color, transform * rotationX);
	}

	void LineRenderer::DrawArrow(float size, const Color &color, const FTransform &transform)
	{
		auto forward = glm::vec3{1, 0, 0};

		DrawLine({}, forward * size, color, transform);
		DrawLine(forward * size, (glm::vec3{.75f, 0, .25f}) * size, color, transform);
		DrawLine(forward * size, (glm::vec3{.75f, 0, -.25f}) * size, color, transform);
	}

	void LineRenderer::StartBatch()
	{
		sData->mVertexDataPtr = sData->mVertexDataBuffer;
		sData->mVertexCount = 0;
	}

	void LineRenderer::NextBatch()
	{
		if (sData->mVertexCount >= RenderData::sMaxVertexCount)
		{
			Flush();
			StartBatch();
		}
	}

	void LineRenderer::Flush()
	{
		GPU_PROFILER_FUNCTION();

		if (sData->mVertexCount > 0)
		{
			sData->mLineShader->Bind();

			uint32_t size = (uint32_t)((uint8_t *)sData->mVertexDataPtr - (uint8_t *)sData->mVertexDataBuffer);
			sData->mVertexBuffer->SetData(sData->mVertexDataBuffer, size);

			RenderCommand::DrawArrays(Lines, *sData->mVertexArray, sData->mVertexCount);
			Renderer::GetStats().DrawCalls++;

			sData->mLineShader->UnBind();
		}
	}

	LineRenderer::RenderData *LineRenderer::sData = nullptr;
} // namespace BHive