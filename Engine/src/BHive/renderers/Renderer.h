#pragma once

#include "core/Core.h"
#include "LineRenderer.h"
#include "QuadRenderer.h"
#include "gfx/Camera.h"
#include "buffers/CameraBuffer.h"
#include "RenderData.h"

namespace BHive
{

	class Texture;
	class Material;
	class VertexArray;
	class Shader;

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
		static void Draw(const Ref<FMeshRenderData> &data);

		static void End();

		static Ref<Texture> GetWhiteTexture();
		static Ref<Texture> GetBlackTexture();
		static const Frustum &GetFrustum();
		static CameraBuffer &GetCamera();

		static void ResetStats();
		static Statitics &GetStats() { return sStats; }

	private:
		struct RenderData;
		static RenderData *sData;
		static inline Statitics sStats;
	};

} // namespace BHive
