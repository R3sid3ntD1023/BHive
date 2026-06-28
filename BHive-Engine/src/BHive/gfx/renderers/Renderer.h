#pragma once

#include "core/Core.h"
#include "LineRenderer.h"
#include "QuadRenderer.h"
#include "gfx/RendererAPI.h"
#include "gfx/GlobalResources.h"
#include "PMREMGenerator.h"
#include "EnvironmentSystem.h"
#include "RenderGraphScheduler.h"
#include "ViewSystem.h"
#include "Lights.h"

namespace BHive
{

	class Texture;
	class VertexArray;
	class BufferBase;

	

	struct FPendingPass
	{
		std::string Name;
		EPassType Type;
		std::function<void(FPass &)> BuildFunc;
	};

	class BHIVE_API Renderer
	{
	public:
		LineRenderer Line;
		QuadRenderer Quad;
		Lights Light;

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

		void SetEnvironmentTexture(const Ref<Texture2D> &hdr);

		FView CreateView(const glm::mat4 &projection, const glm::mat4 &view);

		const Frustum &GetFrustum();

		void ResetStats();

		const Statitics &GetStats() const { return mStats; }

		GlobalResources &GetGlobalResources();

		static Renderer& Get() { return *sInstance;}

#pragma region HELPERS

		void ClearColor(float r, float g, float b, float a = 1.0f) ;

		void Clear(ClearMask mask = ClearMask::All);

		void SetLineWidth(float width);

		void SetViewport(uint32_t x, uint32_t y, uint32_t w, uint32_t h);

		void DrawArrays(ETopologyMode mode, VertexArray* vao, uint32_t count = 0);

		void DrawElements(ETopologyMode mode, VertexArray *vao, uint32_t count = 0);

		void DrawElementsBaseVertex(ETopologyMode mode, VertexArray* vao, uint32_t start, uint32_t start_index, uint32_t count = 0, uint32_t instance_count = 0u);

		void DrawElementsRanged(ETopologyMode mode, VertexArray* vao, uint32_t start, uint32_t end, uint32_t count = 0u);

		void DrawElementsInstanced(ETopologyMode mode, VertexArray* vao, uint32_t instances, uint32_t count = 0u);

		void MultiDrawElementsIndirect(ETopologyMode mode, BufferBase* indirect, VertexArray* vao, uint32_t drawCount, uint32_t stride = 0u, uint32_t start = 0u);

		void DrawFullscreen();

		FAsyncPass* ExecuteComputePass(Pipeline *pipeline, const glm::uvec3 &dispatchSize, const FComputeFunc &builder);

		void ExecuteTransferPass(FTransferFunc &&builder);

		template <typename T, typename Method, typename... Args>
		FAsyncPass *ExecuteComputePass(Pipeline *pipeline, const glm::uvec3 &dispatchSize, T *obj, Method method, Args &&...args)
		{
			return ExecuteComputePass(pipeline, dispatchSize, [obj, method, ... captured = std::forward<Args>(args)](FComputeBindings &b) mutable { (obj->*method)(b, captured...); });
		}

		template <typename T, typename Method, typename... Args>
		void ExecuteTransferPass(T *obj, Method method, Args &&...args)
		{
			ExecuteTransferPass([obj, method, ... captured = std::forward<Args>(args)](ITransferContext &b) mutable { (obj->*method)(b, captured...); });
		}

		template<typename Fn>
		void SubmitTransferImmediate(Fn &&fn)
		{
			mAPI->ExecuteTransferPass(std::forward<Fn>(fn));
		}

#pragma endregion

#pragma region RENDERGRAPH

		ViewSystem &GetViewSystem();

		RenderGraph &GetActiveGraph();

		FPass &GetActivePass();

		FPass &BeginPass(const std::string &name, EPassType type);

		void EndPass();

		void SubmitResourceUpdate(FResourceUpdateList::UpdateCommand cmd);

		void SetPassConfig(const PassConfig &config);

		const PassConfig &GetPassConfig() { return mPassConfig; };

#pragma endregion

		template <typename T>
			requires(std::is_base_of_v<RendererAPI, T>)
		inline T *GetGraphicsAPI() const
		{
			return Cast<T>(mAPI.get());
		}

		inline RendererAPI *GetGraphicsAPI() const { return mAPI.get(); }

	private:
		void BeginBatching();

		void EndBatching();

		void InitAndRegisterResources();

	private:
		Scope<RendererAPI> mAPI;

		Ref<struct RenderData> mData;

		GlobalResources mGlobalResources;

		EnvironmentSystem mEnvironment;

		RenderGraphScheduler mScheduler;
		
		//rendergraph
		RenderGraph mGraph;
		PassConfig mPassConfig;
		FResourceUpdateList mResourceUpdates{};

		bool mFrameActive = false;

		Statitics mStats{};

		static inline Renderer *sInstance = nullptr;
	};

} // namespace BHive
