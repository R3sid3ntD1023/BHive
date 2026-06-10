#pragma once

#include "core/Core.h"
#include "LineRenderer.h"
#include "QuadRenderer.h"
#include "gfx/Camera.h"
#include "RenderData.h"
#include "buffers/ModelBuffer.h"
#include "gfx/RendererAPI.h"
#include "gfx/GlobalBuffers.h"
#include "PMREMGenerator.h"
#include "ViewSystem.h"

namespace BHive
{

	class Texture;
	class VertexArray;
	class BufferBase;

	struct PassConfig
	{
		std::string DefaultPassName = "Default Pass";
		EPassType DefaultPassType = EPassType::SwapChain;
		bool DebugMarkers = false;
	};

	class BHIVE_API Renderer
	{
	public:
		LineRenderer Line;
		QuadRenderer Quad;

	public:
		Renderer(Scope<RendererAPI> api);

		~Renderer();

		struct BHIVE_API Statitics
		{
			uint32_t DrawCalls;
			uint32_t InstanceCount;
		};

		void BeginFrame();

		void SubmitCamera(const glm::mat4 &projection, const glm::mat4 &view);

		void Flush();

		void EndFrame();

		void SetEnvironmentTexture(const Ref<Texture2D> &texture);

		FView CreateView(const glm::mat4 &projection, const glm::mat4 &view);

		Ref<Texture> GetWhiteTexture();

		Ref<Texture> GetBlackTexture();

		const Frustum &GetFrustum();

		void ResetStats();

		const Statitics &GetStats() const { return mStats; }

		FModelBuffer &GetModelBuffer();

		//BRDF textures
		Ref<Texture> GetPreFilterEnvironmentTexture();

		Ref<Texture> GetEnviromentCubeTexture();

		Ref<Texture> GetIrradianceTexture();

		Ref<Texture> GetBRDFLUTTexture();

		static Renderer& Get() { return *sInstance;}

#pragma region HELPERS

		void ClearColor(float r, float g, float b, float a = 1.0f) ;

		void Clear(ClearMask mask = ClearMask::All);

		void SetLineWidth(float width);

		void SetViewport(uint32_t x, uint32_t y, uint32_t w, uint32_t h);

		void DrawArrays(ETopologyMode mode, VertexArray* vao, uint32_t count = 0);

		void DrawElements(ETopologyMode mode, VertexArray *vao, uint32_t count = 0);

		void DrawElementsBaseVertex(ETopologyMode mode, VertexArray* vao, uint32_t start, uint32_t start_index, uint32_t count = 0, uint32_t instance_count = 0);

		void DrawElementsRanged(ETopologyMode mode, VertexArray* vao, uint32_t start, uint32_t end, uint32_t count = 0);

		void DrawElementsInstanced(ETopologyMode mode, VertexArray* vao, uint32_t instances, uint32_t count = 0);

		void MultiDrawElementsIndirect(ETopologyMode mode, BufferBase* indirect, VertexArray* vao, size_t drawCount, size_t stride = 0);

		FAsyncPass* ExecuteComputePass(Pipeline *pipeline, const glm::uvec3 &dispatchSize, const FComputeFunc &builder);

		void ExecuteTransferPass(FTransferFunc &&builder);

		template<typename Fn>
		void SubmitTransferImmediate(Fn &&fn)
		{
			mAPI->ExecuteTransferPass(std::forward<Fn>(fn));
		}

#pragma endregion

#pragma region RENDERGRAPH

		ViewSystem &GetViewSystem() { return mViews; }

		RenderGraph &GetActiveGraph();

		FRenderGraphPass &GetActivePass();

		FRenderGraphPass &BeginPass(const std::string &name, EPassType type);

		void EndPass();

		void SubmitResourceUpdate(FResourceUpdateList::UpdateCommand cmd);

		class PassScope
		{
			PassScope(const std::string &name, EPassType type) { Renderer::Get().BeginPass(name, type); }

			~PassScope() { Renderer::Get().EndPass(); }
		};

		void SetPassConfig(const PassConfig &config);

		const PassConfig &GetPassConfig() { return mPassConfig; };

		void DebugPass(const std::string &msg);

#pragma endregion

		template <typename T>
			requires(std::is_base_of_v<RendererAPI, T>)
		inline T *GetGraphicsAPI() const
		{
			return Cast<T>(mAPI.get());
		}

		inline RendererAPI *GetGraphicsAPI() const { return mAPI.get(); }

	private:
		Statitics mStats{};
		PMREMGenerator mPMREMGenerator{};
		GlobalBuffers mGlobalBuffers{};

		ViewSystem mViews;

		//rendergraph
		RenderGraph mGraph;
		FRenderGraphPass *mActivePass = nullptr;
		bool mFrameActive = false;
		PassConfig mPassConfig;
		FResourceUpdateList mResourceUpdates{};

		//api
		Ref<struct RenderData> mData;
		Scope<RendererAPI> mAPI;
		static inline Renderer *sInstance = nullptr;
	};

} // namespace BHive
