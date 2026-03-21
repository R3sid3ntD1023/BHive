#pragma once

#include "core/Core.h"
#include "LineRenderer.h"
#include "QuadRenderer.h"
#include "gfx/Camera.h"
#include "RenderData.h"
#include "buffers/ModelBuffer.h"

namespace BHive
{

	class Texture;
	class Material;
	class VertexArray;
	class Shader;

	struct FCameraData
	{
		glm::mat4 Projection{1.0f};
		glm::mat4 View{1.0f};
		glm::vec4 NearFar{0.0f, 0.0f, 0.0f, 0.0f};
		glm::vec4 Position{0.0f, 0.0f, 0.0f, 1.0f};
	};


	struct BHIVE_API Renderer
	{
		struct BHIVE_API Statitics
		{
			uint32_t DrawCalls;
			uint32_t InstanceCount;
		};

		static void Init();

		static void Shutdown();

		static void Begin();

		static void SubmitCamera(const glm::mat4 &projection, const glm::mat4 &view);

		static void End();

		static void SetEnvironmentTexture(const Ref<Texture> &texture);

		static Ref<Texture> GetWhiteTexture();

		static Ref<Texture> GetBlackTexture();

		static const Frustum &GetFrustum();

		static FCameraData & GetCameraData();

		static void ResetStats();
		static Statitics &GetStats() { return sStats; }

		static FModelBuffer &GetModelBuffer();

	private:
		struct RenderData;
		static RenderData *sData;
		static inline Statitics sStats;

	};

} // namespace BHive
